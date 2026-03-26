..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*******************
Update Screen Modes
*******************

``update-screen-modes`` demonstrates how ``Terminal::updateScreen()`` behaves
when you toggle overwrite mode, line buffering, safe margins, and the smart
back buffer. It combines a small status dashboard with a sparse animation
field so the redraw cost and the visual behavior are easy to compare.

This is the best demo to study when you want to understand the practical
tradeoffs between clear-and-redraw rendering and smart overwrite updates.

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/update-screen-modes

Use ``O`` to toggle overwrite mode, ``L`` to toggle line buffering, ``B`` to
toggle the back buffer, ``S`` to toggle safe margins, and ``Q`` to quit.

ANSI Output Example
===================

The following capture shows the default overwrite-mode dashboard:

.. erbsland-ansi::
    :escape-char: ␛
    :theme: ela-term

    ␛[40m╭─────────────────────────────────────────────────────────────────────────────────────────────╮
    │                        ␛[97mScreen Update Demo | refresh mode: Overwrite␛[39m                         │
    │                                                                                             │
    │ ┌────────────────────────────┐  ┌─────────────────────────────────────────────────────────┐ │
    │ │␛[100m        Refresh Mode        ␛[40m│  │ ␛[96mSparse animation field | frame 00011 | line buffer: on␛[39m  │ │
    │ │␛[100m                            ␛[40m│  │                                                         │ │
    │ │␛[100m         ␛[93mOverwrite␛[39m          ␛[40m│  │   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m │ │
    │ │␛[97;100mLast Update␛[37m.......␛[97m 0.903 ms␛[39m ␛[40m│  │  ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m  │ │
    │ │␛[97;100mAverage Update␛[37m....␛[97m 0.569 ms␛[39m ␛[40m│  │ ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   │ │
    │ │␛[97;100mOverwrite Mode␛[37m....[␛[92m✓␛[37m]␛[39m       ␛[40m│  │    ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m    │ │
    │ │␛[97;100mLine Buffer␛[37m.......[␛[92m✓␛[37m]␛[39m       ␛[40m│  │   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m │ │
    │ │␛[97;100mBack Buffer␛[37m.......[␛[92m✓␛[37m]␛[39m       ␛[40m│  │ ␛[90m-----------␛[93m@␛[90m-------------------------------------------␛[39m │ │
    │ │␛[97;100mSafe Margins␛[37m......[␛[92m✓␛[37m]␛[39m       ␛[40m│  │ ␛[90m.␛[94m ␛[39m  ␛[90m.␛[39m   ␛[90m.␛[94m*␛[39m  ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   │ │
    │ │␛[100m                            ␛[40m│  │ ␛[97m ␛[94m  ␛[90m.␛[39m  ␛[94m *  ␛[39m ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m    │ │
    │ │␛[100m                            ␛[40m│  │ ␛[94m *␛[90m.␛[94m* *␛[90m.␛[94m ␛[39m  ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m │ │
    │ │␛[100m                            ␛[40m│  │  ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m   ␛[90m.␛[39m  │ │
    │ │␛[100m                            ␛[40m│  │                                                         │ │
    │ │␛[100m                            ␛[40m│  │                                                         │ │
    │ └────────────────────────────┘  └─────────────────────────────────────────────────────────┘ │
    │                                                                                             │
    │ ␛[96;100m[O]␛[97m overwrite ␛[93m[L]␛[97m line buf ␛[95m[B]␛[97m back buf ␛[94m[S]␛[97m margins ␛[92m[Q]␛[97m quit␛[39m                               ␛[40m │
    │                                                                                             │
    ╰─────────────────────────────────────────────────────────────────────────────────────────────╯␛[0m

Features Shown
==============

This demo focuses on runtime screen-update behavior:

* ``Terminal::RefreshMode::Overwrite`` versus ``Clear``.
* Line buffering, safe margins, and smart back-buffer updates.
* Timing feedback for repeated ``updateScreen()`` calls.
* A sparse animation field that makes redraw strategy visible.

Relevant Source Files
=====================

If you want to explore the implementation, start with:

:file:`demo/update-screen-modes/src/UpdateScreenModesApp.cpp`
:file:`demo/update-screen-modes/src/UpdateScreenModesState.hpp`

These files contain the interactive toggle handling and the small state
model used to compare refresh settings.
