// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <functional>

static_assert(Alignment::BottomRight.horizontal() == Alignment::Right);
static_assert(Alignment::BottomRight.vertical() == Alignment::Bottom);
static_assert(Alignment::Center.horizontalOffset(10, 4) == 3);
static_assert(Alignment::BottomRight.verticalOffset(8, 2) == 6);

TESTED_TARGETS(Alignment)
class AlignmentTest final : public el::UnitTest {
public:
    void testDefaultAlignmentIsTopLeft() {
        const Alignment alignment;

        REQUIRE_EQUAL(alignment, Alignment::TopLeft);
    }

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

    void testHorizontalAndVerticalComponents() {
        REQUIRE_EQUAL(Alignment::BottomRight.horizontal(), Alignment::Right);
        REQUIRE_EQUAL(Alignment::BottomRight.vertical(), Alignment::Bottom);
        REQUIRE_EQUAL(Alignment::Center.horizontal(), Alignment::HCenter);
        REQUIRE_EQUAL(Alignment::Center.vertical(), Alignment::VCenter);
    }

    void testHorizontalAlignmentTestsMatchAllRows() {
        REQUIRE(Alignment::TopLeft.isLeft());
        REQUIRE(Alignment::CenterLeft.isLeft());
        REQUIRE(Alignment::BottomLeft.isLeft());

        REQUIRE(Alignment::TopCenter.isHorizontalCenter());
        REQUIRE(Alignment::Center.isHorizontalCenter());
        REQUIRE(Alignment::BottomCenter.isHorizontalCenter());

        REQUIRE(Alignment::TopRight.isRight());
        REQUIRE(Alignment::CenterRight.isRight());
        REQUIRE(Alignment::BottomRight.isRight());

        REQUIRE_FALSE(Alignment::TopRight.isLeft());
        REQUIRE_FALSE(Alignment::CenterLeft.isRight());
    }

    void testVerticalAlignmentTestsMatchAllColumns() {
        REQUIRE(Alignment::TopLeft.isTop());
        REQUIRE(Alignment::TopCenter.isTop());
        REQUIRE(Alignment::TopRight.isTop());

        REQUIRE(Alignment::CenterLeft.isVerticalCenter());
        REQUIRE(Alignment::Center.isVerticalCenter());
        REQUIRE(Alignment::CenterRight.isVerticalCenter());

        REQUIRE(Alignment::BottomLeft.isBottom());
        REQUIRE(Alignment::BottomCenter.isBottom());
        REQUIRE(Alignment::BottomRight.isBottom());

        REQUIRE_FALSE(Alignment::BottomRight.isTop());
        REQUIRE_FALSE(Alignment::TopLeft.isBottom());
    }

    void testOffsetHelpers() {
        REQUIRE_EQUAL(Alignment::Left.horizontalOffset(10, 4), 0);
        REQUIRE_EQUAL(Alignment::HCenter.horizontalOffset(10, 4), 3);
        REQUIRE_EQUAL(Alignment::Right.horizontalOffset(10, 4), 6);
        REQUIRE_EQUAL(Alignment::Top.verticalOffset(8, 2), 0);
        REQUIRE_EQUAL(Alignment::VCenter.verticalOffset(8, 2), 3);
        REQUIRE_EQUAL(Alignment::Bottom.verticalOffset(8, 2), 6);

        REQUIRE_EQUAL(Alignment::Center.horizontalOffset(4, 10), -3);
        REQUIRE_EQUAL(Alignment::BottomRight.verticalOffset(2, 8), -6);
    }

    void testSwitchCompatibility() {
        auto matchedRight = false;

        switch (Alignment::CenterRight.horizontal()) {
        case Alignment::Right:
            matchedRight = true;
            break;
        default:
            break;
        }

        REQUIRE(matchedRight);
    }

    void testHashMatchesStdHashAndAlignmentValue() {
        const auto topLeft = Alignment{Alignment::TopLeft};
        const auto bottomRight = Alignment{Alignment::BottomRight};

        REQUIRE_EQUAL(topLeft.hash(), std::hash<Alignment>{}(topLeft));
        REQUIRE_NOT_EQUAL(topLeft.hash(), bottomRight.hash());
    }
};
