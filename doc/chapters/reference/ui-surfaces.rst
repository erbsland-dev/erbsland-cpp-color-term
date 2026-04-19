..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

***********
UI Surfaces
***********

Surfaces are the visible building blocks of the UI framework. The
built-in set focuses on common terminal patterns: fills, text labels,
scrolling buffer content, and single-line status bars.

Usage
=====

Painting Backgrounds with Panel
-------------------------------

:cpp:any:`Panel <erbsland::cterm::ui::surface::Panel>` fills its region
with an optional background and can host child surfaces.

.. code-block:: cpp

    auto body = ui::Panel::create();
    body->setBackground(Char{" ", bg::Black});
    body->addChild(ui::TextBox::create("Connected", Alignment::Center));

Centering Labels with TextBox
-----------------------------

:cpp:any:`TextBox <erbsland::cterm::ui::surface::TextBox>` renders one
terminal string using the same alignment and wrapping rules as
:cpp:any:`Text <erbsland::cterm::Text>`.

.. code-block:: cpp

    auto title = ui::TextBox::create("Hello World!", Alignment::Center);
    title->geometry().setFixedHeight(3);

Making Buffer Views Scrollable
------------------------------

:cpp:any:`ScrollingBufferView <erbsland::cterm::ui::surface::ScrollingBufferView>`
turns a readable buffer into a ready-to-use scrollable surface. This is
especially useful with
:cpp:any:`CursorBuffer <erbsland::cterm::CursorBuffer>` or
:cpp:any:`BufferView <erbsland::cterm::BufferView>` style content.

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

    page->keyBindings().bind({Key{Key::Up}, Key{Key::Character, U'k'}}, [view] {
        view->scrollUp();
    });
    page->keyBindings().bind({Key::PageDown, Key::Space}, [view] {
        view->pageDown();
    });

Building Reusable Status Bars
-----------------------------

:cpp:any:`AbstractStatusLine <erbsland::cterm::ui::surface::AbstractStatusLine>`
owns the one-line status-bar layout algorithm. Subclasses provide the
section text, optional margins, and collapse behavior.

.. code-block:: cpp

    class DemoStatusLine final : public ui::AbstractStatusLine {
    public:
        explicit DemoStatusLine(ProtectedTag protectedTag) noexcept : AbstractStatusLine{protectedTag} {}

        [[nodiscard]] static auto create() -> std::shared_ptr<DemoStatusLine> {
            return std::make_shared<DemoStatusLine>(ProtectedTag{});
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
        String _left{"Demo", Color{fg::BrightWhite, bg::Blue}};
        String _middle{};
        String _right{"[Q] quit", Color{fg::BrightYellow, bg::Blue}};
    };

:cpp:any:`StatusLine <erbsland::cterm::ui::surface::StatusLine>` builds
on that base and owns three optional fields, so many apps no longer need
to subclass at all.

.. code-block:: cpp

    auto header = ui::StatusLine::create();
    header->setBackground(Char{" ", bg::Blue});
    header->setText(
        ui::StatusLine::Section::Left,
        String{"Viewer", Color{fg::BrightWhite, bg::Blue}});
    header->setMargins(ui::StatusLine::Section::Left, Margins{0, 1, 0, 1});
    header->setUpdateMode(
        ui::StatusLine::Section::Right,
        ui::StatusLine::UpdateMode::OnRefresh);
    header->setUpdateFn(ui::StatusLine::Section::Right, [](String &text, Coordinate width) {
        text = String{std::format("{} cells", width), Color{fg::BrightYellow, bg::Blue}};
    });

Layout Rules
------------

Status lines always use a fixed height of one row and resolve sections
in this order:

* left first
* right second
* middle last inside the remaining gap

The :cpp:any:`sectionWidth() <erbsland::cterm::ui::surface::AbstractStatusLine::sectionWidth>`
value is the current pre-collapse width budget for that section. It
excludes the section's own left and right margins.

Section presence is determined by
:cpp:any:`hasText() <erbsland::cterm::ui::surface::AbstractStatusLine::hasText>`.
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

The concrete :cpp:any:`StatusLine <erbsland::cterm::ui::surface::StatusLine>`
stores one optional
:cpp:any:`Field <erbsland::cterm::ui::surface::StatusLine::Field>` for
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

Reference
=========

.. doxygenclass:: erbsland::cterm::ui::surface::Panel
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::TextBox
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::ScrollingBufferView
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::AbstractStatusLine
    :members:

.. doxygenclass:: erbsland::cterm::ui::surface::StatusLine
    :members:
