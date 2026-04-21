..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*******
UI Core
*******

The core UI types define the runtime, the surface tree, and the layout
constraints shared by all higher-level UI components.

.. important::

    The UI Framework is currently in beta, and parts of the API may
    change in future releases.

Usage
=====

Building a Surface Tree
-----------------------

A UI app usually creates one page, adds a small hierarchy of surfaces,
and then hands the tree to :cpp:any:`Application <erbsland::cterm::ui::Application>`. For compact examples, the
application can be a direct local instance.

.. code-block:: cpp

    using namespace erbsland::cterm;
    using namespace erbsland::cterm::ui;

    auto page = Page::create();
    auto root = Stack::create(Orientation::Vertical);
    auto body = TextBox::create("Ready", Alignment::Center);

    body->editLayoutMetrics().setSizePolicy(SizePolicy{SizePolicy::Grow});
    root->addSurface(body);
    page->addSurface(root);

    auto app = Application{};
    app.setMainPage(page);
    return app.run();

Applications that parse command line arguments or load data for the UI usually subclass
:cpp:any:`Application <erbsland::cterm::ui::Application>` and build the tree in
:cpp:any:`Application::setupUi() <erbsland::cterm::ui::Application::setupUi()>`.
During startup, command line arguments are processed after the terminal is initialized and before the display and event
system are created, so ``processCommandLineArguments()`` can attach loaded data to surfaces created by ``setupUi()``.

Working with Layout Metrics and Invalidation
--------------------------------------------

Each surface owns a
:cpp:any:`LayoutMetrics <erbsland::cterm::ui::LayoutMetrics>` object with minimum,
preferred, and maximum size hints plus a size policy for both axes. You
read it with :cpp:any:`Surface::layoutMetrics()
<erbsland::cterm::ui::Surface::layoutMetrics>` and change it through
:cpp:any:`Surface::editLayoutMetrics()
<erbsland::cterm::ui::Surface::editLayoutMetrics>`. The editor keeps the
layout invalidation rule in one place, so the next display pass can
recompute layout after a real metrics change.

.. code-block:: cpp

    auto header = TextBox::create("Title", Alignment::Center);
    auto body = Panel::create();

    header->editLayoutMetrics().setFixedHeight(1);
    body->editLayoutMetrics().setSizePolicy(SizePolicy{SizePolicy::Grow});

    body->flags().setPaintOutdated();

When something changes:

* call
  :cpp:any:`SurfaceFlags::setLayoutOutdated() <erbsland::cterm::ui::SurfaceFlags::setLayoutOutdated()>`
  when size or placement needs to be recalculated
* call
  :cpp:any:`SurfaceFlags::setPaintOutdated() <erbsland::cterm::ui::SurfaceFlags::setPaintOutdated()>`
  when only the rendered appearance changed

Theme changes through an application or page automatically mark the
affected tree as layout- and paint-outdated. Dynamic theme-attribute
changes on a surface are intentionally explicit: update
``surface->themeAttributes()`` and then call
``surface->flags().setThemeOutdated()`` when the change should be
reflected immediately. This matters because themed block margins can
change the usable content rectangle, not only the colors on screen.

This keeps updates local and avoids redrawing unchanged parts of the
surface tree.

Showing and Hiding Surfaces
---------------------------

Every :cpp:any:`Surface <erbsland::cterm::ui::Surface>` has local
visibility. Hidden surfaces stay in the tree and keep their current
rectangle, but display traversal, layout traversal, and focus/key
routing skip them.

.. code-block:: cpp

    auto details = ui::Panel::create();

    details->flags().setVisible(false);
    details->flags().setVisible(true);

    if (details->flags().isVisibleInTree()) {
        details->flags().setPaintOutdated();
    }

``flags().isVisible()`` reports only the local flag.
``flags().isVisibleInTree()`` also checks all ancestors. This is the
predicate used by focus routing and visible-rectangle calculation.

Showing or hiding a surface invalidates layout and damages the old
parent area so the display can repaint the cells that changed. Scheduled
actions are deliberately not filtered by visibility; hidden surfaces can
still update state and show themselves later.

Keyboard Focus
--------------

Focus is explicit. A surface becomes eligible for direct keyboard focus
after you call
:cpp:any:`SurfaceFlags::setFocusable(true) <erbsland::cterm::ui::SurfaceFlags::setFocusable()>`.
Interactive controls do this themselves. You can move focus with
:cpp:any:`Surface::requestFocus() <erbsland::cterm::ui::Surface::requestFocus()>`
or :cpp:any:`Page::focusTo() <erbsland::cterm::ui::Page::focusTo()>`.

The focused surface receives key events first, then its ancestors, then
the page. Ancestors on the active path report
:cpp:any:`SurfaceFlags::hasFocusWithin() <erbsland::cterm::ui::SurfaceFlags::hasFocusWithin()>`;
the leaf also reports
:cpp:any:`SurfaceFlags::hasFocus() <erbsland::cterm::ui::SurfaceFlags::hasFocus()>`.
The framework keeps the theme states ``Focused`` and ``FocusWithin`` in
sync and calls ``onFocus()`` only when the effective relation changes.

Mutable Surface State
---------------------

``Surface`` keeps the tree, geometry, scheduler, actions, events, and
focus request command small and direct. Mutable state that describes how
the surface participates in rendering and focus traversal lives in
:cpp:any:`SurfaceFlags <erbsland::cterm::ui::SurfaceFlags>` and is
accessed through ``surface->flags()``.

Use ``SurfaceFlags`` for local visibility, focusability, enabled,
selected, and checked state, as well as paint, layout, and theme
invalidation. ``flags().themeStates()`` derives the resolved theme
states from the current flags, including ``Focused``, ``FocusWithin``,
``Disabled``, ``Selected``, and ``Checked``.

Theme selector data that is normally configured once belongs to
:cpp:any:`SurfaceThemeAttributes
<erbsland::cterm::ui::SurfaceThemeAttributes>` and is accessed through
``surface->themeAttributes()``. Built-in surfaces set their element in
their constructor. Custom surfaces should do the same, either by calling
a theme-aware ``Surface`` constructor or by updating
``themeAttributes()`` before the surface is displayed.

Managing Child Containers
-------------------------

``Surface::surfaces()`` returns
:cpp:any:`AbstractSurfaceContainer <erbsland::cterm::ui::AbstractSurfaceContainer>`.
Surfaces store their children in
:cpp:any:`SurfaceContainer <erbsland::cterm::ui::SurfaceContainer>`,
which supports add, insert, replace, remove, and move operations. The
container keeps parent links, reparenting, cycle checks, layout
invalidation, and focus cleanup in one place.

Layouts can attach a
:cpp:any:`SurfaceManager <erbsland::cterm::ui::SurfaceManager>` to their
container. The manager does not replace the container; it only validates
and observes structural changes. This keeps generic code useful while
still allowing a layout to say, for example, "I accept at most one child"
or "this child needs default metadata".

For single-content layouts such as
:cpp:any:`Centered <erbsland::cterm::ui::layout::Centered>`,
:cpp:any:`Frame <erbsland::cterm::ui::layout::Frame>`, and
:cpp:any:`Viewport <erbsland::cterm::ui::layout::Viewport>`, both forms
below are valid when the layout is empty:

.. code-block:: cpp

    auto viewport = ui::Viewport::create();
    auto content = ui::TextBox::create("Long text");

    viewport->setContentSurface(content);
    viewport->surfaces().remove(content);
    viewport->surfaces().add(content);

Adding a second child to such a layout throws, because it would violate
the layout's real invariant. Removing the only child is accepted and
leaves the layout in a stable empty state.

Some surfaces still protect implementation-owned children.
:cpp:any:`ScrollArea <erbsland::cterm::ui::layout::ScrollArea>` owns its
viewport, scroll bars, and scroll corner internally. You configure its
user content with ``setContentSurface()``, but you should not remove or
reorder the scroll area's internal child surfaces through ``surfaces()``.

When a layout needs metadata per child, it stores that metadata as
:cpp:any:`LayoutData <erbsland::cterm::ui::LayoutData>` on the
parent-child relation. :cpp:any:`Sections <erbsland::cterm::ui::layout::Sections>`
uses this mechanism for section titles and right-side text. Generic
``surfaces().add()`` creates default empty section options, while
``addSection(surface, options)`` attaches explicit metadata.

Reference
=========

.. doxygenfunction:: erbsland::cterm::ui::getApplication

.. doxygenclass:: erbsland::cterm::ui::Application
    :members:

.. doxygenclass:: erbsland::cterm::ui::Display
    :members:

.. doxygenclass:: erbsland::cterm::ui::DimensionPolicy
    :members:

.. doxygenclass:: erbsland::cterm::ui::SizePolicy
    :members:

.. doxygenclass:: erbsland::cterm::ui::LayoutMetrics
    :members:

.. doxygenclass:: erbsland::cterm::ui::LayoutMetricsEditor
    :members:

.. doxygenclass:: erbsland::cterm::ui::LayoutProposal
    :members:

.. doxygenclass:: erbsland::cterm::ui::LayoutDimension
    :members:

.. doxygenclass:: erbsland::cterm::ui::MeasureScope
    :members:

.. doxygenclass:: erbsland::cterm::ui::LayoutScope
    :members:

.. doxygenclass:: erbsland::cterm::ui::PaintContext
    :members:

.. doxygenclass:: erbsland::cterm::ui::ThemeContext
    :members:

.. doxygenclass:: erbsland::cterm::ui::LayoutContext
    :members:

.. doxygenclass:: erbsland::cterm::ui::AbstractSurfaceContainer
    :members:

.. doxygenclass:: erbsland::cterm::ui::SurfaceContainer
    :members:

.. doxygenclass:: erbsland::cterm::ui::LayoutData
    :members:

.. doxygenclass:: erbsland::cterm::ui::SurfaceManager
    :members:

.. doxygenclass:: erbsland::cterm::ui::SurfaceFlags
    :members:

.. doxygenclass:: erbsland::cterm::ui::SurfaceThemeAttributes
    :members:

.. doxygenenum:: erbsland::cterm::ui::ScrollBarMode

.. doxygenenum:: erbsland::cterm::ui::FocusChange

.. doxygenclass:: erbsland::cterm::ui::Surface
    :members:

.. doxygenclass:: erbsland::cterm::ui::Layout
    :members:

.. doxygenclass:: erbsland::cterm::ui::Page
    :members:

.. doxygenclass:: erbsland::cterm::ui::page::Choice
    :members:
