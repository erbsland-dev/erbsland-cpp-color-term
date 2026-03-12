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
    *   - :doc:`Backend<terminal>`
        - The interface to the underlying platform.
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
    *   - :doc:`Buffer<buffer>`
        - A mutable 2D buffer storing characters and colors for rendering.
    *   - :doc:`BufferConstRefView<buffer-view>`
        - A non-owning view that exposes a sub-rectangle of another readable buffer.
    *   - :doc:`BufferView<buffer-view>`
        - A shared-ownership view that exposes a sub-rectangle of another readable buffer.
    *   - :doc:`BufferViewBase<buffer-view>`
        - The base class for all buffer views.
    *   - :doc:`Char<text>`
        - Represents a character string with foreground and background colors.
    *   - :doc:`Char16Style<drawing>`
        - Defines a style for drawing tiles.
    *   - :doc:`Char16StylePtr<drawing>`
        - Shared pointer for Char16Style
    *   - :doc:`CharCombinationStyle<drawing>`
        - A style how two characters are visually combined to a new one.
    *   - :doc:`CharCombinationStylePtr<drawing>`
        - Shared pointer for CharCombinationStyle
    *   - :doc:`Color<color>`
        - A foreground/background color pair for terminal rendering.
    *   - :doc:`ColorBase<color>`
        - Shared implementation for foreground and background color values.
    *   - :doc:`ColorBase::Value<color>`
        - Internal color identifiers used for ANSI conversion and parsing.
    *   - :doc:`ColorList<color>`
        - A list of terminal colors.
    *   - :doc:`ColorPart<color>`
        - A foreground or background color.
    *   - :doc:`ColorRole<color>`
        - The type color.
    *   - :doc:`ColorSequence<color>`
        - A configurable sequence of complete Color values with run-length style counts.
    *   - :doc:`CropEdges<buffer-view>`
        - Flags describing which sides and corners of a view are cropped.
    *   - :doc:`Direction<geometry>`
        - A direction in a 2D grid.
    *   - :doc:`Direction::Enum<geometry>`
        - The canonical enum values used by Direction.
    *   - :doc:`Font<font>`
        - A bitmap font used to render stylized terminal text.
    *   - :doc:`FontGlyph<font>`
        - A bitmap glyph that can be used by a terminal text font.
    *   - :doc:`FontPtr<font>`
        - Shared pointer to a terminal font.
    *   - :doc:`Foreground<color>`
        - The foreground color.
    *   - :doc:`FrameColorMode<drawing>`
        - The mode how animated colors are applied to frames.
    *   - :doc:`FrameDrawOptions<drawing>`
        - The options to draw a frame.
    *   - :doc:`FrameStyle<drawing>`
        - Various box styles
    *   - :doc:`Input<input>`
        - Reads key presses from the console depending on the configured mode.
    *   - :doc:`Input::Mode<input>`
        - Supported reading modes for the input backend.
    *   - :doc:`InputDefinition<input>`
        - Definition of a single key mapping for a specific input mode.
    *   - :doc:`InputDefinition::ForMode<input>`
        - The input modes to which a key definition can apply.
    *   - :doc:`InputDefinitionList<input>`
        - A list of input definitions.
    *   - :doc:`Key<input>`
        - A simple representation of a key press.
    *   - :doc:`Key::Type<input>`
        - Supported key kinds.
    *   - :doc:`Margins<geometry>`
        - Represents margins (top, right, bottom, left) around a rectangle.
    *   - :doc:`MatrixCombinationStyle<drawing>`
        - A class that combines characters through an indexed result matrix.
    *   - :doc:`MoveMode<terminal>`
        - The cursor move mode.
    *   - :doc:`ParagraphSpacing<text>`
        - The spacing between explicit newline-separated paragraphs.
    *   - :doc:`Position<geometry>`
        - Represents a 2D integer position or vector (x, y).
    *   - :doc:`ReadableBuffer<buffer>`
        - A readable buffer.
    *   - :doc:`Rectangle<geometry>`
        - Axis-aligned rectangle represented by a top-left position and size.
    *   - :doc:`SimpleCharCombinationStyle<drawing>`
        - A class to use a simple map to combine styles.
    *   - :doc:`Size<geometry>`
        - A non-negative 2D size (width × height).
    *   - :doc:`String<text>`
        - A terminal string represented as a sequence of Char values.
    *   - :doc:`StringLines<text>`
        - A sequence of wrapped terminal text lines.
    *   - :doc:`Terminal<terminal>`
        - High-level terminal interface for screen control, color output, and key input.
    *   - :doc:`Terminal::OutputMode<terminal>`
        - The output mode for the terminal.
    *   - :doc:`Terminal::RefreshMode<terminal>`
        - Screen clearing strategy used between rendered frames.
    *   - :doc:`TerminalPtr<terminal>`
        - Shared pointer to a terminal instance.
    *   - :doc:`Text<text>`
        - Describes a text block to render into a Buffer.
    *   - :doc:`TextAnimation<text>`
        - Supported text animation styles used by Buffer::renderText().
    *   - :doc:`Tile9Style<drawing>`
        - Defines a style for repeating a 3x3 tile pattern across a rectangle.
    *   - :doc:`Tile9StylePtr<drawing>`
        - Shared pointer for Tile9Style
    *   - :doc:`UpdateSettings<terminal>`
        - Settings controlling how Terminal::updateScreen() renders a buffer.
    *   - :doc:`WritableBuffer<buffer>`
        - A writable buffer.
    *   - :doc:`bg<color>`
        - Short alias for Background.
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
    *   -   :doc:`Buffer <buffer>`
        -   The buffer classes represent rendered terminal content in memory before it is written to the screen. They cover reusable frame storage, copying, resizing, and abstract readable and writable buffer workflows.
    *   -   :doc:`Buffer Views <buffer-view>`
        -   Buffer views expose a rectangular window onto a larger readable buffer. They are useful for scrollable panels, previews, clipped subviews, and custom crop indicators.
    *   -   :doc:`Drawing <drawing>`
        -   The drawing types define how borders, tiled fills, and character combinations are rendered into writable buffers. They are the main building blocks for frames, panels, and line-art style layouts.
    *   -   :doc:`Text <text>`
        -   The text classes provide utilities for constructing, formatting, and rendering colored text in terminal buffers. They support mixed styles, Unicode-aware layout, and animated text effects.
    *   -   :doc:`Terminal <terminal>`
        -   The terminal classes provide the runtime connection between rendered buffers and the real console. They manage screen lifecycle, refresh strategies, direct writes, update settings, and custom backends.

.. toctree::
    :hidden:
    :maxdepth: 1

    bitmap
    buffer
    buffer-view
    color
    drawing
    font
    geometry
    input
    terminal
    text
