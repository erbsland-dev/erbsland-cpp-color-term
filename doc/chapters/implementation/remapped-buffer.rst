..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: implementation notes; remapped buffer
    single: implementation notes; CursorBuffer storage

**************
RemappedBuffer
**************

This page documents the implementation of
:cpp:any:`RemappedBuffer <erbsland::cterm::RemappedBuffer>`.
It is written for maintainers who need to debug, extend, or optimize the class.

``RemappedBuffer`` is not just another mutable 2D character grid.
Its main job is to keep row- and column-oriented operations cheap, even when the visible content is large.
That design choice drives the storage layout, the resize behavior, and the helper algorithms described below.

Design Rationale
================

A plain :cpp:any:`Buffer <erbsland::cterm::Buffer>` is ideal when rendering redraws most or all cells every frame.
Its storage is simple and dense, and coordinate lookup is direct.

That simplicity becomes expensive for workloads such as:

* scrollback buffers
* editors that insert or delete complete rows
* views that repeatedly shift or rotate whole rows or columns
* cursor-driven output that appends new lines to an existing history

In those cases, the visible grid changes in a structured way.
The application usually does not want to rewrite every cell.
It wants to say:

* move these rows down
* remove the top line
* insert a blank line here
* keep all existing lines but grow the history by one more row

``RemappedBuffer`` optimizes exactly that.
Instead of moving all affected cells, it keeps the character storage stable and remaps logical rows or columns onto that
storage.

At a high level, the class trades a small amount of indirection during normal access for much cheaper structural edits.

Relevant Source Files
=====================

The implementation is centered in:

* :file:`src/erbsland/cterm/RemappedBuffer.hpp`
* :file:`src/erbsland/cterm/RemappedBuffer.cpp`
* :file:`src/erbsland/cterm/CursorBuffer.hpp`
* :file:`src/erbsland/cterm/CursorBuffer.cpp`

The :cpp:any:`CursorBuffer <erbsland::cterm::CursorBuffer>` relationship matters because the cursor-writing path is the
most important real-world user of fast vertical preserve-content resize.

Core Idea
=========

The class stores three things:

* the physical character storage in ``_buffer``
* a row map in ``_rowRemap``
* a column map in ``_columnRemap``

The visible position is therefore not the same thing as the storage position.
Every read or write first resolves the logical coordinate through the active remap tables.

.. mermaid::

    flowchart LR
        logical["logical position (x, y)"] --> remap["remapPosition()<br/>_columnRemap[x], _rowRemap[y]"]
        remap --> stored["stored position (storedX, storedY)"]
        stored --> index["bufferIndex(stored position, size, orientation)"]
        index --> cell["_buffer[index]"]

This means that row- and column-based edits can often be expressed as remap changes plus a refill of only the recycled
storage lines.

Logical and Stored Coordinates
==============================

The most important mental model is:

* logical coordinates describe what callers see
* stored coordinates describe where the cell currently lives in ``_buffer``

For a vertically oriented buffer, the mapping may look like this:

.. code-block:: text

    logical row -> stored row

    0 -> 2
    1 -> 0
    2 -> 1

That means the visible top row is physically stored in row ``2``, the second visible row is stored in row ``0``, and so
on.

The visible order may therefore differ from the storage order:

.. code-block:: text

    visible order                  physical storage

    row 0 -> stored row 2          stored row 0
    row 1 -> stored row 0          stored row 1
    row 2 -> stored row 1          stored row 2

The same model applies to columns.

Orientation
===========

``RemappedBuffer`` supports two orientations:

* ``Orientation::Vertical``
* ``Orientation::Horizontal``

The orientation decides which axis is the *primary* axis.

* In vertical orientation, rows are primary.
* In horizontal orientation, columns are primary.

That matters in two places:

* how linear storage indexes are computed
* which preserve-content resize can be handled cheaply

The helper ``bufferIndex(pos, size, orientation)`` lays out storage so the primary axis forms the outer dimension.
That keeps the class internally consistent: the same orientation that defines the cheap structural operations also
defines the storage order.

Access Path for ``get()`` and ``set()``
=======================================

Normal random access remains straightforward:

#. Validate that the logical position is inside ``_size``.
#. Convert the logical coordinate to a stored coordinate with ``remapPosition()``.
#. Linearize that stored coordinate with ``bufferIndex()``.
#. Read or write ``_buffer[index]``.

The cost of this indirection is small and predictable.
The benefit is that structural edits can avoid touching most cells.

Wide-character handling mirrors :cpp:any:`Buffer <erbsland::cterm::Buffer>`:

* zero-width characters are ignored
* width-two characters occupy the next logical cell as a continuation cell
* width-two characters at the right edge are ignored

The remap happens before the actual storage write, so the wide-character logic still works in logical coordinates.

Fast Structural Operations
==========================

The public mutation helpers are built around remap manipulation.

Rows and Columns Are Recycled, Not Rebuilt
------------------------------------------

Operations such as ``eraseRows()``, ``insertRows()``, ``eraseColumns()``, and ``insertColumns()`` do not shift every
cell in the visible rectangle.

Instead they:

#. update the relevant remap vector
#. collect the stored rows or columns that became free
#. refill only those recycled stored rows or columns

This keeps the work proportional to the number of affected lines instead of the full visible area.

The helper functions are:

* ``eraseFromMap()``
* ``insertIntoMap()``
* ``moveInMap()``
* ``fillStoredRows()``
* ``fillStoredColumns()``

Example: Erasing the Top Row
----------------------------

For a vertical buffer with this row map:

.. code-block:: text

    logical row -> stored row

    0 -> 2
    1 -> 0
    2 -> 1
    3 -> 3

Calling ``eraseRows(0, fillChar, 1)`` transforms the visible order to:

.. code-block:: text

    logical row -> stored row

    0 -> 0
    1 -> 1
    2 -> 3
    3 -> 2   <- recycled row, now refilled with fillChar

Only stored row ``2`` needs to be cleared.
The other visible rows are kept by remapping.

Rotate and Move
---------------

``rotate()`` simply rotates the corresponding remap vector.

``moveRows()`` and ``moveColumns()`` are a little more involved:

* ``moveInMap()`` extracts the moved span
* it computes which lines remain visible and which drop out of range
* dropped stored lines are returned as ``recycled``
* the caller then refills only those recycled lines

This design keeps the policy separate from the storage update.
``moveInMap()`` decides the new logical order.
``fillStoredRows()`` or ``fillStoredColumns()`` restores deterministic blank content in the freed storage.

Resize Semantics
================

``RemappedBuffer`` supports two resize modes:

* ``BufferResizeMode::Fast``
* ``BufferResizeMode::PreserveContent``

The two modes are intentionally very different.

Fast Resize
-----------

``BufferResizeMode::Fast`` is the cheapest option.

It:

* resizes ``_buffer``
* initializes newly appended storage cells if a fill character was provided
* resets both remap vectors to linear identity maps

The visible content order after a fast resize is therefore undefined.
Use this mode when the caller plans to repaint the buffer anyway.

Preserve-Content Resize
-----------------------

``BufferResizeMode::PreserveContent`` keeps the visible content stable, but the implementation cost depends on *which
axis changes*.

This is the single most important resize rule for maintainers:

.. important::

    Preserve-content resize is fast only when the resize changes the primary axis of the configured orientation.

    If the secondary axis changes, the buffer must rebuild the logical content into new storage, which is much more
    expensive.

Primary-Axis Preserve Resize
----------------------------

When only the primary axis changes, the implementation uses ``primaryAxisResize()``.

Expansion is simple:

.. code-block:: text

    before

    logical row -> stored row

    0 -> 2
    1 -> 0
    2 -> 1

    after growing by one row

    0 -> 2
    1 -> 0
    2 -> 1
    3 -> 3

The implementation:

* grows ``_buffer``
* fills newly appended storage with ``fillChar`` when needed
* extends the primary remap with fresh identity entries for the appended storage lines

Shrinking is more subtle, because some currently visible rows or columns may point outside the new valid primary-axis
range.

The implementation therefore:

#. scans the visible primary-axis entries that will remain
#. marks stored destinations that are already safely inside the new range
#. collects free in-range destinations
#. copies only the still-visible out-of-range stored lines into those free destinations
#. shrinks the storage and the primary remap

In other words, the algorithm copies only the lines that would otherwise be truncated.
All already-safe lines stay untouched.

This is the performance-critical path used by ``CursorBuffer`` when a growing history appends another line.

Secondary-Axis Preserve Resize
------------------------------

If the secondary axis changes, remapping alone is not enough.
The visible rectangle has to be reconstructed in a new storage layout.

That path is implemented by ``reorderedResize()``:

#. allocate new storage for the target size
#. fill it with ``fillChar``
#. iterate over the overlapping logical rectangle
#. read visible cells through ``get()``
#. write them into the new storage using the target orientation layout
#. reset both remap vectors to linear identity maps

This path is correct and explicit, but intentionally not optimized for speed.
That tradeoff is acceptable because the class exists primarily to optimize primary-axis structural edits.

Relationship to ``CursorBuffer``
================================

``CursorBuffer`` inherits from ``RemappedBuffer`` in vertical orientation.
That means rows are the primary axis.

When the cursor reaches the bottom and the overflow mode allows growth, ``CursorBuffer::writeLineBreak()`` calls:

.. code-block:: cpp

    resize(_size + Size{0, 1}, BufferResizeMode::PreserveContent, _fillChar);

That is important for two reasons:

* the call is explicit about preserving visible content
* the resize stays fast because it changes only the primary axis of a vertical buffer

This combination keeps growth on the preserve-content path for primary-axis expansion and avoids an unnecessary content
rebuild.

Internal Invariants
===================

When modifying this class, keep the following invariants in mind:

* ``_size`` is always validated against ``cMinimumSize`` and ``cMaximumSize``.
* ``_rowRemap.size() == _size.height()``.
* ``_columnRemap.size() == _size.width()``.
* Every remap entry refers to a valid stored row or column.
* After ``fill()`` and fast resize, both remap vectors are identity mappings.
* Recycled stored rows or columns are always refilled before they become visible again.

If any of these invariants is broken, the most common symptoms are duplicated visible lines, stale content reappearing,
or out-of-bounds assertions in ``get()`` and ``set()``.

Extending the Class Safely
==========================

If you add another structural operation, this checklist usually keeps the design aligned with the existing class:

#. Decide whether the operation can be expressed as a remap transformation.
#. Prefer changing only one remap vector plus refilling recycled storage lines.
#. Touch ``_buffer`` directly only for the storage lines that truly need rewriting.
#. Preserve the distinction between logical and stored coordinates.
#. Re-check whether the operation behaves differently for vertical and horizontal orientation.
#. Add tests for both visible correctness and map edge cases.

If the new operation needs to preserve visible content across a secondary-axis change, assume that a full rebuild is the
safe baseline until a stronger optimization can be proven correct.

Choosing Between ``Buffer`` and ``RemappedBuffer``
==================================================

As a final rule of thumb:

* choose :cpp:any:`Buffer <erbsland::cterm::Buffer>` for dense redraw workloads
* choose :cpp:any:`RemappedBuffer <erbsland::cterm::RemappedBuffer>` for line-oriented structural edits
* choose :cpp:any:`CursorBuffer <erbsland::cterm::CursorBuffer>` when terminal-style streaming output should retain a
  scrollback history

That division of responsibilities is the reason ``RemappedBuffer`` can stay specialized and performant without trying
to replace every other buffer type in the library.
