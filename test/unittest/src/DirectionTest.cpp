// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>


TESTED_TARGETS(Direction)
class DirectionTest final : public el::UnitTest {
public:
    void testDefaultDirectionIsNone() {
        const Direction direction;

        REQUIRE_EQUAL(direction, Direction::None);
    }

    void testDirectionToDetla() {
        REQUIRE_EQUAL(Direction{Direction::NorthWest}.toDelta(), Position(-1, -1));
        REQUIRE_EQUAL(Direction{Direction::East}.toDelta(), Position(1, 0));
        REQUIRE_EQUAL(Direction{Direction::None}.toDelta(), Position(0, 0));
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
};
