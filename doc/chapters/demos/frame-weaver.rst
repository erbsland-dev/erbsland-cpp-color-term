..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

************
Frame Weaver
************

``frame-weaver`` demonstrates the frame drawing system of the library. The application continuously places
randomly sized frames on the terminal so you can observe how the rendering engine resolves intersections and
combines different frame styles.

While the demo is running, frames with varying sizes, colors, and line styles are added dynamically. This makes
it easy to see how mixed line styles, custom frame tiles, and automatic intersection handling behave in practice.

.. figure:: /images/frame-weaver3.jpg
    :width: 100%

.. figure:: /images/frame-weaver1.jpg
    :width: 100%

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/frame-weaver

The application continuously updates the screen and allows you to interact with the animation using the keyboard.

Features Shown
==============

This demo highlights several important features of the frame rendering system:

* Use of predefined ``FrameStyle`` values as well as custom ``Char16Style`` tiles.
* Automatic resolution of frame intersections using
  ``CharCombinationStyle::commonBoxFrame()``.
* Animated screen updates with a continuously redrawn terminal buffer.
* Palette-driven color variations using ``ColorSequence``.

Relevant Source Files
=====================

If you want to explore the implementation, start with:

:file:`demo/frame-weaver/src/FrameWeaverApp.cpp`

This file contains the rendering loop, the frame placement logic, and the definition
of the custom frame style used by the demo.

