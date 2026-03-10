// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/unittest/UnitTest.hpp>

#include "TestHelper.hpp"


class RectangleTest : public el::UnitTest {
public:
    Rectangle rect;

    void requireRectangleEqual(const Rectangle &actual, const Rectangle &expected) {
        REQUIRE_EQUAL(actual.topLeft(), expected.topLeft());
        REQUIRE_EQUAL(actual.size(), expected.size());
    }

    void requireRectangleSequenceEqual(
        const std::vector<Rectangle> &actual, const std::vector<Rectangle> &expected) {
        REQUIRE_EQUAL(actual.size(), expected.size());
        for (std::size_t index = 0; index < expected.size(); ++index) {
            WITH_CONTEXT(index);
            requireRectangleEqual(actual[index], expected[index]);
        }
    }

    void testDefaultConstructor() {
        rect = Rectangle();
        REQUIRE_EQUAL(rect.topLeft().x(), 0);
        REQUIRE_EQUAL(rect.topLeft().y(), 0);
        REQUIRE_EQUAL(rect.size().width(), 0);
        REQUIRE_EQUAL(rect.size().height(), 0);
    }

    void testConstructorWithInts() {
        rect = Rectangle(1, 2, 3, 4);
        REQUIRE_EQUAL(rect.x1(), 1);
        REQUIRE_EQUAL(rect.y1(), 2);
        REQUIRE_EQUAL(rect.width(), 3);
        REQUIRE_EQUAL(rect.height(), 4);
    }

    void testConstructorWithPositionSize() {
        rect = Rectangle(Position(5, 6), Size(7, 8));
        REQUIRE_EQUAL(rect.topLeft(), Position(5, 6));
        REQUIRE_EQUAL(rect.size(), Size(7, 8));
    }

    void testSetPos() {
        rect = Rectangle(1, 2, 3, 4);
        rect.setPos(Position(9, 10));
        REQUIRE_EQUAL(rect.topLeft(), Position(9, 10));
        REQUIRE_EQUAL(rect.x2(), 9 + 3);
        REQUIRE_EQUAL(rect.y2(), 10 + 4);
    }

    void testSizeAccessors() {
        rect = Rectangle(0, 0, 4, 5);
        REQUIRE_EQUAL(rect.size(), Size(4, 5));
        rect.setSize(Size(6, 7));
        REQUIRE_EQUAL(rect.size(), Size(6, 7));
        REQUIRE_EQUAL(rect.width(), 6);
        REQUIRE_EQUAL(rect.height(), 7);
    }

    void testEdgeCoordinatesAndCorners() {
        rect = Rectangle(2, 3, 4, 5);
        REQUIRE_EQUAL(rect.x1(), 2);
        REQUIRE_EQUAL(rect.y1(), 3);
        REQUIRE_EQUAL(rect.x2(), 6); // 2 + 4
        REQUIRE_EQUAL(rect.y2(), 8); // 3 + 5
        REQUIRE_EQUAL(rect.topLeft(), Position(2, 3));
        REQUIRE_EQUAL(rect.topRight(), Position(6, 3));
        REQUIRE_EQUAL(rect.bottomLeft(), Position(2, 8));
        REQUIRE_EQUAL(rect.bottomRight(), Position(6, 8));
    }

    void testAnchor() {
        rect = Rectangle(10, 20, 11, 21);
        // Using anchors; these rely on Size::position semantics
        REQUIRE_EQUAL(rect.anchor(Anchor::TopLeft), Position(10 + 0, 20 + 0));
        REQUIRE_EQUAL(rect.anchor(Anchor::TopCenter), Position(10 + 5, 20 + 0));
        REQUIRE_EQUAL(rect.anchor(Anchor::TopRight), Position(10 + 10, 20 + 0));
        REQUIRE_EQUAL(rect.anchor(Anchor::CenterLeft), Position(10 + 0, 20 + 10));
        REQUIRE_EQUAL(rect.anchor(Anchor::Center), Position(10 + 5, 20 + 10));
        REQUIRE_EQUAL(rect.anchor(Anchor::CenterRight), Position(10 + 10, 20 + 10));
        REQUIRE_EQUAL(rect.anchor(Anchor::BottomLeft), Position(10 + 0, 20 + 20));
        REQUIRE_EQUAL(rect.anchor(Anchor::BottomCenter), Position(10 + 5, 20 + 20));
        REQUIRE_EQUAL(rect.anchor(Anchor::BottomRight), Position(10 + 10, 20 + 20));
    }

    void testUnionOperatorOrEquals() {
        rect = Rectangle(1, 1, 2, 2);                            // covers [1..3) x [1..3)
        Rectangle other = Rectangle(Position(0, 2), Size(4, 3)); // covers [0..4) x [2..5)
        rect |= other;
        // resulting rect should cover min pos (0,1) to max bottomRight (4,5)
        REQUIRE_EQUAL(rect.topLeft(), Position(0, 1));
        REQUIRE_EQUAL(rect.bottomRight(), Position(4, 5));
        REQUIRE_EQUAL(rect.size(), Size(4, 4));
    }

    void testExpandedByAndInsetBy() {
        rect = Rectangle(5, 6, 7, 8);
        Margins mAll(1);
        auto expanded = rect.expandedBy(mAll);
        REQUIRE_EQUAL(expanded.topLeft(), Position(5 - 1, 6 - 1));
        REQUIRE_EQUAL(expanded.size(), Size(7 + 2, 8 + 2));

        Margins mSides(2, 3); // horizontal=2 (left/right), vertical=3 (top/bottom)
        expanded = rect.expandedBy(mSides);
        REQUIRE_EQUAL(expanded.topLeft(), Position(5 - 2, 6 - 3));
        REQUIRE_EQUAL(expanded.size(), Size(7 + 4, 8 + 6));

        auto inset = rect.insetBy(Margins(1));
        // inset is expanded by -margins
        REQUIRE_EQUAL(inset.topLeft(), Position(5 + 1, 6 + 1));
        REQUIRE_EQUAL(inset.size(), Size(7 - 2, 8 - 2));
    }

    void testSubRectangle() {
        // base rectangle
        rect = Rectangle(10, 10, 20, 10);

        // 1) zero size -> full width/height inside margins
        auto sub = rect.subRectangle(Anchor::TopLeft, Size(0, 0), Margins(0));
        REQUIRE_EQUAL(sub.topLeft(), rect.topLeft());
        REQUIRE_EQUAL(sub.size(), rect.size());

        // 2) specify smaller size centered
        auto small = rect.subRectangle(Anchor::Center, Size(4, 2), Margins(0));
        // centered inside rect: topLeft should be 10+((20-4)/2), 10+((10-2)/2)
        REQUIRE_EQUAL(small.topLeft(), Position(10 + (20 - 4) / 2, 10 + (10 - 2) / 2));
        REQUIRE_EQUAL(small.size(), Size(4, 2));

        // 3) with margins: margins reduce inner rect available area
        auto withMargins = rect.subRectangle(Anchor::TopLeft, Size(0, 0), Margins(2));
        REQUIRE_EQUAL(withMargins.topLeft(), Position(10 + 2, 10 + 2));
        REQUIRE_EQUAL(withMargins.size(), Size(20 - 4, 10 - 4));

        // 4) requested size larger than inner area -> clamped to inner area
        auto tooLarge = rect.subRectangle(Anchor::TopLeft, Size(100, 100), Margins(5));
        REQUIRE_EQUAL(tooLarge.size(), Size(20 - 10, 10 - 10));
    }

    void testContains() {
        rect = Rectangle(2, 3, 4, 5); // covers x=2..5, y=3..7 (exclusive end at 6 and 8)
        REQUIRE(rect.contains(Position(2, 3)));
        REQUIRE(rect.contains(Position(5 - 1, 3)));
        REQUIRE(rect.contains(Position(2, 7 - 1)));
        REQUIRE_FALSE(rect.contains(Position(1, 3)));
        REQUIRE_FALSE(rect.contains(Position(2, 8)));
        REQUIRE_FALSE(rect.contains(Position(6, 3))); // x == x2 is outside
    }

    void testIsFrame() {
        rect = Rectangle(0, 0, 3, 3); // positions inside: (0..2,0..2)
        // corners and edges are frame
        REQUIRE(rect.isFrame(Position(0, 0)));
        REQUIRE(rect.isFrame(Position(1, 0)));
        REQUIRE(rect.isFrame(Position(2, 0)));
        REQUIRE(rect.isFrame(Position(0, 1)));
        REQUIRE(rect.isFrame(Position(2, 1)));
        REQUIRE(rect.isFrame(Position(0, 2)));
        REQUIRE(rect.isFrame(Position(1, 2)));
        REQUIRE(rect.isFrame(Position(2, 2)));
        // interior not frame
        REQUIRE_FALSE(rect.isFrame(Position(1, 1)));
        // outside is not frame
        REQUIRE_FALSE(rect.isFrame(Position(-1, 0)));
        REQUIRE_FALSE(rect.isFrame(Position(3, 1)));
    }

    void testFrameIndexUsesClockwisePerimeterOrder() {
        rect = Rectangle(2, 3, 4, 3);

        REQUIRE_EQUAL(rect.frameIndex(Position(2, 3)), 0);
        REQUIRE_EQUAL(rect.frameIndex(Position(3, 3)), 1);
        REQUIRE_EQUAL(rect.frameIndex(Position(4, 3)), 2);
        REQUIRE_EQUAL(rect.frameIndex(Position(5, 3)), 3);
        REQUIRE_EQUAL(rect.frameIndex(Position(5, 4)), 4);
        REQUIRE_EQUAL(rect.frameIndex(Position(5, 5)), 5);
        REQUIRE_EQUAL(rect.frameIndex(Position(4, 5)), 6);
        REQUIRE_EQUAL(rect.frameIndex(Position(3, 5)), 7);
        REQUIRE_EQUAL(rect.frameIndex(Position(2, 5)), 8);
        REQUIRE_EQUAL(rect.frameIndex(Position(2, 4)), 9);
        REQUIRE_EQUAL(rect.frameIndex(Position(3, 4)), -1);
    }

    void testFrameIndexHandlesDegenerateRectangles() {
        REQUIRE_EQUAL(Rectangle(4, 5, 1, 3).frameIndex(Position(4, 5)), 0);
        REQUIRE_EQUAL(Rectangle(4, 5, 1, 3).frameIndex(Position(4, 6)), 1);
        REQUIRE_EQUAL(Rectangle(4, 5, 1, 3).frameIndex(Position(4, 7)), 2);
        REQUIRE_EQUAL(Rectangle(4, 5, 3, 1).frameIndex(Position(4, 5)), 0);
        REQUIRE_EQUAL(Rectangle(4, 5, 3, 1).frameIndex(Position(5, 5)), 1);
        REQUIRE_EQUAL(Rectangle(4, 5, 3, 1).frameIndex(Position(6, 5)), 2);
    }

    void testForEach() {
        rect = Rectangle(1, 2, 3, 2); // covers positions x=[1,3], y=[2,3] (exclusive x2=4,y2=4)
        std::vector<Position> visited;
        rect.forEach([&](const Position pos) { visited.push_back(pos); });
        // order should be row-major: y=2: x=1,2,3 ; y=3: x=1,2,3
        std::vector<Position> expected = {
            Position(1, 2),
            Position(2, 2),
            Position(3, 2),
            Position(1, 3),
            Position(2, 3),
            Position(3, 3),
        };
        REQUIRE_EQUAL(visited.size(), expected.size());
        for (std::size_t i = 0; i < expected.size(); ++i) {
            REQUIRE_EQUAL(visited[i], expected[i]);
        }
    }

    void testForEachInFrameVisitsAllFramePositions() {
        rect = Rectangle(2, 3, 3, 3);
        std::vector<Position> visited;

        rect.forEachInFrame([&](const Position pos) { visited.push_back(pos); });

        const std::vector<Position> expected = {
            Position(2, 3),
            Position(3, 3),
            Position(4, 3),
            Position(2, 4),
            Position(4, 4),
            Position(2, 5),
            Position(3, 5),
            Position(4, 5),
        };
        REQUIRE_EQUAL(visited.size(), expected.size());
        for (const auto &expectedPosition : expected) {
            WITH_CONTEXT(expectedPosition);
            REQUIRE(std::ranges::find(visited, expectedPosition) != visited.end());
        }
    }

    void testForEachInFrameWithIndexUsesClockwisePerimeterOrder() {
        rect = Rectangle(2, 3, 4, 3);
        std::vector<Position> visited;
        std::vector<int> indexes;

        rect.forEachInFrame([&](const Position pos, const int index) {
            visited.push_back(pos);
            indexes.push_back(index);
        });

        const std::vector<Position> expectedPositions = {
            Position(2, 3),
            Position(3, 3),
            Position(4, 3),
            Position(5, 3),
            Position(5, 4),
            Position(5, 5),
            Position(4, 5),
            Position(3, 5),
            Position(2, 5),
            Position(2, 4),
        };
        REQUIRE_EQUAL(visited, expectedPositions);
        for (std::size_t index = 0; index < indexes.size(); ++index) {
            WITH_CONTEXT(index);
            REQUIRE_EQUAL(indexes[index], static_cast<int>(index));
        }
    }

    void testGridCellsSplitsEvenlyWithoutSpacing() {
        rect = Rectangle(10, 20, 8, 6);

        const auto cells = rect.gridCells(2, 4);

        const std::vector<Rectangle> expected = {
            Rectangle(10, 20, 2, 3),
            Rectangle(12, 20, 2, 3),
            Rectangle(14, 20, 2, 3),
            Rectangle(16, 20, 2, 3),
            Rectangle(10, 23, 2, 3),
            Rectangle(12, 23, 2, 3),
            Rectangle(14, 23, 2, 3),
            Rectangle(16, 23, 2, 3),
        };
        requireRectangleSequenceEqual(cells, expected);
    }

    void testGridCellsDistributesRemainderToTopLeftCells() {
        rect = Rectangle(5, 7, 10, 5);

        const auto cells = rect.gridCells(2, 3);

        const std::vector<Rectangle> expected = {
            Rectangle(5, 7, 4, 3),
            Rectangle(9, 7, 3, 3),
            Rectangle(12, 7, 3, 3),
            Rectangle(5, 10, 4, 2),
            Rectangle(9, 10, 3, 2),
            Rectangle(12, 10, 3, 2),
        };
        requireRectangleSequenceEqual(cells, expected);
    }

    void testGridCellsApplySpacingAndRemainRowMajor() {
        rect = Rectangle(10, 20, 11, 8);

        const auto cells = rect.gridCells(2, 3, 1, 2);

        const std::vector<Rectangle> expected = {
            Rectangle(10, 20, 3, 3),
            Rectangle(14, 20, 3, 3),
            Rectangle(18, 20, 3, 3),
            Rectangle(10, 25, 3, 3),
            Rectangle(14, 25, 3, 3),
            Rectangle(18, 25, 3, 3),
        };
        requireRectangleSequenceEqual(cells, expected);
        REQUIRE_EQUAL(cells[1].x1() - cells[0].x2(), 1);
        REQUIRE_EQUAL(cells[3].y1() - cells[0].y2(), 2);
    }

    void testGridCellsAllowMinimalOneByOneCells() {
        rect = Rectangle(3, 4, 5, 3);

        const auto cells = rect.gridCells(3, 2, 3, 0);

        const std::vector<Rectangle> expected = {
            Rectangle(3, 4, 1, 1),
            Rectangle(7, 4, 1, 1),
            Rectangle(3, 5, 1, 1),
            Rectangle(7, 5, 1, 1),
            Rectangle(3, 6, 1, 1),
            Rectangle(7, 6, 1, 1),
        };
        requireRectangleSequenceEqual(cells, expected);
    }

    void testGridCellsRejectInvalidArguments() {
        rect = Rectangle(0, 0, 4, 4);

        REQUIRE_THROWS_AS(std::invalid_argument, rect.gridCells(0, 1));
        REQUIRE_THROWS_AS(std::invalid_argument, rect.gridCells(1, 0));
        REQUIRE_THROWS_AS(std::invalid_argument, rect.gridCells(1, 1, -1, 0));
        REQUIRE_THROWS_AS(std::invalid_argument, rect.gridCells(1, 1, 0, -1));
        REQUIRE_THROWS_AS(std::invalid_argument, rect.gridCells(2, 3, 1, 0));
        REQUIRE_THROWS_AS(std::invalid_argument, rect.gridCells(3, 2, 0, 2));
    }
};
