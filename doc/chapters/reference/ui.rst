..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

************
UI Framework
************

The :cpp:any:`erbsland::cterm::ui` module lets you build terminal user
interfaces using a structured surface tree instead of manually updating
buffers. It follows a layout-driven approach where each surface defines
its size and behavior, and the framework takes care of rendering and
event propagation.

At its core, the module combines:

* :cpp:any:`Application <erbsland::cterm::ui::Application>` as the
  event-driven runtime managing one
  :cpp:any:`Terminal <erbsland::cterm::Terminal>`
* :cpp:any:`Page <erbsland::cterm::ui::Page>` and
  :cpp:any:`Surface <erbsland::cterm::ui::Surface>` as the root and node
  types of the UI tree
* geometry and size-policy types that control how space is distributed
* a small set of built-in layouts and surfaces such as
  :cpp:any:`ui::Stack <erbsland::cterm::ui::layout::Stack>`,
  :cpp:any:`ui::Panel <erbsland::cterm::ui::surface::Panel>`, and
  :cpp:any:`ui::TextBox <erbsland::cterm::ui::surface::TextBox>`

If you need timers, background workers, or details about the event
infrastructure, continue with :doc:`ui-events`.

.. important::

    The UI Framework is currently in beta, and parts of the API may
    change in future releases.

Usage
=====

Building a Minimal Surface Tree
-------------------------------

A typical application creates a single page, builds a small hierarchy of
surfaces, and hands control over to
:cpp:any:`Application <erbsland::cterm::ui::Application>`.

.. code-block:: cpp

    using namespace erbsland::cterm;
    using namespace erbsland::cterm::ui;

    auto page = Page::create();
    auto root = Stack::create(Orientation::Vertical);
    auto title = TextBox::create("Library Status", Alignment::Center);
    auto body = Panel::create();

    body->setBackground(Char{" ", Color{fg::Inherited, bg::Blue}});
    body->addChild(TextBox::create("Everything is running.", Alignment::Center));

    root->addChild(title);
    root->addChild(body);
    page->addChild(root);
    page->focusTo(body);

    auto app = Application{};
    app.setMainPage(page);
    return app.run();

The umbrella include ``<erbsland/cterm/ui/all.hpp>`` re-exports the
``layout`` and ``surface`` namespaces into ``ui``. This allows you to use
``Stack``, ``Panel``, and ``TextBox`` directly without additional
qualification.

Geometry and Size Policies
--------------------------

Each surface owns a
:cpp:any:`Geometry <erbsland::cterm::ui::Geometry>` object that defines
its layout constraints: minimum, preferred, and maximum size, along with
the size policy for each axis.

.. code-block:: cpp

    auto header = TextBox::create("Title", Alignment::Center);
    header->geometry().setFixedHeight(3);

    auto sidebar = Panel::create();
    sidebar->geometry().setPreferred(Size{18, 0});
    sidebar->geometry().setSizePolicy(SizePolicy{
        DimensionPolicy{DimensionPolicy::Preferred},
        DimensionPolicy{DimensionPolicy::Grow}});

    auto content = Panel::create();
    content->geometry().setSizePolicy(
        SizePolicy{DimensionPolicy{DimensionPolicy::Grow}});

This keeps layout decisions close to the surface that owns them. Layout
containers such as
:cpp:any:`Stack <erbsland::cterm::ui::layout::Stack>` evaluate these
policies when distributing the available space among their children.

Focus, Key Bindings, and Paint State
------------------------------------

Focus handling and input processing are integrated into the surface
tree. The active :cpp:any:`Page <erbsland::cterm::ui::Page>` routes key
events along the current focus chain, and each surface can handle input
locally.

.. code-block:: cpp

    auto page = Page::create();
    auto dialog = Panel::create();

    dialog->keyBindings().bind(
        Key{Key::Escape},
        []() { getApplication().quit(); });

    dialog->setPaintOutdated();

    page->addChild(dialog);
    page->focusTo(dialog);

When a surface changes, you explicitly mark what needs updating:

* Call
  :cpp:any:`Surface::setLayoutOutdated() <erbsland::cterm::ui::Surface::setLayoutOutdated()>`
  if its geometry or layout dependencies changed.
* Call
  :cpp:any:`Surface::setPaintOutdated() <erbsland::cterm::ui::Surface::setPaintOutdated()>`
  if only the visual representation changed.

The renderer updates only the visible parts of the UI, which keeps
redraws efficient even for larger surface trees.

Built-In Layouts and Surfaces
-----------------------------

The UI module intentionally provides a small, composable set of building
blocks:

* :cpp:any:`Layout <erbsland::cterm::ui::Layout>` is the base class for
  container surfaces with background support.
* :cpp:any:`Stack <erbsland::cterm::ui::layout::Stack>` arranges children
  horizontally or vertically.
* :cpp:any:`Panel <erbsland::cterm::ui::surface::Panel>` fills a region
  with an optional background and can host child surfaces.
* :cpp:any:`TextBox <erbsland::cterm::ui::surface::TextBox>` renders a
  single :cpp:any:`String <erbsland::cterm::String>` and derives its
  preferred size from that content.

With these primitives, you can build dashboards, menus, dialog shells,
and panel-based tools. For custom behavior, derive from
:cpp:any:`Surface <erbsland::cterm::ui::Surface>` and override
``onLayout()``, ``onPaint()``, or ``onKeyPress()``.

Interface
=========

.. doxygenfunction:: erbsland::cterm::ui::getApplication

.. doxygenclass:: erbsland::cterm::ui::Application
    :members:

.. doxygenclass:: erbsland::cterm::ui::Display
    :members:

.. doxygenclass:: erbsland::cterm::ui::Geometry
    :members:

.. doxygenclass:: erbsland::cterm::ui::DimensionPolicy
    :members:

.. doxygenclass:: erbsland::cterm::ui::SizePolicy
    :members:

.. doxygenclass:: erbsland::cterm::ui::KeyBindings
    :members:

.. doxygenclass:: erbsland::cterm::ui::PaintContext
    :members:

.. doxygenclass:: erbsland::cterm::ui::Surface
    :members:

.. doxygenclass:: erbsland::cterm::ui::Layout
    :members:

.. doxygenclass:: erbsland::cterm::ui::Page
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::Panel
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::TextBox
    :members:

.. doxygenclass:: erbsland::cterm::ui::layout::Stack
    :members:

