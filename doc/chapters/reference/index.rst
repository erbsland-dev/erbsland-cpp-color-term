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
    *   - :doc:`BufferDrawOptions<buffer>`
        - Options for drawing a buffer onto another buffer.
    *   - :doc:`BufferResizeMode<buffer>`
        - Describes how a buffer resize should handle existing content.
    *   - :doc:`Backend<backend>`
        - The interface to the underlying platform.
    *   - :doc:`BackendPtr<backend>`
        - Shared pointer to a backend instance.
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
    *   - :doc:`CharAttributes<text>`
        - Represents optional ANSI text attributes such as bold, underline, or strikethrough.
    *   - :doc:`CharStyle<text>`
        - Combines a Color value with CharAttributes into one reusable text style.
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
    *   - :doc:`Coordinate<geometry>`
        - Signed integer coordinate value used by the geometry classes.
    *   - :doc:`CursorBuffer<buffer>`
        - A remapped buffer with cursor-style writing, wrapping, and scrollback behavior.
    *   - :doc:`CursorBuffer::OverflowMode<buffer>`
        - Controls whether cursor writes wrap, scroll, or grow the buffer at the bottom edge.
    *   - :doc:`CursorWriter<cursor-output>`
        - The shared interface for buffers and terminals that support cursor-based output.
    *   - :doc:`Direction<geometry>`
        - A direction in a 2D grid.
    *   - :doc:`Direction::Enum<geometry>`
        - The canonical enum values used by Direction.
    *   - :doc:`EncodingErrors<text>`
        - How to handle encoding or Unicode normalization errors.
    *   - :doc:`FastCharSet<paragraph-options>`
        - Shared immutable character set with optimized lookup for common separator patterns.
    *   - :doc:`FastCharSet::FastTrack<paragraph-options>`
        - Fast-track mode for common separator patterns.
    *   - :doc:`FastCharSetPtr<paragraph-options>`
        - Shared pointer for FastCharSet.
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
    *   - :doc:`HtmlRenderer<rich-text>`
        - A renderer for HTML content to strings or cursor writer targets.
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
    *   - :doc:`Orientation<geometry>`
        - Represents the orientation of a layout or a direction.
    *   - :doc:`Key<input>`
        - A simple representation of a key press.
    *   - :doc:`Key::Type<input>`
        - Supported key kinds.
    *   - :doc:`Margins<geometry>`
        - Represents margins (top, right, bottom, left) around a rectangle.
    *   - :doc:`MatrixCombinationStyle<drawing>`
        - A class that combines characters through an indexed result matrix.
    *   - :doc:`MoveMode<cursor-output>`
        - The cursor move mode.
    *   - :doc:`ParagraphBackgroundMode<paragraph-options>`
        - How paragraph rendering extends the background color beyond the visible text.
    *   - :doc:`ParagraphIndents<paragraph-options>`
        - Shared indentation and margin settings for paragraph-like text rendering.
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
    *   - :doc:`Text<drawing-text>`
        - Describes a text block to render into a Buffer.
    *   - :doc:`TextOptions<drawing-text>`
        - Options for text rendering.
    *   - :doc:`TextAnimation<drawing-text>`
        - Supported text animation styles used by Buffer::drawText().
    *   - :doc:`TerminalSession<terminal>`
        - A scoped terminal session.
    *   - :doc:`Tile9Style<drawing>`
        - Defines a style for repeating a 3x3 tile pattern across a rectangle.
    *   - :doc:`Tile9StylePtr<drawing>`
        - Shared pointer for Tile9Style
    *   - :doc:`text::Style<rich-text>`
        - A style for text formatting.
    *   - :doc:`text::Style::Selector<rich-text>`
        - A selector that targets semantic roles, levels, list kinds, and style tokens.
    *   - :doc:`text::StyleRule<rich-text>`
        - One resolved or stored rich-text style rule.
    *   - :doc:`text::StyleRule::Marker<rich-text>`
        - The visible marker used for bullet and numbered list items.
    *   - :doc:`text::TextNode<rich-text>`
        - A node in the text tree.
    *   - :doc:`text::TextNode::Type<rich-text>`
        - The node type.
    *   - :doc:`ui::Application<ui>`
        - UI Application instance for event-driven UI applications.
    *   - :doc:`ui::DimensionPolicy<ui>`
        - The size policy of a single surface dimension.
    *   - :doc:`ui::Display<ui>`
        - A display is a way to show pages on a terminal.
    *   - :doc:`ui::Event<ui-events>`
        - A single event.
    *   - :doc:`ui::EventData<ui-events>`
        - The base class for event data.
    *   - :doc:`ui::EventDriver<ui-events>`
        - Event driver for creating event-driven applications.
    *   - :doc:`ui::EventScheduler<ui-events>`
        - A scheduler for events.
    *   - :doc:`ui::EventThread<ui-events>`
        - A thread that executes queued invocations via the UI event system.
    *   - :doc:`ui::EventType<ui-events>`
        - The event type
    *   - :doc:`ui::Geometry<ui>`
        - The geometry and size policy of a surface.
    *   - :doc:`ui::KeyBindings<ui>`
        - Key-to-action bindings for surfaces and pages.
    *   - :doc:`ui::KeyPressEvent<ui-events>`
        - A key press event.
    *   - :doc:`ui::Layout<ui>`
        - A layout is a surface that arranges other surfaces in a specific way.
    *   - :doc:`ui::Page<ui>`
        - A single page on the screen.
    *   - :doc:`ui::PaintContext<ui>`
        - The paint context for the onPaint method.
    *   - :doc:`ui::Panel<ui>`
        - A fill surface that paints an optional background behind its child surfaces.
    *   - :doc:`ui::QuitEvent<ui-events>`
        - The data for a quit event.
    *   - :doc:`ui::ScheduledActionRef<ui-events>`
        - Represents a scheduled action.
    *   - :doc:`ui::Scheduler<ui-events>`
        - A scheduler for UI actions.
    *   - :doc:`ui::SizePolicy<ui>`
        - The size policy for both width and height of a surface.
    *   - :doc:`ui::Stack<ui>`
        - A layout that stacks surfaces vertically or horizontally.
    *   - :doc:`ui::StopSource<ui-events>`
        - Stop source used by the event system to request a cooperative shutdown.
    *   - :doc:`ui::StopToken<ui-events>`
        - Cooperative stop token passed to stoppable UI callbacks.
    *   - :doc:`ui::Surface<ui>`
        - A surface is a rectangular area on a page.
    *   - :doc:`ui::TextBox<ui>`
        - A surface that renders one string using TextOptions-derived alignment and wrapping rules.
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
    *   -   :doc:`Backend <backend>`
        -   The backend classes connect Terminal to the actual console, host environment, or test harness. Most applications use the built-in backend, while advanced integrations can inject a custom implementation.
    *   -   :doc:`Buffer <buffer>`
        -   The buffer classes represent rendered terminal content in memory before it is written to the screen. ReadableBuffer provides the inspection API, WritableBuffer adds mutation and drawing operations, Buffer is the concrete 2D storage type used in most applications, and CursorBuffer adds cursor-based streaming output.
    *   -   :doc:`Color <color>`
        -   The color classes provide access to the standard 16 terminal colors and their combinations. They are used to style terminal output, buffer cells, and text elements throughout the library.
    *   -   :doc:`Cursor Output <cursor-output>`
        -   CursorWriter defines the shared streaming API used by Terminal and CursorBuffer. It focuses on incremental output, active color and attribute state, cursor movement, and wrapped paragraph printing.
    *   -   :doc:`Frames, Borders, and Tiles <drawing>`
        -   The drawing types define how frames, borders, filled areas, and character combinations are rendered into writable buffers. They are the main building blocks for boxes, panels, decorative borders, tiled fills, and line-art style layouts.
    *   -   :doc:`Font <font>`
        -   The font classes allow you to render large bitmap-based text directly in the terminal. Fonts are primarily used for titles, banners, and other decorative elements in terminal interfaces.
    *   -   :doc:`Strings and Chars <text>`
        -   Char, CharAttributes, CharStyle, and String are the reusable terminal-text value types used to store styled characters, measure Unicode-aware width, and split or wrap terminal text before rendering.
    *   -   :doc:`Geometry <geometry>`
        -   The geometry classes provide the building blocks for positioning and layout inside a terminal buffer. They describe sizes, positions, rectangles, and directions, and allow you to derive new regions from existing ones.
    *   -   :doc:`Input <input>`
        -   The input classes provide access to keyboard input from the terminal. They are designed for interactive applications such as dashboards, tools, and terminal games that need immediate key handling.
    *   -   :doc:`Paragraph Options <paragraph-options>`
        -   ParagraphOptions collects the low-level layout rules for wrapped terminal paragraphs. The same object model is used by Terminal::printParagraph(), TextOptions, and Text, so one paragraph configuration can be reused for direct terminal output and buffer-based text rendering.
    *   -   :doc:`Rich Text and HTML <rich-text>`
        -   The rich-text helpers parse a focused HTML subset into TextNode trees and render it either into String values or directly to CursorWriter targets. Style lets you tune headings, lists, code blocks, block margins, and inline emphasis without inventing a separate markup pipeline for terminal output.
    *   -   :doc:`Terminal <terminal>`
        -   The terminal classes provide the runtime connection between your rendered content and the real console. Terminal manages initialization, refresh strategy, full-screen updates, input access, and direct output for interactive applications.
    *   -   :doc:`Text Rendering <drawing-text>`
        -   Text, TextOptions, and TextAnimation render strings into rectangles, wrapped paragraphs, animated labels, and reusable text presets inside writable buffers.
    *   -   :doc:`UI Events and Scheduling <ui-events>`
        -   The UI event classes provide the queues, timers, cooperative-stop signals, and worker-thread building blocks behind Application. Most apps use them indirectly, but advanced integrations can drive EventDriver, EventScheduler, Scheduler, or EventThread directly when they need custom orchestration.
    *   -   :doc:`UI Framework <ui>`
        -   The UI framework builds terminal applications from a surface tree with pages, layouts, geometry policies, focus routing, and paint invalidation. It provides the high-level structure for interactive full-screen apps that should feel more like a widget system than hand-written buffer math.

.. toctree::
    :hidden:
    :maxdepth: 1

    bitmap
    backend
    buffer
    buffer-view
    color
    cursor-output
    drawing
    drawing-text
    font
    geometry
    input
    paragraph-options
    rich-text
    terminal
    text
    ui
    ui-events
