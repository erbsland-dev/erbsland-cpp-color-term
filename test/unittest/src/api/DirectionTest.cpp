// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <array>
#include <format>
#include <functional>
#include <stdexcept>

TESTED_TARGETS(Direction)
class DirectionTest final : public el::UnitTest {
public:
    void testDefaultDirectionIsNone() {
        const Direction direction;

        REQUIRE_EQUAL(direction, Direction::None);
    }

    void testDirectionToDelta() {
        REQUIRE_EQUAL(Direction{Direction::NorthWest}.toDelta(), Position(-1, -1));
        REQUIRE_EQUAL(Direction{Direction::East}.toDelta(), Position(1, 0));
        REQUIRE_EQUAL(Direction{Direction::None}.toDelta(), Position(0, 0));
    }

    void testDirectionFromDeltaUsesOnlyTheCoordinateSigns() {
        struct TestCase {
            Position delta;
            Direction expected;
        };
        constexpr auto testCases = std::array<TestCase, 9>{{
            {Position(-3, -7), Direction::NorthWest},
            {Position(0, -2), Direction::North},
            {Position(5, -1), Direction::NorthEast},
            {Position(-4, 0), Direction::West},
            {Position(0, 0), Direction::None},
            {Position(9, 0), Direction::East},
            {Position(-8, 6), Direction::SouthWest},
            {Position(0, 4), Direction::South},
            {Position(2, 3), Direction::SouthEast},
        }};

        for (const auto &[delta, expected] : testCases) {
            runWithContext(
                SOURCE_LOCATION(),
                [&]() { REQUIRE_EQUAL(Direction::fromDelta(delta), expected); },
                [&]() -> std::string {
                    return std::format("delta = ({}, {}) / expected = {}", delta.x(), delta.y(), expected.toString());
                });
        }
    }

    void testDirectionToString() {
        REQUIRE_EQUAL(Direction{Direction::North}.toString(), "north");
        REQUIRE_EQUAL(Direction{Direction::SouthEast}.toString(), "south_east");
        REQUIRE_EQUAL(Direction{Direction::None}.toString(), "none");
    }

    void testDirectionFromStringAcceptsVariants() {
        REQUIRE_EQUAL(Direction::fromString("north"), Direction::North);
        REQUIRE_EQUAL(Direction::fromString("North East"), Direction::NorthEast);
        REQUIRE_EQUAL(Direction::fromString("south_west"), Direction::SouthWest);
        REQUIRE_EQUAL(Direction::fromString(""), Direction::None);
    }

    void testDirectionFromStringRejectsUnknownValue() {
        REQUIRE_THROWS_AS(std::invalid_argument, Direction::fromString("sideways"));
    }

    void testContainsMatchesFullDirectionMembership() {
        REQUIRE(Direction{Direction::NorthWest}.contains(Direction::NorthWest));
        REQUIRE(Direction{Direction::NorthWest}.contains(Direction::North));
        REQUIRE(Direction{Direction::NorthWest}.contains(Direction::West));

        REQUIRE(Direction{Direction::SouthEast}.contains(Direction::SouthEast));
        REQUIRE(Direction{Direction::SouthEast}.contains(Direction::South));
        REQUIRE(Direction{Direction::SouthEast}.contains(Direction::East));

        REQUIRE_FALSE(Direction{Direction::NorthWest}.contains(Direction::SouthWest));
        REQUIRE_FALSE(Direction{Direction::East}.contains(Direction::NorthEast));
        REQUIRE_FALSE(Direction{Direction::None}.contains(Direction::None));
    }

    void testHashMatchesStdHashAndDirectionValue() {
        const auto northWest = Direction{Direction::NorthWest};
        const auto southEast = Direction{Direction::SouthEast};

        REQUIRE_EQUAL(northWest.hash(), std::hash<Direction>{}(northWest));
        REQUIRE_NOT_EQUAL(northWest.hash(), southEast.hash());
    }
};
