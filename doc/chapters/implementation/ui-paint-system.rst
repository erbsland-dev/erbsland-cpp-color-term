..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: implementation notes; UI paint system
    single: implementation notes; paint invalidation
    single: implementation notes; surface painting

***************
UI Paint System
***************

This page explains how the UI layer turns dirty surfaces into clipped paint calls.

The paint system has one central rule:

Every surface paints only itself.

Child traversal, clipping, repaint ordering, dirty-flag cleanup, and screen-buffer composition are owned by
:cpp:any:`Display <erbsland::cterm::ui::Display>`. This keeps individual surfaces simple and lets the display repaint
only the parts of the surface tree that can affect the terminal buffer.

Relevant Source Files
=====================

You will find the implementation primarily in:

* :file:`src/erbsland/cterm/ui/Display.hpp`
* :file:`src/erbsland/cterm/ui/Display.cpp`
* :file:`src/erbsland/cterm/ui/Surface.hpp`
* :file:`src/erbsland/cterm/ui/Surface.cpp`
* :file:`src/erbsland/cterm/ui/SurfaceFlags.hpp`
* :file:`src/erbsland/cterm/ui/SurfaceFlags.cpp`
* :file:`src/erbsland/cterm/ui/PaintContext.hpp`
* :file:`test/unittest/src/ui/surface/UiPaintSystemTest.cpp`

Surface Ownership
=================

The surface tree describes ownership and layout:

.. mermaid::

    flowchart TD
        page["Page (opaque)"]
        root["root layout or panel"]
        left["child A"]
        right["child B"]
        overlay["overlay child"]

        page --> root
        root --> left
        root --> right
        right --> overlay

Painting does not recurse through ``Surface::onPaint()``.
``Display`` walks the tree and calls each affected ``onPaint()`` directly.

.. code-block:: text

    Surface::onPaint(buffer, context)
        paints this surface only
        does not paint child surfaces
        does not clear paint dirty flags

    Display
        finds dirty visible surfaces
        computes clipped damage rectangles
        paints from the correct opaque base
        paints overlapping later siblings
        clears paint dirty state before calling onPaint()

Dirty State and Damage
======================

Paint invalidation is surface-local.
Calling ``surface->flags().setPaintOutdated()`` marks that surface dirty, but it does not automatically mark all
parents dirty.
This is what allows a ticking status item to repaint without forcing the whole page to run ``onPaint()``.

``SurfaceFlags`` stores two pieces of paint state:

* a dirty flag
* either a local dirty rectangle or an implicit full-surface repaint

``Display`` refreshes repaint-time action state, resolves the theme context, and clears the dirty state immediately
before calling ``onPaint()``.
If a surface invalidates itself while it is painting, that new dirty state remains set for a future render cycle.

.. mermaid::

    sequenceDiagram
        participant S as Surface
        participant D as Display

        S->>S: flags().setPaintOutdated()
        D->>S: clear paint dirty state
        D->>S: onPaint()
        S->>S: flags().setPaintOutdated()
        Note over S: dirty for next render

Layout invalidation propagates to ancestors, because a child geometry change can require parent layout work.
Rectangle changes also damage the vacated and new child rectangles in the parent, so both screen areas are repainted.

Finding Paint Requests
======================

Before rendering, ``Display`` calculates layout if needed. Then it scans the visible page stack from top to bottom,
stopping behind the first opaque page.

For each visible dirty surface, the display:

#. Uses the surface coordinate helpers to calculate the surface's screen rectangle.
#. Clips it through all ancestor rectangles.
#. Converts the local dirty rectangle into a screen damage rectangle.
#. Chooses the opaque base where repainting must start.

The opaque-base rule is the key to avoiding unnecessary parent paints:

.. mermaid::

    flowchart TD
        dirtyOpaque["dirty opaque child"]
        dirtyTransparent["dirty transparent child"]
        parent["parent surface"]
        page["nearest opaque ancestor or page"]

        dirtyOpaque --> opaqueResult["start repaint at the child"]
        dirtyTransparent --> parent
        parent --> page
        page --> transparentResult["start repaint at opaque ancestor"]

If the dirty surface is opaque, its own visible cells fully cover the parent, so repainting can start there.
If the dirty surface is transparent, content behind it can show through, so repainting starts at the nearest opaque
ancestor and covers the damaged region from back to front.

Fully clipped surfaces are ignored during rendering and keep their dirty state.
If a later layout or move makes them visible, they will be considered again.

Clipping and Local Coordinates
==============================

Every ``onPaint()`` receives a :cpp:any:`WriteClippedBufferRef <erbsland::cterm::WriteClippedBufferRef>`.
The wrapped buffer maps local surface coordinates into the display buffer and rejects writes outside the current dirty
rectangle.

Surface provides the coordinate helpers used by the paint algorithm:

``localSurfaceRect()``
    Returns the full local rectangle of the surface, with ``0,0`` as the top-left corner.

``localToScreen()`` and ``screenToLocal()``
    Convert rectangles between local surface coordinates and screen coordinates.

``visibleScreenRect()``
    Clips the surface rectangle through its ancestors and stops as soon as the visible area becomes empty.

The paint context uses local coordinates:

``surfaceRect``
    The full local rectangle of the surface. Usually ``{0, 0, width, height}``.

``visibleRect``
    The part of the surface that survives clipping by all ancestors.

``dirtyRect``
    The part of the surface that should be repainted in this paint call.

For a child that starts left of its parent, the local visible area may begin at a positive x coordinate:

.. code-block:: text

    parent local x:    0 1 2 3 4
    child local x:   -2-1 0 1 2 3 4
                     | clipped |

    child surfaceRect = 0,0:5x1
    child visibleRect = 2,0:3x1
    child dirtyRect   = 2,0:3x1

Surface code should draw as if the surface starts at ``0,0``.
The clipped buffer decides which writes reach the display buffer.

Repaint Order
=============

Within one parent, child order is paint order. Later children visually cover earlier children.

When a repaint starts at an opaque child, ``Display`` repaints later overlapping siblings, because they may cover the
dirty child:

.. mermaid::

    flowchart LR
        base["dirty child A"]
        sibling["later sibling B"]
        ancestorSibling["later ancestor sibling C"]

        base --> baseSubtree["paint A subtree"]
        baseSubtree --> sibling
        sibling --> ancestorSibling

The display therefore paints:

#. the opaque base and its intersecting descendants
#. later siblings in the same parent
#. later siblings of each ancestor on the path back to the page

This preserves visual stacking without repainting unrelated earlier siblings or parents behind an opaque base.

Maintainer Rules
================

When implementing or reviewing a surface, use these rules:

* ``onPaint()`` paints only the surface's own content.
* Do not call child ``onPaint()`` methods.
* Do not clear paint dirty flags in normal surface code; ``Display`` clears them before calling ``onPaint()``.
* Use local coordinates. Treat ``0,0`` as the top-left corner of the surface.
* Use ``context.surfaceRect()`` for layout and alignment decisions.
* Use ``context.visibleRect()`` when a surface can skip expensive work outside the ancestor-clipped area.
* Use ``context.dirtyRect()`` when a surface can cheaply redraw only the damaged sub-region.
* ``isOpaque()`` must return ``true`` only when the surface paints every visible cell of its rectangle.

For debugging repaint issues, start by asking:

* Is the dirty surface visible after ancestor clipping?
* Is the dirty surface opaque, or should repaint start at an ancestor?
* Does a later sibling overlap the damaged screen rectangle?
* Did the surface invalidate itself during ``onPaint()`` and intentionally keep its dirty flag?
