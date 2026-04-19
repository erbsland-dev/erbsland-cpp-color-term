..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

**********
UI Layouts
**********

The built-in layout layer is intentionally small. It focuses on one
high-value primitive that composes cleanly with geometry policies:
:cpp:any:`Stack <erbsland::cterm::ui::layout::Stack>`.

Usage
=====

Building Header, Body, and Footer Layouts
-----------------------------------------

Use a vertical stack when you want fixed top or bottom rows around one
growing center area.

.. code-block:: cpp

    auto root = ui::Stack::create(Orientation::Vertical);

    auto header = ui::TextBox::create("Demo", Alignment::Center);
    auto content = ui::Panel::create();
    auto footer = ui::TextBox::create("Press Q to quit", Alignment::CenterLeft);

    header->geometry().setFixedHeight(1);
    footer->geometry().setFixedHeight(1);
    content->geometry().setSizePolicy(ui::SizePolicy{ui::SizePolicy::Grow});

    root->addChild(header);
    root->addChild(content);
    root->addChild(footer);

Building Columns
----------------

Use a horizontal stack to place multiple surfaces next to each other.
Each child keeps its own size policy, so fixed-width sidebars and one
growing main area are easy to express.

.. code-block:: cpp

    auto columns = ui::Stack::create(Orientation::Horizontal);
    auto sidebar = ui::Panel::create();
    auto mainView = ui::Panel::create();

    sidebar->geometry().setPreferred(Size{24, 0});
    mainView->geometry().setSizePolicy(ui::SizePolicy{ui::SizePolicy::Grow});

    columns->addChild(sidebar);
    columns->addChild(mainView);

Reference
=========

.. doxygenclass:: erbsland::cterm::ui::layout::Stack
    :members:
