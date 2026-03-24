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
        - A view that uses a reference to the content.
    *   - :doc:`BufferView<buffer-view>`
        - A view that uses a shared pointer to the content.
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
        - Flags for crop edges and corners.
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
    *   - :doc:`ParagraphBackgroundMode<paragraph-options>`
        - How paragraph rendering extends the background color beyond the visible text.
    *   - :doc:`ParagraphOnError<paragraph-options>`
        - The fallback to use when paragraph layout becomes impossible.
    *   - :doc:`ParagraphOptions<paragraph-options>`
        - Options that control paragraph wrapping, indentation, tab handling, and fallback behavior.
    *   - :doc:`ParagraphSpacing<paragraph-options>`
        - The spacing between explicit newline-separated paragraphs.
    *   - :doc:`Position<geometry>`
        - Represents a 2D integer position or vector (x, y).
    *   - :doc:`ReadableBuffer<buffer>`
        - A readable buffer.
    *   - :doc:`RemappedBuffer<buffer>`
        - A buffer optimized for row and column remapping operations.
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
    *   - :doc:`TabOverflowBehavior<paragraph-options>`
        - The handling for tabs whose configured tab stop does not advance the current line.
    *   - :doc:`Terminal<terminal>`
        - High-level terminal interface for screen control, color output, and key input.
    *   - :doc:`TerminalFlag<terminal>`
        - A terminal flag.
    *   - :doc:`TerminalFlags<terminal>`
        - A set of terminal flags.
    *   - :doc:`Terminal::OutputMode<terminal>`
        - The output mode for the terminal.
    *   - :doc:`Terminal::RefreshMode<terminal>`
        - Screen clearing strategy used between rendered frames.
    *   - :doc:`TerminalPtr<terminal>`
        - Shared pointer to a terminal instance.
    *   - :doc:`Text<text>`
        - Describes a text block to render into a Buffer.
    *   - :doc:`TextOptions<text>`
        - Options for text rendering.
    *   - :doc:`TextAnimation<text>`
        - Supported text animation styles used by Buffer::renderText().
    *   - :doc:`Tile9Style<drawing>`
        - Defines a style for repeating a 3x3 tile pattern across a rectangle.
    *   - :doc:`Tile9StylePtr<drawing>`
        - Shared pointer for Tile9Style
    *   - :doc:`UpdateSettings<terminal>`
        - Settings controlling how Terminal::updateScreen() renders a buffer.
    *   - :doc:`WritableBuffer<buffer>`
        - Abstract writable terminal buffer interface.
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
    *   -   :doc:`Buffer Views <buffer-view>`
        -   Buffer views expose a rectangular window onto a larger readable buffer. They are useful whenever the logical content is bigger than the visible terminal area, for example in scrollable panels, minimaps, editors, and diagnostic tools.
    *   -   :doc:`Buffer <buffer>`
        -   The buffer classes represent rendered terminal content in memory before it is written to the screen. ReadableBuffer provides the inspection API, WritableBuffer adds mutation and drawing operations, and Buffer is the concrete 2D storage type used in most applications.
    *   -   :doc:`Color <color>`
        -   The color classes provide access to the standard 16 terminal colors and their combinations. They are used to style terminal output, buffer cells, and text elements throughout the library.
    *   -   :doc:`Drawing <drawing>`
        -   The drawing types define how frames, borders, filled areas, and character combinations are rendered into writable buffers. They are the main building blocks for boxes, panels, decorative borders, tiled fills, and line-art style layouts.
    *   -   :doc:`Font <font>`
        -   The font classes allow you to render large bitmap-based text directly in the terminal. Fonts are primarily used for titles, banners, and other decorative elements in terminal interfaces.
    *   -   :doc:`Geometry <geometry>`
        -   The geometry classes provide the building blocks for positioning and layout inside a terminal buffer. They describe sizes, positions, rectangles, and directions, and allow you to derive new regions from existing ones.
    *   -   :doc:`Input <input>`
        -   The input classes provide access to keyboard input from the terminal. They are designed for interactive applications such as dashboards, tools, and terminal games that need immediate key handling.
    *   -   :doc:`Paragraph Options <paragraph-options>`
        -   ParagraphOptions collects the low-level layout rules for wrapped terminal paragraphs. The same object model is used by Terminal::printParagraph(), TextOptions, and Text, so one paragraph configuration can be reused for direct terminal output and buffer-based text rendering.
    *   -   :doc:`Terminal <terminal>`
        -   The terminal classes provide the runtime connection between your rendered content and the real console. Terminal manages the application lifecycle, refresh strategy, cursor control, direct text writes, and the platform backend that ultimately emits text and control sequences.
    *   -   :doc:`Text <text>`
        -   The text classes provide utilities for constructing, formatting, and rendering colored text in terminal buffers. They support mixed styles, Unicode-aware layout, and animated text effects.

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
    paragraph-options
    terminal
    text
