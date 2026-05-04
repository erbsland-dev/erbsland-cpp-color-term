..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

******
Themes
******

The :cpp:any:`erbsland::cterm::theme` namespace provides the theme
system used by the UI framework. A theme keeps visual decisions out of
individual controls, so you can change the look of an application
without teaching every scroll bar, panel, or status line about the same
colors, block shapes, margins, and padding again.

The public include for this subsystem is:

.. code-block:: cpp

    #include <erbsland/cterm/theme/all.hpp>

The UI umbrella include also makes the theme types available for normal
UI applications:

.. code-block:: cpp

    #include <erbsland/cterm/ui/all.hpp>

Core Model
==========

A theme is an immutable hierarchy of property sheets:

* :cpp:any:`ThemeBuilder <erbsland::cterm::theme::ThemeBuilder>` is the
  mutable authoring API. It stores one
  :cpp:any:`Properties <erbsland::cterm::theme::Properties>` object per
  :cpp:any:`Selector <erbsland::cterm::theme::Selector>`.
* :cpp:any:`Theme <erbsland::cterm::theme::Theme>` freezes those authored
  definitions and resolves fallback chains on demand.
* :cpp:any:`PropertySheet <erbsland::cterm::theme::PropertySheet>` is the
  effective result for one selector. It contains the resolved color,
  color sequence, attributes, block table, margins, and padding.

This model lets you change text styling, terminal-cell graphics, and
layout spacing through the same selector tree. You can override only the
value you care about and inherit the remaining properties from the
nearest base sheet.

The default application theme is
:cpp:any:`Theme::dark() <erbsland::cterm::theme::Theme::dark>`. You can
replace it on the application, or override it on a page:

.. code-block:: cpp

    using namespace erbsland::cterm;

    auto app = ui::Application{};
    app.setTheme(theme::Theme::light());

    auto page = ui::Page::create();
    page->setTheme(theme::Theme::monochrome());

For small customizations, start with a builder copied from an existing
theme and edit only the affected selectors:

.. code-block:: cpp

    auto builder = theme::ThemeBuilder::from(theme::Theme::dark());
    builder.edit(theme::Selector{theme::Element::StatusLine, theme::Part::Background})
        .setColor(Color{fg::BrightWhite, bg::Blue});

    app.setTheme(builder.build());

Selectors
=========

Theme lookup uses compact identifiers instead of strings in the paint
loop. A selector has four parts:

* :cpp:any:`Element <erbsland::cterm::theme::Element>` describes the
  themed control or surface.
* :cpp:any:`Part <erbsland::cterm::theme::Part>` describes a visual role
  inside the element.
* :cpp:any:`States <erbsland::cterm::theme::States>` describes required
  UI states such as focus or disabled.
* :cpp:any:`Tags <erbsland::cterm::theme::Tags>` describes optional,
  application-defined variants.

The built-in elements are static constants on
:cpp:any:`Element <erbsland::cterm::theme::Element>`:

.. code-block:: cpp

    theme::Element::Base
    theme::Element::Page
    theme::Element::Layout
    theme::Element::Surface
    theme::Element::Panel
    theme::Element::TextBox
    theme::Element::StatusLine
    theme::Element::HeaderLine
    theme::Element::FooterLine
    theme::Element::ActionHelp
    theme::Element::HorizontalScrollBar
    theme::Element::VerticalScrollBar
    theme::Element::ScrollCorner
    theme::Element::Sections
    theme::Element::Buttons
    theme::Element::Button
    theme::Element::Frame
    theme::Element::Choice
    theme::Element::HelpViewer

The built-in parts are static constants on
:cpp:any:`Part <erbsland::cterm::theme::Part>`:

.. code-block:: cpp

    theme::Part::Background
    theme::Part::Text
    theme::Part::Border
    theme::Part::Track
    theme::Part::Thumb
    theme::Part::Decrease
    theme::Part::Increase
    theme::Part::Indicator
    theme::Part::Key
    theme::Part::KeyBracket
    theme::Part::Title
    theme::Part::TitleBracket
    theme::Part::Spacing

The built-in states are static constants on
:cpp:any:`State <erbsland::cterm::theme::State>`:

.. code-block:: cpp

    theme::State::Focused
    theme::State::FocusWithin
    theme::State::Selected
    theme::State::Disabled
    theme::State::Checked

You can create custom elements and parts for your own controls:

.. code-block:: cpp

    inline constexpr auto cSearchBox = theme::Element::custom(10'000);
    inline constexpr auto cCursor = theme::Part::custom(10'001);

Keep these identifiers stable in your application or library. They are
small numeric values by design, so lookup stays direct and cheap. Custom
element and part identifiers are offset by the library, so they do not
collide with predefined identifiers.

Custom elements can also inherit from a built-in element:

.. code-block:: cpp

    theme::Theme::registerElement(cSearchBox, theme::Element::Surface);

After that, unresolved search-box selectors fall back through
``Surface`` before they reach the base sheet.

Rule Matching
=============

Theme resolution follows a fallback chain instead of returning just the
single most specific rule. Starting at the requested selector, the theme
looks for an authored sheet and then resolves its base sheet:

* tags fall back to the best authored tag subset, then to no tags
* states fall back to the best authored state subset, then to no states
* parts fall back through the element base hierarchy for the same part,
  then to the same element without a part
* elements fall back through their registered base element, then to
  ``Element::Base``

When several tag or state subsets could match, the larger subset wins.
If two candidates are equally specific, the later edited selector wins.
The resolved :cpp:any:`PropertySheet <erbsland::cterm::theme::PropertySheet>`
inherits every unspecified property from its base sheet.

Tags
====

Tags are compact bit identifiers registered on the builder that creates a
theme. A tag has no built-in string name; keep the returned
:cpp:any:`Tag <erbsland::cterm::theme::Tag>` value in your application
code and use it consistently for selectors and surfaces:

.. code-block:: cpp

    auto builder = theme::ThemeBuilder::from(theme::Theme::dark());
    const auto warning = builder.registerTag();

    builder.edit(theme::Selector{theme::Element::StatusLine, theme::Part::Background}.requireTag(warning))
        .setColor(Color{fg::BrightYellow, bg::Red});

    app.setTheme(builder.build());
    statusLine->themeAttributes().setTags(theme::Tags{warning});
    statusLine->flags().setThemeOutdated();

Use tags for visual variants that are not general UI states. For
example, ``warning``, ``primary``, or ``inspector`` are good tag
candidates. Focus, selection, checked, and disabled state belong to
``SurfaceFlags`` and are translated into theme states automatically.

Building Themes
===============

You normally start with one of the predefined builders and add only the
rules that make your application different. The following example keeps
the default dark structure, changes the regular status line, and gives a
tagged status line a stronger background:

.. code-block:: cpp

    using namespace erbsland::cterm;

    auto builder = theme::ThemeBuilder::dark();
    const auto warning = builder.registerTag();

    builder.edit(theme::Selector{theme::Element::StatusLine, theme::Part::Background})
        .setColor(Color{fg::BrightWhite, bg::Blue});
    builder.edit(theme::Selector{theme::Element::StatusLine, theme::Part::Background}.requireTag(warning))
        .setStyle(CharStyle{Color{fg::BrightYellow, bg::Red}, CharAttributes::Bold});

    app.setTheme(builder.build());

The same editor handles text styling and structural blocks. Action-help
formatting, for example, uses colors for key text, bracket text, and
action names. The ``ActionHelp``/``KeyBracket`` sheet also supplies the
bracket glyphs: ``BlockRole::West`` is the opening bracket,
``BlockRole::Center`` separates multiple keys, and ``BlockRole::East``
is the closing bracket.

.. code-block:: cpp

    auto builder = theme::ThemeBuilder::dark();
    builder.edit(theme::Selector{theme::Element::ActionHelp, theme::Part::KeyBracket})
        .setColor(Color{fg::BrightBlack, bg::Inherited})
        .setBlocks(U"   </>          ");

    app.setTheme(builder.build());

Block tables contain sixteen code points named by
:cpp:any:`BlockRole <erbsland::cterm::theme::BlockRole>`. Use
``setBlock()`` for one role, or ``setBlocks()`` with a 9-character or
16-character UTF-32 string.

Margins and padding have different owners. Margins are outside a themed
part; the parent surface keeps that space and the part does not paint it.
Padding is inside the themed part; text-focused UI elements paint padding
with the part's ``BlockRole::Background`` block and style before drawing
the text. Strict one-line text ignores top and bottom margins and padding
but still uses left and right values. When adjacent one-line text parts
meet, their horizontal margins collapse to the larger value instead of
being added together.

Scroll bars use the track margins to decide where the thumb may appear:

.. code-block:: cpp

    auto builder = theme::ThemeBuilder::dark();
    builder.edit(theme::Selector{theme::Element::VerticalScrollBar, theme::Part::Track})
        .setBlocks(U"░░░░░░░░░░░░↑░↓↑")
        .setMargins(Margins{0, 1});
    builder.edit(theme::Selector{theme::Element::VerticalScrollBar, theme::Part::Thumb})
        .setBlocks(U"            ▔ ▁ ");

    app.setTheme(builder.build());

Using Themes In Custom Surfaces
===============================

Each surface has theme attributes for its element and tags. Built-in
surfaces set their element automatically. Custom surfaces should set the
element in their constructor:

.. code-block:: cpp

    class SearchBox final : public ui::Surface {
    public:
        explicit SearchBox(ProtectedTag) : Surface{cSearchBox} {}

        [[nodiscard]] static auto create() -> std::shared_ptr<SearchBox> {
            return std::make_shared<SearchBox>(ProtectedTag{});
        }

    protected:
        void onPaint(WritableBuffer &buffer, const ui::PaintContext &context) noexcept override {
            auto backgroundTheme = context.theme().forPart(theme::Part::Background);
            theme::ThemePainter{buffer, backgroundTheme}.fill(context.surfaceRect());

            const auto textRect = backgroundTheme.contentRect(context.surfaceRect());
            const auto textColor = context.theme().forPart(theme::Part::Text).color();
            buffer.drawText(_text, textRect, Alignment::CenterLeft, textColor);
        }

    private:
        String _text;
    };

For layout decisions that depend on themed margins, override
``onLayout(Size, const LayoutContext &)`` and use
:cpp:any:`LayoutContext <erbsland::cterm::ui::LayoutContext>` there. The
same theme scope is then used during layout and paint:

.. code-block:: cpp

    void SearchBox::onLayout(Size newParentSize, const ui::LayoutContext &context) noexcept {
        const auto contentRect = context.theme().forPart(theme::Part::Background).contentRect(
            Rectangle{Position{}, newParentSize});
        _contentWidth = contentRect.width();
    }

Runtime Updates
===============

When you call
:cpp:any:`Application::setTheme() <erbsland::cterm::ui::Application::setTheme>`,
:cpp:any:`Page::setTheme() <erbsland::cterm::ui::Page::setTheme>`, or
:cpp:any:`SurfaceFlags::setThemeOutdated()
<erbsland::cterm::ui::SurfaceFlags::setThemeOutdated()>`, the affected
surface tree marks layout and paint as outdated. This is intentional: a
color-only change may only alter cells, while a block or margin change
may also alter content rectangles and therefore layout size.

Changing ``surface->themeAttributes().setElement(...)`` or
``surface->themeAttributes().setTags(...)`` is passive. This keeps
constructor-time setup cheap and predictable. If you change an element or
tag dynamically, call ``surface->flags().setThemeOutdated()`` yourself.
State-like changes such as ``setEnabled(false)``, ``setSelected(true)``,
and ``setChecked(true)`` belong to ``SurfaceFlags`` and invalidate the
theme scope automatically.

Paint code should use the resolved context helpers instead of querying a
global object:

.. code-block:: cpp

    const auto backgroundTheme = context.theme().forPart(theme::Part::Background);
    buffer.fill(context.surfaceRect(), backgroundTheme.block());

The :cpp:any:`ThemePainter <erbsland::cterm::theme::ThemePainter>` is a
thin convenience wrapper around ``WritableBuffer`` and
:cpp:any:`ThemeAccessor <erbsland::cterm::theme::ThemeAccessor>`. It is
not a separate style engine; it simply fills or frames rectangles with
blocks resolved through the active theme scope.

Predefined Themes
=================

The library provides predefined builders:

* :cpp:any:`ThemeBuilder::dark() <erbsland::cterm::theme::ThemeBuilder::dark>`
* :cpp:any:`ThemeBuilder::light() <erbsland::cterm::theme::ThemeBuilder::light>`
* :cpp:any:`ThemeBuilder::monochrome() <erbsland::cterm::theme::ThemeBuilder::monochrome>`

It also provides ready-to-use immutable themes:

* :cpp:any:`Theme::dark() <erbsland::cterm::theme::Theme::dark>`
* :cpp:any:`Theme::light() <erbsland::cterm::theme::Theme::light>`
* :cpp:any:`Theme::monochrome() <erbsland::cterm::theme::Theme::monochrome>`

Reference
=========

.. doxygenclass:: erbsland::cterm::theme::Element
    :members:

.. doxygenenum:: erbsland::cterm::theme::IdentifierType

.. doxygenclass:: erbsland::cterm::theme::Identifier
    :members:

.. doxygenclass:: erbsland::cterm::theme::Part
    :members:

.. doxygenclass:: erbsland::cterm::theme::State
    :members:

.. doxygenclass:: erbsland::cterm::theme::States
    :members:

.. doxygenclass:: erbsland::cterm::theme::Tag
    :members:

.. doxygenclass:: erbsland::cterm::theme::Tags
    :members:

.. doxygenclass:: erbsland::cterm::theme::Selector
    :members:

.. doxygenenum:: erbsland::cterm::theme::BlockRole

.. doxygenclass:: erbsland::cterm::theme::Properties
    :members:

.. doxygenclass:: erbsland::cterm::theme::PropertyEditor
    :members:

.. doxygenclass:: erbsland::cterm::theme::PropertySheet
    :members:

.. doxygentypedef:: erbsland::cterm::theme::PropertySheetPtr

.. doxygentypedef:: erbsland::cterm::theme::PropertySheetConstPtr

.. doxygenclass:: erbsland::cterm::theme::ThemeBuilder
    :members:

.. doxygenclass:: erbsland::cterm::theme::Theme
    :members:

.. doxygentypedef:: erbsland::cterm::theme::ThemePtr

.. doxygentypedef:: erbsland::cterm::theme::ThemeConstPtr

.. doxygenclass:: erbsland::cterm::theme::ThemeAccessor
    :members:

.. doxygenclass:: erbsland::cterm::theme::ThemePainter
    :members:
