..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

***********
UI Surfaces
***********

Surfaces are the visible building blocks of the UI framework. The
built-in set focuses on common terminal patterns: fills, text labels,
scrolling buffer content, visual scroll bars, and single-line status
bars.

.. important::

    The UI Framework is currently in beta, and parts of the API may
    change in future releases.

Usage
=====

Painting Themed Backgrounds with Panel
--------------------------------------

:cpp:any:`Panel <erbsland::cterm::ui::surface::Panel>` fills its region
with the theme block and style for ``theme::Element::Panel`` and
``theme::Part::Background``. Use a custom theme when you want panels to
have a different fill character, color, or inset rules across the whole
application.

.. code-block:: cpp

    auto body = ui::Panel::create();
    body->addSurface(ui::TextBox::create("Connected", Alignment::Center));

Panels can host child surfaces, so they are useful as themed background
regions in larger layouts. The :doc:`theme system <theme>` keeps this
styling out of each individual panel instance.

Centering Labels with TextBox
-----------------------------

:cpp:any:`TextBox <erbsland::cterm::ui::surface::TextBox>` renders one
terminal string using the same alignment and wrapping rules as
:cpp:any:`Text <erbsland::cterm::Text>`.
The ``Text`` theme part may add margins and padding around this content:
margins stay outside the text box's painted area, while padding is filled
with the text part's background block and style.

.. code-block:: cpp

    auto title = ui::TextBox::create("Hello World!", Alignment::Center);
    title->editLayoutMetrics().setFixedHeight(3);

Making Buffer Views Scrollable
------------------------------

:cpp:any:`ScrollingBufferView <erbsland::cterm::ui::surface::ScrollingBufferView>`
turns a readable buffer into a ready-to-use scrollable surface. This is
especially useful with
:cpp:any:`CursorBuffer <erbsland::cterm::CursorBuffer>` or
:cpp:any:`BufferView <erbsland::cterm::BufferView>` style content.
It derives from :cpp:any:`AbstractScrollArea <erbsland::cterm::ui::surface::AbstractScrollArea>`,
so it uses the same ``scrollOffset`` API and optional scroll bars as
other scrollable UI components.

.. code-block:: cpp

    auto history = std::make_shared<CursorBuffer>(
        Size{80, 1},
        CursorBuffer::OverflowMode::ExpandThenShift,
        Size{80, 500},
        Char{" ", bg::Black});

    auto view = ui::ScrollingBufferView::create(history);
    view->setShowCropCharacters(true);
    view->setCropCharacter(Direction::North, Char{U'▲', fg::BrightBlue, bg::Black});
    view->setCropCharacter(Direction::South, Char{U'▼', fg::BrightBlue, bg::Black});
    view->setScrollBarMode(Orientation::Vertical, ui::ScrollBarMode::Automatic);

    auto scrollUpAction = ui::Action::create("up");
    scrollUpAction->setKeys({Key::Up, U'k'});
    scrollUpAction->setTriggerFn([view] { view->scrollUp(); });
    page->actions().add(scrollUpAction);

    auto pageDownAction = ui::Action::create("page down");
    pageDownAction->setKeys({Key::PageDown, Key::Space});
    pageDownAction->setTriggerFn([view] { view->pageDown(); });
    page->actions().add(pageDownAction);

``visibleContentRect()`` reports the buffer rectangle currently visible
through the viewport. ``scrollIntoView()`` adjusts the offset by the
smallest amount needed to reveal a content rectangle.

Painting Custom Scrollable Surfaces
-----------------------------------

Use :cpp:any:`AbstractScrollArea <erbsland::cterm::ui::surface::AbstractScrollArea>`
when the content is not represented by one child surface and is instead
painted directly by a custom surface. The base class owns the scrolling
state, common scroll helpers, scroll bar modes, and scroll bar surfaces.
Subclasses provide content metrics and painting.

.. code-block:: cpp

    class WorldView final : public ui::AbstractScrollArea {
    public:
        explicit WorldView(ProtectedTag protectedTag) noexcept : AbstractScrollArea{protectedTag} {}

        [[nodiscard]] static auto create() -> std::shared_ptr<WorldView> {
            auto view = std::make_shared<WorldView>(ProtectedTag{});
            view->initializeScrollAreaChildren();
            return view;
        }

    protected:
        [[nodiscard]] auto contentSizeForViewport(Size viewportSize) const noexcept -> Size override {
            static_cast<void>(viewportSize);
            return Size{360, 180};
        }

        void onPaintArea(
            Position scrollOffset,
            Rectangle targetRect,
            WritableBuffer &buffer,
            const ui::PaintContext &context) noexcept override {
            static_cast<void>(context);
            // Paint content coordinates starting at scrollOffset into targetRect.
        }
    };

The ``targetRect`` passed to ``onPaintArea()`` is the local viewport
rectangle. ``scrollOffset`` is already clamped for the current content
and viewport size. Applications bind keys to ``scrollUp()``,
``pageDown()``, ``scrollToBottom()``, or any of the other scroll helpers.

Showing Scroll Position with Scroll Bars
----------------------------------------

:cpp:any:`HorizontalScrollBar <erbsland::cterm::ui::surface::HorizontalScrollBar>`
and :cpp:any:`VerticalScrollBar <erbsland::cterm::ui::surface::VerticalScrollBar>`
render visual indicators for a visible range inside a larger scroll
region. They do not handle input themselves, so application code remains
free to connect them to any scrolling model.
``AbstractScrollArea`` and ``ScrollArea`` update their owned scroll bars
automatically, so manual region setup is only needed when using scroll
bars as standalone visual surfaces.

.. code-block:: cpp

    auto horizontal = ui::HorizontalScrollBar::create();
    horizontal->setScrollRegion(IndexRange{0, 2'000});
    horizontal->setVisibleRegion(IndexRange{320, 160});

    auto vertical = ui::VerticalScrollBar::create();
    vertical->setScrollRegion(IndexRange{0, 800});
    vertical->setVisibleRegion(IndexRange{120, 40});

Scroll bar colors, track blocks, thumb blocks, and track content margins
come from the active theme. Set a theme on the application or page when
you want all owned and standalone scroll bars to share the same visual
language.

Layout Rules
------------

Scroll bars always use a fixed thickness of one cell:

* horizontal scroll bars keep a fixed height of ``1`` and grow in width
* vertical scroll bars keep a fixed width of ``1`` and grow in height

The active theme paints the track across the full scroll bar rectangle.
The track block's content margins define the usable area for the thumb.
The default classic terminal blocks use one-cell margins for arrow
cells, while plain blocks allow the thumb to use the full surface. The
handle is projected from ``visibleRegion()`` into the usable track area
using :cpp:any:`IndexRange <erbsland::cterm::IndexRange>`'s exclusive end
semantics.

If the visible region does not intersect the scroll region, the surface
renders only the themed track. A handle that would otherwise become too
small is expanded to stay readable:

* tracks with three or more cells use at least a three-cell thumb
* a two-cell track uses a two-cell thumb
* a one-cell track uses a one-cell thumb

Building Reusable Lines
-----------------------

:cpp:any:`DynamicTextLine <erbsland::cterm::ui::surface::DynamicTextLine>`
is a composed one-line surface for headers and compact status rows. It
owns three managed :cpp:any:`DynamicText <erbsland::cterm::ui::surface::DynamicText>`
children for the left, middle, and right sections. The line calculates
where those children belong; the children paint and crop their own text.

.. code-block:: cpp

    auto header = ui::HeaderLine::create();
    using Section = ui::DynamicTextLine::Section;
    using SpacePriority = ui::DynamicTextLine::SpacePriority;

    header->setText(
        Section::Left,
        String{"Viewer", Color{fg::BrightWhite, bg::Inherited}});
    header->setMargins(Section::Left, Margins{1, 0});

    header->dynamicText(Section::Right)->setUpdateFn([](String &text, Coordinate width) {
        text = String{std::format("{} cells", width), Color{fg::BrightYellow, bg::Inherited}};
    });
    header->dynamicText(Section::Right)->updateText();
    header->setSpacePriority(Section::Right, SpacePriority::Shrink);

The line background itself comes from the active theme. This lets you
change the full application color scheme without rewriting each section.
:cpp:any:`HeaderLine <erbsland::cterm::ui::surface::HeaderLine>` is a
thin specialization with its own theme element for header styling.

Line Layout Rules
-----------------

Line surfaces always use a fixed height of one row and resolve sections
in this order:

* left first
* right second
* middle last inside the remaining gap

The :cpp:any:`sectionWidth() <erbsland::cterm::ui::surface::DynamicTextLine::sectionWidth>`
value is the current width assigned to the child text surface. It
excludes the section's own left and right margins.

Sections with empty text reserve no width and their margins are ignored.
This means a header with only a left section can naturally use the whole
line, while a middle section shrinks only when visible left or right
sections actually consume space.

Margins only affect the horizontal layout:

* the left section uses its left margin as an inset from the line edge
  and its right margin as spacing towards the center
* the middle section uses left and right margins as breathing room
  around the centered text
* the right section uses its right margin as an inset from the line edge
  and its left margin as spacing towards the center

The managed ``DynamicText`` children also honor horizontal margins and
padding from the line's ``Text`` theme part. Top and bottom values are
ignored because these are strict one-line surfaces.

Space priority is evaluated after the width budget is known:

* ``Keep`` preserves the section's preferred width even if sections
  overlap.
* ``Shrink`` assigns no more than the available width and lets
  ``DynamicText`` crop the visible text.
* ``Hide`` omits the section entirely when it does not fit, releasing
  both text width and margins.

Use :cpp:any:`dynamicText() <erbsland::cterm::ui::surface::DynamicTextLine::dynamicText>`
when you need the full text-surface API, for example to install an
update callback, schedule periodic updates, or adjust alignment.

Dynamic Text, Action Help, and Footers
--------------------------------------

:cpp:any:`DynamicText <erbsland::cterm::ui::surface::DynamicText>` is a
one-line text surface for compact labels whose content can update on
resize or every repaint. It is useful for status text that does not need
three sections.

:cpp:any:`ActionHelp <erbsland::cterm::ui::surface::ActionHelp>` finds
the nearest page, collects enabled actions from the focused surface
chain, deduplicates shared ``ActionPtr`` instances, and renders the
available keyboard help. The spaces between key brackets, key labels,
and action names come from collapsed horizontal margins on the ``Key``,
``KeyBracket``, and ``Text`` theme parts.
:cpp:any:`FooterLine <erbsland::cterm::ui::surface::FooterLine>`
combines a left-side ``DynamicText`` with right-side action help and can
temporarily overlay centered status messages.

.. code-block:: cpp

    auto footer = ui::FooterLine::create();
    footer->leftText()->setUpdateFn([](String &text, Coordinate width) {
        text = String{std::format("width {}", width)};
    });
    footer->leftText()->updateText();
    footer->displayMessage(
        "Saved",
        CharStyle{Color{fg::BrightWhite, bg::Blue}},
        std::chrono::milliseconds{1500});

Messages are queued. A timeout of ``0ms`` keeps the current message
visible until ``hideMessage()`` is called, then the next queued message
is shown.

Action Buttons
--------------

:cpp:any:`Button <erbsland::cterm::ui::surface::Button>` is a compact
focusable view for one :cpp:any:`Action <erbsland::cterm::ui::Action>`.
It displays the action name and main key, triggers on ``Enter`` and
``Space`` while focused, and still lets the action key dispatch through
the normal focus chain.

Reference
=========

.. doxygenclass:: erbsland::cterm::ui::surface::Panel
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::TextBox
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::AbstractScrollArea
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::ScrollingBufferView
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::AbstractScrollBar
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::HorizontalScrollBar
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::VerticalScrollBar
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::ScrollCorner
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::DynamicTextLine
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::HeaderLine
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::DynamicText
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::ActionHelp
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::Button
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::FooterLine
    :members:
