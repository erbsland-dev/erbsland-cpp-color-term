// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestTextLine.hpp"

#include "support/BufferTestHelper.hpp"

#include <erbsland/cterm/theme/Theme.hpp>
#include <erbsland/cterm/theme/ThemeBuilder.hpp>
#include <erbsland/cterm/ui/surface/AbstractLine.hpp>
#include <erbsland/cterm/ui/surface/TextLine.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <array>
#include <format>
#include <memory>

TESTED_TARGETS(UiTextLine UiAbstractLine)
class UiTextLineTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testAbstractLineAnchorsLeftMiddleAndRightSections() {
        auto textLine = TestTextLine::create();
        auto buffer = Buffer{Size{20, 1}, Char{U'.'}};

        textLine->setText(ui::AbstractLine::Section::Left, String{"LEFT"});
        textLine->setText(ui::AbstractLine::Section::Middle, String{"MID"});
        textLine->setText(ui::AbstractLine::Section::Right, String{"RIGHT"});
        textLine->setRectangle(Rectangle{0, 0, 20, 1});
        textLine->layout(Size{20, 1}, ui::LayoutContext{});
        textLine->onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), ui::ThemeContext{}});

        requireRowsEqual(buffer, {"LEFT    MID    RIGHT"});
    }

    void testAbstractLineAppliesEllipsisAndHideCollapseBehavior() {
        auto textLine = TestTextLine::create();
        auto ellipsisBuffer = Buffer{Size{6, 1}, Char{U'.'}};

        textLine->setText(ui::AbstractLine::Section::Left, String{"ABCDEFG"});
        textLine->setRectangle(Rectangle{0, 0, 6, 1});
        textLine->layout(Size{6, 1}, ui::LayoutContext{});
        textLine->onPaint(
            ellipsisBuffer,
            ui::PaintContext{ellipsisBuffer.rect(), ellipsisBuffer.rect(), ellipsisBuffer.rect(), ui::ThemeContext{}});

        requireRowsEqual(ellipsisBuffer, {"ABCDE…"});

        auto hideBuffer = Buffer{Size{4, 1}, Char{U'.'}};
        textLine->setText(ui::AbstractLine::Section::Left, String{});
        textLine->setText(ui::AbstractLine::Section::Right, String{"RIGHT"});
        textLine->setCollapseBehavior(ui::AbstractLine::Section::Right, ui::AbstractLine::CollapseBehavior::Hide);
        textLine->setRectangle(Rectangle{0, 0, 4, 1});
        textLine->layout(Size{4, 1}, ui::LayoutContext{});
        textLine->onPaint(
            hideBuffer, ui::PaintContext{hideBuffer.rect(), hideBuffer.rect(), hideBuffer.rect(), ui::ThemeContext{}});

        requireRowsEqual(hideBuffer, {"    "});
    }

    void testAbstractLineNeverCollapseAllowsOverlappingSections() {
        auto textLine = TestTextLine::create();
        auto buffer = Buffer{Size{10, 1}, Char{U'.'}};

        textLine->setText(ui::AbstractLine::Section::Left, String{"LEFTSECTION"});
        textLine->setText(ui::AbstractLine::Section::Right, String{"RIGHTSIDE"});
        textLine->setCollapseBehavior(ui::AbstractLine::Section::Left, ui::AbstractLine::CollapseBehavior::Never);
        textLine->setCollapseBehavior(ui::AbstractLine::Section::Right, ui::AbstractLine::CollapseBehavior::Never);
        textLine->setRectangle(Rectangle{0, 0, 10, 1});
        textLine->layout(Size{10, 1}, ui::LayoutContext{});
        textLine->onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), ui::ThemeContext{}});

        requireRowsEqual(buffer, {"LRIGHTSIDE"});
    }

    void testAbstractLineMarginsInsetVisibleSectionsAndExposeSectionWidths() {
        auto textLine = TestTextLine::create();
        auto buffer = Buffer{Size{10, 1}, Char{U'.'}};

        textLine->setText(ui::AbstractLine::Section::Left, String{"LEFT"});
        textLine->setMargins(ui::AbstractLine::Section::Left, Margins{0, 1, 0, 1});
        textLine->setRectangle(Rectangle{0, 0, 10, 1});
        textLine->layout(Size{10, 1}, ui::LayoutContext{});
        textLine->onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), ui::ThemeContext{}});

        REQUIRE_EQUAL(textLine->sectionWidth(ui::AbstractLine::Section::Left), 8);
        REQUIRE_EQUAL(textLine->sectionWidth(ui::AbstractLine::Section::Middle), 0);
        REQUIRE_EQUAL(textLine->sectionWidth(ui::AbstractLine::Section::Right), 0);
        requireRowsEqual(buffer, {" LEFT     "});
    }

    void testAbstractLineTextInheritsBackgroundStyle() {
        auto textLine = TestTextLine::create();
        auto buffer = Buffer{Size{4, 1}, Char{U'.'}};
        auto backgroundAttributes = CharAttributes{};
        backgroundAttributes.setBold(true);
        auto textAttributes = CharAttributes{};
        textAttributes.setBoldInherited();
        auto text = String{};

        text.append(Char{U'A', Color{fg::Red, bg::Inherited}, textAttributes});
        text.append(Char{U'B', Color{fg::Inherited, bg::Yellow}, textAttributes});
        auto builder = theme::ThemeBuilder::from(theme::Theme::dark());
        builder.edit(theme::Selector{theme::Element::StatusLine, theme::Part::Background})
            .setColor(Color{fg::White, bg::Blue})
            .setAttributes(backgroundAttributes);
        const auto activeTheme = builder.build();
        textLine->setText(ui::AbstractLine::Section::Left, text);
        textLine->setRectangle(Rectangle{0, 0, 4, 1});
        textLine->layout(Size{4, 1}, ui::LayoutContext{});
        textLine->onPaint(
            buffer,
            ui::PaintContext{
                buffer.rect(),
                buffer.rect(),
                buffer.rect(),
                ui::ThemeContext{activeTheme, theme::Element::StatusLine}});

        requireRowsEqual(buffer, {"AB  "});
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Red, bg::Blue));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::White, bg::Yellow));
        REQUIRE(buffer.get(Position{0, 0}).attributes().isBold());
        REQUIRE(buffer.get(Position{1, 0}).attributes().isBold());
    }

    void testConcreteTextLineStaticModeNeverCallsUpdater() {
        auto textLine = ui::TextLine::create();
        auto buffer = Buffer{Size{10, 1}, Char{U'.'}};
        auto updateCount = 0;

        textLine->setText(ui::TextLine::Section::Left, "STATIC");
        textLine->setUpdateFn(
            ui::TextLine::Section::Left, [&updateCount](String &, const Coordinate) { ++updateCount; });
        textLine->setUpdateMode(ui::TextLine::Section::Left, ui::TextLine::UpdateMode::Static);
        textLine->setRectangle(Rectangle{0, 0, 10, 1});
        textLine->layout(Size{10, 1}, ui::LayoutContext{});
        textLine->onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), ui::ThemeContext{}});

        REQUIRE_EQUAL(updateCount, 0);
        requireRowsEqual(buffer, {"STATIC    "});
    }

    void testConcreteTextLineWithoutUpdaterDoesNotRefreshAutomatically() {
        auto textLine = ui::TextLine::create();
        auto buffer = Buffer{Size{10, 1}, Char{U'.'}};

        textLine->setText(ui::TextLine::Section::Left, "STATIC");
        textLine->setUpdateMode(ui::TextLine::Section::Left, ui::TextLine::UpdateMode::OnRefresh);
        textLine->setRectangle(Rectangle{0, 0, 10, 1});
        textLine->layout(Size{10, 1}, ui::LayoutContext{});
        textLine->onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), ui::ThemeContext{}});

        requireRowsEqual(buffer, {"STATIC    "});
    }

    void testConcreteTextLineRefreshModeUpdatesBeforeEveryPaint() {
        auto textLine = ui::TextLine::create();
        auto firstBuffer = Buffer{Size{10, 1}, Char{U'.'}};
        auto secondBuffer = Buffer{Size{10, 1}, Char{U'.'}};
        auto updateCount = 0;

        textLine->setUpdateMode(ui::TextLine::Section::Left, ui::TextLine::UpdateMode::OnRefresh);
        textLine->setUpdateFn(ui::TextLine::Section::Left, [&updateCount](String &text, const Coordinate width) {
            ++updateCount;
            text = String{std::format("R{}-{}", updateCount, width)};
        });
        textLine->setRectangle(Rectangle{0, 0, 10, 1});
        textLine->layout(Size{10, 1}, ui::LayoutContext{});
        textLine->onPaint(
            firstBuffer,
            ui::PaintContext{firstBuffer.rect(), firstBuffer.rect(), firstBuffer.rect(), ui::ThemeContext{}});
        textLine->onPaint(
            secondBuffer,
            ui::PaintContext{secondBuffer.rect(), secondBuffer.rect(), secondBuffer.rect(), ui::ThemeContext{}});

        REQUIRE_EQUAL(updateCount, 2);
        requireRowsEqual(firstBuffer, {"R1-10     "});
        requireRowsEqual(secondBuffer, {"R2-10     "});
    }

    void testConcreteTextLineResizeModeUpdatesOnlyWhenWidthChanges() {
        auto textLine = ui::TextLine::create();
        auto firstBuffer = Buffer{Size{10, 1}, Char{U'.'}};
        auto secondBuffer = Buffer{Size{10, 1}, Char{U'.'}};
        auto thirdBuffer = Buffer{Size{8, 1}, Char{U'.'}};
        auto updateCount = 0;

        textLine->setUpdateMode(ui::TextLine::Section::Left, ui::TextLine::UpdateMode::OnResize);
        textLine->setUpdateFn(ui::TextLine::Section::Left, [&updateCount](String &text, const Coordinate width) {
            ++updateCount;
            text = String{std::format("W{}", width)};
        });

        textLine->setRectangle(Rectangle{0, 0, 10, 1});
        textLine->layout(Size{10, 1}, ui::LayoutContext{});
        textLine->onPaint(
            firstBuffer,
            ui::PaintContext{firstBuffer.rect(), firstBuffer.rect(), firstBuffer.rect(), ui::ThemeContext{}});

        textLine->setRectangle(Rectangle{0, 0, 10, 1});
        textLine->layout(Size{10, 1}, ui::LayoutContext{});
        textLine->onPaint(
            secondBuffer,
            ui::PaintContext{secondBuffer.rect(), secondBuffer.rect(), secondBuffer.rect(), ui::ThemeContext{}});

        textLine->setRectangle(Rectangle{0, 0, 8, 1});
        textLine->layout(Size{8, 1}, ui::LayoutContext{});
        textLine->onPaint(
            thirdBuffer,
            ui::PaintContext{thirdBuffer.rect(), thirdBuffer.rect(), thirdBuffer.rect(), ui::ThemeContext{}});

        REQUIRE_EQUAL(updateCount, 2);
        requireRowsEqual(firstBuffer, {"W10       "});
        requireRowsEqual(secondBuffer, {"W10       "});
        requireRowsEqual(thirdBuffer, {"W8      "});
    }

    void testConcreteTextLineMarginsAffectResizeUpdatesAndWidthArguments() {
        auto textLine = ui::TextLine::create();
        auto firstBuffer = Buffer{Size{20, 1}, Char{U'.'}};
        auto secondBuffer = Buffer{Size{20, 1}, Char{U'.'}};
        auto lastWidth = Coordinate{-1};
        auto updateCount = 0;

        textLine->setText(ui::TextLine::Section::Left, "LEFT");
        textLine->setMargins(ui::TextLine::Section::Right, Margins{0, 1, 0, 1});
        textLine->setUpdateMode(ui::TextLine::Section::Right, ui::TextLine::UpdateMode::OnResize);
        textLine->setUpdateFn(
            ui::TextLine::Section::Right, [&lastWidth, &updateCount](String &text, const Coordinate width) {
                ++updateCount;
                lastWidth = width;
                text = String{std::format("R{}", width)};
            });

        textLine->setRectangle(Rectangle{0, 0, 20, 1});
        textLine->layout(Size{20, 1}, ui::LayoutContext{});
        textLine->onPaint(
            firstBuffer,
            ui::PaintContext{firstBuffer.rect(), firstBuffer.rect(), firstBuffer.rect(), ui::ThemeContext{}});

        REQUIRE_EQUAL(lastWidth, 14);
        REQUIRE_EQUAL(updateCount, 1);

        textLine->setMargins(ui::TextLine::Section::Right, Margins{0, 2, 0, 2});
        textLine->onPaint(
            secondBuffer,
            ui::PaintContext{secondBuffer.rect(), secondBuffer.rect(), secondBuffer.rect(), ui::ThemeContext{}});

        REQUIRE_EQUAL(lastWidth, 12);
        REQUIRE_EQUAL(updateCount, 2);
        requireRowsEqual(firstBuffer, {"LEFT            R14 "});
        requireRowsEqual(secondBuffer, {"LEFT           R12  "});
    }
};
