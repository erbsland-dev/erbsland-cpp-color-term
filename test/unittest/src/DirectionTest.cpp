// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/unittest/UnitTest.hpp>

#include "TestHelper.hpp"

#include <stdexcept>


TESTED_TARGETS(Direction)
class DirectionTest final : public el::UnitTest {
public:
    void testDefaultDirectionIsNone() {
        const term::Direction direction;

        REQUIRE_EQUAL(direction, term::Direction::None);
    }

    void testDirectionToDetla() {
        REQUIRE_EQUAL(term::Direction{term::Direction::NorthWest}.toDelta(), Position(-1, -1));
        REQUIRE_EQUAL(term::Direction{term::Direction::East}.toDelta(), Position(1, 0));
        REQUIRE_EQUAL(term::Direction{term::Direction::None}.toDelta(), Position(0, 0));
    }

    void testDirectionToString() {
        REQUIRE_EQUAL(term::Direction{term::Direction::North}.toString(), "north");
        REQUIRE_EQUAL(term::Direction{term::Direction::SouthEast}.toString(), "south_east");
        REQUIRE_EQUAL(term::Direction{term::Direction::None}.toString(), "none");
    }

    void testDirectionFromStringAcceptsVariants() {
        REQUIRE_EQUAL(term::Direction::fromString("north"), term::Direction::North);
        REQUIRE_EQUAL(term::Direction::fromString("North East"), term::Direction::NorthEast);
        REQUIRE_EQUAL(term::Direction::fromString("south_west"), term::Direction::SouthWest);
        REQUIRE_EQUAL(term::Direction::fromString(""), term::Direction::None);
    }

    void testDirectionFromStringRejectsUnknownValue() {
        REQUIRE_THROWS_AS(std::invalid_argument, term::Direction::fromString("sideways"));
    }
};
