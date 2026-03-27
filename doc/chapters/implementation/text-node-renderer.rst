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

The implementation uses a two-stage pipeline:

* plan semantic nodes into a linear list of ``RenderBlock`` objects
* feed that block plan into either plain-string rendering or terminal
  emission

``RenderPlanner`` acts as the orchestrator.
The planning work is split into named internal components with separate
headers, so the subsystem stays testable and easier to extend.

Relevant Source Files
=====================

The rich-text renderer spans these main files:

* :file:`src/erbsland/cterm/text/impl/TextNodeRenderer.hpp`
* :file:`src/erbsland/cterm/text/impl/TextNodeRenderer.cpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/RenderPlanner.hpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/RenderPlanner.cpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/PlanningContext.hpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/PlanningContext.cpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/PlanningStyleResolver.hpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/PlanningStyleResolver.cpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/InlineTextPlanner.hpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/InlineTextPlanner.cpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/ListBlockPlanner.hpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/ListBlockPlanner.cpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/RenderBlockFactory.hpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/RenderBlockFactory.cpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/TextNodePlanningQueries.hpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/TextNodePlanningQueries.cpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/RenderBlock.hpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/BlockEmitter.hpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/BlockEmitter.cpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/ListItemLayout.hpp`
* :file:`src/erbsland/cterm/text/impl/text_node_renderer/ListItemLayout.cpp`
* :file:`test/unittest/src/impl/HtmlRendererTest.cpp`
* :file:`test/unittest/src/impl/PlanningContextTest.cpp`
* :file:`test/unittest/src/impl/PlanningStyleResolverTest.cpp`
* :file:`test/unittest/src/impl/InlineTextPlannerTest.cpp`
* :file:`test/unittest/src/impl/TextNodePlanningQueriesTest.cpp`
* :file:`test/unittest/src/impl/ListBlockPlannerTest.cpp`

High-Level Architecture
=======================

At a high level the subsystem looks like this:

.. mermaid::

    flowchart TD
        entry["TextNodeRenderer"] --> planner["RenderPlanner::build(document)"]
        planner --> blocks["RenderBlocks<br/>linear block plan"]
        blocks --> stringPath["RenderBlock::renderString()"]
        blocks --> terminalPath["BlockEmitter::render()"]

``TextNodeRenderer`` is intentionally thin.
It owns only:

* the document pointer
* the style pointer
* the choice of output mode

Everything else lives in the planning helpers or the output layer.

Internal Module Layout
======================

The internal module under
:file:`src/erbsland/cterm/text/impl/text_node_renderer/` is split into
small types with narrow responsibilities.

.. list-table::
    :header-rows: 1

    * - Type / file
      - Responsibility
    * - ``RenderPlanner``
      - Dispatch node kinds, coordinate the collaborators, and build the final block list.
    * - ``PlanningContext``
      - Carry inherited text-style overlays and inherited margins and derive child contexts.
    * - ``PlanningStyleResolver``
      - Resolve block rules, heading and list levels, inline roles, and normalized node style tokens.
    * - ``InlineTextPlanner``
      - Render inline subtrees into styled ``String`` values.
    * - ``ListBlockPlanner``
      - Expand lists and list items into physical blocks, including placeholder prefix lines and continuation blocks.
    * - ``RenderBlockFactory``
      - Construct paragraph, heading, filled-line, and horizontal-rule blocks.
    * - ``TextNodePlanningQueries``
      - Answer planning-specific tree questions such as "does this node emit blocks?" and "which child range matters?".
    * - ``RenderBlock``
      - Represent one fully planned physical block shared by both output paths.
    * - ``ListItemLayout``
      - Apply list prefixes and continuation indentation to already planned blocks.
    * - ``BlockEmitter``
      - Stream planned blocks to a :cpp:any:`CursorWriter <erbsland::cterm::CursorWriter>`, including vertical margin collapse.

Planning Pipeline
=================

The planning phase is split so each step has a single owner.

.. mermaid::

    flowchart TD
        nodeTree["TextNode tree"] --> dispatch["RenderPlanner::appendNode()"]
        dispatch --> queries["TextNodePlanningQueries"]
        dispatch --> resolver["PlanningStyleResolver"]
        dispatch --> context["PlanningContext"]
        dispatch --> inline["InlineTextPlanner"]
        dispatch --> listPlanner["ListBlockPlanner"]
        inline --> factory["RenderBlockFactory"]
        listPlanner --> factory
        factory --> blocks["RenderBlocks"]

``RenderPlanner::appendNode()`` only decides which route a node takes:

* semantic containers go through ``appendContainer()``
* bullet and numbered lists go through ``ListBlockPlanner``
* paragraph-like blocks go through ``RenderBlockFactory::paragraph()``
* headings go through ``RenderBlockFactory::heading()``
* horizontal rules go through ``RenderBlockFactory::horizontalRule()``

Planning Context
----------------

``PlanningContext`` owns the inherited state:

* inherited text-style overlay
* accumulated margins from ancestor containers
* edge-margin application for the first and last rendered child
* special list-item child context handling

That gives the subsystem one simple rule:

Context transformations belong to the context type, not to the
orchestrator.

Style Resolution
----------------

``PlanningStyleResolver`` owns all selector lookup and node-specific
style matching:

* role selection
* heading and list level normalization
* list-kind handling
* node class/style token normalization
* inline-role mapping

It intentionally avoids allocation when it can:

* nodes without style tokens resolve directly without building a token
  vector
* token splitting happens only when selector matching actually needs it

Inline Rendering
----------------

``InlineTextPlanner`` is the only planner component that recursively
builds styled ``String`` values from inline subtrees.

Its responsibilities are:

* append literal text, error, and unsupported nodes
* preserve explicit line breaks as newline characters
* apply inline style overlays for emphasis, strong text, underline,
  links, and inline code
* reserve output capacity from
  ``TextNode::estimatedInlineTextCapacity()``

It never talks to the cursor writer directly.

List Planning
-------------

``ListBlockPlanner`` is the only component that knows how one logical
list item can become multiple physical blocks.

It handles:

* list container traversal
* list-item rule resolution
* placeholder prefix blocks for list items that start with nested lists
* item-level top and bottom margin application
* handing off the final first-block versus continuation-block rewrite to
  ``ListItemLayout``

``ListItemLayout`` owns a ``ListPrefix`` value instead of duplicating
the rendered prefix text in parallel fields.
That keeps the list-prefix data model compact and explicit.

Render Block Construction
-------------------------

``RenderBlockFactory`` owns the final construction of physical block
shapes:

* paragraph-like blocks
* headings and filled headings
* horizontal rules

It is also the place where block prefix/suffix decoration is applied.

This keeps block-shape construction close to the ``RenderBlock`` model
and prevents those details from drifting back into ``RenderPlanner``.

Output Phase
============

Once planning is complete, output behavior is unchanged in principle.

``renderString()``
------------------

The plain-string path:

1. builds ``RenderBlocks``
2. joins them with one newline between blocks
3. delegates each block to ``RenderBlock::renderString()``
4. trims outer whitespace-like line separators from the final result

This remains a lightweight semantic string renderer rather than a full
terminal-layout simulation.

``BlockEmitter::render()``
--------------------------

The terminal path consumes the same ``RenderBlocks`` but owns the
physical emission details:

* vertical margin collapse
* paragraph option translation
* filled-line painting
* horizontal-rule width handling

The planner never writes to the terminal directly.

Allocation Notes
================

Allocation discipline matters in this subsystem.

Important constraints of the design:

* collaborators are stack-owned values or references, not heap-managed
  service objects
* planning appends into caller-owned ``RenderBlocks`` where possible
* inline rendering still reserves from estimated inline capacity
* style-token vectors are created only when a node actually carries
  style tokens
* list-prefix rewriting happens in-place on already planned blocks

Performance is not the first concern here, but the planning split should
not introduce avoidable heap churn.

Where to Change What
====================

When extending the renderer, start in the component that owns the
relevant behavior.

Adding a new block-emitting ``TextNode::Type``
----------------------------------------------

Start in ``RenderPlanner::appendNode()``.
Then choose the matching path:

* semantic container
* list node
* paragraph-like block
* special physical block kind

Changing inherited text style or margins
----------------------------------------

Start in ``PlanningContext``.

Changing selector matching or node token handling
-------------------------------------------------

Start in ``PlanningStyleResolver``.

Changing inline rendering behavior
----------------------------------

Start in ``InlineTextPlanner``.

Changing list prefix placement or continuation layout
-----------------------------------------------------

Start in:

* ``ListBlockPlanner``
* ``ListItemLayout``

Changing block construction rules
---------------------------------

Start in ``RenderBlockFactory``.

Changing terminal-only emission behavior
----------------------------------------

Start in ``BlockEmitter``.

Changing plain-string projection
--------------------------------

Start in:

* ``TextNodeRenderer::renderString()``
* ``RenderBlock::renderString()``

Testing Strategy
================

The primary semantic coverage lives in
:file:`test/unittest/src/impl/HtmlRendererTest.cpp`.

Those tests intentionally exercise the whole pipeline through
``HtmlRenderer`` and ``TextNodeRenderer``.

Focused internal tests cover the planning components:

* ``PlanningContextTest``
* ``PlanningStyleResolverTest``
* ``InlineTextPlannerTest``
* ``TextNodePlanningQueriesTest``
* ``ListBlockPlannerTest``

Use both levels:

* end-to-end tests for user-visible rendering behavior
* focused component tests for subtle internal invariants

Summary
=======

The renderer follows this high-level split:

* planning produces inspectable ``RenderBlocks``
* plain-string rendering and terminal output consume the same block plan

The important structural rule is:

* ``RenderPlanner`` orchestrates
* each planning concern lives in its own named internal type
* output stays separate from semantic planning

As long as future changes keep that ownership model intact, the
subsystem should remain readable, testable, and maintainable.
