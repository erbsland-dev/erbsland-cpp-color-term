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

:cpp:any:`AbstractLine <erbsland::cterm::ui::surface::AbstractLine>`
owns the one-line layout algorithm shared by headers, footers, and
compact status rows. Subclasses provide the section text, optional
margins, and collapse behavior.

.. code-block:: cpp

    class DemoLine final : public ui::AbstractLine {
    public:
        explicit DemoLine(ProtectedTag protectedTag) noexcept : AbstractLine{protectedTag} {}

        [[nodiscard]] static auto create() -> std::shared_ptr<DemoLine> {
            return std::make_shared<DemoLine>(ProtectedTag{});
        }

        [[nodiscard]] auto text(Section section) const -> const String & override {
            switch (section) {
            case Section::Left:
                return _left;
            case Section::Middle:
                return _middle;
            case Section::Right:
                return _right;
            }
            return _middle;
        }

    private:
        String _left{"Demo", Color{fg::BrightWhite, bg::Inherited}};
        String _middle{};
        String _right{"Ready", Color{fg::BrightYellow, bg::Inherited}};
    };

:cpp:any:`TextLine <erbsland::cterm::ui::surface::TextLine>` builds on
that base and owns three optional fields, so many apps do not need to
subclass at all. :cpp:any:`HeaderLine <erbsland::cterm::ui::surface::HeaderLine>`
is the same field-based surface with a separate theme element for header
styling.

.. code-block:: cpp

    auto header = ui::HeaderLine::create();
    using Section = ui::TextLine::Section;
    using UpdateMode = ui::TextLine::UpdateMode;
    header->setText(
        Section::Left,
        String{"Viewer", Color{fg::BrightWhite, bg::Inherited}});
    header->setMargins(Section::Left, Margins{1, 0});
    header->setUpdateMode(Section::Right, UpdateMode::OnRefresh);
    header->setUpdateFn(Section::Right, [](String &text, Coordinate width) {
        text = String{std::format("{} cells", width), Color{fg::BrightYellow, bg::Inherited}};
    });

The line background itself comes from the active theme. This lets you
change the full application color scheme without rewriting each field.

Line Layout Rules
-----------------

Line surfaces always use a fixed height of one row and resolve sections
in this order:

* left first
* right second
* middle last inside the remaining gap

The :cpp:any:`sectionWidth() <erbsland::cterm::ui::surface::AbstractLine::sectionWidth>`
value is the current pre-collapse width budget for that section. It
excludes the section's own left and right margins.

Section presence is determined by
:cpp:any:`hasText() <erbsland::cterm::ui::surface::AbstractLine::hasText>`.
Sections that are absent reserve no width and their margins are ignored.
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

Collapse behavior is evaluated after the width budget is known:

* ``Ellipsis`` trims the visible text to fit and appends or prepends one
  ellipsis marker depending on the anchor
* ``Hide`` omits the section entirely when it does not fit, releasing
  both text width and margins
* ``Never`` paints the full text and does not reserve width for sibling
  sections, so overlap is possible and the final drawing is clipped to
  the line rectangle

Owned Field Updates
-------------------

The concrete :cpp:any:`TextLine <erbsland::cterm::ui::surface::TextLine>`
stores one optional
:cpp:any:`Field <erbsland::cterm::ui::surface::TextLine::Field>` for
each section. A field can contain static text, margins, collapse
behavior, and an optional updater callback.

Automatic updates are controlled per field:

* ``Static`` never calls the updater, even if one is assigned. Update
  the text explicitly with ``setText()``.
* ``OnRefresh`` calls the updater before every paint pass when an
  updater exists.
* ``OnResize`` calls the updater when that section's current
  ``sectionWidth()`` changes. Width changes can come from terminal
  resizing, margin changes, sibling text appearing or disappearing, or
  sibling collapse behavior changing.

If no updater is set, the field behaves as static content even when the
stored update mode is ``OnRefresh`` or ``OnResize``.

Dynamic Text, Action Help, and Footers
--------------------------------------

:cpp:any:`DynamicText <erbsland::cterm::ui::surface::DynamicText>` is a
one-line text surface for compact labels whose content can update on
resize or every repaint. It is useful for status text that does not need
three sections.

:cpp:any:`ActionHelp <erbsland::cterm::ui::surface::ActionHelp>` finds
the nearest page, collects enabled actions from the focused surface
chain, deduplicates shared ``ActionPtr`` instances, and renders the
available keyboard help. :cpp:any:`FooterLine <erbsland::cterm::ui::surface::FooterLine>`
combines a left-side ``DynamicText`` with right-side action help and can
temporarily overlay centered status messages.

.. code-block:: cpp

    auto footer = ui::FooterLine::create();
    footer->leftText()->setUpdateMode(ui::DynamicText::UpdateMode::OnRefresh);
    footer->leftText()->setUpdateFn([](String &text, Coordinate width) {
        text = String{std::format("width {}", width)};
    });
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

.. doxygenclass:: erbsland::cterm::ui::surface::AbstractLine
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::TextLine
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
