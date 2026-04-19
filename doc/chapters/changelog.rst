..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: Changelog
    single: Changes

*********
Changelog
*********

Version 1.9.0 - 2026-04-20
==========================

Version 1.9.0 introduces a more efficient shared-storage model for terminal text, making :cpp:any:`String <erbsland::cterm::String>` instances significantly cheaper to reuse. It also adds reusable UI surface primitives for common patterns such as status bars and scrollable views, and expands the documentation around the beta UI framework.

In addition, this release introduces the ``ui-html-viewer`` demo, improves key-binding ergonomics, and adds focused tests for the new text and UI infrastructure.

Highlights
----------

*   Introduced :cpp:any:`StringView <erbsland::cterm::StringView>` and :cpp:any:`IndexRange <erbsland::cterm::IndexRange>` as public APIs for efficient read-only access, cheap slicing, and range-based operations on terminal strings.
*   Added :cpp:any:`ui::ScrollingBufferView <erbsland::cterm::ui::surface::ScrollingBufferView>`, :cpp:any:`ui::AbstractStatusLine <erbsland::cterm::ui::surface::AbstractStatusLine>`, and :cpp:any:`ui::StatusLine <erbsland::cterm::ui::surface::StatusLine>` to simplify building scrollable views and single-line headers or footers with minimal custom surface code.
*   Added the ``ui-html-viewer`` demo and extended demo documentation for ``html-viewer``, ``ui-hello-world``, and ``ui-html-viewer`` to demonstrate how the HTML renderer and the beta UI framework work together in realistic applications.
*   Expanded the reference documentation with dedicated chapters for UI core, input, layouts, and surfaces, and added an implementation note describing the new string-sharing design.
*   Increased unit test coverage for :cpp:any:`StringView <erbsland::cterm::StringView>`, :cpp:any:`IndexRange <erbsland::cterm::IndexRange>`, paragraph rendering, key bindings, and the new UI surfaces.

Added
-----

*   Added :cpp:any:`StringView <erbsland::cterm::StringView>` as the read-only companion to :cpp:any:`String <erbsland::cterm::String>`, together with :cpp:any:`IndexRange <erbsland::cterm::IndexRange>` for explicit range handling within shared string storage.
*   Added :cpp:any:`ui::ScrollingBufferView <erbsland::cterm::ui::surface::ScrollingBufferView>` for scrollable buffer-backed content, and :cpp:any:`ui::AbstractStatusLine <erbsland::cterm::ui::surface::AbstractStatusLine>` plus :cpp:any:`ui::StatusLine <erbsland::cterm::ui::surface::StatusLine>` for reusable single-line status bars with section layout, margins, collapse behavior, and refresh callbacks.
*   Added the ``ui-html-viewer`` demo as a UI-framework-based version of the HTML viewer, showcasing :cpp:any:`ui::ScrollingBufferView <erbsland::cterm::ui::surface::ScrollingBufferView>`, :cpp:any:`ui::StatusLine <erbsland::cterm::ui::surface::StatusLine>`, and the rich-text pipeline in a complete end-to-end example.
*   Added dedicated reference chapters for :doc:`UI Core <reference/ui-core>`, :doc:`UI Input <reference/ui-input>`, :doc:`UI Layouts <reference/ui-layouts>`, and :doc:`UI Surfaces <reference/ui-surfaces>`.
*   Added the :doc:`String Sharing <implementation/string-sharing>` implementation note and new demo chapters for :doc:`HTML Viewer <demos/html-viewer>`, :doc:`UI Hello World <demos/ui-hello-world>`, and :doc:`UI HTML Viewer <demos/ui-html-viewer>`.

Improved
--------

*   Updated read-only text APIs across :cpp:any:`Terminal <erbsland::cterm::Terminal>`, :cpp:any:`CursorWriter <erbsland::cterm::CursorWriter>`, :cpp:any:`CursorBuffer <erbsland::cterm::CursorBuffer>`, :cpp:any:`WritableBuffer <erbsland::cterm::WritableBuffer>`, :cpp:any:`Buffer <erbsland::cterm::Buffer>`, :cpp:any:`String <erbsland::cterm::String>`, and related helpers to accept :cpp:any:`StringView <erbsland::cterm::StringView>` where mutation is not required. This reduces copying and clarifies ownership semantics.
*   Extended :cpp:any:`ui::KeyBindings <erbsland::cterm::ui::KeyBindings>` with convenience overloads for special keys, Unicode characters, combined text input, and initializer-list bindings, making interactions easier to express.
*   Refined paragraph and rich-text rendering with improved token and render-block handling, keeping the HTML renderer and paragraph pipeline more modular and easier to extend.

Documentation
-------------

*   Split the UI reference into smaller, focused chapters to make the beta UI framework easier to navigate.
*   Expanded the text reference to better explain implicit sharing, string views, ranges, and UTF-8 handling.
*   Improved the reference index so new text and UI support types are easier to discover.
*   Unified the demo documentation with screen captures and more focused descriptions.

Implementation
--------------

*   Refactored terminal string storage around dedicated shared data and builder components, making :cpp:any:`String <erbsland::cterm::String>` slicing and detaching behavior more explicit.
*   Expanded the unit test suite with focused coverage for string-view behavior, string building, paragraph rendering, HTML rendering, key bindings, scrolling views, and status lines.
*   Added the repository-local ``utilities/pre_commit.py`` helper to run standard pre-commit checks consistently.

Code Format
-----------

We updated the code formatting rules.

Where two empty lines were previously used to visually separate code sections, only a single empty line is now used.

This change was made deliberately to keep multiple code bases in sync and to simplify automated code formatting.


Version 1.8.0 - 2026-04-17
==========================

Release 1.8.0 introduces two higher-level layers on top of the existing terminal primitives: a beta :cpp:any:`ui::Application <erbsland::cterm::ui::Application>` framework for event-driven terminal interfaces and a new rich-text pipeline centered around :cpp:any:`text::HtmlRenderer <erbsland::cterm::text::HtmlRenderer>` for rendering structured documents and HTML to terminal output. This release also makes UTF-8 recovery deterministic, adds new public support types for paragraph layout and buffer resizing, introduces new demos, and substantially expands the reference and implementation documentation.

Highlights
----------

*   Added the beta :cpp:any:`erbsland::cterm::ui` framework with :cpp:any:`Application <erbsland::cterm::ui::Application>`, pages, surfaces, layouts, key bindings, schedulers, worker threads, and built-in surface types for structured terminal user interfaces.
*   Added rich-text and HTML rendering through :cpp:any:`text::HtmlRenderer <erbsland::cterm::text::HtmlRenderer>`, :cpp:any:`text::Style <erbsland::cterm::text::Style>`, and :cpp:any:`text::TextNode <erbsland::cterm::text::TextNode>`, making it much easier to render headings, lists, links, blockquotes, and code blocks into terminal-friendly output.
*   Added :cpp:any:`EncodingErrors <erbsland::cterm::EncodingErrors>`, :cpp:any:`BufferResizeMode <erbsland::cterm::BufferResizeMode>`, :cpp:any:`ParagraphIndents <erbsland::cterm::ParagraphIndents>`, :cpp:any:`FastCharSet <erbsland::cterm::FastCharSet>`, and :cpp:any:`TerminalSession <erbsland::cterm::TerminalSession>` as public support APIs for safer text handling, reusable paragraph configuration, and clearer terminal lifecycle management.
*   Added the ``html-viewer`` and ``ui-hello-world`` demos and reorganized the demo infrastructure around reusable terminal-application helpers, which makes the examples easier to study and extend.
*   Expanded the documentation and unit tests significantly, including new reference chapters for rich text, the UI framework, UI events, and detailed implementation notes for scheduling, layout, paragraph rendering, and text node planning.

Added
-----

*   Added :cpp:any:`erbsland::cterm::ui` as a new public beta module with :cpp:any:`Application <erbsland::cterm::ui::Application>`, :cpp:any:`Display <erbsland::cterm::ui::Display>`, :cpp:any:`Page <erbsland::cterm::ui::Page>`, :cpp:any:`Surface <erbsland::cterm::ui::Surface>`, :cpp:any:`Layout <erbsland::cterm::ui::Layout>`, :cpp:any:`Geometry <erbsland::cterm::ui::Geometry>`, :cpp:any:`KeyBindings <erbsland::cterm::ui::KeyBindings>`, and the built-in :cpp:any:`ui::Stack <erbsland::cterm::ui::layout::Stack>`, :cpp:any:`ui::Panel <erbsland::cterm::ui::surface::Panel>`, and :cpp:any:`ui::TextBox <erbsland::cterm::ui::surface::TextBox>` types.
*   Added a public event and scheduling layer with :cpp:any:`ui::Event <erbsland::cterm::ui::Event>`, :cpp:any:`ui::EventDriver <erbsland::cterm::ui::EventDriver>`, :cpp:any:`ui::EventScheduler <erbsland::cterm::ui::EventScheduler>`, :cpp:any:`ui::Scheduler <erbsland::cterm::ui::Scheduler>`, :cpp:any:`ui::ScheduledActionRef <erbsland::cterm::ui::ScheduledActionRef>`, :cpp:any:`ui::EventThread <erbsland::cterm::ui::EventThread>`, and :cpp:any:`ui::StopToken <erbsland::cterm::ui::StopToken>` for timers, background work, and cooperative shutdown.
*   Added the :cpp:any:`erbsland::cterm::text` rich-text module with :cpp:any:`text::HtmlRenderer <erbsland::cterm::text::HtmlRenderer>`, :cpp:any:`text::Style <erbsland::cterm::text::Style>`, :cpp:any:`text::StyleRule <erbsland::cterm::text::StyleRule>`, :cpp:any:`text::StyleSelector <erbsland::cterm::text::StyleSelector>`, and :cpp:any:`text::TextNode <erbsland::cterm::text::TextNode>` for parsing, styling, and rendering structured text and HTML.
*   Added :cpp:any:`EncodingErrors <erbsland::cterm::EncodingErrors>`, :cpp:any:`BufferResizeMode <erbsland::cterm::BufferResizeMode>`, :cpp:any:`ParagraphIndents <erbsland::cterm::ParagraphIndents>`, :cpp:any:`FastCharSet <erbsland::cterm::FastCharSet>`, and :cpp:any:`TerminalSession <erbsland::cterm::TerminalSession>` to make UTF-8 validation, buffer resizing, paragraph indentation, separator handling, and scoped terminal setup more explicit in user code.
*   Added umbrella headers for the new text and UI modules, including ``<erbsland/cterm/text/all.hpp>`` and ``<erbsland/cterm/ui/all.hpp>``, together with generated top-level include wrappers.
*   Added the ``html-viewer`` demo to showcase HTML-to-terminal rendering and the ``ui-hello-world`` demo to show the new UI framework in a minimal end-to-end application.

Improved
--------

*   :cpp:any:`String <erbsland::cterm::String>` now acts as the explicit public UTF-8 boundary, and text-based :cpp:any:`Char <erbsland::cterm::Char>` and :cpp:any:`CombinedChar <erbsland::cterm::impl::CombinedChar>` construction now recovers deterministically instead of throwing by default.
*   Malformed UTF-8 replacement is now deterministic, with each erroneous UTF-8 byte becoming one Unicode replacement character. Empty text passed to character constructors also normalizes to the replacement character.
*   :cpp:any:`Buffer <erbsland::cterm::Buffer>` resize-related APIs now use :cpp:any:`BufferResizeMode <erbsland::cterm::BufferResizeMode>`, which makes content-preservation behavior more explicit when buffers are resized.
*   Paragraph rendering internals were reorganized into dedicated layout, painter, printer, and renderer components, which simplifies reuse across direct terminal output, cursor-buffer output, and the new rich-text renderer.
*   Existing demos were modernized around shared terminal-application helpers, and ``update-screen-modes`` gained improved flush-speed tracking for comparing rendering strategies.

Documentation
-------------

*   Added new reference chapters for :doc:`Rich Text and HTML <reference/rich-text>`, :doc:`UI Framework <reference/ui>`, and :doc:`UI Events and Scheduling <reference/ui-events>`.
*   Expanded the reference index so the new text, UI, buffer, paragraph, and terminal support types are documented and easier to discover.
*   Added detailed implementation notes for action scheduling, invocation and event threads, remapped buffers, stack layout, and rich-text rendering, and substantially expanded the paragraph-layout documentation.
*   Added Mermaid support to the Sphinx documentation build so diagrams can be embedded directly in the docs.

Implementation
--------------

*   Refactored paragraph rendering into a dedicated ``impl/paragraph`` subsystem and split the rich-text renderer into parser, tokenizer, planning, and rendering components to keep the internal architecture modular.
*   Expanded the unit test suite across API, implementation, demo, and UI layers, including focused coverage for HTML parsing and rendering, event scheduling, UI layout, text styles, and deterministic UTF-8 recovery.

Version 1.7.0 - 2026-03-27
==========================

Release 1.7.0 unifies direct terminal output and buffer-based text rendering around a shared cursor-oriented API. The biggest additions are :cpp:any:`CursorWriter <erbsland::cterm::CursorWriter>` and :cpp:any:`CursorBuffer <erbsland::cterm::CursorBuffer>`, which make it much easier to reuse output code between live terminals, scrollback buffers, and rich text views. This release also introduces explicit character attributes and reusable text styles, expands buffer composition APIs, adds several new demos and reference pages, and improves key decoding and UTF-8 recovery for interactive applications.

Highlights
----------

*   Added :cpp:any:`CursorWriter <erbsland::cterm::CursorWriter>` as a shared cursor-based output interface implemented by :cpp:any:`Terminal <erbsland::cterm::Terminal>` and :cpp:any:`CursorBuffer <erbsland::cterm::CursorBuffer>`, so the same printing helpers can target both live terminals and in-memory buffers.
*   Added :cpp:any:`CursorBuffer <erbsland::cterm::CursorBuffer>` with VT100-style wrapping, configurable overflow behavior, paragraph printing, inherited color resolution, and fill-character based screen clearing, which makes scrollback panes, log viewers, and text consoles much easier to implement.
*   Added :cpp:any:`CharAttributes <erbsland::cterm::CharAttributes>` and :cpp:any:`CharStyle <erbsland::cterm::CharStyle>` so ANSI text attributes and color can be configured, combined, inherited, and reused consistently across direct terminal output, strings, and buffer rendering.
*   Added :cpp:any:`BufferDrawOptions <erbsland::cterm::BufferDrawOptions>` plus improved :cpp:any:`WritableBuffer <erbsland::cterm::WritableBuffer>` buffer-to-buffer drawing support for aligned placement, source cropping, and character-combination based composition.
*   Added the ``display-all-attributes``, ``key-input-demo``, and ``log-viewer`` demos together with substantially expanded reference and demo documentation for cursor output, text rendering, geometry, fonts, backends, and input handling.

Added
-----

*   Added :cpp:any:`CursorWriter <erbsland::cterm::CursorWriter>` and :cpp:any:`CursorWriterPtr <erbsland::cterm::CursorWriterPtr>` as the shared public abstraction for cursor-based text output, style changes, cursor movement, paragraph printing, and screen clearing.
*   Added :cpp:any:`CursorBuffer <erbsland::cterm::CursorBuffer>` with :cpp:any:`CursorBuffer::OverflowMode <erbsland::cterm::CursorBuffer::OverflowMode>` options for shifting, wrapping, or growing scrollback-style buffers at the bottom edge.
*   Added :cpp:any:`CharAttributes <erbsland::cterm::CharAttributes>` for bold, dim, italic, underline, blink, reverse, hidden, and strikethrough state with explicit specified-versus-inherited semantics.
*   Added :cpp:any:`CharStyle <erbsland::cterm::CharStyle>` as a reusable combined style object for color plus character attributes, including overlay/base resolution and stable hashing.
*   Added :cpp:any:`BufferDrawOptions <erbsland::cterm::BufferDrawOptions>` so callers can draw buffers into other buffers with explicit target rectangles, source rectangles, color overwrite behavior, and optional :cpp:any:`CharCombinationStyle <erbsland::cterm::CharCombinationStyle>` composition.
*   Added the ``display-all-attributes`` demo to show supported ANSI text attributes and style combinations.
*   Added the ``key-input-demo`` demo to visualize interactive key handling, printable text input, special keys, and live terminal resizing behavior.
*   Added the ``log-viewer`` demo to demonstrate cursor-based text generation, scrollback handling, follow mode, and :cpp:any:`BufferView <erbsland::cterm::BufferView>` based viewport rendering.
*   Added new reference chapters for backend integration, cursor output, and text rendering, plus new generated documentation helpers for drawing, drawing text, fonts, and geometry.

Improved
--------

*   :cpp:any:`Terminal <erbsland::cterm::Terminal>` now implements :cpp:any:`CursorWriter <erbsland::cterm::CursorWriter>`, so direct terminal printing and in-memory cursor output share one conceptual API for colors, character attributes, cursor movement, auto-wrap, and paragraph rendering.
*   :cpp:any:`WritableBuffer <erbsland::cterm::WritableBuffer>` now has broader buffer composition support, making it easier to place pre-rendered regions, cropped views, and combined character artwork inside larger layouts.
*   :cpp:any:`Char <erbsland::cterm::Char>`, :cpp:any:`String <erbsland::cterm::String>`, and related text-rendering paths now integrate more cleanly with reusable styles, inherited attributes, and multi-codepoint rendering support.
*   Key decoding and console input handling are more robust, including better handling for printable single-character input, broken UTF-8 lead bytes, chunked input timing, and polling-based POSIX readiness checks.
*   The documentation now covers cursor-oriented output, ANSI character attributes, text rendering, geometry, fonts, demo applications, and backend behavior in much greater depth.

Implementation
--------------

*   Added focused unit tests for :cpp:any:`CursorBuffer <erbsland::cterm::CursorBuffer>`, :cpp:any:`CharAttributes <erbsland::cterm::CharAttributes>`, :cpp:any:`CharStyle <erbsland::cterm::CharStyle>`, combined character handling, key decoding, and UTF-8 resynchronization.
*   Refactored paragraph rendering and related internal helpers so terminal output and cursor-buffer output can share more behavior.
*   Improved backend internals for attribute emission, input polling, and UTF-8 recovery while keeping the public API higher-level and easier to reuse.

Version 1.6.0 - 2026-03-25
==========================

Release 1.6.0 makes text-heavy terminal applications much easier to build. The biggest user-facing addition is a new paragraph layout system that works both for direct terminal output and for text rendered into buffers. This release also adds a new remapped buffer type for row- and column-oriented editing workloads, improves terminal capability detection, and substantially expands the documentation and demos around wrapped text output.

Highlights
----------

*   Added :cpp:any:`ParagraphOptions <erbsland::cterm::ParagraphOptions>`, :cpp:any:`ParagraphBackgroundMode <erbsland::cterm::ParagraphBackgroundMode>`, :cpp:any:`ParagraphOnError <erbsland::cterm::ParagraphOnError>`, :cpp:any:`TabOverflowBehavior <erbsland::cterm::TabOverflowBehavior>`, and :cpp:any:`TextOptions <erbsland::cterm::TextOptions>` to configure alignment, indentation, wrap markers, word splitting, tab handling, ellipsis behavior, background fill, and fallback behavior in one reusable model.
*   Added :cpp:any:`Terminal::printParagraph() <erbsland::cterm::Terminal::printParagraph()>` so applications can print structured, width-aware paragraphs directly to the terminal without first building a full-screen buffer.
*   Added :cpp:any:`RemappedBuffer <erbsland::cterm::RemappedBuffer>` and :cpp:any:`Orientation <erbsland::cterm::Orientation>` for efficient row and column insertion, deletion, movement, shifting, and rotation in editors, log viewers, scrollback panes, and other large grid-based views.
*   Expanded :cpp:any:`Text <erbsland::cterm::Text>`, :cpp:any:`String <erbsland::cterm::String>`, and :cpp:any:`Char <erbsland::cterm::Char>` with paragraph-aware rendering options and new helpers such as :cpp:any:`String::terminalLines() <erbsland::cterm::String::terminalLines()>`, :cpp:any:`String::containsControlCharacters() <erbsland::cterm::String::containsControlCharacters()>`, and :cpp:any:`Char::isControl() <erbsland::cterm::Char::isControl()>`.
*   Added :cpp:any:`Terminal::isInteractive() <erbsland::cterm::Terminal::isInteractive()>`, a new ``command-line-help`` demo, and a much broader documentation set for paragraph layout, backends, and Unicode width handling.

Added
-----

*   Added :cpp:any:`ParagraphOptions <erbsland::cterm::ParagraphOptions>` as a reusable public configuration object for wrapped paragraph rendering in :cpp:any:`Terminal <erbsland::cterm::Terminal>`, :cpp:any:`TextOptions <erbsland::cterm::TextOptions>`, and :cpp:any:`Text <erbsland::cterm::Text>`.
*   Added :cpp:any:`ParagraphBackgroundMode <erbsland::cterm::ParagraphBackgroundMode>` so wrapped paragraphs can optionally extend their background color into continuation indents, the remaining cells at the right edge, or both. This makes highlighted help blocks and callout text look consistent even when they wrap.
*   Added :cpp:any:`ParagraphOnError <erbsland::cterm::ParagraphOnError>` and :cpp:any:`TabOverflowBehavior <erbsland::cterm::TabOverflowBehavior>` to control narrow-layout fallbacks and non-advancing tab stops in a predictable way.
*   Added :cpp:any:`TextOptions <erbsland::cterm::TextOptions>` so color, font, animation, and paragraph layout settings can be bundled and reused independently from the :cpp:any:`Text <erbsland::cterm::Text>` content object.
*   Added :cpp:any:`Terminal::printParagraph() <erbsland::cterm::Terminal::printParagraph()>` for direct terminal output with indentation, wrap markers, tab stops, paragraph spacing, wrap limits, ellipsis markers, and layout fallback handling.
*   Added :cpp:any:`Terminal::isInteractive() <erbsland::cterm::Terminal::isInteractive()>` so applications can detect after initialization whether a real interactive terminal is attached and switch to a plain-text output path when needed.
*   Added :cpp:any:`RemappedBuffer <erbsland::cterm::RemappedBuffer>` with efficient operations for resizing, shifting, rotating, erasing, inserting, and moving complete rows and columns while keeping the visible grid model intact.
*   Added :cpp:any:`Orientation <erbsland::cterm::Orientation>` and :cpp:any:`Coordinate <erbsland::cterm::Coordinate>` as public support types used by the new buffer and geometry APIs.
*   Added :cpp:any:`String::String(std::size_t, Char) <erbsland::cterm::String::String()>`, :cpp:any:`String::terminalLines() <erbsland::cterm::String::terminalLines()>`, :cpp:any:`String::containsControlCharacters() <erbsland::cterm::String::containsControlCharacters()>`, and :cpp:any:`Char::isControl() <erbsland::cterm::Char::isControl()>` for common text-processing and validation tasks.
*   Added the ``command-line-help`` demo application to show how paragraph-aware output can keep a classic command-line help screen readable across narrow and wide terminal widths.

Improved
--------

*   :cpp:any:`Text <erbsland::cterm::Text>` now renders through a structured paragraph pipeline instead of only using the earlier simpler layout settings. The same paragraph configuration can therefore be reused for direct terminal printing and for text rendered inside rectangles.
*   Tab handling in left-aligned paragraphs is now much more practical for help text and option lists. Applications can define explicit tab stops and choose whether a non-advancing tab inserts a space or starts a wrapped continuation line.
*   Paragraph rendering now supports visible wrap markers, word-break markers, wrap limits with ellipsis, configurable paragraph spacing, and clearer fallback handling when a layout becomes too narrow to render safely.
*   The terminal backends now expose whether the process is attached to an interactive terminal, making it easier to produce a rich UI in a real terminal while keeping redirected output, logs, and CI runs readable.
*   Geometry and buffer APIs now consistently use :cpp:any:`Coordinate <erbsland::cterm::Coordinate>`, and :cpp:any:`Buffer::get() <erbsland::cterm::Buffer::get()>` returns a shared space for out-of-range reads, which simplifies caller code that inspects the edges of a buffer.

Documentation
-------------

*   Added a detailed new :doc:`Paragraph Options <reference/paragraph-options>` reference chapter with rendered examples for alignment, indentation, wrap markers, background modes, word splitting, tab stops, wrap limits, and error handling.
*   Added documentation for the new ``command-line-help`` demo, showing a practical end-to-end use of :cpp:any:`Terminal::printParagraph() <erbsland::cterm::Terminal::printParagraph()>`.
*   Expanded the reference documentation for buffer, terminal, and text APIs.
*   Added implementation chapters for paragraph layout and painting, Unicode width handling, and the POSIX and Windows backends, plus a helper tool used to generate consistent paragraph-layout examples for the documentation.

Implementation
--------------

*   Refactored the text rendering internals into dedicated paragraph layout and paragraph painting components, which improves consistency between terminal output and buffer-based text rendering.
*   Refactored writable and readable buffer convenience APIs into explicit internal wrapper implementations and expanded unit test coverage for text rendering, terminal convenience methods, and :cpp:any:`RemappedBuffer <erbsland::cterm::RemappedBuffer>`.

Version 1.5.0 - 2026-03-21
==========================

Release 1.5.0 sharpens the library's low-level building blocks for bitmap processing, geometry, and hash-based containers, while also fixing how inherited colors are resolved when strings and characters are appended or written incrementally. From the user perspective, the most noticeable additions are new helpers for expanding and probing bitmaps, new geometry clamping and neighbor APIs, and direct support for using core value types in ``std::unordered_map`` and ``std::unordered_set``.

Highlights
----------

*   Added :cpp:any:`Bitmap::expanded() <erbsland::cterm::Bitmap::expanded()>`, :cpp:any:`Bitmap::pixelCardinal() <erbsland::cterm::Bitmap::pixelCardinal()>`, :cpp:any:`Position::ringEight() <erbsland::cterm::Position::ringEight()>`, :cpp:any:`Position::ringEightDeltas() <erbsland::cterm::Position::ringEightDeltas()>`, and :cpp:any:`Direction::fromDelta() <erbsland::cterm::Direction::fromDelta()>` to make neighborhood analysis, contour detection, and grid navigation easier to express with the public API.
*   Added :cpp:any:`Size::clamp() <erbsland::cterm::Size::clamp()>` and :cpp:any:`Rectangle::clamp() <erbsland::cterm::Rectangle::clamp()>` so positions can be forced into valid drawable bounds without repeating manual ``min``/``max`` code throughout applications and demos.
*   Added ``hash()`` methods and ``std::hash`` specializations for :cpp:any:`Foreground <erbsland::cterm::Foreground>`, :cpp:any:`Background <erbsland::cterm::Background>`, :cpp:any:`Color <erbsland::cterm::Color>`, :cpp:any:`Char <erbsland::cterm::Char>`, :cpp:any:`Direction <erbsland::cterm::Direction>`, :cpp:any:`Position <erbsland::cterm::Position>`, :cpp:any:`Rectangle <erbsland::cterm::Rectangle>`, and :cpp:any:`Key <erbsland::cterm::Key>`, which makes these value types directly usable in ``std::unordered_map`` and ``std::unordered_set``.
*   Added :cpp:any:`TerminalFlag <erbsland::cterm::TerminalFlag>` and :cpp:any:`TerminalFlags <erbsland::cterm::TerminalFlags>` so applications can control built-in terminal signal handling explicitly, including an opt-out for environments that already manage process shutdown and terminal restoration.
*   Improved color parsing and color inheritance: color names now accept spaces, underscores, and hyphens, and inherited color components in :cpp:any:`String::append() <erbsland::cterm::String::append()>` and :cpp:any:`Terminal::write() <erbsland::cterm::Terminal::write()>` now resolve against the currently active color as users expect.

Added
-----

*   Added :cpp:any:`Bitmap::expanded() <erbsland::cterm::Bitmap::expanded()>` to grow a bitmap by margins or crop it with negative margins, with a configurable fill value for newly created cells.
*   Added :cpp:any:`Bitmap::pixelCardinal() <erbsland::cterm::Bitmap::pixelCardinal()>` as a four-neighbor bitmask helper, complementing :cpp:any:`pixelQuad() <erbsland::cterm::Bitmap::pixelQuad()>` and :cpp:any:`pixelRing() <erbsland::cterm::Bitmap::pixelRing()>` for cellular automata, pathfinding, and shape-analysis code.
*   Added :cpp:any:`Position::ringEight() <erbsland::cterm::Position::ringEight()>` and :cpp:any:`Position::ringEightDeltas() <erbsland::cterm::Position::ringEightDeltas()>` for clockwise access to the eight surrounding positions, plus :cpp:any:`Direction::fromDelta() <erbsland::cterm::Direction::fromDelta()>` for converting arbitrary deltas into one of the canonical directions by sign.
*   Added :cpp:any:`Size::componentClamp() <erbsland::cterm::Size::componentClamp()>`, :cpp:any:`Size::clamp() <erbsland::cterm::Size::clamp()>`, :cpp:any:`Rectangle::clamp() <erbsland::cterm::Rectangle::clamp()>`, and :cpp:any:`Rectangle::center() <erbsland::cterm::Rectangle::center()>` for layout and cursor-position calculations that need safe in-bounds coordinates.
*   Added :cpp:any:`TerminalFlag <erbsland::cterm::TerminalFlag>` and :cpp:any:`TerminalFlags <erbsland::cterm::TerminalFlags>` and corresponding :cpp:any:`Terminal <erbsland::cterm::Terminal>` constructors, so applications can opt out of the built-in signal restoration logic when integrating with another framework or host environment.
*   Added hashing support through ``hash()`` methods and ``std::hash`` specializations on the most common value objects, including geometry types, colors, characters, directions, and keys.

Improved
--------

*   :cpp:any:`String::append() <erbsland::cterm::String::append()>` now applies the currently active color to appended :cpp:any:`Char <erbsland::cterm::Char>` and :cpp:any:`String <erbsland::cterm::String>` values when those values contain inherited foreground or background components. This makes mixed argument lists behave consistently with plain text arguments.
*   :cpp:any:`Terminal::write() <erbsland::cterm::Terminal::write()>` now resolves inherited colors against the terminal's tracked current color before output. Incremental writes therefore keep foreground and background inheritance consistent with previous terminal output.
*   Built-in terminal backends now restore the terminal state more robustly on POSIX and Windows when an application is interrupted, which reduces the chance of leaving the shell in an inconsistent state after ``Ctrl+C`` or similar termination events.
*   :cpp:any:`Foreground::fromString() <erbsland::cterm::Foreground::fromString()>`, :cpp:any:`Background::fromString() <erbsland::cterm::Background::fromString()>`, and :cpp:any:`Color::fromString() <erbsland::cterm::Color::fromString()>` now accept more forgiving names such as ``"bright blue"``, ``"bright_blue"``, and ``"bright-blue"``, which simplifies configuration parsing and hand-written theme definitions.
*   :cpp:any:`TerminalFlag <erbsland::cterm::TerminalFlag>` values can now be combined directly with the bitwise OR operator, which makes terminal construction code more concise.

Implementation
--------------

*   Expanded unit test coverage significantly across bitmap helpers, buffer conveniences, buffer views, color parsing, geometry helpers, terminal convenience APIs, update settings, and hashing behavior.
*   Reworked built-in signal handling with platform-specific dispatchers for POSIX and Windows and refreshed the terminal reference documentation for the new terminal flag API.
*   Added shared hash utilities and refreshed API documentation for the new public methods and behavior clarifications.


Version 1.4.0 - 2026-03-15
==========================

Release 1.4.0 reorganizes the rendering API around reusable buffer abstractions, adds scrollable buffer views and backend customization, and expands the text and bitmap helper APIs. From the user perspective, the most important behavior changes are the new generic :cpp:any:`ReadableBuffer <erbsland::cterm::ReadableBuffer>`/:cpp:any:`WritableBuffer <erbsland::cterm::WritableBuffer>` pipeline, richer :cpp:any:`Terminal <erbsland::cterm::Terminal>` control, and updated screen rendering defaults for safe margins and alternate-screen updates.

Highlights
----------

*   Added :cpp:any:`ReadableBuffer <erbsland::cterm::ReadableBuffer>` and :cpp:any:`WritableBuffer <erbsland::cterm::WritableBuffer>` as public base interfaces, so application code can render, compare, copy, and mutate terminal content without depending on one concrete :cpp:any:`Buffer <erbsland::cterm::Buffer>` implementation.
*   Added :cpp:any:`BufferView <erbsland::cterm::BufferView>`, :cpp:any:`BufferConstRefView <erbsland::cterm::BufferConstRefView>`, and :cpp:any:`CropEdges <erbsland::cterm::CropEdges>` for view-based rendering of larger logical canvases, including optional in-view crop markers for scrollable or clipped content.
*   Extended :cpp:any:`Terminal <erbsland::cterm::Terminal>` with :cpp:any:`Backend <erbsland::cterm::Backend>`, :cpp:any:`OutputMode <erbsland::cterm::OutputMode>`, :cpp:any:`MoveMode <erbsland::cterm::MoveMode>`, safe-margin handling, alternate-screen control, direct cursor movement helpers, and backend injection for custom terminal integrations and tests.
*   Expanded :cpp:any:`UpdateSettings <erbsland::cterm::UpdateSettings>` with configurable minimum-size background and message handling, explicit bottom-right crop marks, and control over automatic switching to the alternate screen buffer.
*   Added :cpp:any:`ParagraphSpacing <erbsland::cterm::ParagraphSpacing>` plus new :cpp:any:`Text <erbsland::cterm::Text>` and :cpp:any:`String <erbsland::cterm::String>` support for multi-paragraph layouts, and expanded :cpp:any:`Bitmap <erbsland::cterm::Bitmap>` with higher-level analysis and editing helpers such as outlining, flood fill, and bounding boxes.

Added
-----

*   Added :cpp:any:`ReadableBuffer::countDifferencesTo() <erbsland::cterm::ReadableBuffer::countDifferencesTo()>` and :cpp:any:`ReadableBuffer::toMask() <erbsland::cterm::ReadableBuffer::toMask()>` to compare rendered frames and derive bitmap masks from visible buffer content.
*   Added :cpp:any:`WritableBuffer::setFrom() <erbsland::cterm::WritableBuffer::setFrom()>`, :cpp:any:`WritableBuffer::setAndResizeFrom() <erbsland::cterm::WritableBuffer::setAndResizeFrom()>`, shared drawing helpers, and :cpp:any:`Buffer::clone() <erbsland::cterm::Buffer::clone()>` so frame-generation code can work against abstract writable targets and keep copies of previous frames.
*   Added a default :cpp:any:`Buffer() <erbsland::cterm::Buffer::Buffer()>` constructor, validating ``Buffer(Size, Char)`` construction, ``Buffer::resize(Size, BufferResizeMode, Char)``, and :cpp:any:`Buffer::fromLinesInString() <erbsland::cterm::Buffer::fromLinesInString()>`/:cpp:any:`Buffer::fromLines() <erbsland::cterm::Buffer::fromLines()>` for more flexible buffer lifecycle management.
*   Added :cpp:any:`Backend <erbsland::cterm::Backend>` as a public extension point for custom platform output and input handling, and changed :cpp:any:`Input <erbsland::cterm::Input>` into a public interface that backends can implement.
*   Added :cpp:any:`Terminal::OutputMode <erbsland::cterm::Terminal::OutputMode>` with :cpp:any:`FullControl <erbsland::cterm::FullControl>` and :cpp:any:`Text <erbsland::cterm::Text>` modes, plus :cpp:any:`Terminal::setBackend() <erbsland::cterm::Terminal::setBackend()>`, :cpp:any:`setAlternateScreen() <erbsland::cterm::Terminal::setAlternateScreen()>`, :cpp:any:`moveTo() <erbsland::cterm::Terminal::moveTo()>`, :cpp:any:`moveHome() <erbsland::cterm::Terminal::moveHome()>`, :cpp:any:`moveCursor() <erbsland::cterm::Terminal::moveCursor()>`, :cpp:any:`moveRight() <erbsland::cterm::Terminal::moveRight()>`, :cpp:any:`moveUp() <erbsland::cterm::Terminal::moveUp()>`, :cpp:any:`moveDown() <erbsland::cterm::Terminal::moveDown()>`, :cpp:any:`setAutoWrap() <erbsland::cterm::Terminal::setAutoWrap()>`, and :cpp:any:`setCursorVisible() <erbsland::cterm::Terminal::setCursorVisible()>`.
*   Added :cpp:any:`Bitmap::rect() <erbsland::cterm::Bitmap::rect()>`, :cpp:any:`pixelRing() <erbsland::cterm::Bitmap::pixelRing()>`, :cpp:any:`boundingRect() <erbsland::cterm::Bitmap::boundingRect()>`, :cpp:any:`pixelCount() <erbsland::cterm::Bitmap::pixelCount()>`, :cpp:any:`invert() <erbsland::cterm::Bitmap::invert()>`/:cpp:any:`inverted() <erbsland::cterm::Bitmap::inverted()>`, :cpp:any:`outlined() <erbsland::cterm::Bitmap::outlined()>`, :cpp:any:`fillRect() <erbsland::cterm::Bitmap::fillRect()>`, :cpp:any:`floodFill() <erbsland::cterm::Bitmap::floodFill()>`, :cpp:any:`fromFunction() <erbsland::cterm::Bitmap::fromFunction()>`, and :cpp:any:`toPattern() <erbsland::cterm::Bitmap::toPattern()>`.
*   Added :cpp:any:`Text::setParagraphSpacing() <erbsland::cterm::Text::setParagraphSpacing()>` and ``String::wrapIntoLines(..., ParagraphSpacing)``, plus :cpp:any:`String::count() <erbsland::cterm::String::count()>`, :cpp:any:`indexOf() <erbsland::cterm::String::indexOf()>`, :cpp:any:`substr() <erbsland::cterm::String::substr()>`, :cpp:any:`splitLines() <erbsland::cterm::String::splitLines()>`, and :cpp:any:`String::fromLines() <erbsland::cterm::String::fromLines()>` helpers.
*   Added more geometry and character helpers, including :cpp:any:`Rectangle <erbsland::cterm::Rectangle>` overlap/containment/intersection tools, :cpp:any:`Rectangle::frameDirection() <erbsland::cterm::Rectangle::frameDirection()>`, :cpp:any:`Rectangle::bounds() <erbsland::cterm::Rectangle::bounds()>`, and new :cpp:any:`Char <erbsland::cterm::Char>` convenience checks such as :cpp:any:`isEmpty() <erbsland::cterm::Char::isEmpty()>` and :cpp:any:`isOneOf() <erbsland::cterm::Char::isOneOf()>`.

Improved
--------

*   :cpp:any:`Terminal::updateScreen() <erbsland::cterm::Terminal::updateScreen()>` and :cpp:any:`Terminal::write() <erbsland::cterm::Terminal::write()>` now accept any :cpp:any:`ReadableBuffer <erbsland::cterm::ReadableBuffer>` implementation instead of requiring a concrete :cpp:any:`Buffer <erbsland::cterm::Buffer>`, which makes buffer views and custom readable buffers work throughout the screen-update pipeline.
*   :cpp:any:`Terminal::size() <erbsland::cterm::Terminal::size()>` now reports a drawable size with a one-column and one-row compatibility margin by default. This makes full-screen layouts safer on real terminals, and applications that want the full detected size can disable the margin explicitly.
*   :cpp:any:`Terminal::updateScreen() <erbsland::cterm::Terminal::updateScreen()>` can now automatically switch to the alternate screen buffer through :cpp:any:`UpdateSettings <erbsland::cterm::UpdateSettings>`. The default settings enable this behavior when the backend supports alternate-screen updates.
*   Minimum-size handling in :cpp:any:`Terminal::updateScreen() <erbsland::cterm::Terminal::updateScreen()>` now supports a custom background fill and centered message instead of only a single marker character.
*   Paragraph wrapping now distinguishes between compact and double-spaced paragraphs through :cpp:any:`ParagraphSpacing <erbsland::cterm::ParagraphSpacing>`, which improves structured help text and other multi-section layouts.
*   Deprecated ``Terminal::colorEnabled()``/``setColorEnabled()`` in favor of :cpp:any:`outputMode() <erbsland::cterm::Terminal::outputMode()>`, deprecated :cpp:any:`Terminal::lineBreak() <erbsland::cterm::Terminal::lineBreak()>` in favor of :cpp:any:`writeLineBreak() <erbsland::cterm::Terminal::writeLineBreak()>`, deprecated the old ``Buffer::drawText(std::string_view, Alignment, Rectangle, ...)`` overload, and deprecated the old :cpp:any:`UpdateSettings <erbsland::cterm::UpdateSettings>` marker naming/constructor API.

Implementation
--------------

*   Expanded API documentation, demos, and unit tests across buffer rendering, terminal behavior, text layout, bitmap processing, and backend integration.
*   Refactored the rendering pipeline into focused painter, backend, string-wrapping, and line-buffer components to improve maintainability and testability.


Version 1.3.0 - 2026-03-10
==========================

Release 1.3.0 adds a more expressive frame-drawing API for animated borders and fills. The main change for library users is that frame rendering can now be configured through reusable :cpp:any:`FrameDrawOptions <erbsland::cterm::FrameDrawOptions>` instances instead of piecing together several lower-level overloads.

Highlights
----------

*   Added :cpp:any:`FrameDrawOptions <erbsland::cterm::FrameDrawOptions>` and :cpp:any:`FrameColorMode <erbsland::cterm::FrameColorMode>` for configurable frame styles, fill styles, combination behavior, and animated color effects.
*   Added a new ``Buffer::drawFrame(Rectangle, const FrameDrawOptions &, std::size_t)`` overload to render frames and optional fills from one reusable options object.
*   Added :cpp:any:`Rectangle::frameIndex() <erbsland::cterm::Rectangle::frameIndex()>` to address frame cells in clockwise order, enabling border-based animations such as chasing light effects.

Added
-----

*   Added :cpp:any:`FrameDrawOptions <erbsland::cterm::FrameDrawOptions>` with support for :cpp:any:`FrameStyle <erbsland::cterm::FrameStyle>`, :cpp:any:`Char16Style <erbsland::cterm::Char16Style>`, and :cpp:any:`Tile9Style <erbsland::cterm::Tile9Style>` selection in one API.
*   Added animated frame and fill color handling with :cpp:any:`FrameColorMode::OneColor <erbsland::cterm::FrameColorMode::OneColor>`, stripe modes, diagonal modes, and clockwise/counter-clockwise chasing-border modes.
*   Added reusable frame configuration for fill blocks, combination styles, separate frame and fill color sequences, and shared animation offsets.
*   Added a templated :cpp:any:`BitmapDrawOptions <erbsland::cterm::BitmapDrawOptions>` color constructor so callers can pass the same color argument forms that other rendering APIs already accept.

Improved
--------

*   Frame rendering can now animate the border independently from the fill area, which makes status boxes, dashboards, and decorative panels much easier to build with the public API.
*   :cpp:any:`Tile9Style <erbsland::cterm::Tile9Style>` and :cpp:any:`Char16Style <erbsland::cterm::Char16Style>` frame rendering now integrate more cleanly with animated colors and base-color overlays through the unified :cpp:any:`FrameDrawOptions <erbsland::cterm::FrameDrawOptions>` workflow.

Implementation
--------------

*   Added the ``frame-color-animations`` demo plus focused unit tests for the new frame color and frame-index behavior.
*   Improved internal frame-rendering helpers and related documentation.


Version 1.2.0 - 2026-03-10
==========================

Release 1.2.0 brings richer rendering primitives, better Unicode handling, smarter terminal updates, and broader examples and test coverage.

Highlights
----------

*   Added :cpp:any:`BitmapDrawOptions <erbsland::cterm::BitmapDrawOptions>` and new :cpp:any:`Buffer::drawBitmap() <erbsland::cterm::Buffer::drawBitmap()>` overloads for configurable bitmap rendering with scale modes, animated color sequences, stripe-based color modes, custom block characters, and optional :cpp:any:`Char16Style <erbsland::cterm::Char16Style>` integration.
*   Added :cpp:any:`Tile9Style <erbsland::cterm::Tile9Style>` plus matching :cpp:any:`Buffer <erbsland::cterm::Buffer>` support for repeating 3x3 and extended 16-tile patterns when filling rectangles, drawing frames, and drawing filled frames.
*   Added :cpp:any:`Rectangle::gridCells() <erbsland::cterm::Rectangle::gridCells()>` to split layouts into evenly spaced grid cells.
*   Expanded :cpp:any:`Char <erbsland::cterm::Char>` and :cpp:any:`String <erbsland::cterm::String>` with UTF-32 support, multi-codepoint handling, improved combining-character support, and more flexible color overlay and base-color operations.
*   Improved color composition with explicit :cpp:any:`Inherited <erbsland::cterm::Inherited>` handling across :cpp:any:`ColorPart <erbsland::cterm::ColorPart>`, :cpp:any:`Color <erbsland::cterm::Color>`, text rendering, bitmap rendering, and buffer-based drawing.
*   Extended :cpp:any:`Terminal <erbsland::cterm::Terminal>` screen updates with clearer refresh strategies, optional line buffering, optional back-buffer diff updates, improved crop-mark handling, and better minimum-size rendering behavior.

Added
-----

*   New ``display-all-colors`` demo for exploring the color model and layout helpers.
*   New ``bitmap-showcase`` demo for bitmap rendering options, styles, and animated color modes.
*   New ``update-screen-modes`` demo for comparing clear, overwrite, and back-buffer update behavior.
*   New unit tests for :cpp:any:`Tile9Style <erbsland::cterm::Tile9Style>`, terminal update behavior, bitmap rendering paths, and :cpp:any:`Rectangle::gridCells() <erbsland::cterm::Rectangle::gridCells()>`.
*   Added ``display-all-colors``, ``bitmap-showcase``, and ``update-screen-modes`` demo applications.
*   Added :cpp:any:`BitmapDrawOptions <erbsland::cterm::BitmapDrawOptions>` and configurable bitmap rendering in :cpp:any:`Buffer <erbsland::cterm::Buffer>`.
*   Added :cpp:any:`Tile9Style <erbsland::cterm::Tile9Style>` and tile-based fill/frame rendering support.
*   Added :cpp:any:`Rectangle::gridCells() <erbsland::cterm::Rectangle::gridCells()>`.
*   Added UTF-32 and multi-codepoint support to :cpp:any:`Char <erbsland::cterm::Char>` and :cpp:any:`String <erbsland::cterm::String>`.

Improved
--------

*   Updated ``frame-weaver``, ``text-gallery``, ``retro-plasma``, and ``terminal-chronicle`` to use the newer rendering and color APIs.
*   Refined terminal output APIs and documentation around refresh modes, direct writes, buffer rendering, crop marks, and smart overwrites.
*   Improved bitmap, text, geometry, color, and output reference documentation.
*   Improved color overlay behavior and inherited color handling throughout the library.
*   Improved :cpp:any:`Terminal::updateScreen() <erbsland::cterm::Terminal::updateScreen()>` with refresh-mode, back-buffer, crop-mark, and minimum-size handling updates.

Implementation
--------------

*   Improved memory efficiency.
*   Expanded unit tests and API/reference documentation across rendering, geometry, text, and terminal output.


Version 1.0.0 — 2026-03-08
==========================

First public open-source release.
