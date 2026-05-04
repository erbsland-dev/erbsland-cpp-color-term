..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: implementation notes; UI scrollable areas
    single: implementation notes; scroll metrics
    single: implementation notes; managed surface containers

*******************
UI Scrollable Areas
*******************

This page explains how the UI layer implements scrollable content,
managed child containers, and visibility-aware traversal.

The scroll framework has one central rule:

Scrolling state is shared, content ownership is not.

The common scroll base owns offsets, ranges, and scroll bar state, while
the concrete surface decides whether content is custom-painted or managed
as a child surface.

Relevant Source Files
=====================

You will find the implementation primarily in:

* :file:`src/erbsland/cterm/ui/AbstractSurfaceContainer.hpp`
* :file:`src/erbsland/cterm/ui/Surface.hpp`
* :file:`src/erbsland/cterm/ui/Surface.cpp`
* :file:`src/erbsland/cterm/ui/SurfaceContainer.hpp`
* :file:`src/erbsland/cterm/ui/SurfaceContainer.cpp`
* :file:`src/erbsland/cterm/ui/SurfaceManager.hpp`
* :file:`src/erbsland/cterm/ui/SurfaceManager.cpp`
* :file:`src/erbsland/cterm/ui/LayoutData.hpp`
* :file:`src/erbsland/cterm/ui/impl/ScrollMetrics.hpp`
* :file:`src/erbsland/cterm/ui/impl/ScrollMetrics.cpp`
* :file:`src/erbsland/cterm/ui/surface/AbstractScrollArea.hpp`
* :file:`src/erbsland/cterm/ui/surface/AbstractScrollArea.cpp`
* :file:`src/erbsland/cterm/ui/layout/Viewport.hpp`
* :file:`src/erbsland/cterm/ui/layout/Viewport.cpp`
* :file:`src/erbsland/cterm/ui/layout/SingleContentLayout.hpp`
* :file:`src/erbsland/cterm/ui/layout/SingleContentLayout.cpp`
* :file:`src/erbsland/cterm/ui/layout/ScrollArea.hpp`
* :file:`src/erbsland/cterm/ui/layout/ScrollArea.cpp`
* :file:`src/erbsland/cterm/ui/surface/ScrollingBufferView.hpp`
* :file:`src/erbsland/cterm/ui/surface/ScrollingBufferView.cpp`
* :file:`test/unittest/src/ui/layout/UiScrollAreaTest.cpp`
* :file:`test/unittest/src/ui/surface/UiScrollingBufferViewTest.cpp`

Layer Model
===========

The implementation is deliberately split into three layers:

``impl::ScrollMetrics``
    Stateless helper functions for resolving content size, clamping
    offsets, calculating aligned origins, deriving scroll bar ranges, and
    implementing ``scrollIntoView()``.

``surface::AbstractScrollArea``
    Shared base for custom-painted scrollable surfaces. It owns
    ``scrollOffset``, viewport metrics, scroll bar modes, the two scroll
    bars, and the scroll corner. Subclasses provide content size and
    paint the visible area.

``layout::SingleContentLayout`` / ``layout::Viewport`` / ``layout::ScrollArea``
    Composition-based layout surfaces. ``SingleContentLayout`` keeps one
    optional content surface synchronized with generic container changes.
    ``Viewport`` positions that content surface. ``ScrollArea`` owns one
    viewport plus the scroll bar surfaces and forwards the public scroll
    API to the viewport.

Visualized:

.. mermaid::

    flowchart TD
        metrics["impl::ScrollMetrics"]
        abstract["surface::AbstractScrollArea"]
        buffer["surface::ScrollingBufferView"]
        viewport["layout::Viewport"]
        area["layout::ScrollArea"]
        content["content surface"]
        bars["horizontal/vertical bars + corner"]

        metrics --> abstract
        metrics --> viewport
        abstract --> buffer
        abstract --> area
        area --> viewport
        area --> bars
        viewport --> content

Managed Child Containers
========================

``Surface::surfaces()`` returns ``AbstractSurfaceContainer``. The real
storage is ``SurfaceContainer``, which owns parent links, reparenting,
cycle checks, ordering, layout invalidation, and focus cleanup.

The policy layer is ``SurfaceManager``. A manager is attached to a
``SurfaceContainer`` and observes the normal container operations. It can
limit the child count, reject an invalid child, provide default
``LayoutData``, or keep layout-specific cached state synchronized.
Importantly, the manager does not replace the container. Generic
algorithms still go through ``surfaces()`` and get the same parent-link
behavior as hand-written layout APIs.

``Layout`` inherits ``SurfaceManager`` with permissive defaults and
attaches itself to its child storage. Layout subclasses override only the
policy hooks they need:

* ``SingleContentLayout`` limits the count to one and keeps
  ``contentSurface()`` synchronized.
* ``Sections`` creates default section metadata and stores explicit
  ``SectionOptions`` as layout data.
* ``Buttons`` accepts only button surfaces and synchronizes its action
  dispatch list.
* ``AbstractScrollArea`` protects implementation-owned scroll bar
  children.

For ``Viewport``, generic mutation is intentionally valid when it
preserves the one-content invariant:

.. code-block:: cpp

    viewport->surfaces().add(content);
    viewport->surfaces().remove(content);

Adding a second child throws. Removing the only child leaves an empty
viewport with stable layout behavior. ``ScrollArea`` is different: its
direct child surfaces are implementation-owned, so public structural
mutation of those children is rejected. User content belongs to the
internal viewport and is assigned through ``setContentSurface()``.

Visibility Traversal
====================

Visibility is a local flag on ``SurfaceFlags`` and is accessed through
``surface->flags()``.

Hidden surfaces:

* remain in their parent container
* keep their last rectangle
* are ignored by default layout traversal
* are ignored by ``Stack`` layout item construction
* are ignored by dirty collection and paint traversal
* cannot receive or retain focus
* still keep scheduled actions armed and executable

The scheduler rule is intentional. A hidden surface may still update
internal state, reschedule work, or show itself later.

The display and focus code use effective visibility.
``flags().isVisibleInTree()`` checks the surface and all ancestors, while
layout code usually only needs to test direct children with
``flags().isVisible()``.

Scroll Metrics
==============

``impl::ScrollMetrics`` centralizes the math that must stay consistent
between ``AbstractScrollArea``, ``Viewport``, and
``ScrollingBufferView``.

The key calculations are:

``resolveScrollContentSize()``
    Resolves content geometry against a candidate viewport. A grow policy
    expands to the viewport size, bounded by minimum and maximum. A
    non-grow policy keeps the preferred size within its bounds. This is
    what allows constrained content to remain smaller or larger than the
    viewport.

``maximumScrollOffset()`` and ``clampedScrollOffset()``
    Derive the valid offset range. The viewport extent is treated as at
    least one cell when computing the maximum offset, so an empty
    viewport does not create negative scroll ranges.

``alignedContentOrigin()``
    Anchors undersized content by alignment and scrolls oversized content
    by subtracting ``scrollOffset`` from the regular origin.

``scrollRegion()`` and ``visibleRegion()``
    Convert content size, viewport size, and offset into
    :cpp:any:`IndexRange <erbsland::cterm::IndexRange>` values for the
    scroll bars.

``scrollOffsetForVisibleRect()``
    Implements minimal movement for ``scrollIntoView()``. If the target
    rectangle is already visible, the offset is unchanged. If it is
    outside the viewport, the offset moves just enough to reveal it. If
    the target is larger than the viewport on an axis, its leading edge
    wins.

Viewport Layout
===============

``Viewport`` owns at most one content surface through
``SingleContentLayout``.

During layout:

#. If there is no content surface, or the content is hidden, content size
   and offset are reset to zero.
#. Content size is resolved from the content geometry and viewport size.
#. The stored scroll offset is clamped to the new metrics.
#. The content origin is calculated from alignment and offset.
#. The content surface receives a rectangle at that origin with the
   resolved content size.
#. The content surface receives its own layout pass.

This means content can naturally be:

* smaller than the viewport and aligned inside it
* exactly viewport-sized because it grows
* larger than the viewport because its geometry requires it

``setScrollOffset()`` and ``scrollIntoView()`` calculate current content
metrics from the current rectangle before clamping. This allows scroll
requests to be made after assigning a viewport rectangle but before the
first layout pass.

Scroll Area Layout
==================

``ScrollArea`` inherits ``AbstractScrollArea`` and adds one viewport as
an internal child. The base class resolves the viewport rectangle and
scroll bar visibility. The derived layout then assigns the viewport
rectangle, forwards the current scroll offset, and layouts the viewport.
The direct children of a scroll area are protected implementation
surfaces; the public content surface is owned by the viewport.

The internal children are persistent:

.. code-block:: text

    ScrollArea
        Viewport
            content surface
        HorizontalScrollBar
        VerticalScrollBar
        ScrollCorner

Scroll bar mode is configured per orientation:

``Hidden``
    The bar is hidden and consumes no space.

``Automatic``
    The bar appears only if content overflows on that axis.

``Visible``
    The bar appears whenever the scroll area has enough area for that
    orientation.

Automatic visibility needs a stable pass. A vertical bar consumes one
column and can force horizontal overflow. A horizontal bar consumes one
row and can force vertical overflow. ``resolveScrollBarVisibility()`` runs
up to three passes, which is enough for both axes to stabilize.

When a bar is hidden, it remains in child storage but its visibility flag
is false. Hidden bars are skipped by layout, painting, and focus routing,
and the previous rectangle is left untouched.

Custom-Painted Scroll Areas
===========================

``AbstractScrollArea`` is for scrollable content that is not represented
by one child surface. ``ScrollingBufferView`` is the simplest example:
the source buffer is not a child surface, so the subclass reports the
buffer size from ``contentSizeForViewport()`` and draws a ``BufferView``
inside ``onPaintArea()``.

Subclass responsibilities are:

* return the content size for a candidate viewport
* paint only inside the ``targetRect`` passed to ``onPaintArea()``
* interpret ``scrollOffset`` as content coordinates
* call ``initializeScrollAreaChildren()`` in the factory after
  ``shared_from_this()`` is safe

Maintainer Rules
================

When changing the scroll framework, keep these invariants intact:

* Generic child mutation goes through ``surfaces()`` and should work
  whenever it preserves the layout's invariants.
* Use ``SurfaceManager`` hooks for layout policy; do not reintroduce
  proxy containers for ordinary child-count or metadata constraints.
* Internal framework children are added through ``childStorage()`` and
  protected by manager policy when public mutation would break the
  structure.
* Parent pointers are owned by ``Surface`` and maintained by
  ``SurfaceContainer``.
* Hidden surfaces keep their rectangles.
* Layouts skip hidden direct children instead of clearing their
  rectangles.
* Display traversal and focus routing must use effective visibility.
* Scroll offset names stay consistent; do not reintroduce ``viewOffset``
  in UI surfaces.
* Scroll math shared by more than one scroll component belongs in
  ``impl::ScrollMetrics``.
