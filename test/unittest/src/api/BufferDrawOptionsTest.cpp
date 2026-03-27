// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>


TESTED_TARGETS(BufferDrawOptions)
class BufferDrawOptionsTest final : public el::UnitTest {
public:
    void testDefaultConstructionUsesPositionTargetAndFullSource() {
        const auto options = BufferDrawOptions{};
        const auto expectedRect = Rectangle{0, 0, 0, 0};

        REQUIRE_EQUAL(options.targetRect(), expectedRect);
        REQUIRE(options.isTargetPosition());
        REQUIRE_EQUAL(options.sourceRect(), expectedRect);
        REQUIRE(options.useFullSource());
        REQUIRE(options.combinationStyle() == nullptr);
        REQUIRE_FALSE(options.overwriteColors());
    }

    void testPositionConstructorStoresOnlyTheTargetPosition() {
        const auto options = BufferDrawOptions{Position{3, 4}};
        const auto expectedTargetRect = Rectangle{3, 4, 0, 0};
        const auto expectedSourceRect = Rectangle{0, 0, 0, 0};

        REQUIRE_EQUAL(options.targetRect(), expectedTargetRect);
        REQUIRE(options.isTargetPosition());
        REQUIRE_EQUAL(options.sourceRect(), expectedSourceRect);
        REQUIRE(options.useFullSource());
    }

    void testRectangleConstructorAndSettersStoreExplicitRectsAndFlags() {
        auto options = BufferDrawOptions{Rectangle{1, 2, 7, 8}, Rectangle{4, 5, 2, 3}};
        const auto combinationStyle = CharCombinationStyle::colorOverlay();
        const auto expectedInitialTargetRect = Rectangle{1, 2, 7, 8};
        const auto expectedInitialSourceRect = Rectangle{4, 5, 2, 3};

        REQUIRE_EQUAL(options.targetRect(), expectedInitialTargetRect);
        REQUIRE_FALSE(options.isTargetPosition());
        REQUIRE_EQUAL(options.sourceRect(), expectedInitialSourceRect);
        REQUIRE_FALSE(options.useFullSource());

        options.setTargetRect(Rectangle{9, 8, 1, 2});
        options.setSourceRect(Rectangle{6, 5, 4, 3});
        options.setCombinationStyle(combinationStyle);
        options.setOverwriteColors(true);

        REQUIRE_EQUAL(options.targetRect(), (Rectangle{9, 8, 1, 2}));
        REQUIRE_FALSE(options.isTargetPosition());
        REQUIRE_EQUAL(options.sourceRect(), (Rectangle{6, 5, 4, 3}));
        REQUIRE_FALSE(options.useFullSource());
        REQUIRE(options.combinationStyle() == combinationStyle);
        REQUIRE(options.overwriteColors());
    }
};
