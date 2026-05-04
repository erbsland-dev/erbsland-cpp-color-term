..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*******************
Update Screen Modes
*******************

``update-screen-modes`` demonstrates how ``Terminal::updateScreen()`` behaves when you toggle overwrite mode, line
buffering, safe margins, and the smart back buffer. It combines a small status dashboard with a sparse animation field
so the redraw cost and visual behavior are easy to compare.

Use This Demo When You Need...
==============================

* A support example for diagnosing ``updateScreen()`` behavior and performance tradeoffs.
* A visual comparison of overwrite versus clear refresh strategies.
* A reference for line buffering, safe margins, and smart back-buffer updates.

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/update-screen-modes

Use ``O`` to toggle overwrite mode, ``L`` to toggle line buffering, ``B`` to toggle the back buffer, ``S`` to toggle
safe margins, and ``Q`` to quit.

Captured Output (80x25)
=======================

.. include:: _captures/update-screen-modes.rstinc

Features Demonstrated
=====================

* ``Terminal::RefreshMode::Overwrite`` versus ``Clear``.
* Line buffering, safe margins, and smart back-buffer updates.
* Timing feedback for repeated ``updateScreen()`` calls.
* A sparse animation field that makes redraw strategy visible.

Related Demos
=============

* :doc:`Retro Plasma <retro-plasma>` for a more visual animation built on the same update path.
* :doc:`Key Input Demo <key-input-demo>` for another input-driven full-screen loop.
* :doc:`Frame Color Animations <frame-color-animations>` for a less diagnostic but still redraw-heavy full-screen demo.

Relevant Source Files
=====================

If you want to explore the implementation, start with :file:`demo/update-screen-modes/src/UpdateScreenModesApp.cpp`
and :file:`demo/update-screen-modes/src/UpdateScreenModesState.hpp`.
