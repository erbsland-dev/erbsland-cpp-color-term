..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*********
Reference
*********

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

Alphabetical
============

.. list-table::
    :header-rows: 1
    :widths: 30 70
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
    *   - :doc:`BlockStringLines<text>`
        - A sequence of wrapped terminal text lines.
    *   - :doc:`Buffer<output>`
        - A mutable 2D buffer storing characters and colors for rendering.
    *   - :doc:`Char<text>`
        - Represents a character string with foreground and background colors.
    *   - :doc:`Char16Style<output>`
        - Defines a style for drawing tiles.
    *   - :doc:`Char16StylePtr<output>`
        - Shared pointer to a ``Char16Style`` instance.
    *   - :doc:`CharCombinationStyle<output>`
        - A style how two characters are visually combined to a new one.
    *   - :doc:`CharCombinationStylePtr<output>`
        - Shared pointer to a ``CharCombinationStyle`` instance.
    *   - :doc:`Color<color>`
        - A foreground/background color pair for terminal rendering.
    *   - :doc:`ColorBase<color>`
        - Shared implementation for foreground and background color values.
    *   - :doc:`ColorList<color>`
        - A list of terminal colors.
    *   - :doc:`ColorPart<color>`
        - A foreground or background color.
    *   - :doc:`ColorRole<color>`
        - Distinguishes between foreground and background color parts.
    *   - :doc:`ColorSequence<color>`
        - A configurable sequence of complete colors with run-length style counts.
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
        - Predefined styles for drawing frames and boxes.
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
    *   - :doc:`UpdateSettings<output>`
        - Settings controlling how ``Terminal::updateScreen()`` renders a buffer.
    *   - :doc:`bg<color>`
        - Short alias for ``Background``.
    *   - :doc:`createInputForPlatform<input>`
        - Create the platform-specific input backend for the current process.
    *   - :doc:`fg<color>`
        - Short alias for ``Foreground``.

By Topic
========

.. list-table::
    :header-rows: 1
    :widths: 30 70
    :width: 100%

    *   -   Topic
        -   Description
    *   -   :doc:`Bitmap <bitmap>`
        -   The low-level bitmap class allows to prepare masks for rendering various shapes on an output buffer.
    *   -   :doc:`Color <color>`
        -   Work with the basic 16 colors using the color classes.
    *   -   :doc:`Font <font>`
        -   With these classes, you can define and render large fonts on a terminal.
    *   -   :doc:`Geometry <geometry>`
        -   The geometry classes provide a simple interface to perform various operations on an output buffer.
    *   -   :doc:`Input <input>`
        -   Use the input interface to get interactive keyboard input from the terminal.
    *   -   :doc:`Output <output>`
        -   For displaying text, you can prepare a buffer or directly write to the terminal.
    *   -   :doc:`Text <text>`
        -   The text classes allow preparing, formatting and animating colored text.
