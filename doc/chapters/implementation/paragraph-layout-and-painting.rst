..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: implementation notes; paragraph layout
    single: implementation notes; paragraph painter

*****************************
Paragraph Layout and Painting
*****************************

This page explains the internal paragraph rendering pipeline used for wrapped text.
It focuses on :cpp:any:`ParagraphLayout <erbsland::cterm::impl::ParagraphLayout>` and
:cpp:any:`ParagraphPainter <erbsland::cterm::impl::ParagraphPainter>`.

Together, these components transform a logical paragraph into positioned and painted terminal cells.

Relevant Source Files
=====================

You will find the implementation primarily in:

* :file:`src/erbsland/cterm/impl/ParagraphLayout.hpp`
* :file:`src/erbsland/cterm/impl/ParagraphLayout.cpp`
* :file:`src/erbsland/cterm/impl/ParagraphPainter.hpp`
* :file:`src/erbsland/cterm/impl/ParagraphPainter.cpp`
* :file:`src/erbsland/cterm/Terminal.cpp`
* :file:`src/erbsland/cterm/impl/TextPainter.cpp`

Why the Rendering Is Split into Two Classes
===========================================

The library deliberately separates paragraph rendering into two distinct phases:

#. ``ParagraphLayout`` converts source text and :cpp:any:`ParagraphOptions <erbsland::cterm::ParagraphOptions>`
   into a neutral layout result.
#. ``ParagraphPainter`` renders that result into a concrete
   :cpp:any:`WritableBuffer <erbsland::cterm::WritableBuffer>`.

This separation addresses several design goals:

* The same wrapping logic is reused across different front ends.
  ``Terminal::printParagraph()`` builds a temporary buffer for direct output,
  while ``TextPainter::drawText()`` renders into an existing screen buffer.
* Layout can fail early, before any drawing occurs.
  If indentation, markers, or width constraints make rendering impossible,
  the caller receives ``Result.valid == false`` and can react accordingly.
* Painting can apply context-specific coloring without affecting layout logic.
  ``TextPainter`` uses a color resolver for dynamic effects,
  while ``Terminal`` follows a simpler default path.
* The layout result retains semantic information such as ``indentWidth``,
  ``wrapsToNext``, and the separately aligned ``endMark``.
  This allows the painter to implement background rules without recomputing layout decisions.

Where the Pipeline Is Used
==========================

There are currently two primary entry points:

* ``Terminal::printParagraph()`` uses ``ParagraphLayout::NewlineMode::HardLineBreak``.
  Every newline in the source text becomes a line break within a single paragraph.
* ``TextPainter::drawText()`` uses ``ParagraphLayout::NewlineMode::ParagraphBreak``.
  Each newline starts a new paragraph, allowing spacing between paragraphs.

This distinction is subtle but important.
It allows the same layout engine to support both immediate terminal output
and structured layout inside a defined rectangle—without duplicating logic.

The Layout Result Data Model
============================

``ParagraphLayout`` produces a ``Result`` consisting of physical ``Line`` objects.

Each ``Line`` contains:

* ``text`` – the visible content, including indentation and optional start markers
* ``endMark`` – an optional right-aligned wrap marker, painted separately
* ``indentWidth`` – the width of the indentation area before visible content
* ``wrappedFromPrevious`` – whether this line continues a previous one
* ``wrapsToNext`` – whether this line continues onto the next one

Separating ``text`` from ``endMark`` is intentional.
The end mark must remain visually aligned to the right edge,
independent of text alignment within the available space.

How Input Is Split into Paragraphs
==================================

``build()`` begins with validation and then processes the input in two steps:

#. Widths less than or equal to zero immediately result in ``valid = false``.
#. ``splitInputIntoParagraphs()`` determines how newline characters are interpreted.

The splitting rules are:

* In ``HardLineBreak`` mode, the entire input is treated as a single paragraph with multiple lines.
* In ``ParagraphBreak`` mode, each source line becomes its own paragraph.

When paragraph-break mode is active and
:cpp:any:`ParagraphSpacing::DoubleLine <erbsland::cterm::ParagraphSpacing::DoubleLine>` is enabled,
``build()`` inserts an empty physical line between paragraphs.

Preparing a Source Line
=======================

Each source line is normalized by ``prepareSourceLine()`` into a sequence of ``WordItem`` objects.
This step is central to the layout algorithm.

What ``prepareSourceLine()`` Preserves
--------------------------------------

Instead of simply splitting on ASCII spaces,
the implementation preserves enough structure to accurately reproduce:

* collapsible inter-word spacing
* tab stops
* spacing colors
* the distinction between leading indentation and regular spacing

Each ``WordItem`` contains:

* ``word`` – the textual content
* ``prefixSpacing`` – the spaces and tabs preceding the word

Spacing is stored as ``SpacingElement`` values rather than expanded text.
This allows tabs to be resolved later based on the current column.

Word Separators and Tabs
------------------------

Parsing depends on alignment:

* For left-aligned paragraphs, TAB characters are treated as tab-stop elements.
* For centered or right-aligned paragraphs, TAB is only treated as a separator
  if included in ``ParagraphOptions::wordSeparators()``.

Separators are defined by ``ParagraphOptions::wordSeparators()``.
Consecutive separators collapse into at most one rendered space.

Two subtle but important details:

* Leading spaces are removed via ``removeLeadingSpaces()``.
  A paragraph does not start with arbitrary spacing unless explicitly configured.
* ``spaceFrom()`` converts separators into colored spaces,
  preserving their visual appearance without rendering the original characters.

Why Empty Source Lines Are Preserved
------------------------------------

If ``prepareSourceLine()`` produces no words,
``layoutSourceLine()`` emits an empty physical ``Line``.

This ensures that explicit blank lines remain visible in ``HardLineBreak`` mode.

Building Physical Lines
=======================

``layoutSourceLine()`` transforms prepared input into one or more physical lines.
Progress is tracked using ``State``:

* ``wordIndex`` – current word
* ``wordOffset`` – offset within a split word
* ``spacingOffset`` – remaining prefix spacing
* ``tabStopIndex`` – next tab stop to use

The algorithm proceeds as follows:

#. ``tryBuildLastLine()`` checks whether all remaining content fits.
#. If not, and the wrap limit is reached, ``buildTruncatedLine()`` produces the final line.
#. Otherwise, ``buildWrappedLine()`` creates a continuation line and advances the state.

This structure avoids special handling for the final line—it is simply a normal build with stricter constraints.

The ``buildLine()`` Algorithm
-----------------------------

``buildLine()`` is the core routine used for all line types.

It performs these steps:

#. Reserve space for suffix elements such as end marks or ellipsis.
#. Build the prefix (indentation and optional start mark for wrapped lines).
#. Process the current word and its prefix spacing.
#. Expand spacing using ``evaluatePrefixSpacing()``.
#. If the full word fits, append it and continue.
#. If it does not fit and the line already has content, stop here.
#. If it does not fit and the line is still empty, attempt splitting via ``buildSplitWord()``.
#. If no content can be placed, return ``std::nullopt``.

This final case is what results in ``Result.valid == false``.

Indentation and Wrapped-Line Start Marks
----------------------------------------

Indentation is only applied for left-aligned paragraphs.

``calculateIndentWidth()`` returns:

* ``firstLineIndent()`` for the first line
* ``wrappedLineIndent()`` for continuation lines
* ``0`` for centered or right-aligned paragraphs

If a continuation line uses a start mark,
it is appended after the indentation.

Tabs and Prefix Spacing Resolution
----------------------------------

``evaluatePrefixSpacing()`` converts ``SpacingElement`` entries into concrete output.

Rules for spaces:

* Leading spaces at the beginning of a line are ignored.
* Subsequent spaces are rendered normally.

Tabs behave differently:

* Each tab consumes one entry from ``ParagraphOptions::tabStops()``.
* ``resolveTabStop()`` supports ``cTabWrappedLineIndent`` to reuse indentation as a tab stop.
* If a tab advances the column, it expands into that many spaces.

If no advancement occurs or no tab stops remain,
``tabOverflowBehavior()`` applies:

* ``AddSpace`` → replace with a single space
* ``LineBreak`` → break the line (with safeguards to avoid infinite loops)

Word Splitting
--------------

``buildSplitWord()`` is used only when:

* a word does not fit, and
* the line contains no visible content yet

This ensures splitting is a fallback, not the default.

The algorithm:

* reserves space for ``wordBreakMark()`` if configured
* iterates character by character
* ignores zero-width characters
* stops before overflow
* appends the break mark only if a split occurred

Because it operates on ``Char`` values,
it respects display width, including wide and combining characters.

Truncation and Wrap Limits
--------------------------

``maximumLineWraps()`` limits how often a line may wrap.

When the limit is reached,
``buildTruncatedLine()`` rebuilds the line with space reserved for
``paragraphEllipsisMark()``.

Important consequences:

* Hard line breaks reset the wrap counter.
* The ellipsis consumes real width and can cause layout failure.
* Truncation remains width-aware.

How Painting Works
==================

Once a ``ParagraphLayout::Result`` is available,
``ParagraphPainter`` maps it into a ``WritableBuffer``.

The painter does not perform layout.
It only positions lines and applies visual rules.

Vertical and Horizontal Placement
---------------------------------

``paint()`` determines how many lines fit into the target rectangle.
Excess lines are clipped.

Vertical placement:

* top → ``rect.y1()``
* center → offset by unused height
* bottom → align last line to ``rect.y2()``

Horizontal placement:

The ``endMark`` width is excluded first,
ensuring it remains anchored at the right edge.

The remaining text is aligned within the reduced width.

Drawing Characters
------------------

``drawSegment()`` renders a string and returns the final color.

For each character:

#. Skip zero-width characters
#. Stop if exceeding the right boundary
#. Check bounds
#. Resolve colors
#. Write the character
#. Advance by display width

Color handling differs:

* Without resolver → merge with existing buffer via ``withBaseColor()``
* With resolver → apply dynamic color before merging

The latter is used by ``TextPainter``.

Background Extension Modes
==========================

Background behavior is controlled by
:cpp:any:`ParagraphBackgroundMode <erbsland::cterm::ParagraphBackgroundMode>`.

This logic is separate from layout.

Right-Side Fill
---------------

The background extends using the color of the last visible character.

* With ``endMark`` → fill up to the mark
* Without → fill to the right edge depending on mode

Left-Side Fill for Continuation Indents
---------------------------------------

Left fill uses ``previousWrapColor``.

If a line wraps:

* the last visible color is stored
* the next line uses it to fill indentation

This relies on ``indentWidth`` and ``wrapsToNext`` from the layout result.
