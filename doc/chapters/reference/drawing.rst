..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*******
Drawing
*******

The drawing types define how frames, borders, filled areas, and
character combinations are rendered into writable buffers. They are the
main building blocks for boxes, panels, decorative borders, tiled fills,
and line-art style layouts.

Use this page when you want to choose a frame style, combine intersecting
lines cleanly, or configure reusable border and fill presets.

Usage
=====

Filling Panels and Drawing Simple Frames
----------------------------------------

:cpp:any:`WritableBuffer <erbsland::cterm::WritableBuffer>` offers convenient overloads to fill rectangles and
draw frames using predefined styles. The styling types on this page
define how those operations look.

.. code-block:: cpp

    const auto panel = Rectangle{2, 2, 30, 10};

    buffer.fill(panel, Char{" ", Color{fg::Inherited, bg::Blue}});
    buffer.drawFrame(panel, FrameStyle::LightWithRoundedCorners, Color{fg::BrightWhite, bg::Blue});
    buffer.drawText("Overview", panel.insetBy(Margins{1}), Alignment::TopLeft);

:cpp:any:`FrameStyle <erbsland::cterm::FrameStyle>` is the fastest way to pick one of the built-in Unicode box
styles for common panels, separators, and dialog windows.

Combining Frames Automatically
------------------------------

When multiple frames intersect, :cpp:any:`CharCombinationStyle <erbsland::cterm::CharCombinationStyle>` resolves the
overlapping characters so the resulting frame lines match correctly.

.. code-block:: cpp

    auto frameStyle = Char16Style::lightFrame();
    auto combination = CharCombinationStyle::commonBoxFrame();

    buffer.drawFrame(Rectangle{2, 2, 18, 8}, frameStyle, combination, Color{fg::BrightCyan, bg::Black});
    buffer.drawFrame(Rectangle{8, 5, 20, 8}, frameStyle, combination, Color{fg::BrightYellow, bg::Black});

This mechanism ensures that intersecting frames produce consistent box
drawing characters instead of overlapping glyphs. :cpp:any:`SimpleCharCombinationStyle <erbsland::cterm::SimpleCharCombinationStyle>`
and :cpp:any:`MatrixCombinationStyle <erbsland::cterm::MatrixCombinationStyle>` are the two public strategies for building
your own combination tables.

Repeating 9-Tile Styles
-----------------------

:cpp:any:`Tile9Style <erbsland::cterm::Tile9Style>` fills or frames a rectangle with a repeating 3x3 tile
layout. This is useful when the border needs different corner, edge,
and center tiles, but should still stretch cleanly to any size.

.. code-block:: cpp

    const auto panel = Rectangle{2, 2, 30, 10};
    const auto style = Tile9Style::create("╔═╗║ ║╚═╝");

    buffer.fill(panel, style, Color{fg::BrightBlack, bg::Black});
    buffer.drawFrame(panel, style, Color{fg::BrightCyan, bg::Black});

The 9-tile layout covers the normal 3x3 cases. If you also need special
tiles for rectangles that collapse to a single row, a single column, or
a single cell, construct the style with 16 characters instead.

Configuring Reusable Frame Presets
----------------------------------

When a frame style needs more than a simple color and border preset,
:cpp:any:`FrameDrawOptions <erbsland::cterm::FrameDrawOptions>` keeps the configuration together so you can reuse it
across multiple panels.

.. code-block:: cpp

    auto panelStyle = FrameDrawOptions{};
    panelStyle.setStyle(FrameStyle::LightWithRoundedCorners);
    panelStyle.setFillBlock(Char{" ", Color{fg::Inherited, bg::Black}});
    panelStyle.setFrameColorSequence(
        ColorSequence{
            Color{fg::BrightBlue, bg::Black},
            Color{fg::BrightCyan, bg::Black},
            Color{fg::BrightWhite, bg::Black},
        },
        FrameColorMode::ChasingBorderCW);
    panelStyle.setFillColor(Color{fg::Inherited, bg::Blue});

    buffer.drawFrame(Rectangle{2, 2, 28, 10}, panelStyle, animationCycle);
    buffer.drawFrame(Rectangle{34, 2, 28, 10}, panelStyle, animationCycle + 4);

This is the best fit when the same panel style is used repeatedly,
especially with animated border colors, optional fill blocks, and custom
combination rules.

Choosing Between Predefined and Custom Block Styles
---------------------------------------------------

Use :cpp:any:`FrameStyle <erbsland::cterm::FrameStyle>` when one of the built-in border presets is enough.
Use :cpp:any:`Char16Style <erbsland::cterm::Char16Style>` when each cell should depend on the surrounding
neighbors, for example for custom line-art sets or circuit-like
rendering. Use :cpp:any:`Tile9Style <erbsland::cterm::Tile9Style>` when corners, edges, and center tiles
should stretch independently.

.. figure:: /images/frame-weaver3.jpg
    :width: 100%

    The ``frame-weaver`` demo focuses on frame styles, combinations, and
    buffer-based rendering.

Interface
=========

.. doxygenclass:: erbsland::cterm::Char16Style
    :members:

.. doxygentypedef:: erbsland::cterm::Char16StylePtr

.. doxygenclass:: erbsland::cterm::CharCombinationStyle
    :members:

.. doxygentypedef:: erbsland::cterm::CharCombinationStylePtr

.. doxygenclass:: erbsland::cterm::SimpleCharCombinationStyle
    :members:

.. doxygenclass:: erbsland::cterm::MatrixCombinationStyle
    :members:

.. doxygenclass:: erbsland::cterm::Tile9Style
    :members:

.. doxygentypedef:: erbsland::cterm::Tile9StylePtr

.. doxygenenum:: erbsland::cterm::FrameStyle

.. doxygenenum:: erbsland::cterm::FrameColorMode

.. doxygenclass:: erbsland::cterm::FrameDrawOptions
    :members:
