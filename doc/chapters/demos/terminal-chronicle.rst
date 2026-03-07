..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

******************
Terminal Chronicle
******************

``terminal-chronicle`` is the most straightforward demo in the repository. It tells a short story using only
``Terminal::print()`` and ``Terminal::printLine()`` without relying on a back buffer or complex rendering logic.

This makes the example particularly useful if you want to generate structured and colorful terminal output for
command-line tools, status messages, or splash screens while keeping the implementation compact and easy to read.

.. figure:: /images/terminal-chronicle.jpg
    :width: 100%

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/terminal-chronicle

The program prints a short animated story directly to the terminal using simple formatted output calls.

Features Shown
==============

This demo highlights how to produce expressive terminal output with minimal infrastructure:

* Direct terminal output using ``Terminal::print()`` and ``Terminal::printLine()``.
* Readable mixed argument lists that combine text, colors, and formatting.
* Foreground and background color changes within a single output line.
* Structured console output without building or managing a ``Buffer``.
* Explicit terminal setup and restore operations.

Relevant Source Files
=====================

If you want to explore the implementation, start with:

:file:`demo/terminal-chronicle/src/StoryDemo.cpp`

This file contains the entire story sequence and demonstrates how formatted terminal
output can remain readable even when mixing text and styling instructions.

