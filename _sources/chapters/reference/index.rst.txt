..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*********
Reference
*********

.. rubric:: Alphabetical

.. list-table::
    :header-rows: 1
    :widths: 40 60
    :width: 100%

    *   - Name
        - Description
    *   - :doc:`Alignment<geometry>`
        - Alignment of text or graphics in a box.
    *   - :doc:`Anchor<geometry>`
        - Anchor describes a location inside a rectangular area using a combination of a vertical and a horizontal component.
    *   - :doc:`Background<color>`
        - The background color.
    *   - :doc:`Bitmap<bitmap>`
        - A mutable bitmap storing boolean pixels in row-major order.
    *   - :doc:`BitmapColorMode<bitmap>`
        - The mode how color is applied to the bitmap.
    *   - :doc:`BitmapDrawOptions<bitmap>`
        - The options to draw a bitmap.
    *   - :doc:`BitmapScaleMode<bitmap>`
        - The mode how the bitmap is scaled.
    *   - :doc:`BlockStringLines<text>`
        - A sequence of wrapped terminal text lines.
    *   - :doc:`Buffer<output>`
        - A mutable 2D buffer storing characters and colors for rendering.
    *   - :doc:`Char<text>`
        - Represents a character string with foreground and background colors.
    *   - :doc:`Char16Style<output>`
        - Defines a style for drawing tiles.
    *   - :doc:`Char16StylePtr<output>`
        - Shared pointer for Char16Style
    *   - :doc:`CharCombinationStyle<output>`
        - A style how two characters are visually combined to a new one.
    *   - :doc:`CharCombinationStylePtr<output>`
        - Shared pointer for CharCombinationStyle
    *   - :doc:`Color<color>`
        - A foreground/background color pair for terminal rendering.
    *   - :doc:`ColorBase<color>`
        - Shared implementation for foreground and background color values.
    *   - :doc:`ColorList<color>`
        - A list of terminal colors.
    *   - :doc:`ColorPart<color>`
        - A foreground or background color.
    *   - :doc:`ColorRole<color>`
        - The type color.
    *   - :doc:`ColorSequence<color>`
        - A configurable sequence of complete Color values with run-length style counts.
    *   - :doc:`Direction<geometry>`
        - A direction in a 2D grid.
    *   - :doc:`Font<font>`
        - A bitmap font used to render stylized terminal text.
    *   - :doc:`FontGlyph<font>`
        - A bitmap glyph that can be used by a terminal text font.
    *   - :doc:`FontPtr<font>`
        - Shared pointer to a terminal font.
    *   - :doc:`Foreground<color>`
        - The foreground color.
    *   - :doc:`FrameStyle<output>`
        - Various box styles
    *   - :doc:`Input<input>`
        - Reads key presses from the console depending on the configured mode.
    *   - :doc:`InputDefinition<input>`
        - Definition of a single key mapping for a specific input mode.
    *   - :doc:`InputDefinitionList<input>`
        - A list of input definitions.
    *   - :doc:`Key<input>`
        - A simple representation of a key press.
    *   - :doc:`Margins<geometry>`
        - Represents margins (top, right, bottom, left) around a rectangle.
    *   - :doc:`MatrixCombinationStyle<output>`
        - A class that combines characters through an indexed result matrix.
    *   - :doc:`Position<geometry>`
        - Represents a 2D integer position or vector (x, y).
    *   - :doc:`Rectangle<geometry>`
        - Axis-aligned rectangle represented by a top-left position and size.
    *   - :doc:`SimpleCharCombinationStyle<output>`
        - A class to use a simple map to combine styles.
    *   - :doc:`Size<geometry>`
        - A non-negative 2D size (width × height).
    *   - :doc:`String<text>`
        - A terminal string represented as a sequence of Char values.
    *   - :doc:`Terminal<output>`
        - High-level terminal interface for screen control, color output, and key input.
    *   - :doc:`TerminalPtr<output>`
        - Shared pointer to a terminal instance.
    *   - :doc:`Text<text>`
        - Describes a text block to render into a Buffer.
    *   - :doc:`TextAnimation<text>`
        - Supported text animation styles used by Buffer::renderText().
    *   - :doc:`Tile9Style<output>`
        - Defines a style for repeating a 3x3 tile pattern across a rectangle.
    *   - :doc:`Tile9StylePtr<output>`
        - Shared pointer for Tile9Style
    *   - :doc:`UpdateSettings<output>`
        - Settings controlling how Terminal::updateScreen() renders a buffer.
    *   - :doc:`bg<color>`
        - Short alias for Background.
    *   - :doc:`createInputForPlatform<input>`
        -
    *   - :doc:`fg<color>`
        - Short alias for Foreground.

.. rubric:: By Topic

.. list-table::
    :header-rows: 1
    :widths: 30 70
    :width: 100%

    *   -   Topic
        -   Description
    *   -   :doc:`Bitmap <bitmap>`
        -   Bitmap is the low-level pixel container used by the library whenever a boolean mask needs to be rendered, copied, or transformed. A bitmap stores only on/off pixels; the actual terminal representation is chosen later by Buffer::drawBitmap() and BitmapDrawOptions.
    *   -   :doc:`Color <color>`
        -   The color classes provide access to the standard 16 terminal colors and their combinations. They are used to style terminal output, buffer cells, and text elements throughout the library.
    *   -   :doc:`Font <font>`
        -   The font classes allow you to render large bitmap-based text directly in the terminal. Fonts are primarily used for titles, banners, and other decorative elements in terminal interfaces.
    *   -   :doc:`Geometry <geometry>`
        -   The geometry classes provide the building blocks for positioning and layout inside a terminal buffer. They describe sizes, positions, rectangles, and directions, and allow you to derive new regions from existing ones.
    *   -   :doc:`Input <input>`
        -   The input classes provide access to keyboard input from the terminal. They are designed for interactive applications such as dashboards, tools, and terminal games that need immediate key handling.
    *   -   :doc:`Output <output>`
        -   The output classes provide the core rendering workflow for terminal applications. You can either write text directly to the terminal or prepare a full-screen buffer and render it in a single update.
    *   -   :doc:`Text <text>`
        -   The text classes provide utilities for constructing, formatting, and rendering colored text in terminal buffers. They support mixed styles, Unicode-aware layout, and animated text effects.

.. toctree::
    :hidden:
    :maxdepth: 1

    bitmap
    color
    font
    geometry
    input
    output
    text
