..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*******
UI Core
*******

The core UI types define the runtime, the surface tree, and the layout
constraints shared by all higher-level UI components.

Usage
=====

Building a Surface Tree
-----------------------

A UI app usually creates one page, adds a small hierarchy of surfaces,
and then hands the tree to :cpp:any:`Application <erbsland::cterm::ui::Application>`.

.. code-block:: cpp

    using namespace erbsland::cterm;
    using namespace erbsland::cterm::ui;

    auto page = Page::create();
    auto root = Stack::create(Orientation::Vertical);
    auto body = TextBox::create("Ready", Alignment::Center);

    body->geometry().setSizePolicy(SizePolicy{SizePolicy::Grow});
    root->addChild(body);
    page->addChild(root);

    auto app = Application{};
    app.setMainPage(page);
    return app.run();

Working with Geometry and Invalidation
--------------------------------------

Each surface owns a
:cpp:any:`Geometry <erbsland::cterm::ui::Geometry>` object with minimum,
preferred, and maximum size hints plus a size policy for both axes.

.. code-block:: cpp

    auto header = TextBox::create("Title", Alignment::Center);
    auto body = Panel::create();

    header->geometry().setFixedHeight(1);
    body->geometry().setSizePolicy(SizePolicy{SizePolicy::Grow});

    body->setBackground(Char{" ", bg::Black});
    body->setPaintOutdated();

When something changes:

* call
  :cpp:any:`Surface::setLayoutOutdated() <erbsland::cterm::ui::Surface::setLayoutOutdated()>`
  when size or placement needs to be recalculated
* call
  :cpp:any:`Surface::setPaintOutdated() <erbsland::cterm::ui::Surface::setPaintOutdated()>`
  when only the rendered appearance changed

This keeps updates local and avoids redrawing unchanged parts of the
surface tree.

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

.. doxygenclass:: erbsland::cterm::ui::Geometry
    :members:

.. doxygenclass:: erbsland::cterm::ui::PaintContext
    :members:

.. doxygenclass:: erbsland::cterm::ui::Surface
    :members:

.. doxygenclass:: erbsland::cterm::ui::Layout
    :members:

.. doxygenclass:: erbsland::cterm::ui::Page
    :members:
