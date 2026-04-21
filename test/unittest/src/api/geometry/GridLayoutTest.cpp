// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/cterm/GridLayout.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>
#include <vector>

TESTED_TARGETS(GridLayout)
class GridLayoutTest final : public el::UnitTest {
public:
    void testConstructorAndAccessorsStoreCellGeometry() {
        const auto layout = GridLayout{{2, 3}, {1, 2}};

        REQUIRE_EQUAL(layout.columnCount(), std::size_t{2});
        REQUIRE_EQUAL(layout.rowCount(), std::size_t{2});
        REQUIRE_EQUAL(layout.columnWidth(0), 2);
        REQUIRE_EQUAL(layout.columnWidth(1), 3);
        REQUIRE_EQUAL(layout.rowHeight(0), 1);
        REQUIRE_EQUAL(layout.rowHeight(1), 2);
        REQUIRE_EQUAL(layout.columnWidths(), (std::vector<Coordinate>{2, 3}));
        REQUIRE_EQUAL(layout.rowHeights(), (std::vector<Coordinate>{1, 2}));
    }

    void testDefaultBorderDoesNotAddAnyLineCells() {
        const auto layout = GridLayout{{2, 3}, {1, 2}};
        const auto border = FrameBorder{};

        REQUIRE_EQUAL(layout.size(border), (Size{5, 3}));
        REQUIRE_EQUAL(layout.cellRect(1, 1, Position{10, 20}, border), (Rectangle{12, 21, 3, 2}));
    }

    void testActiveBorderAddsOuterAndSeparatorLineCells() {
        const auto layout = GridLayout{{2, 3}, {1, 2}};
        const auto border = FrameBorder{FrameStyle::Light};

        REQUIRE_EQUAL(layout.size(border), (Size{8, 6}));
        REQUIRE_EQUAL(layout.cellRect(0, 0, Position{10, 20}, border), (Rectangle{11, 21, 2, 1}));
        REQUIRE_EQUAL(layout.cellRect(1, 1, Position{10, 20}, border), (Rectangle{14, 23, 3, 2}));
    }

    void testUnsupportedBorderStylesDoNotAddLineCells() {
        const auto layout = GridLayout{{2, 3}, {1, 2}};
        const auto border = FrameBorder{FrameStyle::FullBlock};

        REQUIRE_EQUAL(layout.size(border), (Size{5, 3}));
        REQUIRE_EQUAL(layout.cellRect(1, 1, Position{10, 20}, border), (Rectangle{12, 21, 3, 2}));
    }

    void testOmittedLinesDoNotContributeToSizeOrCellPositions() {
        const auto layout = GridLayout{{2, 3}, {1, 2}};
        auto border = FrameBorder{};
        border.set(FrameBorder::Element::VLine, FrameStyle::Light);

        REQUIRE_EQUAL(layout.size(border), (Size{6, 3}));
        REQUIRE_EQUAL(layout.cellRect(0, 0, Position{10, 20}, border), (Rectangle{10, 20, 2, 1}));
        REQUIRE_EQUAL(layout.cellRect(1, 1, Position{10, 20}, border), (Rectangle{13, 21, 3, 2}));
    }

    void testConstructorRejectsInvalidDimensions() {
        REQUIRE_THROWS_AS(std::invalid_argument, GridLayout({}, {1}));
        REQUIRE_THROWS_AS(std::invalid_argument, GridLayout({1}, {}));
        REQUIRE_THROWS_AS(std::invalid_argument, GridLayout({1, 0}, {1}));
        REQUIRE_THROWS_AS(std::invalid_argument, GridLayout({1}, {1, -1}));
    }

    void testCellRectRejectsInvalidIndexes() {
        const auto layout = GridLayout{{2}, {1}};
        const auto border = FrameBorder{};

        REQUIRE_THROWS_AS(std::out_of_range, layout.cellRect(1, 0, Position{}, border));
        REQUIRE_THROWS_AS(std::out_of_range, layout.cellRect(0, 1, Position{}, border));
    }
};
