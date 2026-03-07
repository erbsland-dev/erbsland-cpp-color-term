// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/unittest/UnitTest.hpp>

#include "TestHelper.hpp"


class SizeTest : public el::UnitTest {
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
        std::vector<Position> positions;
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
