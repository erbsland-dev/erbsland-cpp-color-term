// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/BufferTestHelper.hpp"

#include <erbsland/cterm/theme/Theme.hpp>
#include <erbsland/cterm/theme/ThemeBuilder.hpp>
#include <erbsland/cterm/ui/surface/HorizontalScrollBar.hpp>
#include <erbsland/cterm/ui/surface/VerticalScrollBar.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <memory>
#include <utility>

TESTED_TARGETS(UiAbstractScrollBar UiHorizontalScrollBar UiVerticalScrollBar)
class UiScrollBarTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testHorizontalScrollBarUsesFixedHeightAndGrowingWidth() {
        const auto scrollBar = ui::HorizontalScrollBar::create();

        REQUIRE(scrollBar->isOpaque());
        REQUIRE_EQUAL(scrollBar->layoutMetrics().minimum(), Size(1, 1));
        REQUIRE_EQUAL(scrollBar->layoutMetrics().preferred(), Size(20, 1));
        REQUIRE_EQUAL(scrollBar->layoutMetrics().maximum().height(), 1);
        REQUIRE_EQUAL(scrollBar->layoutMetrics().sizePolicy().width().type(), ui::DimensionPolicy::Grow);
        REQUIRE_EQUAL(scrollBar->layoutMetrics().sizePolicy().height().type(), ui::DimensionPolicy::Preferred);
    }

    void testVerticalScrollBarUsesFixedWidthAndGrowingHeight() {
        const auto scrollBar = ui::VerticalScrollBar::create();

        REQUIRE(scrollBar->isOpaque());
        REQUIRE_EQUAL(scrollBar->layoutMetrics().minimum(), Size(1, 1));
        REQUIRE_EQUAL(scrollBar->layoutMetrics().preferred(), Size(1, 20));
        REQUIRE_EQUAL(scrollBar->layoutMetrics().maximum().width(), 1);
        REQUIRE_EQUAL(scrollBar->layoutMetrics().sizePolicy().width().type(), ui::DimensionPolicy::Preferred);
        REQUIRE_EQUAL(scrollBar->layoutMetrics().sizePolicy().height().type(), ui::DimensionPolicy::Grow);
    }

    void testHorizontalScrollBarRendersTinyStartMiddleEndAndFullRanges() {
        const auto scrollBar = ui::HorizontalScrollBar::create();
        scrollBar->setScrollRegion(IndexRange{0, 100});

        scrollBar->setVisibleRegion(IndexRange{0, 1});
        requireRowsEqual(renderHorizontal(*scrollBar, 8), {"←▏ ▕░░░→"});

        scrollBar->setVisibleRegion(IndexRange{40, 20});
        requireRowsEqual(renderHorizontal(*scrollBar, 12), {"←░░░░▏ ▕░░░→"});

        scrollBar->setVisibleRegion(IndexRange{99, 1});
        requireRowsEqual(renderHorizontal(*scrollBar, 8), {"←░░░▏ ▕→"});

        scrollBar->setVisibleRegion(IndexRange{0, 100});
        requireRowsEqual(renderHorizontal(*scrollBar, 7), {"←▏   ▕→"});
    }

    void testVerticalScrollBarRendersTinyAndFullRanges() {
        const auto scrollBar = ui::VerticalScrollBar::create();
        scrollBar->setScrollRegion(IndexRange{0, 100});

        scrollBar->setVisibleRegion(IndexRange{0, 1});
        requireRowsEqual(renderVertical(*scrollBar, 8), {"↑", "▔", " ", "▁", "░", "░", "░", "↓"});

        scrollBar->setVisibleRegion(IndexRange{0, 100});
        requireRowsEqual(renderVertical(*scrollBar, 7), {"↑", "▔", " ", " ", " ", "▁", "↓"});
    }

    void testScrollBarClampsVisibleRegionToScrollRegion() {
        const auto horizontal = ui::HorizontalScrollBar::create();
        horizontal->setScrollRegion(IndexRange{10, 20});
        horizontal->setVisibleRegion(IndexRange{0, IndexRange::npos});

        requireRowsEqual(renderHorizontal(*horizontal, 7), {"←▏   ▕→"});

        const auto vertical = ui::VerticalScrollBar::create();
        vertical->setScrollRegion(IndexRange{10, 20});
        vertical->setVisibleRegion(IndexRange{0, 0});

        requireRowsEqual(renderVertical(*vertical, 6), {"↑", "░", "░", "░", "░", "↓"});
    }

    void testShortHorizontalScrollBarsDegradeGracefully() {
        const auto scrollBar = ui::HorizontalScrollBar::create();
        scrollBar->setScrollRegion(IndexRange{0, 10});
        scrollBar->setVisibleRegion(IndexRange{0, 10});

        requireRowsEqual(renderHorizontal(*scrollBar, 1), {"←"});
        requireRowsEqual(renderHorizontal(*scrollBar, 2), {"←→"});
        requireRowsEqual(renderHorizontal(*scrollBar, 3), {"← →"});
        requireRowsEqual(renderHorizontal(*scrollBar, 4), {"←▏▕→"});
    }

    void testThemeStylesHorizontalScrollBar() {
        const auto scrollBar = ui::HorizontalScrollBar::create();
        scrollBar->setScrollRegion(IndexRange{0, 10});
        scrollBar->setVisibleRegion(IndexRange{0, 1});

        auto builder = theme::ThemeBuilder::from(theme::Theme::dark());
        builder.edit(theme::Selector{theme::Element::HorizontalScrollBar, theme::Part::Track})
            .setColor(Color{fg::Green, bg::Blue})
            .setBlocks(U".........<.>....")
            .setMargins(Margins{0, 1, 0, 1});
        builder.edit(theme::Selector{theme::Element::HorizontalScrollBar, theme::Part::Thumb})
            .setColor(Color{fg::Red, bg::Yellow})
            .setBlocks(U"         [=]====");
        const auto theme = builder.build();

        const auto buffer = renderHorizontal(*scrollBar, 7, theme);

        requireRowsEqual(buffer, {"<[=]..>"});
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Green, bg::Blue));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::Red, bg::Yellow));
        REQUIRE_EQUAL(buffer.get(Position{4, 0}).color(), Color(fg::Green, bg::Blue));
    }

private:
    auto renderHorizontal(
        ui::HorizontalScrollBar &scrollBar,
        const Coordinate width,
        theme::ThemeConstPtr activeTheme = theme::Theme::dark()) -> Buffer {
        auto buffer = Buffer{Size{width, 1}, Char{U'?'}};
        scrollBar.setRectangle(Rectangle{0, 0, width, 1});
        scrollBar.layout(Size{width, 1}, ui::LayoutContext{});
        scrollBar.onPaint(
            buffer,
            ui::PaintContext{
                buffer.rect(),
                buffer.rect(),
                buffer.rect(),
                ui::ThemeContext{std::move(activeTheme), theme::Element::HorizontalScrollBar}});
        return buffer;
    }

    auto renderVertical(
        ui::VerticalScrollBar &scrollBar,
        const Coordinate height,
        theme::ThemeConstPtr activeTheme = theme::Theme::dark()) -> Buffer {
        auto buffer = Buffer{Size{1, height}, Char{U'?'}};
        scrollBar.setRectangle(Rectangle{0, 0, 1, height});
        scrollBar.layout(Size{1, height}, ui::LayoutContext{});
        scrollBar.onPaint(
            buffer,
            ui::PaintContext{
                buffer.rect(),
                buffer.rect(),
                buffer.rect(),
                ui::ThemeContext{std::move(activeTheme), theme::Element::VerticalScrollBar}});
        return buffer;
    }
};
