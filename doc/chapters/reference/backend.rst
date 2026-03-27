..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*******
Backend
*******

The backend classes connect :cpp:any:`Terminal <erbsland::cterm::Terminal>`
to the actual console, host environment, or test harness that emits text
and receives input.

Most applications should use the built-in platform backend. Reach for
:cpp:any:`Backend <erbsland::cterm::Backend>` only when you need to embed
the library into another console abstraction, capture terminal output
for tests, or adapt terminal behavior to a non-standard runtime.

Usage
=====

Choosing When to Use a Custom Backend
-------------------------------------

The default backend already handles normal terminal work on POSIX and
Windows systems. A custom backend is usually only worth the extra code
in three situations:

* You want to integrate the library into an existing UI or console host.
* You need a deterministic recording backend for tests or demos.
* You are targeting a platform where terminal access is provided through
  a different abstraction layer.

The simplest way to install a backend is to pass it to the terminal
constructor before calling
:cpp:any:`Terminal::initializeScreen() <erbsland::cterm::Terminal::initializeScreen()>`.

.. code-block:: cpp

    auto backend = std::make_shared<MyBackend>();
    auto terminal = Terminal{backend, Size{80, 25}};

    terminal.initializeScreen();
    // ...
    terminal.restoreScreen();

You can also swap the backend later with
:cpp:any:`Terminal::setBackend() <erbsland::cterm::Terminal::setBackend()>`,
but configuring the terminal once up front is usually easier to follow.

As a general application-level rule, keep exactly one
:cpp:any:`Terminal <erbsland::cterm::Terminal>` instance alive for the
process lifetime. The built-in platform backends maintain shared
process-wide state, so a single terminal instance is the safest default
design.

How Terminal Uses Backend Capabilities
--------------------------------------

The backend API is capability-based. :cpp:any:`Terminal <erbsland::cterm::Terminal>`
checks what the backend supports and then chooses between direct ANSI
output and backend callbacks.

The most important capability methods are:

* :cpp:any:`Backend::supportsColorCodes() <erbsland::cterm::Backend::supportsColorCodes()>`
  controls whether :cpp:any:`Terminal <erbsland::cterm::Terminal>`
  emits ANSI color sequences itself. If it returns ``false``, the
  terminal calls
  :cpp:any:`Backend::emitColor() <erbsland::cterm::Backend::emitColor()>`
  instead.
* :cpp:any:`Backend::supportsCursorCodes() <erbsland::cterm::Backend::supportsCursorCodes()>`
  controls whether cursor movement and screen clearing use ANSI escape
  sequences or backend hooks such as
  :cpp:any:`Backend::moveCursor() <erbsland::cterm::Backend::moveCursor()>`
  and :cpp:any:`Backend::clearScreen() <erbsland::cterm::Backend::clearScreen()>`.
* :cpp:any:`Backend::supportsCursorVisibilityCodes() <erbsland::cterm::Backend::supportsCursorVisibilityCodes()>`
  decides whether cursor visibility uses ANSI ``ESC[?25h`` and
  ``ESC[?25l`` or
  :cpp:any:`Backend::setCursorVisible() <erbsland::cterm::Backend::setCursorVisible()>`.
* :cpp:any:`Backend::supportsAlternateScreenBufferCodes() <erbsland::cterm::Backend::supportsAlternateScreenBufferCodes()>`
  decides whether alternate-screen switching uses ANSI ``ESC[?1049h``
  and ``ESC[?1049l`` in addition to
  :cpp:any:`Backend::setAlternateScreenBuffer() <erbsland::cterm::Backend::setAlternateScreenBuffer()>`.
* :cpp:any:`Backend::supportedCharAttributes() <erbsland::cterm::Backend::supportedCharAttributes()>`
  reports which character attributes the backend can represent at all.
* :cpp:any:`Backend::supportedCharAttributeCodes() <erbsland::cterm::Backend::supportedCharAttributeCodes()>`
  tells :cpp:any:`Terminal <erbsland::cterm::Terminal>` which of those
  attributes can be emitted directly as ANSI SGR codes. If a supported
  attribute is missing here, the terminal flushes pending text and calls
  :cpp:any:`Backend::emitCharAttributes() <erbsland::cterm::Backend::emitCharAttributes()>`
  instead.

Two details are easy to overlook:

* :cpp:any:`Backend::emitText() <erbsland::cterm::Backend::emitText()>`
  always receives UTF-8 text. When ANSI is enabled, escape sequences are
  passed through as text as well, and each call contains complete UTF-8
  and ANSI sequences.
* Line buffering is only available when color codes, cursor codes, and
  character-attribute codes can all stay in sync through ANSI. If a
  backend requires callback-based attribute updates, the terminal
  disables that optimization automatically.

Implementing a Recording Backend
--------------------------------

This example shows a small backend that records emitted text and tracks
the cursor through backend callbacks instead of ANSI cursor codes.

.. code-block:: cpp

    class RecordingBackend final : public Backend {
    public:
        void initializePlatform() override {}
        void restorePlatform() override {}

        [[nodiscard]] auto supportsColorCodes() const noexcept -> bool override { return false; }
        [[nodiscard]] auto supportsCursorCodes() const noexcept -> bool override { return false; }
        [[nodiscard]] auto isInteractive() const noexcept -> bool override { return true; }
        [[nodiscard]] auto detectScreenSize() -> std::optional<Size> override { return Size{80, 25}; }

        void emitColor(Color color) override { _lastColor = color; }
        void moveCursor(Position posOrDelta, MoveMode mode) override {
            if (mode == MoveMode::Absolute) {
                _cursor = posOrDelta;
            } else {
                _cursor += posOrDelta;
            }
        }
        void clearScreen() override { _cursor = Position{0, 0}; }

        void emitText(std::string_view text) override { _log += text; }
        void emitFlush() override {}

        [[nodiscard]] auto inputMode() const noexcept -> Input::Mode override { return _inputMode; }
        void setInputMode(Input::Mode mode) override { _inputMode = mode; }
        [[nodiscard]] auto readKey(std::chrono::milliseconds) -> Key override { return {}; }
        [[nodiscard]] auto waitForKey() -> Key override { return {}; }
        [[nodiscard]] auto readLine() -> std::string override { return {}; }

        [[nodiscard]] auto log() const noexcept -> const std::string & { return _log; }

    private:
        Position _cursor;
        Color _lastColor;
        Input::Mode _inputMode{Input::Mode::ReadLine};
        std::string _log;
    };

This style of backend is especially useful for integration tests that
need to inspect what the terminal would have emitted without depending
on a real terminal session.

Screen Size and Input Responsibilities
--------------------------------------

Backends are also responsible for the low-level parts of interactivity:

* :cpp:any:`Backend::detectScreenSize() <erbsland::cterm::Backend::detectScreenSize()>`
  reports the visible terminal size or ``std::nullopt`` if the host
  cannot provide one.
* :cpp:any:`Backend::isInteractive() <erbsland::cterm::Backend::isInteractive()>`
  tells :cpp:any:`Terminal <erbsland::cterm::Terminal>` whether a real,
  interactive console is attached.
* :cpp:any:`Backend::inputMode() <erbsland::cterm::Backend::inputMode()>`,
  :cpp:any:`Backend::setInputMode() <erbsland::cterm::Backend::setInputMode()>`,
  :cpp:any:`Backend::readKey() <erbsland::cterm::Backend::readKey()>`,
  :cpp:any:`Backend::waitForKey() <erbsland::cterm::Backend::waitForKey()>`,
  and :cpp:any:`Backend::readLine() <erbsland::cterm::Backend::readLine()>`
  power the public :cpp:any:`Input <erbsland::cterm::Input>` API.

For backend authors, the important contract is:

* ``readKey(timeout)`` must normalize negative timeouts to zero.
* In key mode, any timeout less than or equal to zero must be non-blocking.
* ``waitForKey()`` is the dedicated blocking key-input path.

The terminal applies its own one-column and one-row safe margin on top
of the detected screen size. Backend implementations should therefore
return the real visible terminal dimensions instead of subtracting that
margin themselves.

Public API and Internal Implementations
---------------------------------------

This page describes the stable public contract. The built-in backend
implementations are documented separately in the implementation notes:

* :doc:`../implementation/posix-backend`
* :doc:`../implementation/windows-backend`
* :doc:`../implementation/index`

These pages explain how the shipped backends satisfy this contract on
their respective platforms, including size detection, raw key handling,
and shutdown cleanup.

Interface
=========

.. doxygenclass:: erbsland::cterm::Backend
    :members:

.. doxygentypedef:: erbsland::cterm::BackendPtr
