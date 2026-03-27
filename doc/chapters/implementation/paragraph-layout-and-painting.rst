..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: implementation notes; paragraph layout
    single: implementation notes; paragraph painter
    single: implementation notes; paragraph printer

*****************************
Paragraph Layout and Painting
*****************************

This page explains the wrapped-paragraph pipeline in
``src/erbsland/cterm/impl/paragraph``.

The implementation is split into three main parts:

* ``impl::paragraph::Layout`` builds a reusable width-aware layout.
* ``impl::paragraph::Painter`` paints that layout into a
  :cpp:any:`WritableBuffer <erbsland::cterm::WritableBuffer>`.
* ``impl::paragraph::Printer`` streams the same layout through a
  :cpp:any:`CursorWriter <erbsland::cterm::CursorWriter>`.

``RendererBase`` provides the shared placement and background-fill rules for the
two renderers.

.. dropdown:: How to read the visuals on this page

    The visuals on this page are intentionally schematic.
    Plain-text diagrams show control flow and internal data structures.
    ANSI blocks show the final visible cells, with gray background cells used to
    expose spacing, indentation, and reserved areas that would otherwise be
    invisible.

Pipeline at a Glance
====================

The current rendering path looks like this:

.. mermaid::

    flowchart TD
        input["text + width + ParagraphOptions"] --> build["Layout::build()"]
        build --> split["splitIntoSourceLines()<br/>newline interpretation"]
        build --> prepare["prepareSourceLine()<br/>LayoutLineToken{Word|SeparatorSpace|Tab}"]
        split --> source["layoutSourceLine()"]
        prepare --> source
        source --> builder["LineBuilder"]
        builder --> line["buildLine()<br/>tryBuildLastLine()<br/>buildWrappedLine()<br/>buildTruncatedLine()"]
        line --> result["LayoutResult<br/>LayoutLine + LayoutFragment"]
        result --> painter["Painter::paint()"]
        result --> printer["Printer::print()"]
        painter --> buffer["WritableBuffer cells"]
        printer --> writer["CursorWriter output"]

Relevant Source Files
=====================

You will find the implementation primarily in:

* :file:`src/erbsland/cterm/impl/paragraph/Layout.hpp`
* :file:`src/erbsland/cterm/impl/paragraph/Layout.cpp`
* :file:`src/erbsland/cterm/impl/paragraph/LineBuilder.hpp`
* :file:`src/erbsland/cterm/impl/paragraph/LineBuilder.cpp`
* :file:`src/erbsland/cterm/impl/paragraph/LayoutPreparedSourceLine.hpp`
* :file:`src/erbsland/cterm/impl/paragraph/LayoutLineToken.hpp`
* :file:`src/erbsland/cterm/impl/paragraph/RendererBase.hpp`
* :file:`src/erbsland/cterm/impl/paragraph/Painter.hpp`
* :file:`src/erbsland/cterm/impl/paragraph/Painter.cpp`
* :file:`src/erbsland/cterm/impl/paragraph/Printer.hpp`
* :file:`src/erbsland/cterm/impl/paragraph/Printer.cpp`
* :file:`src/erbsland/cterm/Terminal.cpp`
* :file:`src/erbsland/cterm/CursorBuffer.cpp`
* :file:`src/erbsland/cterm/impl/TextPainter.cpp`

One Example Through the Pipeline
================================

The following simplified example is reused across several visuals below.
It keeps the moving parts small enough to inspect:

.. code-block:: text

    source line
        "Signal lanterns sailors home"

    width
        20 columns

    selected options
        wrappedLineIndent = 3
        lineBreakStartMark = "» "
        lineBreakEndMark = " ↩"

At the token and layout level, the data model looks like this:

.. code-block:: text

    prepared tokens
        [0] Word("Signal")
        [1] SeparatorSpace(" ")
        [2] Word("lanterns")
        [3] SeparatorSpace(" ")
        [4] Word("sailors")
        [5] SeparatorSpace(" ")
        [6] Word("home")

    physical lines
        line 0:
            indentWidth = 0
            wrapsToNext = true
            fragments = [SourceRange("Signal"), Spaces(1), SourceRange("lanterns")]

        line 1:
            indentWidth = 3
            wrapsToNext = false
            fragments = [LineBreakStartMark("» "), SourceRange("sailors"), Spaces(1), SourceRange("home")]

The wrap-end marker is not stored as a fragment inside ``LayoutLine``.
Instead, renderers derive it from ``wrapsToNext`` and
``ParagraphOptions::lineBreakEndMark()`` so it can stay anchored to the right
edge of the paragraph area.

After rendering, those two lines occupy the 20-cell grid like this:

.. erbsland-ansi::
    :escape-char: ␛
    :theme: ela-term

    ␛[90m01234567890123456789␛[39m
    ␛[97mSignal lanterns␛[100m   ␛[96m ↩␛[39;49m
    ␛[100m   ␛[96m» ␛[97msailors home␛[39;49m

Why the Pipeline Is Split
=========================

The library deliberately separates paragraph work into layout plus rendering:

* ``Layout`` turns source text and
  :cpp:any:`ParagraphOptions <erbsland::cterm::ParagraphOptions>` into a
  neutral ``LayoutResult``.
* ``Painter`` renders that result into an existing buffer and can use a color
  resolver for animated or context-sensitive colors.
* ``Printer`` renders the same result as streamed cursor-writer output.
* ``RendererBase`` keeps alignment and background-fill behavior identical
  between both rendering paths.

This split matters because the entry points have different output models while
sharing the same wrap rules:

* :cpp:any:`Terminal::printParagraph() <erbsland::cterm::Terminal::printParagraph()>`
  and :cpp:any:`CursorBuffer::printParagraph() <erbsland::cterm::CursorBuffer::printParagraph()>`
  use ``Layout`` plus ``Printer``.
* ``TextPainter::drawText()`` uses ``Layout`` plus ``Painter``.
* Layout failure is detected before any drawing occurs, so the caller can apply
  :cpp:any:`ParagraphOnError <erbsland::cterm::ParagraphOnError>` consistently.

Where the Pipeline Is Used
==========================

The layout engine supports two newline modes:

* ``LayoutNewlineMode::HardLineBreak``:
  each newline starts a new source line inside one printed paragraph.
  This is used for terminal and cursor-writer style output.
* ``LayoutNewlineMode::ParagraphBreak``:
  each newline starts a new paragraph.
  This is used when text is rendered inside a rectangle.

That means:

* :cpp:any:`Terminal::printParagraph() <erbsland::cterm::Terminal::printParagraph()>`
  and :cpp:any:`CursorBuffer::printParagraph() <erbsland::cterm::CursorBuffer::printParagraph()>`
  treat embedded newlines as hard line breaks and append
  :cpp:any:`ParagraphSpacing <erbsland::cterm::ParagraphSpacing>` only after the
  whole call finishes.
* ``TextPainter::drawText()`` treats embedded newlines as paragraph boundaries,
  so paragraph spacing is inserted between the resulting paragraphs.

The Layout Result Data Model
============================

``Layout::build()`` produces a ``LayoutResult`` with:

* ``valid`` – ``false`` if the chosen width and paragraph settings cannot
  produce any visible layout
* ``sourceText`` – the original string used by source-range fragments
* ``options`` – the paragraph options used for the build
* ``lines`` – the rendered physical lines

Each ``LayoutLine`` stores:

* ``indentWidth`` – the width reserved before the first visible fragment
* ``wrappedFromPrevious`` – whether this line continues a previous physical line
* ``wrapsToNext`` – whether the renderer must add the configured wrap-end marker
* ``fragments`` – an ordered sequence of:

  * ``SourceRange`` fragments that point back into the original text
  * generated ``Spaces``
  * ``LineBreakStartMark``
  * ``WordBreakMark``
  * ``ParagraphEllipsis``

This is enough for renderers to position text, keep the wrap-end marker aligned
to the right edge, and apply background-fill rules without re-running layout.

How Input Is Split into Source Lines
====================================

``Layout::build()`` starts with two structural steps:

#. Widths less than or equal to zero immediately produce ``valid = false``.
#. ``splitIntoSourceLines()`` splits the input on newline characters.

The resulting source-line ranges are then interpreted differently depending on
the newline mode:

* In ``HardLineBreak`` mode, all source lines belong to one rendered paragraph.
* In ``ParagraphBreak`` mode, each source line is laid out as its own paragraph.

Two details are easy to miss:

* Interior empty source lines are preserved, so explicit blank lines stay
  visible.
* A trailing newline does not create an extra trailing source line because
  ``splitIntoSourceLines()`` only appends the final range when it still contains
  characters.

When paragraph-break mode is active and
:cpp:any:`ParagraphSpacing::DoubleLine <erbsland::cterm::ParagraphSpacing::DoubleLine>`
is enabled, ``build()`` inserts one empty physical line between paragraphs.

Preparing a Source Line
=======================

Each source line is normalized by ``prepareSourceLine()`` into a
``LayoutPreparedSourceLine``. This step keeps enough structure for the later
line builder to handle separators, tabs, colors, and indentation correctly.

The prepared token stream uses three token kinds:

* ``Word`` – a contiguous source word plus its cached display width
* ``SeparatorSpace`` – one collapsed separator run that later becomes one
  rendered space
* ``Tab`` – a left-aligned tab that will be resolved later from the current
  column and the configured tab stops

Tokenization depends on alignment:

* For left-aligned paragraphs, TAB characters become dedicated ``Tab`` tokens.
* For centered or right-aligned paragraphs, tabs are only separators if they are
  contained in ``ParagraphOptions::wordSeparators()``.

``LayoutPreparedSourceLine`` also keeps pending spacing tokens until the next
word is known. That gives the implementation two useful behaviors:

* leading separator runs are dropped entirely
* leading tabs are preserved for left-aligned paragraphs and resolved later from
  tab-stop settings

Visual Example of Token Preparation
-----------------------------------

.. code-block:: text

    source line
        "  title:\talpha,,,beta"

    separators include ','

    prepared tokens
        [0] Word("title:")
        [1] Tab
        [2] Word("alpha")
        [3] SeparatorSpace(",,,")
        [4] Word("beta")

The important detail is that the three commas do not survive as three rendered
cells. They collapse into one logical separator token that later becomes one
generated space with the separator run's color.

Why Empty Source Lines Are Preserved
------------------------------------

If ``prepareSourceLine()`` produces no tokens, ``layoutSourceLine()`` appends
an empty ``LayoutLine`` immediately.

That keeps explicit blank input lines visible in both newline modes. In
paragraph-break mode, the configured paragraph spacing is then applied around
those empty paragraphs in exactly the same way as for non-empty ones.

Building Physical Lines
=======================

``layoutSourceLine()`` turns one prepared source line into one or more physical
lines by creating a ``LineBuilder``.

The builder tracks:

* ``tokenIndex`` – the next token in the prepared source line
* ``wordOffset`` – the current offset inside a split word token
* ``tabStopIndex`` – the next configured tab stop to consume

For one source line, the control flow is:

.. mermaid::

    flowchart TD
        start["layoutSourceLine()"] --> prepare["prepareSourceLine()"]
        prepare --> empty{"tokens.empty()?"}
        empty -->|yes| blank["append empty LayoutLine"]
        empty -->|no| loop["LineBuilder loop"]
        loop --> last["tryBuildLastLine()"]
        last --> lastOk{"all remaining content fits?"}
        lastOk -->|yes| appendLast["append final line"]
        lastOk -->|no| limit{"wrap limit reached?"}
        limit -->|yes| truncate["buildTruncatedLine()"]
        limit -->|no| wrapped["buildWrappedLine()"]
        truncate --> appendTruncated["append truncated line"]
        wrapped --> appendWrapped["append wrapped line and continue"]

This keeps the last line, wrapped lines, and truncated lines on the same
``buildLine()`` foundation instead of maintaining three unrelated code paths.

The ``buildLine()`` Algorithm
-----------------------------

``buildLine()`` is the core routine behind:

* ``tryBuildLastLine()``
* ``buildWrappedLine()``
* ``buildTruncatedLine()``

Its steps are:

#. Reserve suffix width for either the wrap-end marker or the paragraph
   ellipsis.
#. Build the prefix from ``indentWidth`` plus an optional start mark on wrapped
   continuation lines.
#. Evaluate the next spacing run until the next word token is reached.
#. If the next whole word fits, append the spacing run and the word.
#. If the word does not fit but the line already contains visible content, stop
   and let the caller emit a wrapped or truncated continuation line.
#. If the word does not fit and the line is still empty, try splitting the word.
#. If even that cannot place any visible content, return ``std::nullopt`` so the
   whole layout becomes invalid.

Two details matter here:

* The optional wrap-start mark is only added for wrapped continuation lines in
  left-aligned paragraphs.
* The line builder counts automatic wraps per source line. A new source line
  creates a new ``LineBuilder``, which is why hard line breaks reset the wrap
  counter.

Spacing Runs, Tabs, and Separators
----------------------------------

Spacing is processed as a run before the next word, not as isolated tokens.
That is why multiple separators still collapse cleanly and tabs can force a
line break before the next visible word is emitted.

Rules for separator tokens:

* leading separator runs are ignored
* once visible content exists on the line, a separator run contributes exactly
  one generated space

Rules for tabs in left-aligned paragraphs:

* each tab consumes the next entry from ``ParagraphOptions::tabStops()``
* ``resolveTabStop()`` resolves
  :cpp:any:`cTabWrappedLineIndent <erbsland::cterm::ParagraphOptions::cTabWrappedLineIndent>`
  to the configured wrapped-line indent
* if the resolved tab stop is ahead of the current column, the tab expands into
  that many spaces

If the resolved stop does not advance the current column, or if no further stop
exists, ``tabOverflowBehavior()`` decides what happens next:

* ``AddSpace`` inserts one space and continues
* ``LineBreak`` consumes the tab and continues on the next physical line

The implementation also includes one safeguard for ``LineBreak``:
if the tab appears at the start of a line and would not add any visible content,
the tab is consumed and dropped instead of producing an endless stream of empty
wrapped lines.

Word Splitting
--------------

Splitting is handled by ``LayoutLineToken::split()`` and is only attempted when
a word does not fit and the current physical line still contains no visible
text.

The split logic:

* reserves room for ``wordBreakMark()`` when a wrapped line is being built
* walks the word character by character
* counts visible width using each ``Char``'s display width
* skips zero-width characters when calculating the consumed width
* stops before overflow
* adds the break mark only when there is still source text left for later lines

Because the split works on ``Char`` display widths, it stays consistent with the
library's Unicode width system.

Truncation and Wrap Limits
--------------------------

``maximumLineWraps()`` limits the number of automatic wraps produced while
laying out one source line.

When the limit is reached, ``buildTruncatedLine()`` rebuilds the current line
with width reserved for ``paragraphEllipsisMark()``.

Important consequences:

* hard line breaks and paragraph breaks reset the wrap counter because they
  start a new source line
* the ellipsis consumes real width and can itself make the layout invalid
* truncation still respects indentation, start marks, tabs, and Unicode width

Rendering the Layout
====================

Once a ``LayoutResult`` exists, rendering is handled by ``Painter`` or
``Printer``. Neither renderer performs line breaking; they only consume the
precomputed layout.

Shared Placement Rules
----------------------

``RendererBase::linePlacement()`` computes three values for every physical line:

* ``textX`` – where the text and non-end-mark fragments begin
* ``textWidth`` – the width of ``indentWidth`` plus all fragments
* ``endMarkX`` – where the wrap-end marker begins if ``wrapsToNext`` is true

The end-mark width is removed from the available alignment width first.
This keeps the configured wrap-end marker anchored to the right edge of the
paragraph area even when the text itself is centered or right-aligned.

Schematic placement example:

.. code-block:: text

    paragraph width  = 24
    endMark width    = 2
    text width space = 22

    012345678901234567890123
        Harbor reports     ↩
        ^                  ^
        textX              endMarkX

Painting into Buffers
---------------------

``Painter::paint()`` determines how many layout lines fit into the target
rectangle, applies vertical alignment, and then draws each line into the buffer.

For each visible line it:

#. computes the final placement with ``linePlacement()``
#. draws the line fragments starting at ``textX + indentWidth``
#. draws the wrap-end marker at ``endMarkX`` when ``wrapsToNext`` is set
#. applies left and right background fill according to
   :cpp:any:`ParagraphBackgroundMode <erbsland::cterm::ParagraphBackgroundMode>`

``Painter`` uses an optional color resolver. When present, each character color
is resolved against the current position before it is merged with the buffer's
existing base color.

Printing through CursorWriter
-----------------------------

``Printer::print()`` uses the same placement rules, but because it streams text
instead of mutating an existing buffer, it must materialize indentation and
padding as spaces.

That leads to two practical differences from ``Painter``:

* left padding, indentation, gaps before the end mark, and trailing fill are all
  written explicitly as spaces
* spaces not covered by wrapped-text background use the writer's current
  background color

``Printer`` resolves each emitted character against the writer's current base
style before sending it to ``CursorWriter``.

Background Extension Modes
==========================

Background fill is controlled by
:cpp:any:`ParagraphBackgroundMode <erbsland::cterm::ParagraphBackgroundMode>`.

``RendererBase`` splits the logic into three decisions:

* whether continuation indents use left fill
* whether wrapped lines use right fill
* whether the final physical line also uses right fill

Left fill:

* uses the last visible color from the previous wrapped line
* applies only to the continuation indent of the next line
* is enabled by ``WrappedLeft``, ``WrappedBoth``, and ``FullBoth``

Right fill:

* uses the last visible color on the current physical line
* fills either up to the wrap-end marker or, for ``FullRight`` / ``FullBoth``,
  to the right edge of the paragraph area

Two rendering paths expose the same modes differently:

* ``Painter`` preserves the surrounding buffer content wherever the selected
  mode does not paint.
* ``Printer`` must emit spaces, so non-filled padding uses the current
  cursor-writer background instead.

Visual Example of ``WrappedBoth``
---------------------------------

The following block shows both fill directions at once:

* right fill continues the wrapped line's background up to the end mark
* left fill carries that background into the continuation indent

.. erbsland-ansi::
    :escape-char: ␛
    :theme: ela-term

    ␛[90m01234567890123456789␛[39m
    ␛[97;44mSignal lanterns␛[44m   ␛[96;44m ↩␛[39;49m
    ␛[44m   ␛[96;44m» ␛[97;44msailors home␛[39;49m

``FullRight`` and ``FullBoth`` keep going one step further: their final line
also fills to the right edge of the paragraph area even when it no longer
wraps.
