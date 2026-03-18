// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <functional>
#include <limits>


TESTED_TARGETS(Position)
class PositionTest final : public el::UnitTest {
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

    void testCardinalFourReturnsNeighborsInRightDownLeftUpOrder() {
        pos = Position(5, 6);

        const auto neighbors = pos.cardinalFour();

        REQUIRE_EQUAL(neighbors[0], Position(6, 6));
        REQUIRE_EQUAL(neighbors[1], Position(5, 7));
        REQUIRE_EQUAL(neighbors[2], Position(4, 6));
        REQUIRE_EQUAL(neighbors[3], Position(5, 5));
    }

    void testRingEightReturnsNeighborsInClockwiseOrder() {
        pos = Position(5, 6);

        const auto neighbors = pos.ringEight();

        REQUIRE_EQUAL(neighbors[0], Position(6, 6));
        REQUIRE_EQUAL(neighbors[1], Position(6, 7));
        REQUIRE_EQUAL(neighbors[2], Position(5, 7));
        REQUIRE_EQUAL(neighbors[3], Position(4, 7));
        REQUIRE_EQUAL(neighbors[4], Position(4, 6));
        REQUIRE_EQUAL(neighbors[5], Position(4, 5));
        REQUIRE_EQUAL(neighbors[6], Position(5, 5));
        REQUIRE_EQUAL(neighbors[7], Position(6, 5));
    }

    void testRingEightDeltasReturnsClockwiseOffsets() {
        const auto &deltas = Position::ringEightDeltas();

        REQUIRE_EQUAL(deltas[0], Position(1, 0));
        REQUIRE_EQUAL(deltas[1], Position(1, 1));
        REQUIRE_EQUAL(deltas[2], Position(0, 1));
        REQUIRE_EQUAL(deltas[3], Position(-1, 1));
        REQUIRE_EQUAL(deltas[4], Position(-1, 0));
        REQUIRE_EQUAL(deltas[5], Position(-1, -1));
        REQUIRE_EQUAL(deltas[6], Position(0, -1));
        REQUIRE_EQUAL(deltas[7], Position(1, -1));
    }

    void testMinimumAndMaximumReturnIntegerLimits() {
        REQUIRE_EQUAL(Position::minimum(), Position(std::numeric_limits<int>::min(), std::numeric_limits<int>::min()));
        REQUIRE_EQUAL(Position::maximum(), Position(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()));
    }

    void testCardinalFourBitmaskUsesEastSouthWestNorthBits() {
        pos = Position(5, 6);

        REQUIRE_EQUAL(
            pos.cardinalFourBitmask([&](const Position testedPosition) { return testedPosition == Position{6, 6}; }),
            1U);
        REQUIRE_EQUAL(
            pos.cardinalFourBitmask([&](const Position testedPosition) { return testedPosition == Position{5, 7}; }),
            2U);
        REQUIRE_EQUAL(
            pos.cardinalFourBitmask([&](const Position testedPosition) { return testedPosition == Position{4, 6}; }),
            4U);
        REQUIRE_EQUAL(
            pos.cardinalFourBitmask([&](const Position testedPosition) { return testedPosition == Position{5, 5}; }),
            8U);
        REQUIRE_EQUAL(
            pos.cardinalFourBitmask([&](const Position testedPosition) {
                return testedPosition == Position{6, 6} || testedPosition == Position{5, 5};
            }),
            9U);
    }

    void testHashMatchesStdHashAndDependsOnCoordinates() {
        const auto first = Position{12, -7};
        const auto second = Position{-7, 12};

        REQUIRE_EQUAL(first.hash(), std::hash<Position>{}(first));
        REQUIRE_NOT_EQUAL(first.hash(), second.hash());
    }
};
