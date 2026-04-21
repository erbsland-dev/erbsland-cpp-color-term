// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "erbsland/cterm/geometry/AxisMapper.hpp"
#include "support/TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

static_assert(AxisMapper{Orientation::Horizontal}.size(3, 7) == Size(3, 7));
static_assert(AxisMapper{Orientation::Vertical}.size(3, 7) == Size(7, 3));
static_assert(AxisMapper{Orientation::Horizontal}.position(3, 7) == Position(3, 7));
static_assert(AxisMapper{Orientation::Vertical}.position(3, 7) == Position(7, 3));

TESTED_TARGETS(AxisMapper)
class AxisMapperTest final : public el::UnitTest {
public:
    void testHorizontalOrientationMapsMainAxisToWidthAndX() {
        const auto mapper = AxisMapper{Orientation::Horizontal};

        REQUIRE_EQUAL(mapper.size(12, 5), Size(12, 5));
        REQUIRE_EQUAL(mapper.position(7, 2), Position(7, 2));
        REQUIRE_EQUAL(mapper.position(7), Position(7, 0));
        REQUIRE_EQUAL(mapper.horizontalValue(11, 22), 11);
        REQUIRE_EQUAL(mapper.verticalValue(11, 22), 22);
    }

    void testVerticalOrientationMapsMainAxisToHeightAndY() {
        const auto mapper = AxisMapper{Orientation::Vertical};

        REQUIRE_EQUAL(mapper.size(12, 5), Size(5, 12));
        REQUIRE_EQUAL(mapper.position(7, 2), Position(2, 7));
        REQUIRE_EQUAL(mapper.position(7), Position(0, 7));
        REQUIRE_EQUAL(mapper.horizontalValue(11, 22), 22);
        REQUIRE_EQUAL(mapper.verticalValue(11, 22), 11);
    }
};
