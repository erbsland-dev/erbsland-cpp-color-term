// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <format>
#include <functional>
#include <type_traits>

// Keep this check at compile time so the constexpr Rectangle constructor cannot regress unnoticed.
static_assert([]() constexpr {
    return Rectangle(Position(5, 6), Position(12, 15)).size() == Size(7, 9) &&
        Rectangle(Position(5, 6), Position(2, 4)).size() == Size(0, 0);
}());

static_assert([]() constexpr {
    const auto rect = Rectangle(1, 2, 3, 4);
    return rect.hash() == rect.hash();
}());

static_assert(std::is_same_v<decltype(Rectangle{}.x1()), Coordinate>);
static_assert(std::is_same_v<decltype(Rectangle{}.y1()), Coordinate>);
static_assert(std::is_same_v<decltype(Rectangle{}.x2()), Coordinate>);
static_assert(std::is_same_v<decltype(Rectangle{}.y2()), Coordinate>);
static_assert(std::is_same_v<decltype(Rectangle{}.width()), Coordinate>);
static_assert(std::is_same_v<decltype(Rectangle{}.height()), Coordinate>);
static_assert(std::is_same_v<decltype(Rectangle{}.alignmentOffset(Size{}, Alignment::TopLeft)), Position>);
static_assert(std::is_same_v<decltype(Rectangle{}.alignedSource(Rectangle{}, Alignment::TopLeft)), AlignedSource>);


TESTED_TARGETS(Rectangle)
class RectangleTest final : public el::UnitTest {
public:
    Rectangle rect;

    void requireRectangleEqual(const Rectangle &actual, const Rectangle &expected) {
        REQUIRE_EQUAL(actual.topLeft(), expected.topLeft());
        REQUIRE_EQUAL(actual.size(), expected.size());
    }

    void requireRectangleSequenceEqual(const std::vector<Rectangle> &actual, const std::vector<Rectangle> &expected) {
        REQUIRE_EQUAL(actual.size(), expected.size());
        for (std::size_t index = 0; index < expected.size(); ++index) {
            runWithContext(
                SOURCE_LOCATION(),
                [&]() { requireRectangleEqual(actual[index], expected[index]); },
                [&]() -> std::string {
                    return std::format(
                        "index = {} / actual = ({}, {}, {}, {}) / expected = ({}, {}, {}, {})",
                        index,
                        actual[index].x1(),
                        actual[index].y1(),
                        actual[index].width(),
                        actual[index].height(),
                        expected[index].x1(),
                        expected[index].y1(),
                        expected[index].width(),
                        expected[index].height());
                });
        }
    }

    void requirePositionSequenceEqual(const PositionList &actual, const PositionList &expected) {
        REQUIRE_EQUAL(actual.size(), expected.size());
        for (std::size_t index = 0; index < expected.size(); ++index) {
            runWithContext(
                SOURCE_LOCATION(),
                [&]() { REQUIRE_EQUAL(actual[index], expected[index]); },
                [&]() -> std::string {
                    return std::format(
                        "index = {} / actual = ({}, {}) / expected = ({}, {})",
                        index,
                        actual[index].x(),
                        actual[index].y(),
                        expected[index].x(),
                        expected[index].y());
                });
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

    void testConstructorWithTopLeftAndBottomRightUsesExclusiveBottomRight() {
        rect = Rectangle(Position(5, 6), Position(12, 15));
        REQUIRE_EQUAL(rect.topLeft(), Position(5, 6));
        REQUIRE_EQUAL(rect.bottomRight(), Position(12, 15));
        REQUIRE_EQUAL(rect.size(), Size(7, 9));

        const auto emptyRectangle = Rectangle(Position(5, 6), Position(2, 4));
        REQUIRE_EQUAL(emptyRectangle.topLeft(), Position(5, 6));
        REQUIRE_EQUAL(emptyRectangle.size(), Size(0, 0));
    }

    void testComparisonOperators() {
        const auto baseline = Rectangle(1, 2, 3, 4);

        REQUIRE(baseline == Rectangle(1, 2, 3, 4));
        REQUIRE_FALSE(baseline != Rectangle(1, 2, 3, 4));
        REQUIRE_FALSE(baseline == Rectangle(1, 2, 4, 4));
        REQUIRE(baseline != Rectangle(1, 3, 3, 4));
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

    void testUnionOperatorOrCreatesTheBoundingRectangle() {
        const auto left = Rectangle(1, 1, 2, 2);
        const auto right = Rectangle(Position(0, 2), Size(4, 3));

        const auto united = left | right;

        REQUIRE_EQUAL(united.topLeft(), Position(0, 1));
        REQUIRE_EQUAL(united.bottomRight(), Position(4, 5));
        REQUIRE_EQUAL(united.size(), Size(4, 4));
    }

    void testIntersectionOperatorsReturnOnlyTheOverlappingArea() {
        rect = Rectangle(1, 1, 4, 4);
        const auto other = Rectangle(3, 0, 3, 3);

        const auto intersection = rect & other;

        REQUIRE_EQUAL(intersection.topLeft(), Position(3, 1));
        REQUIRE_EQUAL(intersection.bottomRight(), Position(5, 3));
        REQUIRE_EQUAL(intersection.size(), Size(2, 2));

        rect &= other;
        requireRectangleEqual(rect, intersection);
    }

    void testIntersectionOfNonOverlappingRectanglesReturnsAnEmptyRectangle() {
        const auto left = Rectangle(0, 0, 2, 2);
        const auto right = Rectangle(3, 3, 2, 2);

        const auto intersection = left & right;

        REQUIRE_EQUAL(intersection.topLeft(), Position(0, 0));
        REQUIRE_EQUAL(intersection.size(), Size(0, 0));
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

    void testAlignedSourcePlacesASmallerSourceInsideTheTargetRectangle() {
        rect = Rectangle(10, 20, 8, 5);

        const auto aligned = rect.alignedSource(Rectangle(3, 4, 3, 2), Alignment::BottomRight);

        requireRectangleEqual(aligned.targetRect, Rectangle(15, 23, 3, 2));
        requireRectangleEqual(aligned.sourceRect, Rectangle(3, 4, 3, 2));
    }

    void testAlignmentOffsetAddsTheRectangleTopLeftCorner() {
        rect = Rectangle(10, 20, 8, 5);

        REQUIRE_EQUAL(rect.alignmentOffset(Size(3, 2), Alignment::TopLeft), Position(10, 20));
        REQUIRE_EQUAL(rect.alignmentOffset(Size(3, 2), Alignment::BottomRight), Position(15, 23));
        REQUIRE_EQUAL(rect.alignmentOffset(Size(10, 7), Alignment::Center), Position(9, 19));
    }

    void testAlignedSourceCropsALargerSourceAccordingToAlignment() {
        rect = Rectangle(10, 20, 4, 2);

        const auto aligned = rect.alignedSource(Rectangle(3, 4, 8, 5), Alignment::Center);

        requireRectangleEqual(aligned.targetRect, Rectangle(10, 20, 4, 2));
        requireRectangleEqual(aligned.sourceRect, Rectangle(5, 5, 4, 2));
    }

    void testAlignedSourceCanCropAndPlaceOnDifferentAxes() {
        rect = Rectangle(10, 20, 10, 3);

        const auto aligned = rect.alignedSource(Rectangle(2, 4, 4, 5), Alignment::BottomCenter);

        requireRectangleEqual(aligned.targetRect, Rectangle(13, 20, 4, 3));
        requireRectangleEqual(aligned.sourceRect, Rectangle(2, 6, 4, 3));
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

    void testContainsRectangleRequiresFullContainment() {
        rect = Rectangle(2, 3, 6, 5);

        REQUIRE(rect.contains(Rectangle(2, 3, 6, 5)));
        REQUIRE(rect.contains(Rectangle(3, 4, 2, 2)));
        REQUIRE_FALSE(rect.contains(Rectangle(1, 3, 2, 2)));
        REQUIRE_FALSE(rect.contains(Rectangle(3, 7, 2, 2)));
    }

    void testHashMatchesStdHashAndDependsOnGeometry() {
        const auto first = Rectangle(2, 3, 6, 5);
        const auto second = Rectangle(2, 3, 5, 6);

        REQUIRE_EQUAL(first.hash(), std::hash<Rectangle>{}(first));
        REQUIRE_NOT_EQUAL(first.hash(), second.hash());
    }

    void testClamp() {
        struct ClampCase {
            Rectangle rectangle;
            Position input;
            Position expected;
        };
        const std::vector<ClampCase> cases = {
            {.rectangle = Rectangle(10, 20, 5, 4), .input = Position(12, 21), .expected = Position(12, 21)},
            {.rectangle = Rectangle(10, 20, 5, 4), .input = Position(2, 9), .expected = Position(10, 20)},
            {.rectangle = Rectangle(10, 20, 5, 4), .input = Position(99, 99), .expected = Position(14, 23)},
            {.rectangle = Rectangle(10, 20, 1, 4), .input = Position(99, 22), .expected = Position(10, 22)},
            {.rectangle = Rectangle(10, 20, 5, 1), .input = Position(12, -5), .expected = Position(12, 20)},
        };
        for (std::size_t index = 0; index < cases.size(); ++index) {
            const auto &[rectangle, input, expected] = cases[index];
            runWithContext(
                SOURCE_LOCATION(),
                [&]() { REQUIRE_EQUAL(rectangle.clamp(input), expected); },
                [&]() -> std::string {
                    return std::format(
                        "index = {} / rectangle = ({}, {}, {}, {}) / input = ({}, {}) / expected = ({}, {})",
                        index,
                        rectangle.x1(),
                        rectangle.y1(),
                        rectangle.width(),
                        rectangle.height(),
                        input.x(),
                        input.y(),
                        expected.x(),
                        expected.y());
                });
        }
    }

    void testOverlapsDetectsSharedAreaAndRejectsTouchingEdges() {
        const auto horizontal = Rectangle(0, 1, 4, 2);
        const auto vertical = Rectangle(1, 0, 2, 4);
        const auto touching = Rectangle(4, 1, 2, 2);

        REQUIRE(horizontal.overlaps(vertical));
        REQUIRE(vertical.overlaps(horizontal));
        REQUIRE_FALSE(horizontal.overlaps(touching));
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

    void testFrameDirectionReturnsCardinalAndCornerDirections() {
        rect = Rectangle(10, 20, 4, 3);

        REQUIRE_EQUAL(rect.frameDirection(Position(10, 20)), Direction::NorthWest);
        REQUIRE_EQUAL(rect.frameDirection(Position(11, 20)), Direction::North);
        REQUIRE_EQUAL(rect.frameDirection(Position(13, 20)), Direction::NorthEast);
        REQUIRE_EQUAL(rect.frameDirection(Position(13, 21)), Direction::East);
        REQUIRE_EQUAL(rect.frameDirection(Position(13, 22)), Direction::SouthEast);
        REQUIRE_EQUAL(rect.frameDirection(Position(12, 22)), Direction::South);
        REQUIRE_EQUAL(rect.frameDirection(Position(10, 22)), Direction::SouthWest);
        REQUIRE_EQUAL(rect.frameDirection(Position(10, 21)), Direction::West);
        REQUIRE_EQUAL(rect.frameDirection(Position(11, 21)), Direction::None);
        REQUIRE_EQUAL(Rectangle(10, 20, 1, 1).frameDirection(Position(10, 20)), Direction::None);
    }

    void testForEach() {
        rect = Rectangle(1, 2, 3, 2); // covers positions x=[1,3], y=[2,3] (exclusive x2=4,y2=4)
        PositionList visited;
        rect.forEach([&](const Position pos) { visited.push_back(pos); });
        // order should be row-major: y=2: x=1,2,3 ; y=3: x=1,2,3
        PositionList expected = {
            Position(1, 2),
            Position(2, 2),
            Position(3, 2),
            Position(1, 3),
            Position(2, 3),
            Position(3, 3),
        };
        requirePositionSequenceEqual(visited, expected);
    }

    void testForEachInFrameVisitsAllFramePositions() {
        rect = Rectangle(2, 3, 3, 3);
        PositionList visited;

        rect.forEachInFrame([&](const Position pos) { visited.push_back(pos); });

        const PositionList expected = {
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
        for (std::size_t index = 0; index < expected.size(); ++index) {
            const auto &expectedPosition = expected[index];
            runWithContext(
                SOURCE_LOCATION(),
                [&]() { REQUIRE(std::ranges::find(visited, expectedPosition) != visited.end()); },
                [&]() -> std::string {
                    return std::format(
                        "index = {} / expectedPosition = ({}, {})", index, expectedPosition.x(), expectedPosition.y());
                });
        }
    }

    void testForEachInFrameWithIndexUsesClockwisePerimeterOrder() {
        rect = Rectangle(2, 3, 4, 3);
        PositionList visited;
        std::vector<int> indexes;

        rect.forEachInFrame([&](const Position pos, const int index) {
            visited.push_back(pos);
            indexes.push_back(index);
        });

        const PositionList expectedPositions = {
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
        requirePositionSequenceEqual(visited, expectedPositions);
        for (std::size_t index = 0; index < indexes.size(); ++index) {
            runWithContext(
                SOURCE_LOCATION(),
                [&]() { REQUIRE_EQUAL(indexes[index], static_cast<int>(index)); },
                [&]() -> std::string {
                    return std::format(
                        "index = {} / actualIndex = {} / expectedIndex = {}", index, indexes[index], index);
                });
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

    void testBoundsCreatesTheInclusiveEnvelopeAroundPositions() {
        const PositionList positions = {
            Position(5, 7),
            Position(-1, 4),
            Position(3, 10),
            Position(2, 3),
        };

        const auto bounds = Rectangle::bounds(positions);

        REQUIRE_EQUAL(bounds.topLeft(), Position(-1, 3));
        REQUIRE_EQUAL(bounds.bottomRight(), Position(6, 11));
        REQUIRE_EQUAL(bounds.size(), Size(7, 8));
        REQUIRE_EQUAL(Rectangle::bounds(PositionList{}), Rectangle());
    }
};
