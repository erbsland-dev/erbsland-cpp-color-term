// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/unittest/UnitTest.hpp>

#include "TestHelper.hpp"


class AlignmentTest final : public el::UnitTest {
public:
    void testCombinationWithOrOperator() {
        REQUIRE_EQUAL(Alignment::Top | Alignment::Left, Alignment::TopLeft);
        REQUIRE_EQUAL(Alignment::Top | Alignment::HCenter, Alignment::TopCenter);
        REQUIRE_EQUAL(Alignment::Top | Alignment::Right, Alignment::TopRight);
        REQUIRE_EQUAL(Alignment::VCenter | Alignment::Left, Alignment::CenterLeft);
        REQUIRE_EQUAL(Alignment::VCenter | Alignment::HCenter, Alignment::Center);
        REQUIRE_EQUAL(Alignment::VCenter | Alignment::Right, Alignment::CenterRight);
        REQUIRE_EQUAL(Alignment::Bottom | Alignment::Left, Alignment::BottomLeft);
        REQUIRE_EQUAL(Alignment::Bottom | Alignment::HCenter, Alignment::BottomCenter);
        REQUIRE_EQUAL(Alignment::Bottom | Alignment::Right, Alignment::BottomRight);
    }

    void testMaskingWithAndOperator() {
        REQUIRE_EQUAL(Alignment::BottomRight & Alignment::VerticalMask, Alignment::Bottom);
        REQUIRE_EQUAL(Alignment::BottomRight & Alignment::HorizontalMask, Alignment::Right);
        REQUIRE_EQUAL(Alignment::TopCenter & Alignment::VerticalMask, Alignment::Top);
        REQUIRE_EQUAL(Alignment::TopCenter & Alignment::HorizontalMask, Alignment::HCenter);
    }
};
