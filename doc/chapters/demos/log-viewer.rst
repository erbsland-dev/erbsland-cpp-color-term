..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

**********
Log Viewer
**********

``log-viewer`` demonstrates how :cpp:any:`CursorBuffer <erbsland::cterm::CursorBuffer>` and
:cpp:any:`BufferView <erbsland::cterm::BufferView>` work together in a live scrollback-style application. The demo
generates fictive web-server log messages, appends them to a growing cursor buffer, and shows only the currently
visible window in the center area of the terminal UI.

The viewport follows the newest lines by default, so the demo starts like a live log tail. You can then pan through
older lines with the cursor keys and jump back to the live bottom-left view with ``F``.

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/log-viewer

The demo keeps generating new entries until you quit it.

Features Shown
==============

This demo highlights several practical buffer and layout features:

* ``CursorBuffer`` configured with ``ExpandThenShift`` and a black-background ``fillChar`` for stable log history
  cells while the buffer grows and scrolls.
* ``BufferView`` used as a live viewport onto the bottom-left or a manually panned area of that history.
* ``printParagraph()`` with custom indent, wrap markers, and ellipsis handling for terminal-friendly log formatting.
* Independent coloring for timestamps, log levels, and continuation lines inside one scrolling text surface.
* Interactive panning with follow mode for a realistic log-tail experience.

Relevant Source Files
=====================

If you want to explore the implementation, start with:

:file:`demo/log-viewer/src/LogViewerApp.cpp`

This file contains the log generator, the cursor-buffer rendering logic, and the follow/pan viewport handling.
