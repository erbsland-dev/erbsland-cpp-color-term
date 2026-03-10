..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*******************************
Compile and Run the Application
*******************************

Build the Tutorial Project
==========================

Create an out-of-source build directory and configure the project:

.. code-block:: console

    $ mkdir cmake-build
    $ cmake -B cmake-build -G Ninja .
    ...

Then compile the executable:

.. code-block:: console

    $ cmake --build cmake-build
    ...
    [100%] Built target signal-board

Run the Program
===============

Start the application from the project directory:

.. code-block:: console

    $ ./cmake-build/signal-board/signal-board

You should now see a few colored lines printed to the terminal.
If the program runs successfully, your project structure, CMake configuration,
and basic terminal output pipeline are all working as expected.

Why We Move to a Buffer Next
============================

Direct terminal output is perfect for logs, prompts, and small command-line
tools. However, when an application behaves more like a terminal UI or
animated screen, managing the output through a back buffer becomes much
simpler.

Using a buffer provides several advantages:

* you can redraw the entire screen from a clean state
* layout logic becomes independent from the terminal cursor position
* complex elements such as frames, wrapped text, and animations become easier to manage

In the next step, we will replace the direct print calls with a
``Buffer``-based rendering approach.

.. button-ref:: 05-render-with-buffer
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Render the Screen from a Buffer →

