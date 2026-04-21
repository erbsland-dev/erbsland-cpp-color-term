// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/BufferTestHelper.hpp"
#include "ui-world-view/src/WorldMapData.hpp"
#include "ui-world-view/src/WorldMapView.hpp"

#include <erbsland/cterm/CharCombinationStyle.hpp>
#include <erbsland/cterm/ui/PaintContext.hpp>
#include <erbsland/cterm/WriteClippedBuffer.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <memory>
#include <string>

using demo::ui_world_view::Coordinate;
using demo::ui_world_view::Position;
using demo::ui_world_view::Rectangle;
using demo::ui_world_view::Size;
using demo::ui_world_view::WorldMapData;
using demo::ui_world_view::WorldMapView;

namespace {

auto createFixtureText() -> std::string {
    auto text = std::string{"format ui-world-view-map 1\n"};
    text += "zoom 0 120 60\n";
    for (auto y = Coordinate{0}; y < 60; ++y) {
        if (y == 31) {
            text += "row 60S 1L 59S\n";
        } else {
            text += "row 120S\n";
        }
    }
    text += "labels 3\n";
    text += "label 56 27 TOP\n";
    text += "label 56 31 EDGE\n";
    text += "label 60 34 MID\n";
    text += "endzoom\n";
    return text;
}

auto renderView(WorldMapView &view, const Rectangle visibleContentRect) -> Buffer {
    auto buffer = Buffer{visibleContentRect.size(), Char{U'?'}};
    auto clippedBuffer = WriteClippedBufferRef{buffer, visibleContentRect.topLeft(), buffer.rect()};
    view.setRectangle(Rectangle{Position{}, view.contentSize()});
    view.layout(view.contentSize(), ui::LayoutContext{});
    view.onPaint(
        clippedBuffer,
        ui::PaintContext{view.localSurfaceRect(), visibleContentRect, visibleContentRect, ui::ThemeContext{}});
    return buffer;
}

}

TESTED_TARGETS(WorldMapView)
class WorldMapViewTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testGridStepAndProjectionHelpersFollowTheDemoRules() {
        REQUIRE_EQUAL(WorldMapView::gridStepForZoomIndex(0), Coordinate{30});
        REQUIRE_EQUAL(WorldMapView::gridStepForZoomIndex(1), Coordinate{30});
        REQUIRE_EQUAL(WorldMapView::gridStepForZoomIndex(2), Coordinate{15});
        REQUIRE_EQUAL(WorldMapView::gridStepForZoomIndex(3), Coordinate{15});

        REQUIRE_EQUAL(WorldMapView::projectLongitude(-150, 120), Coordinate{10});
        REQUIRE_EQUAL(WorldMapView::projectLongitude(0, 120), Coordinate{60});
        REQUIRE_EQUAL(WorldMapView::projectLongitude(150, 120), Coordinate{109});
        REQUIRE_EQUAL(WorldMapView::projectLatitude(75, 60), Coordinate{5});
        REQUIRE_EQUAL(WorldMapView::projectLatitude(0, 60), Coordinate{30});
        REQUIRE_EQUAL(WorldMapView::projectLatitude(-75, 60), Coordinate{54});
    }

    void testWorldMapViewRendersGridCoordinatesAndReservesPinnedLabelAreas() {
        auto mapData = std::make_shared<WorldMapData>(WorldMapData::parse(createFixtureText()));
        auto view = WorldMapView::create(mapData);

        const auto visibleContentRect = Rectangle{110, 27, 12, 8};
        const auto gridBuffer = renderView(*view, visibleContentRect);
        const auto expectedCross = CharCombinationStyle::commonBoxFrame()->combine(Char{U'│'}, Char{U'─'});

        REQUIRE_EQUAL(gridBuffer.get(Position{10, 3}).charStr(), expectedCross.charStr());
        REQUIRE_EQUAL(gridBuffer.get(Position{10, 2}).color(), Color(fg::BrightBlue, bg::Blue));
        REQUIRE_EQUAL(gridBuffer.get(Position{10, 4}).color(), Color(fg::BrightGreen, bg::Green));
        REQUIRE_EQUAL(gridBuffer.get(Position{10, 0}).charStr(), std::string{"0"});
        REQUIRE_EQUAL(gridBuffer.get(Position{0, 3}).charStr(), std::string{"0"});
        REQUIRE_EQUAL(gridBuffer.get(Position{1, 0}).charStr(), std::string{" "});
        REQUIRE_EQUAL(gridBuffer.get(Position{0, 4}).charStr(), std::string{" "});
        REQUIRE_EQUAL(gridBuffer.get(Position{9, 7}).charStr(), std::string{"M"});

        view->setGridShown(false);
        const auto plainBuffer = renderView(*view, visibleContentRect);

        REQUIRE_EQUAL(plainBuffer.get(Position{10, 3}).charStr(), std::string{" "});
        REQUIRE_EQUAL(plainBuffer.get(Position{10, 0}).charStr(), std::string{" "});
        REQUIRE_EQUAL(plainBuffer.get(Position{1, 0}).charStr(), std::string{"T"});
        REQUIRE_EQUAL(plainBuffer.get(Position{0, 4}).charStr(), std::string{"E"});
        REQUIRE_EQUAL(plainBuffer.get(Position{9, 7}).charStr(), std::string{"M"});
    }
};
