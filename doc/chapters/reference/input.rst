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
            if (key == Key{Key::Character, 'q'}) {
                quitRequested = true;
            } else if (key == Key{Key::Left}) {
                // Move selection.
            }
        }
    }

Using a timeout keeps the redraw loop responsive without busy waiting.

Describing Key Bindings
-----------------------

``InputDefinition`` represents a key binding together with the input
mode it applies to. It is useful when describing configurable shortcuts
or when displaying the currently active bindings.

.. code-block:: cpp

    auto quitKey = InputDefinition{Key{Key::Character, 'q'}, InputDefinition::ForMode::Key};
    auto helpKey = InputDefinition{Key{Key::F1}, InputDefinition::ForMode::Key};

    std::cout << "Quit: " << quitKey.toDisplayText() << "\n";
    std::cout << "Help: " << helpKey.toString() << "\n";

The helper functions ``toDisplayText()`` and ``toString()`` make it easy
to present key bindings in help screens or configuration output.

.. figure:: /images/retro-plasma.jpg
    :width: 100%

    The animated demos use ``Input`` in key mode so screen redraws and
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

.. doxygenfunction:: erbsland::cterm::createInputForPlatform

