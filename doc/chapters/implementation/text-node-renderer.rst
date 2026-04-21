..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: implementation notes; TextNodeRenderer
    single: implementation notes; rich text rendering
    single: implementation notes; text node renderer

****************
TextNodeRenderer
****************

This page describes how ``TextNodeRenderer`` turns a parsed
:cpp:any:`TextNode <erbsland::cterm::text::TextNode>` tree into either:

* a human-readable :cpp:any:`String <erbsland::cterm::String>` via
  ``renderString()``
* streamed terminal output via ``renderTo(CursorWriter)``

The implementation uses a single streaming pipeline.
The tree is walked once, physical ``RenderBlock`` values are emitted as
they are discovered, and the selected output target consumes those
blocks immediately.

No normal rendering path builds a complete block vector before output.
The only retained block is the newest pending block inside
``BlockStream`` so closing container scopes can still apply bottom
margins to the last descendant.

Relevant Source Files
=====================

The public renderer entry point lives in:

* :file:`src/erbsland/cterm/text/impl/TextNodeRenderer.hpp`
* :file:`src/erbsland/cterm/text/impl/TextNodeRenderer.cpp`

The internal streaming renderer lives in
:file:`src/erbsland/cterm/text/impl/text_node_renderer/`:

* :file:`BlockTarget.hpp`
* :file:`BlockStream.hpp`
* :file:`CursorBlockTarget.hpp`
* :file:`CursorBlockTarget.cpp`
* :file:`InlineTextRenderer.hpp`
* :file:`InlineTextRenderer.cpp`
* :file:`ListItemLayout.hpp`
* :file:`ListItemLayout.cpp`
* :file:`ListPrefix.hpp`
* :file:`RenderBlock.hpp`
* :file:`RenderBlock.cpp`
* :file:`RenderContext.hpp`
* :file:`RenderContext.cpp`
* :file:`StringBuilderWithWhitespaceState.hpp`
* :file:`StringBuilderWithWhitespaceState.cpp`
* :file:`StringBlockTarget.hpp`
* :file:`StyleResolver.hpp`
* :file:`StyleResolver.cpp`
* :file:`TreeRenderer.hpp`
* :file:`TreeRenderer.cpp`

High-Level Architecture
=======================

At a high level the subsystem looks like this:

.. mermaid::

    flowchart TD
        entry["TextNodeRenderer"] --> target{"output mode"}
        target --> stringTarget["StringBlockTarget"]
        target --> cursorTarget["CursorBlockTarget"]
        stringTarget --> treeString["TreeRenderer"]
        cursorTarget --> treeCursor["TreeRenderer"]
        treeString --> streamString["BlockStream"]
        treeCursor --> streamCursor["BlockStream"]
        streamString --> stringOut["String"]
        streamCursor --> terminalOut["CursorWriter"]

``TextNodeRenderer`` is intentionally thin.
It stores the document pointer and the style pointer, creates the
appropriate target, and asks ``TreeRenderer`` to render the document.

The renderer has four main collaborators:

.. list-table::
    :header-rows: 1

    * - Type / file
      - Responsibility
    * - ``TreeRenderer``
      - Walk the ``TextNode`` tree, resolve block shapes, manage scopes, and emit ``RenderBlock`` values.
    * - ``StyleResolver``
      - Resolve style rules for block roles, inline roles, list kinds, levels, and style-token selectors.
    * - ``InlineTextRenderer``
      - Render inline subtrees into styled ``String`` values using whitespace-aware string building.
    * - ``BlockStream``
      - Hold one pending block, flush older blocks to the selected target, and keep the pending block patchable.

The target side is deliberately small:

.. list-table::
    :header-rows: 1

    * - Type / file
      - Responsibility
    * - ``BlockTarget``
      - Abstract sink for streamed render blocks.
    * - ``StringBlockTarget``
      - Build the plain string representation used by ``renderString()``.
    * - ``CursorBlockTarget``
      - Write the terminal representation to a :cpp:any:`CursorWriter <erbsland::cterm::CursorWriter>`.

Streaming Pipeline
==================

The active rendering path is:

.. mermaid::

    flowchart TD
        nodeTree["TextNode tree"] --> tree["TreeRenderer::render()"]
        tree --> resolver["StyleResolver"]
        tree --> inline["InlineTextRenderer"]
        tree --> context["RenderContext"]
        tree --> scopes["open BlockScope stack"]
        inline --> block["RenderBlock"]
        resolver --> block
        context --> block
        scopes --> block
        block --> stream["BlockStream"]
        stream --> target["BlockTarget"]

``TreeRenderer::appendNode()`` dispatches each node by type:

* semantic containers open a scope and render block-emitting children
* bullet and numbered lists stream their list items directly
* paragraph-like nodes render inline text and emit one paragraph block
* headings emit either a paragraph block or a filled-line block
* horizontal rules emit a horizontal-rule block
* inline nodes render as paragraph-like content only when the current
  tree position allows that behavior

Direct inline and empty children of document, section, blockquote, and
definition-list containers are ignored.
This preserves the renderer contract that these containers only expose
block-level children.
List items are different: inline and text children of a list item are
valid item content and become paragraph-like blocks.

Render Context
==============

``RenderContext`` carries inherited state that descendants need:

* the inherited text-style overlay
* accumulated horizontal margins from ancestor containers

It does not own vertical edge-margin decisions.
Top and bottom margins are scope behavior in ``TreeRenderer`` because
they depend on which descendant becomes the first or last emitted block.

``RenderContext`` creates derived contexts for:

* semantic containers
* horizontal-margin inheritance
* list-item bodies

It also resolves the final text style and final block style for a
``RenderBlock``.

Style Resolution
================

``StyleResolver`` owns all renderer-facing style lookup.
It stores the :cpp:any:`Style <erbsland::cterm::text::Style>` pointer by
value, so the resolver does not depend on an external reference staying
alive.

It resolves:

* base text style
* block rules for semantic roles
* heading and list levels
* bullet versus numbered list rules
* horizontal-rule style
* node-local style-token selectors

Rules without node-local style tokens are cached.
Nodes with style tokens are resolved directly from normalized tokens.
Token splitting trims whitespace, removes duplicates, and sorts tokens
so selector matching stays deterministic.

Inline Rendering
================

``InlineTextRenderer`` renders inline node subtrees into styled
``String`` values.

Its responsibilities are:

* append literal text, error, and unsupported nodes
* preserve explicit line breaks as newline characters
* apply inline style overlays for emphasis, strong text, underline,
  links, inline code, and styled spans
* respect the selected whitespace mode

``StringBuilderWithWhitespaceState`` keeps whitespace handling local to
inline rendering.
The tree renderer only asks for a final styled string and does not know
how inline whitespace was normalized.

Render Blocks
=============

``RenderBlock`` is the single value type passed through the block
stream.
It represents one physical block in one of these shapes:

* paragraph-like text
* filled-line text, used by filled headings
* horizontal rule

The block owns its resolved ``StyleRule``.
That rule includes inherited horizontal margins and all block-level
indentation needed by the output target.

List prefixes are stored as optional ``ListPrefix`` data on the first
block of a list item.
The prefix is not copied into both the block text and an additional
terminal-text field.
``CursorBlockTarget`` combines the terminal prefix with the block text
only when it writes the paragraph.
``RenderBlock::renderString()`` uses the same prefix information for the
plain-string representation.

``RenderBlock`` also keeps string-only indentation fields.
These preserve the current ``renderString()`` behavior without forcing
terminal output to carry duplicate text.

Block Scopes
============

``TreeRenderer`` keeps a stack of open ``BlockScope`` values.
Each scope stores:

* vertical margins for the container or list item
* optional ``ListItemLayout`` information
* whether the scope has already emitted a block

When ``TreeRenderer`` emits a block, every open scope sees that block.
For the first emitted block in a scope, the scope contributes its top
margin.
For a list-item scope, the first emitted block also receives the list
prefix layout.
Later blocks in the same list item receive continuation indentation.

When a scope closes, the scope patches the currently pending block in
``BlockStream`` with its bottom margin.
That pending block is the last emitted descendant of the scope.
This is why the stream keeps one block back before flushing it to the
target.

List Rendering
==============

Lists are rendered directly while the tree is walked.
There is no temporary vector of blocks for each list item.

For each list item:

1. ``TreeRenderer`` resolves the item rule and creates a
   ``ListItemLayout``.
2. A list-item scope opens with that layout.
3. Child nodes stream normally.
4. The first emitted child block receives the marker prefix.
5. Later emitted child blocks receive continuation indentation.
6. The scope closes and applies the item bottom margin to the last
   emitted child block.

If a list item starts with a nested list, the renderer first emits an
empty paragraph-like block for the item itself.
This keeps the marker visible on its own line before the nested list.

If a list item emits no content at all, the renderer emits one empty
list-item paragraph before closing the item scope.

Output Targets
==============

``renderString()``
------------------

The plain-string path creates a ``StringBlockTarget`` and streams the
tree into it.

``StringBlockTarget``:

* appends one newline between streamed blocks
* delegates each block to ``RenderBlock::renderString()``
* trims outer newline-like separators from the final result

This remains a semantic string renderer.
It does not simulate terminal width, wrapping, or cursor movement.

``renderTo(CursorWriter)``
--------------------------

The terminal path creates a ``CursorBlockTarget`` and streams the tree
into it.

``CursorBlockTarget`` owns physical terminal emission:

* vertical margin collapse
* conversion from block styles to ``ParagraphOptions``
* paragraph writing
* filled-line painting
* horizontal-rule painting

The tree renderer never writes directly to the terminal.

Allocation Notes
================

Allocation discipline in this subsystem comes from the shape of the
pipeline:

* there is no full ``RenderBlock`` list for normal output
* ``BlockStream`` buffers only the newest block
* inline rendering reserves from the node's estimated inline capacity
* no-style selector lookups are cached in ``StyleResolver``
* list prefixes are stored separately and combined only by the target
  that needs the combined text
* list items stream directly instead of first collecting all descendant
  blocks

The design favors readable ownership over micro-optimization, but the
main avoidable allocations from the previous collect-then-emit model are
removed from the hot path.

Where to Change What
====================

When extending the renderer, start in the component that owns the
relevant behavior.

Adding a new block-emitting ``TextNode::Type``
----------------------------------------------

Start in ``TreeRenderer::appendNode()``.
Then choose the matching helper:

* semantic container
* list node
* paragraph-like block
* heading
* horizontal rule

Changing inherited text style or horizontal margins
---------------------------------------------------

Start in ``RenderContext``.

Changing vertical edge-margin behavior
--------------------------------------

Start in the ``BlockScope`` handling in ``TreeRenderer`` and the
one-block buffering behavior in ``BlockStream``.

Changing selector matching or node token handling
-------------------------------------------------

Start in ``StyleResolver``.

Changing inline rendering behavior
----------------------------------

Start in ``InlineTextRenderer`` and
``StringBuilderWithWhitespaceState``.

Changing list prefix placement or continuation layout
-----------------------------------------------------

Start in:

* ``TreeRenderer::appendListItem()``
* ``TreeRenderer::emitBlock()``
* ``ListItemLayout``

Changing block string projection
--------------------------------

Start in ``RenderBlock::renderString()``.

Changing plain-string collection
--------------------------------

Start in ``StringBlockTarget``.

Changing terminal-only emission behavior
----------------------------------------

Start in ``CursorBlockTarget``.

Testing Strategy
================

The primary semantic coverage lives in:

* :file:`test/unittest/src/impl/text/render/HtmlRendererTest.cpp`
* :file:`test/unittest/src/impl/text/render/TextNodeRendererTest.cpp`

These tests exercise the public rendering behavior through
``HtmlRenderer`` and ``TextNodeRenderer``.

Focused internal tests cover the streaming collaborators:

* :file:`test/unittest/src/impl/text/text-node/InlineTextRendererTest.cpp`
* :file:`test/unittest/src/impl/text/text-node/RenderContextTest.cpp`
* :file:`test/unittest/src/impl/text/text-node/StyleResolverTest.cpp`
* :file:`test/unittest/src/impl/text/text-node/TreeRendererTest.cpp`

Use both levels:

* end-to-end tests for user-visible rendering behavior
* focused component tests for subtle internal invariants

Summary
=======

The renderer now follows this high-level ownership model:

* ``TextNodeRenderer`` selects the target
* ``TreeRenderer`` walks the tree and streams blocks
* ``StyleResolver`` owns style lookup
* ``InlineTextRenderer`` owns inline text construction
* ``RenderContext`` carries inherited text style and horizontal margins
* ``BlockStream`` keeps one pending block for scope finalization
* ``StringBlockTarget`` and ``CursorBlockTarget`` own the output format

The important structural rule is that tree walking, style lookup,
inline text construction, scope handling, and output emission each have
one clear owner.
That keeps the renderer easier to read, easier to extend, and cheaper to
run than a separate collection phase followed by a second output phase.
