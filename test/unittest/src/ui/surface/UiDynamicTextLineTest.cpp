// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/BufferTestHelper.hpp"

#include <erbsland/cterm/theme/ThemeBuilder.hpp>
#include <erbsland/cterm/ui/surface/DynamicTextLine.hpp>
#include <erbsland/cterm/ui/surface/HeaderLine.hpp>
#include <erbsland/cterm/ui/surface/Panel.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <format>

TESTED_TARGETS(UiDynamicTextLine UiHeaderLine)
class UiDynamicTextLineTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testLineAnchorsLeftMiddleAndRightSections() {
        auto textLine = ui::DynamicTextLine::create();

        textLine->setText(ui::DynamicTextLine::Section::Left, String{"LEFT"});
        textLine->setText(ui::DynamicTextLine::Section::Middle, String{"MID"});
        textLine->setText(ui::DynamicTextLine::Section::Right, String{"RIGHT"});

        requireRowsEqual(renderLine(*textLine, 20), {"LEFT    MID    RIGHT"});
    }

    void testShrinkUsesDynamicTextCropping() {
        auto textLine = ui::DynamicTextLine::create();

        textLine->setText(ui::DynamicTextLine::Section::Left, String{"ABCDEFG"});
        textLine->setText(ui::DynamicTextLine::Section::Right, String{"ABCDEFG"});

        requireRowsEqual(renderLine(*textLine, 6), {"ABCDE…"});

        textLine->clearText(ui::DynamicTextLine::Section::Left);

        requireRowsEqual(renderLine(*textLine, 6), {"…CDEFG"});
    }

    void testHidePriorityOmitsSectionThatDoesNotFit() {
        auto textLine = ui::DynamicTextLine::create();

        textLine->setText(ui::DynamicTextLine::Section::Right, String{"RIGHT"});
        textLine->setSpacePriority(ui::DynamicTextLine::Section::Right, ui::DynamicTextLine::SpacePriority::Hide);

        requireRowsEqual(renderLine(*textLine, 4), {"    "});
        REQUIRE_FALSE(textLine->dynamicText(ui::DynamicTextLine::Section::Right)->flags().isVisible());
        REQUIRE_EQUAL(textLine->sectionWidth(ui::DynamicTextLine::Section::Right), Coordinate{0});
    }

    void testKeepPriorityAllowsOverlappingSections() {
        auto textLine = ui::DynamicTextLine::create();

        textLine->setText(ui::DynamicTextLine::Section::Left, String{"LEFTSECTION"});
        textLine->setText(ui::DynamicTextLine::Section::Right, String{"RIGHTSIDE"});
        textLine->setSpacePriority(ui::DynamicTextLine::Section::Left, ui::DynamicTextLine::SpacePriority::Keep);
        textLine->setSpacePriority(ui::DynamicTextLine::Section::Right, ui::DynamicTextLine::SpacePriority::Keep);

        requireRowsEqual(renderLine(*textLine, 10), {"LRIGHTSIDE"});
    }

    void testMarginsInsetVisibleSectionsAndExposeAssignedWidths() {
        auto textLine = ui::DynamicTextLine::create();

        textLine->setText(ui::DynamicTextLine::Section::Left, String{"LEFT"});
        textLine->setMargins(ui::DynamicTextLine::Section::Left, Margins{0, 1, 0, 1});

        requireRowsEqual(renderLine(*textLine, 10), {" LEFT     "});
        REQUIRE_EQUAL(textLine->margins(ui::DynamicTextLine::Section::Left), (Margins{0, 1, 0, 1}));
        REQUIRE_EQUAL(textLine->sectionWidth(ui::DynamicTextLine::Section::Left), Coordinate{4});
        REQUIRE_EQUAL(textLine->sectionWidth(ui::DynamicTextLine::Section::Middle), Coordinate{0});
        REQUIRE_EQUAL(textLine->sectionWidth(ui::DynamicTextLine::Section::Right), Coordinate{0});
    }

    void testTextThemeMarginsAndPaddingAffectVisibleSectionsOnly() {
        auto builder = theme::ThemeBuilder::zero();
        builder.edit(theme::Selector{theme::Element::StatusLine, theme::Part::Text})
            .setBlocks(U'_')
            .setMargins(Margins{0, 1, 0, 1})
            .setPadding(Margins{0, 1, 0, 1});
        const auto activeTheme = builder.build();
        auto textLine = ui::DynamicTextLine::create();

        textLine->setText(ui::DynamicTextLine::Section::Left, String{"L"});

        requireRowsEqual(renderLine(*textLine, 5, activeTheme), {" _L_ "});
        REQUIRE_EQUAL(textLine->sectionWidth(ui::DynamicTextLine::Section::Left), Coordinate{3});
        REQUIRE_EQUAL(textLine->sectionWidth(ui::DynamicTextLine::Section::Middle), Coordinate{0});
        REQUIRE_EQUAL(textLine->sectionWidth(ui::DynamicTextLine::Section::Right), Coordinate{0});
    }

    void testDynamicTextAccessAllowsDirectUpdates() {
        auto textLine = ui::DynamicTextLine::create();
        auto updateCount = 0;

        textLine->dynamicText(ui::DynamicTextLine::Section::Middle)
            ->setUpdateFn([&updateCount](String &text, const Coordinate width) -> void {
                updateCount += 1;
                text = String{std::format("M{}-{}", updateCount, width)};
            });
        textLine->setRectangle(Rectangle{0, 0, 8, 1});
        textLine->layout(Size{8, 1}, ui::LayoutContext{});
        textLine->dynamicText(ui::DynamicTextLine::Section::Middle)->setRectangle(Rectangle{0, 0, 8, 1});
        textLine->dynamicText(ui::DynamicTextLine::Section::Middle)->updateText();

        REQUIRE_EQUAL(updateCount, 1);
        REQUIRE_EQUAL(render(textLine->text(ui::DynamicTextLine::Section::Middle)), "M1-8");
    }

    void testLineRejectsExternalChildren() {
        auto textLine = ui::DynamicTextLine::create();

        REQUIRE_THROWS_AS(std::logic_error, textLine->surfaces().add(ui::Panel::create()));
    }

    void testHeaderLineKeepsDedicatedThemeElement() {
        auto header = ui::HeaderLine::create();

        REQUIRE_EQUAL(header->themeAttributes().element(), theme::Element::HeaderLine);
        REQUIRE_EQUAL(header->dynamicText(ui::DynamicTextLine::Section::Left)->alignment(), Alignment::TopLeft);
        REQUIRE_EQUAL(header->dynamicText(ui::DynamicTextLine::Section::Middle)->alignment(), Alignment::TopCenter);
        REQUIRE_EQUAL(header->dynamicText(ui::DynamicTextLine::Section::Right)->alignment(), Alignment::TopRight);
    }

    void testHeaderLineTextKeepsHeaderBackground() {
        auto header = ui::HeaderLine::create();
        header->setText(ui::DynamicTextLine::Section::Left, String{"Header", fg::BrightWhite});
        auto builder = theme::ThemeBuilder::zero();
        builder.edit(theme::Selector{theme::Element::HeaderLine, theme::Part::Background})
            .setColor(fg::White, bg::Blue);

        const auto buffer = renderLine(*header, 8, builder.build());

        requireRowsEqual(buffer, {"Header  "});
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::BrightWhite, bg::Blue));
        REQUIRE_EQUAL(buffer.get(Position{6, 0}).color(), Color(fg::White, bg::Blue));
    }

private:
    [[nodiscard]] static auto
    renderLine(ui::DynamicTextLine &textLine, const Coordinate width, theme::ThemeConstPtr activeTheme = {}) -> Buffer {
        auto buffer = Buffer{Size{width, 1}, Char{U'.'}};
        if (activeTheme == nullptr) {
            auto builder = theme::ThemeBuilder::zero();
            builder.edit(theme::Selector{theme::Element::StatusLine, theme::Part::Ellipsis})
                .setBlock(theme::BlockRole::Main, U'…');
            activeTheme = builder.build();
        }
        const auto context = ui::ThemeContext{activeTheme, textLine.themeAttributes().element()};
        textLine.setRectangle(Rectangle{0, 0, width, 1});
        textLine.layout(Size{width, 1}, ui::LayoutContext{context});
        textLine.onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), context});
        for (const auto &child : textLine.surfaces()) {
            if (!child->flags().isVisible()) {
                continue;
            }
            child->onPaint(
                buffer,
                ui::PaintContext{
                    child->rectangle(),
                    child->rectangle(),
                    child->rectangle(),
                    ui::ThemeContext{activeTheme, child->themeAttributes().element()}});
        }
        return buffer;
    }
};
