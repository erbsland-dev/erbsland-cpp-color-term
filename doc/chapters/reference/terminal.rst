..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

********
Terminal
********

The terminal classes provide the runtime connection between your rendered
content and the real console. :cpp:any:`Terminal <erbsland::cterm::Terminal>` manages the application
lifecycle, refresh strategy, cursor control, direct text writes, and the
platform backend that ultimately emits text and control sequences.

Use this page when you want to initialize the screen, configure how
frames are written to the terminal, or integrate the library with a
custom backend.

Usage
=====

Preparing and Rendering a Screen
--------------------------------

:cpp:any:`Terminal <erbsland::cterm::Terminal>` is the high-level entry point for full-screen terminal
applications. It owns the platform backend, manages the visible screen
state, and renders :cpp:any:`ReadableBuffer <erbsland::cterm::ReadableBuffer>` objects with clipping and optional
crop marks.

.. code-block:: cpp

    auto terminal = Terminal{Size{90, 28}};
    terminal.initializeScreen();
    terminal.setRefreshMode(Terminal::RefreshMode::Overwrite);
    auto buffer = Buffer{terminal.size()};
    buffer.fill(Char{" ", Color{fg::Default, bg::Black}});
    buffer.drawText(
        "Frame and buffer rendering",
        Rectangle{2, 2, buffer.size().width() - 4, 5},
        Alignment::Center,
        Color{fg::BrightWhite, bg::Blue});

    auto settings = UpdateSettings{};
    settings.setSwitchToAlternateBuffer(false);
    terminal.updateScreen(buffer, settings);
    terminal.flush();
    terminal.restoreScreen();

:cpp:any:`Terminal::size() <erbsland::cterm::Terminal::size()>` returns the drawable size that ``updateScreen()``
uses. By default this includes a one-column and one-row compatibility
margin so full-screen buffers work out of the box. Disable that margin
with ``setSafeMarginEnabled(false)`` if you want to use the full
detected terminal size and cursor-based updates without newline output.
For short-lived tools and examples that should leave their output visible
in the normal terminal history, disable alternate-screen switching in
:cpp:any:`UpdateSettings <erbsland::cterm::UpdateSettings>` as shown above.

Render Loop for Interactive Applications
----------------------------------------

The following example shows a minimal render loop for an interactive terminal application.

.. code-block:: cpp

    struct MyApp {
        void renderFrame() {
            _terminal.testScreenSize();
            _buffer.resize(_terminal.size());
            _buffer.fill(Char{" ", bg::Black});

            // Draw the current frame into the buffer.
            _terminal.updateScreen(_buffer, _updateSettings);
        }

        void handleKey(Key key) {
            if (key == Key{Key::Character, 'q'}) {
                _quitRequested = true;
            }
        }

        void run() {
            _terminal.initializeScreen();
            _terminal.input().setMode(Input::Mode::Key);

            _updateSettings.setMinimumSize(Size{40, 20});
            _updateSettings.setMinimumSizeMessage(String("Too Small!"));

            while (!_quitRequested) {
                const auto key = _terminal.input().read(std::chrono::milliseconds{90});
                if (key.valid()) {
                    handleKey(key);
                }
                renderFrame();
            }

            _terminal.restoreScreen();
        }

        Terminal _terminal;
        Buffer _buffer;
        UpdateSettings _updateSettings;
        bool _quitRequested;
    };

The most important rule is to call
:cpp:any:`Terminal::initializeScreen() <erbsland::cterm::Terminal::initializeScreen()>`
**once** when your application starts, and
:cpp:any:`Terminal::restoreScreen() <erbsland::cterm::Terminal::restoreScreen()>`
**once** just before the application exits.

If your application is interrupted between these calls (for example by ``Ctrl+C`` or other signals), the library automatically restores the terminal state before control returns to the shell.

You should also create **exactly one instance** of
:cpp:any:`Terminal <erbsland::cterm::Terminal>`
for the lifetime of your application. While it is technically possible to create multiple instances, they all share the same underlying backend. For most applications it is therefore simpler and safer to keep a single terminal instance (for example as a shared pointer ``TerminalPtr``).

For interactive programs, switch the input mode to ``Input::Mode::Key``. This hides the cursor and allows your application to react to **individual key presses** using the ``input().read()`` method. The timeout of this method also provides a convenient way to control your refresh rate.

When rendering frames, keep a **persistent instance** of ``Buffer`` and resize it to the current terminal size. After preparing the next frame in the buffer, call ``updateScreen()`` to display it.

By default, the library uses an optimized rendering strategy that only redraws the **parts of the screen that actually changed**. This makes frequent refresh rates practical. Even if you refresh every 100 ms, only modified regions of the screen are updated, minimizing terminal output and improving performance.

Direct Writes, Line Buffering, and Refresh Modes
------------------------------------------------

For simple tools and status output, :cpp:any:`Terminal <erbsland::cterm::Terminal>` can also be used without
a full-screen buffer. ``print()``, ``printLine()``, and ``write()`` let
you send text directly to the terminal while still benefiting from the
library's color handling.

.. code-block:: cpp

    terminal.setOutputMode(Terminal::OutputMode::FullControl);
    terminal.printLine(fg::BrightGreen, "Service started");
    terminal.print("Current mode: ", fg::BrightYellow, "interactive");
    terminal.writeLineBreak();
    terminal.flush();

Line buffering keeps partial lines in memory until a newline is written
or ``flush()`` is called. This keeps incremental log output tidy. For
full-screen apps, ``RefreshMode::Clear`` clears the whole screen before a
frame, while ``RefreshMode::Overwrite`` moves to the home position and
can cooperate with the optional back buffer for smart updates.

Controlling Screen Update Settings
----------------------------------

:cpp:any:`UpdateSettings <erbsland::cterm::UpdateSettings>` lets you control how :cpp:any:`Terminal::updateScreen() <erbsland::cterm::Terminal::updateScreen()>`
behaves when the content is larger than the terminal or the terminal is
too small for the intended layout.

.. code-block:: cpp

    auto settings = UpdateSettings{};
    settings.setMinimumSize(Size{60, 18});
    settings.setMinimumSizeBackground(Char{" ", Color{fg::Inherited, bg::Red}});
    settings.setMinimumSizeMessage(String{"Please enlarge the terminal"});
    settings.setShowCropMarks(true);

    terminal.updateScreen(buffer, settings);

This is the right place to customize crop indicators, minimum-size
warnings, and the fallback background used while a full layout cannot be
rendered safely.

Output Modes and Backend Capabilities
-------------------------------------

:cpp:any:`Terminal <erbsland::cterm::Terminal>` supports two output modes:

* :cpp:any:`Terminal::OutputMode::FullControl <erbsland::cterm::Terminal::OutputMode::FullControl>` is the default. It enables
  colored output, cursor control, refresh modes, back-buffer updates,
  and terminal-size detection.
* :cpp:any:`Terminal::OutputMode::Text <erbsland::cterm::Terminal::OutputMode::Text>` keeps the terminal in plain-text mode.
  In this mode, ``write()`` and ``print()`` still work, but cursor
  control, refresh handling, screen clearing, size detection, and the
  back buffer are disabled.

Backends may also provide only part of the full terminal feature set. If
ANSI color codes are unavailable, :cpp:any:`Terminal <erbsland::cterm::Terminal>` falls back to the backend
``emitColor()`` hook. If cursor-control ANSI codes are unavailable,
:cpp:any:`Terminal <erbsland::cterm::Terminal>` uses backend cursor and clear-screen hooks instead.

Implementing a Custom Backend
-----------------------------

:cpp:any:`Backend <erbsland::cterm::Backend>` is the extension point for custom terminal environments. It
is primarily useful when you need to integrate the library into an
existing console framework, test harness, or platform abstraction.

.. code-block:: cpp

    class RecordingBackend final : public Backend {
    public:
        void initializePlatform() override {}
        void restorePlatform() override {}
        [[nodiscard]] auto supportsColorCodes() const noexcept -> bool override { return false; }
        [[nodiscard]] auto supportsCursorCodes() const noexcept -> bool override { return false; }
        [[nodiscard]] auto detectScreenSize() -> std::optional<Size> override { return Size{80, 25}; }
        void moveCursor(Position posOrDelta, MoveMode mode) override {
            if (mode == MoveMode::Absolute) {
                _cursor = posOrDelta;
            } else {
                _cursor += posOrDelta;
            }
        }
        void emitText(std::string_view text) override { _log += text; }
        void emitFlush() override {}
        [[nodiscard]] auto inputMode() const noexcept -> Input::Mode override { return Input::Mode::ReadLine; }
        void setInputMode(Input::Mode) override {}
        [[nodiscard]] auto readKey(std::chrono::milliseconds) -> Key override { return {}; }
        [[nodiscard]] auto readLine() -> std::string override { return {}; }

    private:
        Position _cursor;
        std::string _log;
    };

:cpp:any:`MoveMode <erbsland::cterm::MoveMode>` tells the backend whether a cursor command is absolute or
relative. Most applications use the built-in backend, but a custom
backend is helpful for integration tests and platforms where terminal
access is provided through another library.

Interface
=========

.. doxygenclass:: erbsland::cterm::Backend
    :members:

.. doxygenenum:: erbsland::cterm::MoveMode

.. doxygenclass:: erbsland::cterm::UpdateSettings
    :members:

.. doxygenclass:: erbsland::cterm::Terminal
    :members:

.. doxygentypedef:: erbsland::cterm::TerminalPtr

.. doxygenenum:: erbsland::cterm::TerminalFlag

.. doxygenclass:: erbsland::cterm::TerminalFlags
    :members:
