..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*****
Input
*****

The input classes provide access to keyboard input from the terminal.
They are designed for interactive applications such as dashboards,
tools, and terminal games that need immediate key handling.

Usage
=====

Polling for Keys in a Redraw Loop
---------------------------------

For interactive applications, switch the input backend to key mode and
poll for input with a timeout. This allows your application to update
the screen regularly while still reacting to keyboard events.

.. code-block:: cpp

    using namespace std::chrono_literals;

    terminal.input().setMode(Input::Mode::Key);
    auto quitRequested = false;

    while (!quitRequested) {
        if (const auto key = terminal.input().read(90ms); key.valid()) {
            if (key == Key{Key::Character, U'q'}) {
                quitRequested = true;
            } else if (key == Key{Key::Left}) {
                // Move selection.
            }
        }
    }

Using a timeout keeps the redraw loop responsive without busy waiting.

Switching Between Key and Line Input
------------------------------------

:cpp:any:`Input::Mode <erbsland::cterm::Input::Mode>` controls whether the terminal reads raw key presses or
full lines of text. ``Mode::Key`` is the right choice for interactive
applications with a redraw loop, while ``Mode::ReadLine`` fits prompts,
configuration tools, and simple command-driven interfaces.

.. code-block:: cpp

    terminal.input().setMode(Input::Mode::ReadLine);
    terminal.print("Name: ");
    const auto name = terminal.input().readLine();

    terminal.input().setMode(Input::Mode::Key);
    terminal.printLine("Press any key to continue...");
    const auto key = terminal.input().read();

Switching modes on the same terminal makes it easy to combine
menu-driven screens with occasional free-form text input.

Describing Key Bindings
-----------------------

:cpp:any:`InputDefinition <erbsland::cterm::InputDefinition>` represents a key binding together with the input
mode it applies to. It is useful when describing configurable shortcuts
or when displaying the currently active bindings.

.. code-block:: cpp

    auto quitKey = InputDefinition{Key{Key::Character, U'q'}, InputDefinition::ForMode::Key};
    auto helpKey = InputDefinition{Key{Key::F1}, InputDefinition::ForMode::Key};

    std::cout << "Quit: " << quitKey.toDisplayText() << "\n";
    std::cout << "Help: " << helpKey.toString() << "\n";

The helper functions ``toDisplayText()`` and ``toString()`` make it easy
to present key bindings in help screens or configuration output.

Matching Decoded Key Types
--------------------------

When you only care about the general kind of a key event, inspect
:cpp:any:`Key::Type <erbsland::cterm::Key::Type>` instead of comparing against a long list of individual keys.
For character comparisons, prefer Unicode code points such as ``U'q'`` and
use :cpp:any:`Key::unicode() <erbsland::cterm::Key::unicode()>` or
:cpp:any:`Key::combined() <erbsland::cterm::Key::combined()>` instead of the deprecated ASCII accessor.

.. code-block:: cpp

    using namespace std::chrono_literals;

    if (const auto key = terminal.input().read(50ms); key.valid()) {
        if (key.type() == Key::Character || key.type() == Key::Combined) {
            terminal.printLine("Typed: ", String{key.combined()});
        } else if (key.type() == Key::Escape) {
            terminal.printLine("Leaving key mode.");
        }
    }

This is especially handy when the application wants to distinguish
between text entry, navigation keys, and control keys before it decides
how to handle the event.

.. figure:: /images/retro-plasma.jpg
    :width: 100%

    The animated demos use :cpp:any:`Input <erbsland::cterm::Input>` in key mode so screen redraws and
    keyboard handling remain responsive.

Interface
=========

.. doxygenclass:: erbsland::cterm::Input
    :members:

.. doxygenclass:: erbsland::cterm::InputDefinition
    :members:

.. doxygentypedef:: erbsland::cterm::InputDefinitionList

.. doxygenclass:: erbsland::cterm::Key
    :members:
