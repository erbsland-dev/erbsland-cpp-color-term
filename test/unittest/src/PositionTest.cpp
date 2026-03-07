// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/unittest/UnitTest.hpp>

#include "TestHelper.hpp"


class PositionTest : public el::UnitTest {
public:
    Position pos;

    void testDefaultConstructor() {
        pos = Position();
        REQUIRE_EQUAL(pos.x(), 0);
        REQUIRE_EQUAL(pos.y(), 0);
    }

    void testConstructor() {
        pos = Position(1, 2);
        REQUIRE_EQUAL(pos.x(), 1);
        REQUIRE_EQUAL(pos.y(), 2);
    }

    void testSetters() {
        pos = Position(1, 2);
        pos.setX(3);
        pos.setY(4);
        REQUIRE_EQUAL(pos.x(), 3);
        REQUIRE_EQUAL(pos.y(), 4);
    }

    void testCopyConstructor() {
        pos = Position(1, 2);
        Position pos2{pos};
        REQUIRE_EQUAL(pos2.x(), 1);
        REQUIRE_EQUAL(pos2.y(), 2);
    }

    void testComparison() {
        pos = Position(1, 2);
        Position pos2{1, 2};
        REQUIRE_EQUAL(pos, pos2);
        REQUIRE_NOT_EQUAL(pos, Position(1, 3));
    }

    void testAddition() {
        pos = Position(1, 2);
        Position pos2{3, 4};
        REQUIRE_EQUAL(pos + pos2, Position(4, 6));
    }

    void testSubtraction() {
        pos = Position(1, 2);
        Position pos2{3, 4};
        REQUIRE_EQUAL(pos - pos2, Position(-2, -2));
    }

    void testDistanceTo() {
        pos = Position(2, 3);
        Position pos2{12, 23};
        REQUIRE_EQUAL(pos.distanceTo(pos2), 30);
    }

    void testComponentMax() {
        pos = Position(1, 2);
        Position pos2{3, 4};
        REQUIRE_EQUAL(pos.componentMax(pos2), Position(3, 4));
    }

    void testComponentMin() {
        pos = Position(1, 2);
        Position pos2{3, 4};
        REQUIRE_EQUAL(pos.componentMin(pos2), Position(1, 2));
    }

    void testCardinalFourBitmaskUsesEastSouthWestNorthBits() {
        pos = Position(5, 6);

        REQUIRE_EQUAL(
            pos.cardinalFourBitmask([&](const Position testedPosition) { return testedPosition == Position{6, 6}; }), 1U);
        REQUIRE_EQUAL(
            pos.cardinalFourBitmask([&](const Position testedPosition) { return testedPosition == Position{5, 7}; }), 2U);
        REQUIRE_EQUAL(
            pos.cardinalFourBitmask([&](const Position testedPosition) { return testedPosition == Position{4, 6}; }), 4U);
        REQUIRE_EQUAL(
            pos.cardinalFourBitmask([&](const Position testedPosition) { return testedPosition == Position{5, 5}; }), 8U);
        REQUIRE_EQUAL(
            pos.cardinalFourBitmask([&](const Position testedPosition) {
                return testedPosition == Position{6, 6} || testedPosition == Position{5, 5};
            }),
            9U);
    }
};
