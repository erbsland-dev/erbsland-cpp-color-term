..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

******
Bitmap
******

``Bitmap`` is a low-level helper class used to represent a grid of pixels.
It allows you to prepare boolean masks that can later be copied, combined,
or interpreted when rendering to a terminal buffer.

The class is primarily used internally for glyph rendering and similar
pixel-based operations, but it is also useful when implementing custom
rendering helpers or procedural shapes.

Usage
=====

Preparing a Small Pixel Mask
----------------------------

``Bitmap`` stores a two-dimensional grid of pixels. Each pixel can be
set individually and later queried or copied into another bitmap.

This makes it easy to construct reusable masks or small procedural
patterns.

.. code-block:: cpp

    auto bitmap = Bitmap{Size{8, 8}};
    bitmap.setPixel({1, 1}, true);
    bitmap.setPixel({2, 2}, true);
    bitmap.setPixel({3, 3}, true);

    auto copy = Bitmap{Size{8, 8}};
    copy.draw(Position{2, 1}, bitmap);

    if (copy.pixelQuad({1, 1}) != 0) {
        // Use the encoded 2x2 mask for custom rendering.
    }

The ``draw()`` function copies one bitmap into another at a given
position.

The ``pixelQuad()`` function reads a 2×2 group of pixels and encodes
them as a small integer value. This is useful when converting bitmap
patterns into terminal cell representations.

For most text rendering tasks, you typically work with higher-level
types such as ``FontGlyph`` or ``Font``. Use ``Bitmap`` directly when
you need precise control over individual pixels or when building
custom bitmap-based rendering logic.

Interface
=========

.. doxygenclass:: erbsland::cterm::Bitmap
    :members:

