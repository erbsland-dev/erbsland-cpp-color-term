..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

**********
UI Layouts
**********

The built-in layout layer is intentionally small. It focuses on
high-value primitives that compose cleanly with layout-size policies:
:cpp:any:`Stack <erbsland::cterm::ui::layout::Stack>` for ordered
composition, :cpp:any:`Centered <erbsland::cterm::ui::layout::Centered>`
and :cpp:any:`Frame <erbsland::cterm::ui::layout::Frame>` for compact
one-child composition, :cpp:any:`Viewport <erbsland::cterm::ui::layout::Viewport>`
for clipped one-surface content, and
:cpp:any:`ScrollArea <erbsland::cterm::ui::layout::ScrollArea>` for
classic overflow-style scrolling with optional scroll bars. Use
:cpp:any:`Sections <erbsland::cterm::ui::layout::Sections>` for titled
vertical regions, and
:cpp:any:`Buttons <erbsland::cterm::ui::layout::Buttons>` for centered
action-button rows.

.. important::

    The UI Framework is currently in beta, and parts of the API may
    change in future releases.

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

    header->editLayoutMetrics().setFixedHeight(1);
    footer->editLayoutMetrics().setFixedHeight(1);
    content->editLayoutMetrics().setSizePolicy(ui::SizePolicy{ui::SizePolicy::Grow});

    root->addSurface(header);
    root->addSurface(content);
    root->addSurface(footer);

Building Columns
----------------

Use a horizontal stack to place multiple surfaces next to each other.
Each child keeps its own size policy, so fixed-width sidebars and one
growing main area are easy to express.

.. code-block:: cpp

    auto columns = ui::Stack::create(Orientation::Horizontal);
    auto sidebar = ui::Panel::create();
    auto mainView = ui::Panel::create();

    sidebar->editLayoutMetrics().setPreferredSize(Size{24, 0});
    mainView->editLayoutMetrics().setSizePolicy(ui::SizePolicy{ui::SizePolicy::Grow});

    columns->addSurface(sidebar);
    columns->addSurface(mainView);

Hiding Layout Children
----------------------

Layouts only allocate space for visible children. Hidden children keep
their last rectangle and are ignored until they are shown again. This
lets applications keep optional panels in the tree without constantly
removing and re-adding them.

.. code-block:: cpp

    auto stack = ui::Stack::create(Orientation::Vertical);
    auto header = ui::TextBox::create("Title");
    auto details = ui::Panel::create();
    auto body = ui::Panel::create();

    header->editLayoutMetrics().setFixedHeight(1);
    details->editLayoutMetrics().setFixedHeight(4);
    body->editLayoutMetrics().setSizePolicy(ui::SizePolicy{ui::SizePolicy::Grow});

    stack->addSurface(header);
    stack->addSurface(details);
    stack->addSurface(body);

    details->flags().setVisible(false); // body now receives the space details would have used

How Layout Margins Work
-----------------------

Margins in ``LayoutMetrics`` are recommendations to the parent layout.
They are different from padding: a surface owns its content and its own
padding, while the parent that arranges child surfaces owns the spacing
between those children. Metric sizes always describe the child content
rectangle without margins.

When a layout places adjacent child surfaces, it collapses the two
touching margins. The larger value wins and becomes spacing owned by the
layouting parent. In a vertical stack, the previous child's bottom
margin collapses with the next child's top margin. In a horizontal stack,
the previous child's right margin collapses with the next child's left
margin.

Margins around the outside of a layout depend on the role of that
layout:

* Pure layouts arrange peer surfaces. ``Stack`` and ``Buttons`` are pure
  layouts. They consume only the margins between children and propagate
  surrounding child margins through their own ``LayoutMetrics``. This is
  what lets nested layouts collapse margins at the level where siblings
  actually meet.
* Enclosing layouts place one selected or contained surface inside an
  area they own. ``Frame``, ``Pages``, and ``Viewport`` are enclosing
  layouts. They collapse their own padding or inset with the enclosed
  child's margins and place the child inside the reduced rectangle. The
  enclosed child's margins are not propagated further.

For example, in a horizontal stack of vertical stacks, the vertical
stacks propagate their left and right child margins. The horizontal
stack then collapses the margins between columns. If the vertical stacks
consumed those margins themselves, the column spacing would be added
instead of collapsed.

Clipping One Content Surface with Viewport
------------------------------------------

:cpp:any:`Viewport <erbsland::cterm::ui::layout::Viewport>` manages one
content surface. The clearest API is ``setContentSurface()``, but the
generic child container works too as long as the layout still has at most
one child. Removing the content is valid and leaves the viewport empty.

.. code-block:: cpp

    auto viewport = ui::Viewport::create();
    auto map = ui::Panel::create();

    map->editLayoutMetrics().setFixedSize(Size{200, 80});
    viewport->setContentSurface(map);
    viewport->setScrollOffset(Position{20, 10});
    viewport->setAlignment(Alignment::Center);

    viewport->surfaces().remove(map);
    viewport->surfaces().add(map);

If the content is smaller than the viewport on an axis, the alignment
anchors it inside the viewport. If the content is larger, the
``scrollOffset`` moves it relative to that regular placement. The offset
is always clamped to the valid range.

Use the common scroll helpers from actions:

.. code-block:: cpp

    auto upAction = ui::Action::create("up");
    upAction->addKey(Key::Up);
    upAction->setTriggerFn([viewport] { viewport->scrollUp(); });
    page->actions().add(upAction);

    auto pageDownAction = ui::Action::create("page down");
    pageDownAction->addKey(Key::PageDown);
    pageDownAction->setTriggerFn([viewport] { viewport->pageDown(); });
    page->actions().add(pageDownAction);

    auto topAction = ui::Action::create("top");
    topAction->addKey(Key::Home);
    topAction->setTriggerFn([viewport] { viewport->scrollToTop(); });
    page->actions().add(topAction);

Building Classic Overflow Areas
-------------------------------

:cpp:any:`ScrollArea <erbsland::cterm::ui::layout::ScrollArea>` is the
high-level layout for a scrollable child surface. It expands by default,
owns a viewport, owns persistent scroll bar surfaces, and forwards the
same scroll API as the viewport.

.. code-block:: cpp

    auto scrollArea = ui::ScrollArea::create();
    auto document = ui::Panel::create();

    document->editLayoutMetrics().setFixedSize(Size{120, 80});
    scrollArea->setContentSurface(document);
    scrollArea->setAlignment(Alignment::TopLeft);
    scrollArea->setScrollBarMode(Orientation::Vertical, ui::ScrollBarMode::Automatic);
    scrollArea->setScrollBarMode(Orientation::Horizontal, ui::ScrollBarMode::Hidden);

    auto downAction = ui::Action::create("down");
    downAction->addKey(Key::Down);
    downAction->setTriggerFn([scrollArea] { scrollArea->scrollDown(); });
    page->actions().add(downAction);

``ScrollBarMode::Hidden`` hides a bar and reserves no space.
``ScrollBarMode::Automatic`` shows a bar only when content overflows on
that axis. ``ScrollBarMode::Visible`` reserves a bar whenever the
scroll area's size permits it. Automatic mode uses a stable pass because
one axis can force the other; for example, a vertical bar consumes one
column and may make horizontal overflow appear.

The scroll bars and corner are persistent child surfaces. They are shown
and hidden with the surface visibility flag instead of being recreated
when the scroll area resizes. Access them for styling through
``horizontalScrollBar()``, ``verticalScrollBar()``, and
``scrollCorner()``. These implementation children are protected from
generic public mutation, so use ``setContentSurface()`` for the user
content and leave the scroll area structure intact.

Centering and Framing One Child
-------------------------------

:cpp:any:`Centered <erbsland::cterm::ui::layout::Centered>` manages one
content surface, leaves configurable padding around it, and centers the
child in the remaining area. It is useful for dialogs and empty-state
views.

:cpp:any:`Frame <erbsland::cterm::ui::layout::Frame>` also manages one
content surface. It paints a themed border and optional title, then lays
the child into the inner rectangle after border and padding have been
removed.

Both layouts inherit the same single-content behavior as ``Viewport``:
they tolerate zero children, accept one child through either
``setContentSurface()`` or ``surfaces().add()``, and reject a second
child with an exception.

.. code-block:: cpp

    auto centered = ui::Centered::create();
    auto frame = ui::Frame::create();
    auto body = ui::Stack::create(Orientation::Vertical);

    centered->setPadding(Margins{4, 2});
    centered->setContentSurface(frame);
    frame->setTitle(String{"Prompt"});
    frame->setPadding(Margins{1, 2});
    frame->setContentSurface(body);

    // Equivalent when the layout is empty:
    // frame->surfaces().add(body);

Building Titled Sections
------------------------

Use :cpp:any:`Sections <erbsland::cterm::ui::layout::Sections>` when a
vertical area should read as named blocks. Section metadata belongs to
the layout, not to the child surface, so the same surface can stay simple
and reusable.

.. code-block:: cpp

    auto sections = ui::Sections::create();
    sections->addSection(
        ui::TextBox::create("Licensed to.........: Example GmbH"),
        ui::Sections::SectionOptions{String{"License Info"}, String{"[h]"}});

Section options are stored as layout data on the parent-child relation.
If you add a child through ``sections->surfaces().add(surface)``, the
layout creates default empty options for it. You can update them later
with ``setSectionOptions()``.

The default theme draws ``─⟨ title ⟩────``. Customize the separator
through the ``Sections`` theme element: ``Border`` defines the separator
line and title/right insets, ``Title`` defines the title style and
padding, and ``TitleBracket`` defines the bracket style and bracket
glyphs. Title padding is painted as part of the title, while title
margins collapse with adjacent one-line text parts. Use theme tags when
only selected section groups should use a different decoration.

Laying Out Buttons
------------------

:cpp:any:`Buttons <erbsland::cterm::ui::layout::Buttons>` centers
button actions and wraps them to additional rows when the available
width is too small. Arrow keys, ``Tab``, and ``BackTab`` move focus
between enabled buttons.

Buttons require
:cpp:any:`ButtonAction <erbsland::cterm::ui::ButtonAction>` so the
action and button surface can keep their enabled state synchronized.
Generic :cpp:any:`Action <erbsland::cterm::ui::Action>` remains the right
choice for page actions, surface key bindings, help entries, and
scheduled or non-button commands.

.. code-block:: cpp

    auto buttons = ui::Buttons::create();

    auto openAction = ui::ButtonAction::create("Open");
    openAction->addKey(U'o');
    openAction->setTriggerFn([] { openDocument(); });

    buttons->addAction(openAction);

Button spacing is resolved from ``theme::Element::Buttons`` and
``theme::Part::Spacing``. The spacing part uses block-style content
margins: left/right configure horizontal spacing between buttons, and
top/bottom configure vertical spacing between wrapped rows.

``Buttons`` also supports generic child mutation for actual
``surface::Button`` instances. Non-button children are rejected, and the
layout keeps the represented actions synchronized as buttons are added,
replaced, or removed.

Reference
=========

.. doxygenclass:: erbsland::cterm::ui::layout::Stack
    :members:

.. doxygenclass:: erbsland::cterm::ui::layout::Centered
    :members:

.. doxygenclass:: erbsland::cterm::ui::layout::Frame
    :members:

.. doxygenclass:: erbsland::cterm::ui::layout::SingleContentLayout
    :members:

.. doxygenclass:: erbsland::cterm::ui::layout::Viewport
    :members:

.. doxygenclass:: erbsland::cterm::ui::layout::ScrollArea
    :members:

.. doxygenclass:: erbsland::cterm::ui::layout::Sections
    :members:

.. doxygenclass:: erbsland::cterm::ui::layout::Buttons
    :members:
