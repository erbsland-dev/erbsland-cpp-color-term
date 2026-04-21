// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "ui-world-view/src/WorldMapData.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <filesystem>
#include <string>

using demo::ui_world_view::Coordinate;
using demo::ui_world_view::Position;
using demo::ui_world_view::Size;

namespace {

auto defaultWorldMapDataPath() -> std::filesystem::path {
    return std::filesystem::path{ERBSLAND_COLOR_TERM_UI_WORLD_VIEW_TEST_DATA_FILE};
}

}

TESTED_TARGETS(WorldMapData)
class WorldMapDataTest final : public el::UnitTest {
public:
    void testParsesSmallInlineFixture() {
        const auto text = std::string{"format ui-world-view-map 1\n"
                                      "zoom 0 6 2\n"
                                      "row 2S 2L 2S\n"
                                      "row 1S 3L 2S\n"
                                      "labels 2\n"
                                      "label 2 0 Testland\n"
                                      "label 3 1 South Testland\n"
                                      "endzoom\n"};

        const auto data = demo::ui_world_view::WorldMapData::parse(text);

        REQUIRE_EQUAL(data.zoomLevelCount(), std::size_t{1});
        const auto &zoom = data.zoomLevel(0);
        REQUIRE_EQUAL(zoom.size, (Size{6, 2}));
        REQUIRE_FALSE(zoom.landMask.pixel(Position{0, 0}));
        REQUIRE_FALSE(zoom.landMask.pixel(Position{1, 0}));
        REQUIRE(zoom.landMask.pixel(Position{2, 0}));
        REQUIRE(zoom.landMask.pixel(Position{3, 0}));
        REQUIRE_EQUAL(zoom.labels.size(), std::size_t{2});
        REQUIRE_EQUAL(zoom.labels[0].anchor, (Position{2, 0}));
        REQUIRE_EQUAL(zoom.labels[0].displayWidth, Coordinate{8});
    }

    void testLoadsGeneratedDemoAsset() {
        const auto data = demo::ui_world_view::WorldMapData::loadFromFile(defaultWorldMapDataPath());

        REQUIRE_EQUAL(data.zoomLevelCount(), std::size_t{4});
        REQUIRE_EQUAL(data.zoomLevel(0).size, (Size{120, 60}));
        REQUIRE_EQUAL(data.zoomLevel(1).size, (Size{240, 120}));
        REQUIRE_EQUAL(data.zoomLevel(2).size, (Size{480, 240}));
        REQUIRE_EQUAL(data.zoomLevel(3).size, (Size{960, 480}));
        REQUIRE(data.zoomLevel(0).landMask.pixelCount() > 0);
        REQUIRE(data.zoomLevel(0).labels.size() > 0);
        REQUIRE(data.zoomLevel(3).labels.size() >= data.zoomLevel(0).labels.size());
    }
};
