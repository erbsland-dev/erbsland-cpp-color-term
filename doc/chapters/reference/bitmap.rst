..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

******
Bitmap
******

:cpp:any:`Bitmap <erbsland::cterm::Bitmap>` is the low-level pixel container used throughout the library whenever a
boolean mask needs to be rendered, copied, or transformed.

A bitmap stores only on/off pixels. The actual terminal representation is
chosen later by :cpp:any:`Buffer::drawBitmap() <erbsland::cterm::Buffer::drawBitmap()>`
together with :cpp:any:`BitmapDrawOptions <erbsland::cterm::BitmapDrawOptions>`.

You will mostly encounter this class indirectly during glyph rendering and
other pixel-based operations. That said, it is also a great tool when you
want to build your own rendering helpers—such as icons, line maps, or small
procedural effects.

Usage
=====

Preparing a Small Pixel Mask
----------------------------

A :cpp:any:`Bitmap <erbsland::cterm::Bitmap>` represents a two-dimensional grid of pixels. You can set or query each
pixel individually and copy regions between bitmaps.

This makes it straightforward to construct reusable masks or generate
small procedural patterns.

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

Use :cpp:any:`draw() <erbsland::cterm::Bitmap::draw()>` to copy one bitmap into another at a given position.

The :cpp:any:`pixelQuad() <erbsland::cterm::Bitmap::pixelQuad()>` function reads a 2×2 group of pixels and encodes
them as a small integer. This is especially useful when translating bitmap
data into terminal cell representations (for example, when working with
half-block characters).

Building Bitmaps from Text Patterns
-----------------------------------

For small icons or handcrafted masks, :cpp:any:`Bitmap::fromPattern() <erbsland::cterm::Bitmap::fromPattern()>`
is usually the fastest and most readable way to define pixels.

Each input string represents one row. Dots and spaces are treated as
cleared pixels, while every other character sets a pixel.

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

Rows may have different lengths. :cpp:any:`fromPattern() <erbsland::cterm::Bitmap::fromPattern()>` automatically uses
the longest row as the bitmap width and pads shorter rows with cleared
pixels on the right.

Rendering a Bitmap to a Buffer
------------------------------

Once your bitmap is prepared, you can render it using
:cpp:any:`Buffer::drawBitmap() <erbsland::cterm::Buffer::drawBitmap()>`.

The :cpp:any:`BitmapDrawOptions <erbsland::cterm::BitmapDrawOptions>` control how pixels are translated into terminal
cells—this includes scaling, block characters, colors, and optional
neighbor-aware line styles.

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

When using the rectangle overload, the *rendered* bitmap is aligned inside
the target rectangle. If it exceeds the available space, it is cropped
according to the selected alignment.

Choosing the Scale Mode
-----------------------

:cpp:any:`BitmapScaleMode <erbsland::cterm::BitmapScaleMode>` determines how bitmap pixels are mapped to terminal cells:

* :cpp:any:`HalfBlock <erbsland::cterm::BitmapScaleMode::HalfBlock>`
  Groups pixels into 2×2 blocks per character. This is the default mode and
  allows even empty cells to be colored, because the zero entry in
  :cpp:any:`halfBlocks() <erbsland::cterm::BitmapDrawOptions::halfBlocks()>` is rendered.

* :cpp:any:`FullBlock <erbsland::cterm::BitmapScaleMode::FullBlock>`
  Uses one character per set bitmap pixel.

* :cpp:any:`DoubleBlock <erbsland::cterm::BitmapScaleMode::DoubleBlock>`
  Also uses one character per set pixel, but doubles the width in terminal
  cells to better match typical terminal aspect ratios.

If you enable :cpp:any:`char16Style() <erbsland::cterm::BitmapDrawOptions::char16Style()>`,
it overrides the scale mode entirely. In this mode, each set pixel becomes
one terminal cell, and the selected :cpp:any:`Char16Style <erbsland::cterm::Char16Style>`
determines the character based on the pixel’s east, south, west, and north
neighbors.

This is particularly useful for rendering circuit diagrams, line art, or
bitmap-derived frame structures.

Color and Animation Rules
-------------------------

The base color is defined by :cpp:any:`BitmapDrawOptions::color() <erbsland::cterm::BitmapDrawOptions::color()>`.

If you provide a color sequence, it is applied depending on the selected
:cpp:any:`BitmapColorMode <erbsland::cterm::BitmapColorMode>`. Stripe modes
derive their color from the rendered bitmap coordinates, combined with
``animationCycle`` and
:cpp:any:`colorAnimationOffset() <erbsland::cterm::BitmapDrawOptions::colorAnimationOffset()>`.

If the sequence is empty, the bitmap inherits the color from the underlying buffer.

After selecting the base color, the color defined in
:cpp:any:`fullBlock() <erbsland::cterm::BitmapDrawOptions::fullBlock()>`,
:cpp:any:`doubleBlocks() <erbsland::cterm::BitmapDrawOptions::doubleBlocks()>`,
:cpp:any:`halfBlocks() <erbsland::cterm::BitmapDrawOptions::halfBlocks()>`,
or the chosen :cpp:any:`Char16Style <erbsland::cterm::Char16Style>` is applied on top.

To explore all combinations interactively, run the ``bitmap-showcase`` demo.

Interface
=========

.. doxygenclass:: erbsland::cterm::Bitmap
    :members:

.. doxygenclass:: erbsland::cterm::BitmapDrawOptions
    :members:

.. doxygenenum:: erbsland::cterm::BitmapColorMode

.. doxygenenum:: erbsland::cterm::BitmapScaleMode
