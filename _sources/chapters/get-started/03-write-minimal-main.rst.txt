..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*****************************
Write a Minimal Main Function
*****************************

Before introducing buffers and animations, let’s start with the smallest useful
program. In this step you will initialize the terminal, print a few colored
lines, flush the output, and restore the screen state before exiting.

This example shows how simple terminal output can already produce readable and
structured results.

#. Create and open the source file:

   .. code-block:: console

        $ nano signal-board/src/main.cpp

#. Add the following code:

   .. code-block:: cpp
      :caption: <project>/signal-board/src/main.cpp

        #include <erbsland/cterm/Terminal.hpp>

        using namespace erbsland::cterm;

        auto main() -> int {
            auto terminal = Terminal{Size{80, 25}};
            terminal.initializeScreen();

            terminal.printLine(
                bg::Blue,
                fg::BrightWhite,
                " Signal Board ",
                Color::reset(),
                " ",
                fg::BrightBlack,
                "Direct output with readable color arguments.");
            terminal.lineBreak();
            terminal.printLine(fg::BrightGreen, "Status", fg::BrightBlack, ": online");
            terminal.printLine(fg::BrightCyan, "Mode", fg::BrightBlack, ": tutorial step 1");

            terminal.setDefaultColor();
            terminal.flush();
            terminal.restoreScreen();
            return 0;
        }

What This Example Shows
=======================

Even this minimal program already demonstrates several useful parts of the API:

* ``Terminal`` handles screen initialization and restoration.
* ``printLine()`` accepts text, full colors, foreground colors, and background
  colors in a single call.
* ``fg::...`` and ``bg::...`` provide a compact and readable way to apply colors.
* ``Color::reset()`` resets the colors in the middle of a line.

For small tools, status messages, or diagnostic output, this direct printing
style is often all you need.

.. button-ref:: 04-compile-and-run
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Build and Run the First Version →
