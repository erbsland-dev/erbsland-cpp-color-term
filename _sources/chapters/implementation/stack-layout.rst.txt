..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: implementation notes; stack layout
    single: implementation notes; UI layout

************
Stack Layout
************

This page explains how :cpp:any:`Stack <erbsland::cterm::ui::layout::Stack>` calculates the size
and position of its child surfaces.

The stack layout is intentionally simple to use from the outside, but the internal sizing algorithm
contains enough policy handling and overflow behavior that deriving it from code alone would take time.
This note focuses on the actual implementation in the current codebase.

Relevant Source Files
=====================

You will find the implementation primarily in:

* :file:`src/erbsland/cterm/ui/layout/Stack.hpp`
* :file:`src/erbsland/cterm/ui/layout/Stack.cpp`
* :file:`src/erbsland/cterm/ui/layout/impl/DimensionConstraints.hpp`
* :file:`src/erbsland/cterm/ui/layout/impl/DimensionConstraints.cpp`
* :file:`src/erbsland/cterm/ui/layout/impl/StackLayoutItem.hpp`
* :file:`src/erbsland/cterm/ui/layout/impl/StackLayoutItem.cpp`
* :file:`src/erbsland/cterm/ui/layout/impl/StackLayoutItems.hpp`
* :file:`src/erbsland/cterm/ui/layout/impl/StackLayoutItems.cpp`

What the Stack Promises
=======================

The stack arranges children in source order on one axis:

* a horizontal stack places children from left to right
* a vertical stack places children from top to bottom

There is no wrapping and no overlaying. The stack does, however, own
the spacing between children by collapsing their adjacent layout
margins. The whole algorithm is therefore about two questions:

How much size should each child receive on the main axis, and how much
collapsed spacing belongs between neighboring children?

The cross axis is simpler:

* each child resolves itself independently against the full available cross size
* the stack does not distribute cross-axis free space between siblings
* cross-axis child margins are propagated as the stack's own outer margins

Layout Margins
==============

``LayoutMetrics`` sizes exclude margins. A child reports margins as
parent-owned outer spacing, and the stack decides which parts it can
resolve.

On the main axis, the stack consumes only margins between adjacent
children. The spacing between two children is:

.. code-block:: text

    max(previous.trailing_margin, next.leading_margin)

For a vertical stack this means ``max(previous.bottom, next.top)``. For a
horizontal stack it means ``max(previous.right, next.left)``.

The first leading margin and last trailing margin are not consumed by
the stack. They are reported as the stack's own margins, so an outer
layout can collapse them with neighboring surfaces. Cross-axis margins
are also propagated using the largest child margin on each side. This is
important for nested layouts. In a horizontal stack of vertical stacks,
only the horizontal parent knows which column margins touch each other,
so the vertical stacks must report their side margins instead of
consuming them internally.

Hidden children are ignored completely. They contribute neither size nor
margins.

Main Axis and Cross Axis
========================

The implementation always works with two logical axes:

* ``main axis``: the axis along which children are placed one after another
* ``cross axis``: the perpendicular axis

For a horizontal stack:

.. mermaid::

    flowchart LR
        subgraph horizontal["main axis = width / cross axis = height"]
            direction LR
            h1["child 1"] --> h2["child 2"] --> h3["child 3"] --> h4["child 4"]
        end

For a vertical stack:

.. mermaid::

    flowchart TD
        subgraph vertical["main axis = height / cross axis = width"]
            direction TB
            v1["child 1"] --> v2["child 2"] --> v3["child 3"]
        end

This separation is important because almost the entire algorithm is expressed in terms of
``main`` and ``cross`` rather than ``width`` and ``height``.

High-Level Pipeline
===================

``Stack::onLayout()`` delegates the real work to ``StackLayoutItems``.
The full pipeline is:

#. Convert each child surface into one ``StackLayoutItem``.
#. Resolve an initial main-axis size and a final cross-axis size for every child.
#. Sum the initial main-axis sizes plus internal collapsed margins.
#. If space is left over, distribute it to ``Grow`` items.
#. If there is overflow, shrink items in a fixed priority order.
#. Apply the final rectangles in child order.

In pseudo code:

.. code-block:: text

    items = build items from visible children
    resolve initial sizes
    add collapsed margins between adjacent items

    if total_main_size < available_main_size:
        distribute extra space
    else if total_main_size > available_main_size:
        shrink in several passes

    place children in source order

The Data Model
==============

The algorithm is split into three layers.

``DimensionConstraints``
------------------------

``DimensionConstraints`` represents one dimension of a child:

* minimum size
* maximum size
* preferred size
* size policy

It knows how to answer questions like:

* What is the initial size on this axis?
* Can this dimension still grow?
* Can it still shrink?
* How far may it grow or shrink?

``StackLayoutItem``
-------------------

``StackLayoutItem`` wraps one child surface and stores:

* the main-axis constraints
* the cross-axis constraints
* the currently assigned main-axis size
* the resolved cross-axis size

This means that once the item has been created, the layout algorithm no longer needs to repeatedly
inspect the child geometry object.

``StackLayoutItems``
--------------------

``StackLayoutItems`` is the orchestration layer for the whole sibling list.
It owns the vector of items and performs the two complex phases:

* distribute extra space
* shrink overflow

Step 1: Create Items
====================

Each child is first converted into a ``StackLayoutItem``.

During this step:

* the child geometry is split into main-axis and cross-axis constraints
* the available parent size is also split into main and cross values
* the item receives its first assigned sizes

The item is initialized differently on the two axes:

* main axis: use ``initialSize()``
* cross axis: use ``resolve()``

That difference is deliberate.

Why the Main Axis Uses ``initialSize()``
----------------------------------------

On the main axis, the stack first builds a neutral starting point from each child's preferred size.
Only after all children have an initial size does the stack compare the total with the available space.

This makes the algorithm a two-phase solver:

* phase A: every child states what it would like to have initially
* phase B: the stack adjusts that result globally

If the main axis used ``resolve()`` directly, every ``Grow`` child would immediately try to consume the
entire available main size, which would make sibling distribution impossible.

Why the Cross Axis Uses ``resolve()``
-------------------------------------

On the cross axis there is no sibling competition.
Every child is resolved independently against the full available cross size.

That produces the expected behavior:

* ``Grow`` children expand across the stack
* ``Preferred`` children keep their preferred cross size when possible
* all children still obey minimum and maximum limits

Visual Example of Item Initialization
=====================================

Assume a horizontal stack with total width ``30`` and height ``4``.
Three children have these width preferences:

.. list-table::
    :header-rows: 1

    * - Child
      - Minimum
      - Preferred
      - Maximum
      - Policy
      - Initial width
    * - A
      - 4
      - 6
      - 20
      - ``Grow``
      - 6
    * - B
      - 5
      - 5
      - 5
      - ``Preferred``
      - 5
    * - C
      - 3
      - 8
      - 10
      - ``Grow``
      - 8

The initial total is ``6 + 5 + 8 = 19``.
That means the stack still has ``11`` columns of free space to distribute.

Step 2: Distribute Extra Space
==============================

If the initial total is smaller than the available main size, the stack enters the growth phase.

Only children with ``DimensionPolicy::Grow`` participate.

The implementation repeats this loop:

#. Sum the factors of all growable ``Grow`` items.
#. Ask each participating item for a proportional growth share.
#. Clamp that share to the item's remaining growth limit.
#. Apply the growth.
#. Stop if no progress was made.

The factor math is:

.. code-block:: text

    desired_growth = max(1, remaining_space * item_factor / total_factor_sum)

Two details matter here.

First, ``max(1, ...)`` guarantees progress even when integer division would otherwise produce ``0``.
Without that rule, the algorithm could stall while space is still available.

Second, growth is applied sequentially in child order.
That means the result is factor-based and fair, but not mathematically perfect proportional splitting.
Integer rounding and per-item limits are resolved incrementally.

Worked Growth Example
---------------------

Continuing the previous example:

.. code-block:: text

    remaining space = 11
    A factor = 1
    C factor = 1
    factor sum = 2

Round 1:
    A wants 11 * 1 / 2 = 5  -> grows from 6 to 11
    C wants 11 * 1 / 2 = 5  -> grows from 8 to 13
    used = 10
    remaining = 1

Round 2:
    A wants max(1, 1 * 1 / 2) = 1 -> grows from 11 to 12
    remaining = 0

Final widths:
    A = 12
    B = 5
    C = 13

Visualized:

.. code-block:: text

    initial: [AAAAAA][BBBBB][CCCCCCCC]
    final:   [AAAAAAAAAAAA][BBBBB][CCCCCCCCCCCCC]

Step 3: Shrink Overflow
=======================

If the initial total is larger than the available main size, the stack enters the shrink phase.

This is the most important part of the algorithm.
The stack does not shrink all items equally.
Instead, it shrinks in six ordered passes.

Shrink Order
------------

.. list-table::
    :header-rows: 1

    * - Pass
      - Policy group
      - May go below minimum?
      - Meaning
    * - 1
      - ``Shrink``
      - No
      - Shrink the items that explicitly prefer shrinking.
    * - 2
      - ``Preferred``
      - No
      - Then shrink neutral items down to their minimum.
    * - 3
      - ``Grow``
      - No
      - Finally shrink growing items down to their minimum.
    * - 4
      - ``Grow``
      - Yes
      - If space is still missing, consume growing items below minimum.
    * - 5
      - ``Preferred``
      - Yes
      - Then neutral items below minimum.
    * - 6
      - ``Shrink``
      - Yes
      - Last resort: shrink-preferring items below minimum.

This order expresses a policy decision:

* ``Shrink`` items should give up space first, but they are also protected last when minimums must be broken
* ``Grow`` items are protected while minimums still matter, but sacrificed first once the layout becomes impossible

Why There Are Two Shrink Modes
------------------------------

The algorithm distinguishes between:

* shrink down to minimum
* shrink down to zero

The first mode preserves the contractual geometry whenever possible.
The second mode is a fallback for impossible layouts where the sum of all minimum sizes is still too large.

This makes the algorithm robust in tiny terminals instead of simply overflowing forever.

How One Shrink Pass Works
-------------------------

Each policy-group pass behaves similarly to growth:

#. Select all items in the current policy group that may still shrink.
#. Sum their factors.
#. Compute a proportional desired shrink for each item.
#. Clamp that shrink to the item's remaining shrink limit.
#. Apply the shrink.
#. Repeat while overflow remains.

The formula is:

.. code-block:: text

    desired_shrink = max(1, overflow * item_factor / total_factor_sum)

Again, ``max(1, ...)`` guarantees forward progress.

Worked Shrink Example
=====================

Assume a vertical stack with available height ``10`` and these initial heights:

.. list-table::
    :header-rows: 1

    * - Child
      - Minimum
      - Preferred
      - Policy
      - Initial height
    * - Header
      - 1
      - 2
      - ``Preferred``
      - 2
    * - Content
      - 4
      - 8
      - ``Grow``
      - 8
    * - Status
      - 1
      - 3
      - ``Shrink``
      - 3

Initial total:

.. code-block:: text

    2 + 8 + 3 = 13

Overflow:

.. code-block:: text

    13 - 10 = 3

Shrink passes:

.. code-block:: text

    Pass 1: Shrink group, above minimum
        Status can shrink from 3 to 1
        overflow 3 -> 1

    Pass 2: Preferred group, above minimum
        Header can shrink from 2 to 1
        overflow 1 -> 0

Final heights:
    Header  = 1
    Content = 8
    Status  = 1

Visualized:

.. mermaid::

    block
        columns 2
        block
            columns 1
            before("before")
            beforeHeader["Header<br/>2"]
            beforeContent["Content<br/>8"]
            beforeStatus["Status<br/>3"]
        end
        block
            columns 1
            after("after")
            afterHeader["Header<br/>1"]
            afterContent["Content<br/>8"]
            afterStatus["Status<br/>1"]
        end
        style before stroke:none;fill:transparent
        style after stroke:none;fill:transparent

If even that were not enough, the algorithm would continue into the below-minimum passes.

Step 4: Apply Rectangles
========================

Once all final main-axis sizes are known, placement is straightforward.

The stack walks through the items in source order and keeps one running offset:

.. code-block:: text

    offset = 0
    for each item:
        place item at offset on main axis
        offset += item.assigned_main_size

This preserves child order exactly.
The stack never reorders items to improve packing.

Horizontal placement:

.. code-block:: text

    child 1: x = 0
    child 2: x = width(child 1)
    child 3: x = width(child 1) + width(child 2)

Vertical placement:

.. code-block:: text

    child 1: y = 0
    child 2: y = height(child 1)
    child 3: y = height(child 1) + height(child 2)

The cross-axis position is always ``0`` in the current implementation.

Properties and Trade-Offs
=========================

The current algorithm has several useful properties:

* deterministic: same input always produces the same layout
* order-preserving: children stay in source order
* bounded: minimum and maximum sizes are always respected unless the fallback below-minimum passes are required
* progress-safe: ``max(1, ...)`` avoids endless loops caused by integer rounding
* policy-aware: ``Grow``, ``Preferred``, and ``Shrink`` are not treated as synonyms

At the same time, it is important to understand what the algorithm does *not* try to do:

* It does not compute globally optimal proportional distribution.
  Distribution is iterative and integer-based.
* It does not balance visual aesthetics such as equal gaps or centering of the full child group.
* It does not wrap children to a second row or column.
* It does not negotiate cross-axis space between siblings.

Edge Cases
==========

Empty stack
-----------

If the stack has no children, the algorithm simply performs no work.

All children fixed
------------------

If all items are fixed by geometry and no item can grow or shrink, the stack keeps the assigned sizes.
That can leave unused space or force later clipping if the parent is too small.

Impossible layout
-----------------

If the sum of all minimum sizes is larger than the available main size, the algorithm still finishes.
It enters the below-minimum passes and compresses items toward zero until the overflow disappears
or nobody can shrink further.

Large factor differences
------------------------

Factors influence distribution, but always through integer arithmetic and capped growth or shrink limits.
An item with a very large factor is favored, but cannot exceed its configured maximum or shrink below the current bound.

How to Read the Code Efficiently
================================

If you want to trace one layout from the code:

#. Start at :file:`src/erbsland/cterm/ui/layout/Stack.cpp`.
#. Follow item creation into ``StackLayoutItem::fromSurface()``.
#. Read ``DimensionConstraints`` to understand how one axis is bounded.
#. Return to ``StackLayoutItems::resolveMainSizes()``.
#. Read ``distributeExtraSpace()`` and ``shrinkOverflow()`` in that order.
#. Finish with ``applyLayout()`` and ``StackLayoutItem::applyLayout()``.

That reading order mirrors the real execution path and is the fastest way to understand the implementation.
