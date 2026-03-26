..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

********
Terminal
********

The terminal classes provide the runtime connection between your rendered
content and the real console. :cpp:any:`Terminal <erbsland::cterm::Terminal>` manages the application
lifecycle, refresh strategy, cursor control, direct text output, and the
platform backend that ultimately emits text and control sequences.

This section focuses on application-level terminal control. For the
shared streaming output API used by both
:cpp:any:`Terminal <erbsland::cterm::Terminal>` and
:cpp:any:`CursorBuffer <erbsland::cterm::CursorBuffer>`, see
:doc:`cursor-output`. For custom backend implementations and capability
fallback behavior, see :doc:`backend`.

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

:cpp:any:`Terminal::size() <erbsland::cterm::Terminal::size()>` returns the drawable size used by
``updateScreen()``. By default, this includes a one-column and one-row
compatibility margin so full-screen buffers work out of the box.

Disable that margin with ``setSafeMarginEnabled(false)`` if you want to
use the full detected terminal size and rely purely on cursor-based
updates.

For short-lived tools that should leave their output in the normal
terminal history, disable alternate-screen switching via
:cpp:any:`UpdateSettings <erbsland::cterm::UpdateSettings>` as shown above.

Render Loop for Interactive Applications
----------------------------------------

The following example shows a minimal render loop for an interactive
terminal application.

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
            if (key == Key{Key::Character, U'q'}) {
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

Call :cpp:any:`Terminal::initializeScreen() <erbsland::cterm::Terminal::initializeScreen()>`
once when your application starts, and
:cpp:any:`Terminal::restoreScreen() <erbsland::cterm::Terminal::restoreScreen()>`
once just before it exits.

After initialization, use
:cpp:any:`Terminal::isInteractive() <erbsland::cterm::Terminal::isInteractive()>`
to detect whether a real terminal is attached. This allows you to switch
to a plain-text fallback when output is redirected.

If your application is interrupted (for example via ``Ctrl+C``), the
library restores the terminal state automatically before returning
control to the shell.

Create exactly one :cpp:any:`Terminal <erbsland::cterm::Terminal>` instance
for the lifetime of your application. While multiple instances are
possible, they share the same underlying backend, so a single instance is
simpler and safer.

For interactive programs, switch the input mode to
``Input::Mode::Key``. This hides the cursor and lets your application
react to individual key presses via ``input().read()``. The timeout also
serves as a natural frame rate control.

Keep a persistent ``Buffer`` instance, resize it to the current terminal
size, and reuse it for each frame. After drawing into the buffer, call
``updateScreen()`` to present the new frame.

By default, rendering is optimized to update only the parts of the screen
that actually changed. This makes frequent refresh rates practical, as
only modified regions are written to the terminal.

Direct Writes, Line Buffering, and Refresh Modes
------------------------------------------------

For simpler tools and status output, you can use
:cpp:any:`Terminal <erbsland::cterm::Terminal>` without a full-screen buffer.
``print()``, ``printLine()``, and ``write()`` send text directly to the
terminal while still applying color handling.

These direct-write functions are part of the shared
:cpp:any:`CursorWriter <erbsland::cterm::CursorWriter>` interface. This page
focuses on how they behave on a real terminal; the reusable streaming
API itself is documented on :doc:`cursor-output`.

.. code-block:: cpp

    terminal.setOutputMode(Terminal::OutputMode::FullControl);
    terminal.printLine(fg::BrightGreen, "Service started");
    terminal.print("Current mode: ", fg::BrightYellow, "interactive");
    terminal.writeLineBreak();
    terminal.flush();

Line buffering keeps partial lines in memory until a newline is written
or ``flush()`` is called. This keeps incremental output clean and avoids
fragmented lines.

For full-screen applications:

* ``RefreshMode::Clear`` clears the entire screen before each frame.
* ``RefreshMode::Overwrite`` moves to the home position and works with
  the back buffer for efficient updates.

Controlling Screen Update Settings
----------------------------------

:cpp:any:`UpdateSettings <erbsland::cterm::UpdateSettings>` controls how
:cpp:any:`Terminal::updateScreen() <erbsland::cterm::Terminal::updateScreen()>`
behaves when content does not fit the terminal or the terminal is too
small.

.. code-block:: cpp

    auto settings = UpdateSettings{};
    settings.setMinimumSize(Size{60, 18});
    settings.setMinimumSizeBackground(Char{" ", Color{fg::Inherited, bg::Red}});
    settings.setMinimumSizeMessage(String{"Please enlarge the terminal"});
    settings.setShowCropMarks(true);

    terminal.updateScreen(buffer, settings);

Use this to define minimum sizes, customize fallback backgrounds, and
control crop indicators when the layout cannot be rendered as intended.

Output Modes
------------

:cpp:any:`Terminal <erbsland::cterm::Terminal>` supports two output modes:

* :cpp:any:`Terminal::OutputMode::FullControl <erbsland::cterm::Terminal::OutputMode::FullControl>`
  (default) enables colored output, cursor control, refresh handling,
  back-buffer updates, and terminal-size detection.
* :cpp:any:`Terminal::OutputMode::Text <erbsland::cterm::Terminal::OutputMode::Text>`
  keeps the terminal in plain-text mode. In this mode, ``write()`` and
  ``print()`` still work, but cursor control, refresh handling, screen
  clearing, size detection, and the back buffer are disabled.

The terminal always owns a backend internally, but most applications can
treat it as an implementation detail and rely on the high-level API.

If you need to inject a custom backend or understand capability fallback
behavior, continue with :doc:`backend`.

Interface
=========

.. doxygenclass:: erbsland::cterm::UpdateSettings
    :members:

.. doxygenclass:: erbsland::cterm::Terminal
    :members:

.. doxygentypedef:: erbsland::cterm::TerminalPtr

.. doxygenenum:: erbsland::cterm::TerminalFlag

.. doxygenclass:: erbsland::cterm::TerminalFlags
    :members:
