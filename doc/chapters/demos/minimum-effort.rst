..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

**************
Minimum Effort
**************

``minimum-effort`` is a deliberately tiny full-screen program that still
shows a framed terminal layout, styled text, and a title. The demo exists to
demonstrate how little code is needed before the library already feels
productive.

It is a good starting point when you want a minimal example to copy into a
fresh project.

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/minimum-effort

ANSI Output Example
===================

The following capture shows the rendered frame and the embedded source code:

.. erbsland-ansi::
    :escape-char: ␛
    :theme: ela-term

    ␛[94;40m┌─────────────────────────────────────────────────────────────────────────────┐
    │                                                                             │
    │                                                                             │
    │                       ␛[93mDemo with Only 14 Lines of Code␛[94m                       │
    │                                                                             │
    │  ␛[96mauto main() -> int {␛[94m                                                       │
    │  ␛[96m    Terminal term{Size{80, 24}};␛[94m                                           │
    │  ␛[96m    term.initializeScreen();␛[94m                                               │
    │  ␛[96m    Buffer buffer{term.size()};␛[94m                                            │
    │  ␛[96m    buffer.fill(Char{' ', fg::BrightBlue, bg::Black});␛[94m                     │
    │  ␛[96m    buffer.drawText(Position{3, 5}, cThisCode);␛[94m                            │
    │  ␛[96m    buffer.drawFrame(buffer.rect(), FrameStyle::Light);␛[94m                    │
    │  ␛[96m    auto titleRect = buffer.rect().subRectangle(Anchor::Top, {0, 3}, {3, 2}│
    ␛[94m│  ␛[96m    buffer.drawText("Demo with Only 14 Lines of Code",␛[94m                     │
    │  ␛[96m        titleRect, Alignment::Center, Color{fg::BrightYellow});␛[94m            │
    │  ␛[96m    term.write(buffer);␛[94m                                                    │
    │  ␛[96m    term.restoreScreen();␛[94m                                                  │
    │  ␛[96m    return 0;␛[94m                                                              │
    │  ␛[96m}␛[94m                                                                          │
    │                                                                             │
    │                                                                             │
    │                                                                             │
    └─────────────────────────────────────────────────────────────────────────────┘␛[0m

Features Shown
==============

This demo is intentionally compact, but it still touches the core workflow:

* ``Terminal`` setup and teardown.
* One persistent ``Buffer`` with a fill color.
* ``drawFrame()`` and ``drawText()`` for a simple full-screen composition.
* A self-documenting code sample rendered inside the frame.

Relevant Source Files
=====================

If you want to explore the implementation, start with:

:file:`demo/minimum-effort/src/main.cpp`

The full demo lives in that one file.
