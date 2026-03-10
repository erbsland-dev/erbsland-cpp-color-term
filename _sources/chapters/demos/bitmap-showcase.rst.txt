..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

****************
Bitmap Showcase
****************

``bitmap-showcase`` demonstrates how bitmap rendering behaves once a ``Bitmap`` is passed to ``Buffer::drawBitmap()``.
The demo uses a selector on the left and one large preview panel on the right, so the active variant stays readable
even on regular terminal sizes.

It is the quickest way to see how low-level bitmap data turns into terminal cells in practice.

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/bitmap-showcase

Use the left and right arrow keys to switch pages. Use the up and down arrow keys to move through the variants on the
current page.

Features Shown
==============

This demo highlights the complete bitmap rendering pipeline:

* ``HalfBlock``, ``FullBlock``, ``DoubleBlock``, and ``Char16Style`` rendering.
* Animated ``BitmapColorMode`` variants with ``ColorSequence``.
* Drawing a bitmap into a rectangle with alignment-dependent cropping.
* Custom ``fullBlock()``, ``doubleBlocks()``, and ``halfBlocks()`` replacements.
* ``CharCombinationStyle`` when a line-based bitmap is drawn over an existing frame grid.

Relevant Source Files
=====================

If you want to explore the implementation, start with:

:file:`demo/bitmap-showcase/src/BitmapShowcaseApp.cpp`

This file contains the page layout, the sample bitmaps, and the option combinations used to render the demo.
