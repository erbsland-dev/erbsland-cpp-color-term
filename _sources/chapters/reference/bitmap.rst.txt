..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

******
Bitmap
******

``Bitmap`` is the low-level pixel container used by the library whenever a
boolean mask needs to be rendered, copied, or transformed. A bitmap stores
only on/off pixels; the actual terminal representation is chosen later by
``Buffer::drawBitmap()`` and ``BitmapDrawOptions``.

The class is primarily used internally for glyph rendering and similar
pixel-based operations, but it is also useful when implementing custom
rendering helpers, small icons, line maps, or procedural effects.

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

Building Bitmaps from Text Patterns
-----------------------------------

For small icons and handcrafted masks, ``Bitmap::fromPattern()`` is
usually the fastest way to define the pixels. Each input string becomes
one bitmap row. Dots and spaces are treated as cleared pixels; every
other character sets a pixel.

.. code-block:: cpp

    const auto rocket = Bitmap::fromPattern({
        ".....#.....",
        "....###....",
        "...#####...",
        "....###....",
        "...#####...",
        "..#######..",
        "...#####...",
        "..#######..",
        ".#########.",
        ".....#.....",
        ".....#.....",
    });

Rows may have different lengths. ``fromPattern()`` uses the longest row
as the bitmap width and pads shorter rows with cleared pixels on the
right.

Rendering a Bitmap to a Buffer
------------------------------

Once a bitmap is prepared, ``Buffer::drawBitmap()`` converts it into
terminal cells. ``BitmapDrawOptions`` controls the scale mode, the block
characters that are used, the base colors, and the optional line style
for neighbor-aware rendering.

.. code-block:: cpp

    auto bitmap = Bitmap{Size{8, 8}};
    bitmap.setPixel({1, 1}, true);
    bitmap.setPixel({2, 2}, true);
    bitmap.setPixel({3, 3}, true);
    bitmap.setPixel({4, 4}, true);

    auto options = BitmapDrawOptions{};
    options.setColorSequence(
        ColorSequence{
            Color{fg::BrightBlue, bg::Black},
            Color{fg::BrightCyan, bg::Black},
            Color{fg::BrightMagenta, bg::Black},
        },
        BitmapColorMode::ForwardDiagonalStripes);

    buffer.drawBitmap(bitmap, Rectangle{2, 2, 20, 8}, Alignment::Center, options, animationCycle);

The rectangle overload aligns the *rendered* bitmap inside the target
rectangle. If the rendered bitmap is larger, it is cropped according to
the chosen alignment.

Choosing the Scale Mode
-----------------------

``BitmapScaleMode`` decides how pixels are grouped into terminal cells:

* ``HalfBlock`` uses one character for each 2x2 pixel group. This is the
  default mode and allows colored empty cells because the zero entry in
  ``halfBlocks()`` is rendered too.
* ``FullBlock`` uses one character per set bitmap pixel.
* ``DoubleBlock`` also uses one character per set bitmap pixel, but
  doubles the width in terminal cells to compensate for rectangular
  terminal proportions.

If ``char16Style()`` is set, it overrides the scale mode completely.
Each set pixel becomes one terminal cell, and the chosen ``Char16Style``
selects the block from the east, south, west, and north neighbors of the
pixel. This is useful for circuit maps, line art, and bitmap-derived
frame structures.

Color and Animation Rules
-------------------------

The base color comes from ``BitmapDrawOptions::color()``. A single-color
sequence paints the whole bitmap uniformly, while the stripe modes look up
the sequence by rendered bitmap coordinates plus ``animationCycle`` and
``colorAnimationOffset()``.

If the sequence is empty, the color is inherited from the buffer below.
After the base color is selected, the color stored in ``fullBlock()``,
``doubleBlocks()``, ``halfBlocks()``, or the chosen ``Char16Style`` block
is overlaid on top of it.

To see all combinations in action, run the ``bitmap-showcase`` demo.

Interface
=========

.. doxygenclass:: erbsland::cterm::Bitmap
    :members:

.. doxygenclass:: erbsland::cterm::BitmapDrawOptions
    :members:

.. doxygenenum:: erbsland::cterm::BitmapColorMode

.. doxygenenum:: erbsland::cterm::BitmapScaleMode
