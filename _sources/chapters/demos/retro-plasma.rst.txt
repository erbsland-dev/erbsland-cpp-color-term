..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

************
Retro Plasma
************

``retro-plasma`` is a full-screen animation demo that renders a classic retro plasma effect directly in the terminal.
The application continuously updates the screen using an animated buffer and allows you to switch between multiple
color palettes while it is running.

Besides being visually fun, the demo also illustrates how to build responsive terminal applications that react to
keyboard input and terminal resize events.

.. figure:: /images/retro-plasma.jpg
    :width: 100%

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/retro-plasma

The animation runs continuously and can be controlled using the keyboard.

Features Shown
==============

This demo demonstrates several aspects of animated terminal rendering:

* Full-screen rendering using ``Buffer`` together with ``Terminal::updateScreen()``.
* A timed animation loop combined with keyboard input using ``Input::Mode::Key``.
* Palette-based color animation implemented with ``ColorSequence``.
* Resize-aware rendering using custom ``UpdateSettings`` with minimum size handling.
* Dynamic footer prompts composed from colored ``String`` elements.

Relevant Source Files
=====================

If you want to explore how the effect is implemented, start with:

* :file:`demo/retro-plasma/src/RetroPlasmaApp.cpp`
* :file:`demo/retro-plasma/src/PlasmaRenderer.cpp`

The application file contains the main event loop and terminal interaction logic,
while the renderer implements the plasma effect itself.

