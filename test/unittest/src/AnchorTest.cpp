// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/unittest/UnitTest.hpp>

#include "TestHelper.hpp"


class AnchorTest : public el::UnitTest {
public:
    void testCombinationWithOrOperator() {
        REQUIRE_EQUAL(Anchor::Top | Anchor::Left, Anchor::TopLeft);
        REQUIRE_EQUAL(Anchor::Top | Anchor::HCenter, Anchor::TopCenter);
        REQUIRE_EQUAL(Anchor::Top | Anchor::Right, Anchor::TopRight);
        REQUIRE_EQUAL(Anchor::VCenter | Anchor::Left, Anchor::CenterLeft);
        REQUIRE_EQUAL(Anchor::VCenter | Anchor::HCenter, Anchor::Center);
        REQUIRE_EQUAL(Anchor::VCenter | Anchor::Right, Anchor::CenterRight);
        REQUIRE_EQUAL(Anchor::Bottom | Anchor::Left, Anchor::BottomLeft);
        REQUIRE_EQUAL(Anchor::Bottom | Anchor::HCenter, Anchor::BottomCenter);
        REQUIRE_EQUAL(Anchor::Bottom | Anchor::Right, Anchor::BottomRight);
    }

    void testMaskingWithAndOperator() {
        REQUIRE_EQUAL(Anchor::BottomRight & Anchor::VMask, Anchor::Bottom);
        REQUIRE_EQUAL(Anchor::BottomRight & Anchor::HMask, Anchor::Right);
        REQUIRE_EQUAL(Anchor::TopCenter & Anchor::VMask, Anchor::Top);
        REQUIRE_EQUAL(Anchor::TopCenter & Anchor::HMask, Anchor::HCenter);
    }
};
