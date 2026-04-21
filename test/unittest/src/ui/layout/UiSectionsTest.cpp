// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/BufferTestHelper.hpp"

#include <erbsland/cterm/theme/Theme.hpp>
#include <erbsland/cterm/theme/ThemeBuilder.hpp>
#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <algorithm>
#include <memory>

TESTED_TARGETS(UiSections UiLayout UiFocus)
class UiSectionsTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testSectionsStoreOptionsAsLayoutDataForGenericMutation() {
        auto sections = ui::Sections::create();
        auto first = ui::Panel::create();
        auto second = ui::Panel::create();
        auto third = ui::Panel::create();

        sections->surfaces().add(first);
        sections->addSection(second, ui::SectionOptions{String{"Second"}});

        REQUIRE_EQUAL(sections->sectionCount(), std::size_t{2});
        REQUIRE(sections->sectionOptions(0).title().empty());
        REQUIRE(equalText(sections->sectionOptions(1).title(), String{"Second"}));

        sections->surfaces().moveBefore(second, first);

        REQUIRE(equalText(sections->sectionOptions(0).title(), String{"Second"}));
        REQUIRE(sections->sectionOptions(1).title().empty());

        sections->surfaces().replace(std::size_t{1}, third, std::make_shared<ui::SectionOptions>(String{"Third"}));

        REQUIRE(equalText(sections->sectionOptions(1).title(), String{"Third"}));

        sections->surfaces().remove(std::size_t{0});

        REQUIRE_EQUAL(sections->sectionCount(), std::size_t{1});
        REQUIRE(equalText(sections->sectionOptions(0).title(), String{"Third"}));
    }

    void testSectionsReserveSeparatorRowsAndTrailingSeparator() {
        auto sections = ui::Sections::create();
        auto first = ui::TextBox::create("A");
        auto second = ui::TextBox::create("B");
        first->editLayoutMetrics().setFixedHeight(1);
        second->editLayoutMetrics().setFixedHeight(2);
        sections->addSection(first, ui::SectionOptions{String{"First"}});
        sections->addSection(second, ui::SectionOptions{String{"Second"}});

        sections->setRectangle(Rectangle{0, 0, 20, 6});
        sections->layout(Size{20, 6}, ui::LayoutContext{});

        REQUIRE_EQUAL(first->rectangle(), Rectangle(0, 1, 20, 1));
        REQUIRE_EQUAL(second->rectangle(), Rectangle(0, 3, 20, 2));
    }

    void testSectionsIgnoreHiddenChildren() {
        auto sections = ui::Sections::create();
        auto first = ui::TextBox::create("A");
        auto hidden = ui::TextBox::create("B");
        auto third = ui::TextBox::create("C");
        first->editLayoutMetrics().setFixedHeight(1);
        hidden->editLayoutMetrics().setFixedHeight(1);
        third->editLayoutMetrics().setFixedHeight(1);
        hidden->flags().setVisible(false);
        sections->addSection(first, ui::SectionOptions{String{"First"}});
        sections->addSection(hidden, ui::SectionOptions{String{"Hidden"}});
        sections->addSection(third, ui::SectionOptions{String{"Third"}});

        sections->setRectangle(Rectangle{0, 0, 12, 5});
        sections->layout(Size{12, 5}, ui::LayoutContext{});

        REQUIRE_EQUAL(first->rectangle(), Rectangle(0, 1, 12, 1));
        REQUIRE_EQUAL(third->rectangle(), Rectangle(0, 3, 12, 1));
    }

    void testSectionsRenderTitleRightTextAndDecoration() {
        auto sections = ui::Sections::create();
        auto body = ui::TextBox::create("Body");
        body->editLayoutMetrics().setFixedHeight(1);
        sections->addSection(body, ui::SectionOptions{String{"Info"}, String{"[h]"}});
        sections->setTrailingSeparatorVisible(false);
        sections->setRectangle(Rectangle{0, 0, 24, 2});
        const auto context = sections->themeContextFrom(ui::ThemeContext{});
        sections->layout(Size{24, 2}, ui::LayoutContext{context});

        auto buffer = Buffer{Size{24, 2}, Char{U'.'}};
        sections->onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), context});

        requireRowsEqual(buffer, {"──⟨ Info ⟩──────────[h]─", "........................"});
    }

    void testSectionsUseThemeDecoration() {
        auto builder = theme::ThemeBuilder::from(theme::Theme::dark());
        builder.edit(theme::Selector{theme::Element::Sections, theme::Part::Border})
            .setBlock(theme::BlockRole::Background, U'=')
            .setMargins(Margins{0, 2, 0, 1});
        builder.edit(theme::Selector{theme::Element::Sections, theme::Part::Title})
            .setBlock(theme::BlockRole::Background, U' ')
            .setMargins(Margins{0});
        builder.edit(theme::Selector{theme::Element::Sections, theme::Part::TitleBracket})
            .setBlocks(U"         [ ]    ");
        const auto activeTheme = builder.build();
        auto sections = ui::Sections::create();
        const auto context = sections->themeContextFrom(ui::ThemeContext{activeTheme});
        auto body = ui::TextBox::create("Body");
        body->editLayoutMetrics().setFixedHeight(1);
        sections->addSection(body, ui::SectionOptions{String{"A"}, String{"R"}});
        sections->setTrailingSeparatorVisible(false);
        sections->setRectangle(Rectangle{0, 0, 10, 2});
        sections->layout(Size{10, 2}, ui::LayoutContext{context});

        auto buffer = Buffer{Size{10, 2}, Char{U'.'}};
        sections->onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), context});

        requireRowsEqual(buffer, {"=[A]===R==", ".........."});
    }

    void testFocusedSectionUsesFocusWithinStyleOnSeparator() {
        auto sections = ui::Sections::create();
        auto page = ui::Page::create();
        auto body = ui::TextBox::create("Body");
        body->flags().setFocusable(true);
        body->editLayoutMetrics().setFixedHeight(1);
        sections->addSection(body, ui::SectionOptions{String{"Focus"}});
        sections->setTrailingSeparatorVisible(false);
        page->addSurface(sections);
        page->focusTo(body);

        auto builder = theme::ThemeBuilder::from(theme::Theme::dark());
        builder.edit(theme::Selector{theme::Element::Sections, theme::Part::Border}).setColor(Color{fg::White});
        auto selector = theme::Selector{theme::Element::Sections, theme::Part::Border};
        selector.requireState(theme::State::FocusWithin);
        builder.edit(selector).setColor(Color{fg::Red});
        const auto activeTheme = builder.build();
        auto context = sections->themeContextFrom(ui::ThemeContext{activeTheme});

        sections->setRectangle(Rectangle{0, 0, 20, 2});
        sections->layout(Size{20, 2}, ui::LayoutContext{context});
        auto buffer = Buffer{Size{20, 2}, Char{U'.'}};
        sections->onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), context});

        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color().fg(), fg::Red);
    }

private:
    [[nodiscard]] static auto equalText(const String &left, const String &right) noexcept -> bool {
        return left.size() == right.size() && std::ranges::equal(left, right);
    }
};
