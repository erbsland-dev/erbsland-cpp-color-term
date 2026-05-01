// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <format>
#include <type_traits>
#include <vector>

static_assert(std::is_same_v<decltype(Size{}.width()), Coordinate>);
static_assert(std::is_same_v<decltype(Size{}.height()), Coordinate>);
static_assert(std::is_same_v<decltype(Size{}.coordinate(Orientation::Horizontal)), Coordinate>);
static_assert(std::is_same_v<decltype(Size{}.alignmentOffset(Size{}, Alignment::TopLeft)), Position>);
static_assert(Size{3, 7}.coordinate(Orientation::Horizontal) == 3);
static_assert(Size{3, 7}.coordinate(Orientation::Vertical) == 7);

TESTED_TARGETS(Size)
class SizeTest final : public el::UnitTest {
public:
    Size size;

    void testDefaultConstructor() {
        size = Size();
        REQUIRE_EQUAL(size.width(), 0);
        REQUIRE_EQUAL(size.height(), 0);
    }

    void testConstructor() {
        size = Size(1, 2);
        REQUIRE_EQUAL(size.width(), 1);
        REQUIRE_EQUAL(size.height(), 2);
        size = Size(Position(2, 3), Position(5, 7));
        REQUIRE_EQUAL(size.width(), 3);
        REQUIRE_EQUAL(size.height(), 4);
        size = Size(-2, -3);
        REQUIRE_EQUAL(size.width(), 0);
        REQUIRE_EQUAL(size.height(), 0);
        size = Size(Position(-2, -3), Position(5, 7));
        REQUIRE_EQUAL(size.width(), 7);
        REQUIRE_EQUAL(size.height(), 10);
    }

    void testSetters() {
        size = Size(1, 2);
        size.setWidth(3);
        size.setHeight(4);
        REQUIRE_EQUAL(size.width(), 3);
        REQUIRE_EQUAL(size.height(), 4);

        size = Size(1, 2);
        size.setWidth(-2);
        size.setHeight(-3);
        REQUIRE_EQUAL(size.width(), 0);
        REQUIRE_EQUAL(size.height(), 0);
    }

    void testCoordinate() {
        size = Size(3, 7);
        REQUIRE_EQUAL(size.coordinate(Orientation::Horizontal), 3);
        REQUIRE_EQUAL(size.coordinate(Orientation::Vertical), 7);
    }

    void testFitsInto() {
        size = Size(3, 4);
        REQUIRE(size.fitsInto(Size(3, 4)));
        REQUIRE(size.fitsInto(Size(4, 4)));
        REQUIRE(size.fitsInto(Size(3, 5)));
        REQUIRE(size.fitsInto(Size(10, 10)));
        REQUIRE_FALSE(size.fitsInto(Size(2, 4)));
        REQUIRE_FALSE(size.fitsInto(Size(3, 3)));
        REQUIRE_FALSE(size.fitsInto(Size(2, 10)));
        REQUIRE_FALSE(size.fitsInto(Size(10, 3)));
    }

    void testComparison() {
        size = Size(1, 2);
        Size size2{1, 2};
        REQUIRE_EQUAL(size, size2);
        REQUIRE_NOT_EQUAL(size, Size(1, 3));
    }

    void testArea() {
        size = Size(1, 2);
        REQUIRE_EQUAL(size.area(), 2);
        size = Size(3, 4);
        REQUIRE_EQUAL(size.area(), 12);
        size = Size(0, 0);
        REQUIRE_EQUAL(size.area(), 0);
        size = Size(0, 5);
        REQUIRE_EQUAL(size.area(), 0);
    }

    void testPosition() {
        size = Size(11, 21);
        REQUIRE_EQUAL(size.anchor(Anchor::TopLeft), Position(0, 0));
        REQUIRE_EQUAL(size.anchor(Anchor::TopCenter), Position(5, 0));
        REQUIRE_EQUAL(size.anchor(Anchor::TopRight), Position(10, 0));
        REQUIRE_EQUAL(size.anchor(Anchor::CenterLeft), Position(0, 10));
        REQUIRE_EQUAL(size.anchor(Anchor::Center), Position(5, 10));
        REQUIRE_EQUAL(size.anchor(Anchor::CenterRight), Position(10, 10));
        REQUIRE_EQUAL(size.anchor(Anchor::BottomLeft), Position(0, 20));
        REQUIRE_EQUAL(size.anchor(Anchor::BottomCenter), Position(5, 20));
        REQUIRE_EQUAL(size.anchor(Anchor::BottomRight), Position(10, 20));
    }

    void testAlignmentOffset() {
        size = Size(10, 6);
        REQUIRE_EQUAL(size.alignmentOffset(Size(4, 2), Alignment::TopLeft), Position(0, 0));
        REQUIRE_EQUAL(size.alignmentOffset(Size(4, 2), Alignment::Center), Position(3, 2));
        REQUIRE_EQUAL(size.alignmentOffset(Size(4, 2), Alignment::BottomRight), Position(6, 4));
        REQUIRE_EQUAL(size.alignmentOffset(Size(14, 8), Alignment::TopLeft), Position(0, 0));
        REQUIRE_EQUAL(size.alignmentOffset(Size(14, 8), Alignment::Center), Position(-2, -1));
        REQUIRE_EQUAL(size.alignmentOffset(Size(14, 8), Alignment::BottomRight), Position(-4, -2));
    }

    void testIsInRange() {
        size = Size(3, 4);
        REQUIRE(size.isInRange(Size(0, 0), Size(10, 10)));
        REQUIRE(size.isInRange(Size(3, 4), Size(10, 10)));
        REQUIRE(size.isInRange(Size(0, 0), Size(3, 4)));
        REQUIRE_FALSE(size.isInRange(Size(0, 0), Size(2, 4)));
        REQUIRE_FALSE(size.isInRange(Size(0, 0), Size(3, 3)));
        REQUIRE_FALSE(size.isInRange(Size(4, 4), Size(10, 10)));
        REQUIRE_FALSE(size.isInRange(Size(3, 5), Size(10, 10)));
    }

    void testComponentMax() {
        size = Size(1, 2);
        Size size2{3, 4};
        REQUIRE_EQUAL(size.componentMax(size2), Size(3, 4));
    }

    void testExpandedWith() {
        size = Size(1, 2);
        Size size2{3, 4};
        REQUIRE_EQUAL(size.expandedWith(size2), Size(3, 4));
    }

    void testAdd() {
        size = Size{1, 2};
        REQUIRE_EQUAL(size.add(Size{3, 4}), (Size{4, 6}));
        REQUIRE_EQUAL(size, (Size{4, 6}));

        size = Size::maximum();
        REQUIRE_EQUAL(size.add(Size{1, 1}), Size::maximum());
        REQUIRE_EQUAL((Size::maximum() + Size{1, 1}), Size::maximum());

        size = Size{3, 4};
        REQUIRE_EQUAL((size += Size{5, 6}), (Size{8, 10}));
    }

    void testAddOrientation() {
        size = Size{1, 2};
        REQUIRE_EQUAL(size.add(Size{3, 4}, Orientation::Horizontal), (Size{4, 2}));
        REQUIRE_EQUAL(size.add(Size{3, 4}, Orientation::Vertical), (Size{4, 6}));
    }

    void testSubtract() {
        size = Size{7, 9};
        REQUIRE_EQUAL(size.subtract(Size{3, 4}), (Size{4, 5}));
        REQUIRE_EQUAL(size, (Size{4, 5}));

        size = Size{3, 4};
        REQUIRE_EQUAL(size.subtract(Size{9, 1}), (Size{0, 3}));
        REQUIRE_EQUAL((Size{3, 4} - Size{9, 1}), (Size{0, 3}));

        size = Size{8, 10};
        REQUIRE_EQUAL((size -= Size{5, 6}), (Size{3, 4}));
    }

    void testSubtractOrientation() {
        size = Size{7, 9};
        REQUIRE_EQUAL(size.subtract(Size{3, 4}, Orientation::Horizontal), (Size{4, 9}));
        REQUIRE_EQUAL(size.subtract(Size{3, 4}, Orientation::Vertical), (Size{4, 5}));
    }

    void testExpandTo() {
        size = Size{3, 7};
        REQUIRE_EQUAL(size.expandTo(Size{5, 2}), (Size{5, 7}));
        REQUIRE_EQUAL(size, (Size{5, 7}));
        REQUIRE_EQUAL((Size{3, 7}.expandedWith(Size{5, 2})), (Size{5, 7}));

        size = Size{3, 7};
        REQUIRE_EQUAL(size.expandTo(Size{5, 9}, Orientation::Horizontal), (Size{5, 7}));
        REQUIRE_EQUAL(size.expandTo(Size{8, 9}, Orientation::Vertical), (Size{5, 9}));
        REQUIRE_EQUAL((Size{3, 7}.expandedWith(Size{5, 9}, Orientation::Horizontal)), (Size{5, 7}));
        REQUIRE_EQUAL((Size{3, 7}.componentMax(Size{5, 2})), (Size{3, 7}.expandedWith(Size{5, 2})));
    }

    void testLimitTo() {
        size = Size{7, 3};
        REQUIRE_EQUAL(size.limitTo(Size{5, 8}), (Size{5, 3}));
        REQUIRE_EQUAL(size, (Size{5, 3}));
        REQUIRE_EQUAL((Size{7, 3}.limitedWith(Size{5, 8})), (Size{5, 3}));

        size = Size{7, 9};
        REQUIRE_EQUAL(size.limitTo(Size{5, 4}, Orientation::Horizontal), (Size{5, 9}));
        REQUIRE_EQUAL(size.limitTo(Size{3, 4}, Orientation::Vertical), (Size{5, 4}));
        REQUIRE_EQUAL((Size{7, 9}.limitedWith(Size{5, 4}, Orientation::Horizontal)), (Size{5, 9}));
        REQUIRE_EQUAL((Size{7, 3}.limitedWith(Size{5, 8})), (Size{7, 3}.limitedWith(Size{5, 8})));
    }

    void testContains() {
        size = Size(8, 4);
        REQUIRE(size.contains(Position(0, 0)));
        REQUIRE(size.contains(Position(7, 3)));
        REQUIRE_FALSE(size.contains(Position(8, 2)));
        REQUIRE_FALSE(size.contains(Position(6, 4)));
        REQUIRE_FALSE(size.contains(Position(8, 4)));
        REQUIRE_FALSE(size.contains(Position(-1, 0)));
        REQUIRE_FALSE(size.contains(Position(0, -1)));
    }

    void testClamp() {
        struct ClampCase {
            Size size;
            Position input;
            Position expected;
        };
        const std::vector<ClampCase> cases = {
            {.size = Size(8, 4), .input = Position(3, 2), .expected = Position(3, 2)},
            {.size = Size(8, 4), .input = Position(-2, -3), .expected = Position(0, 0)},
            {.size = Size(8, 4), .input = Position(99, 99), .expected = Position(7, 3)},
            {.size = Size(0, 4), .input = Position(5, 2), .expected = Position(0, 2)},
            {.size = Size(3, 0), .input = Position(2, 9), .expected = Position(2, 0)},
            {.size = Size(0, 0), .input = Position(-5, 9), .expected = Position(0, 0)},
        };
        for (std::size_t index = 0; index < cases.size(); ++index) {
            const auto &[testedSize, input, expected] = cases[index];
            runWithContext(
                SOURCE_LOCATION(),
                [&]() { REQUIRE_EQUAL(testedSize.clamp(input), expected); },
                [&]() -> std::string {
                    return std::format(
                        "index = {} / size = ({}, {}) / input = ({}, {}) / expected = ({}, {})",
                        index,
                        testedSize.width(),
                        testedSize.height(),
                        input.x(),
                        input.y(),
                        expected.x(),
                        expected.y());
                });
        }
    }

    void testIndex() {
        size = Size(3, 4);
        REQUIRE_EQUAL(size.index(Position(0, 0)), 0);
        REQUIRE_EQUAL(size.index(Position(1, 0)), 1);
        REQUIRE_EQUAL(size.index(Position(2, 0)), 2);
        REQUIRE_EQUAL(size.index(Position(0, 1)), 3);
        REQUIRE_EQUAL(size.index(Position(1, 1)), 4);
        REQUIRE_EQUAL(size.index(Position(2, 1)), 5);
        REQUIRE_EQUAL(size.index(Position(0, 2)), 6);
        REQUIRE_EQUAL(size.index(Position(1, 2)), 7);
        REQUIRE_EQUAL(size.index(Position(2, 2)), 8);
        REQUIRE_EQUAL(size.index(Position(0, 3)), 9);
        REQUIRE_EQUAL(size.index(Position(1, 3)), 10);
        REQUIRE_EQUAL(size.index(Position(2, 3)), 11);
    }

    void testForEach() {
        size = Size(2, 3);
        PositionList positions;
        size.forEach([&](const Position pos) { positions.push_back(pos); });
        REQUIRE_EQUAL(positions.size(), 6);
        REQUIRE_EQUAL(positions[0], Position(0, 0));
        REQUIRE_EQUAL(positions[1], Position(1, 0));
        REQUIRE_EQUAL(positions[2], Position(0, 1));
        REQUIRE_EQUAL(positions[3], Position(1, 1));
        REQUIRE_EQUAL(positions[4], Position(0, 2));
        REQUIRE_EQUAL(positions[5], Position(1, 2));
    }
};
