..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

************
UI Framework
************

The :cpp:any:`erbsland::cterm::ui` module lets you build full-screen
terminal applications from a structured surface tree instead of manually
repainting one big buffer. Pages, layouts, and surfaces describe the UI
declaratively, while :cpp:any:`Application <erbsland::cterm::ui::Application>`
takes care of layout passes, paint invalidation, key routing, and timed
actions.

At a glance, the UI framework gives you:

* :cpp:any:`Application <erbsland::cterm::ui::Application>` as the
  runtime and terminal owner
* :cpp:any:`Page <erbsland::cterm::ui::Page>` and
  :cpp:any:`Surface <erbsland::cterm::ui::Surface>` as the tree model
* :cpp:any:`Stack <erbsland::cterm::ui::layout::Stack>` for common
  vertical and horizontal compositions
* built-in surfaces such as
  :cpp:any:`Panel <erbsland::cterm::ui::surface::Panel>`,
  :cpp:any:`TextBox <erbsland::cterm::ui::surface::TextBox>`,
  :cpp:any:`ScrollingBufferView <erbsland::cterm::ui::surface::ScrollingBufferView>`,
  :cpp:any:`AbstractStatusLine <erbsland::cterm::ui::surface::AbstractStatusLine>`,
  and :cpp:any:`StatusLine <erbsland::cterm::ui::surface::StatusLine>`
* :cpp:any:`KeyBindings <erbsland::cterm::ui::KeyBindings>` and surface
  schedulers for responsive interaction

.. important::

    The UI Framework is currently in beta, and parts of the API may
    change in future releases.

Quick Start
===========

The umbrella include ``<erbsland/cterm/ui/all.hpp>`` re-exports the
``layout`` and ``surface`` namespaces into ``ui``. This lets you write
compact code such as ``ui::Stack`` or ``ui::TextBox`` directly.

.. code-block:: cpp

    using namespace erbsland::cterm;
    using namespace erbsland::cterm::ui;

    auto page = Page::create();
    auto root = Stack::create(Orientation::Vertical);
    auto title = TextBox::create("Library Status", Alignment::Center);
    auto body = Panel::create();

    title->geometry().setFixedHeight(1);
    body->setBackground(Char{" ", Color{fg::Inherited, bg::Blue}});
    body->addChild(TextBox::create("Everything is running.", Alignment::Center));

    root->addChild(title);
    root->addChild(body);
    page->addChild(root);

    auto app = Application{};
    app.setMainPage(page);
    return app.run();
