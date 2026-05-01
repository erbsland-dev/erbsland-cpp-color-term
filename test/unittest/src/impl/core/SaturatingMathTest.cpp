// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/cterm/impl/SaturatingMath.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <cstdint>
#include <limits>

TESTED_TARGETS(SaturatingMath)
class SaturatingMathTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testSameTypeArithmeticSaturatesAtLimits() {
        using Int = std::int16_t;
        using UInt = std::uint16_t;

        REQUIRE_EQUAL(
            termimpl::saturatingAdd<Int>(std::numeric_limits<Int>::max(), Int{1}), std::numeric_limits<Int>::max());
        REQUIRE_EQUAL(
            termimpl::saturatingAdd<Int>(std::numeric_limits<Int>::min(), Int{-1}), std::numeric_limits<Int>::min());
        REQUIRE_EQUAL(
            termimpl::saturatingAdd<UInt>(std::numeric_limits<UInt>::max(), UInt{1}), std::numeric_limits<UInt>::max());

        REQUIRE_EQUAL(
            termimpl::saturatingSubtract<Int>(std::numeric_limits<Int>::min(), Int{1}),
            std::numeric_limits<Int>::min());
        REQUIRE_EQUAL(
            termimpl::saturatingSubtract<Int>(std::numeric_limits<Int>::max(), Int{-1}),
            std::numeric_limits<Int>::max());
        REQUIRE_EQUAL(termimpl::saturatingSubtract<UInt>(UInt{0}, UInt{1}), UInt{0});

        REQUIRE_EQUAL(
            termimpl::saturatingMultiply<Int>(std::numeric_limits<Int>::max(), Int{2}),
            std::numeric_limits<Int>::max());
        REQUIRE_EQUAL(
            termimpl::saturatingMultiply<Int>(std::numeric_limits<Int>::min(), Int{2}),
            std::numeric_limits<Int>::min());
        REQUIRE_EQUAL(
            termimpl::saturatingMultiply<UInt>(std::numeric_limits<UInt>::max(), UInt{2}),
            std::numeric_limits<UInt>::max());

        REQUIRE_EQUAL(
            termimpl::saturatingDivide<Int>(std::numeric_limits<Int>::min(), Int{-1}), std::numeric_limits<Int>::max());
        REQUIRE_EQUAL(termimpl::saturatingModulo<Int>(std::numeric_limits<Int>::min(), Int{-1}), Int{0});
    }

    void testMixedSignedUnsignedOperationsSaturate() {
        using Int = std::int8_t;
        using UInt = std::uint8_t;

        REQUIRE_EQUAL(termimpl::saturatingAdd<Int>(Int{120}, UInt{20}), std::numeric_limits<Int>::max());
        REQUIRE_EQUAL(termimpl::saturatingAdd<UInt>(UInt{5}, Int{-10}), UInt{0});
        REQUIRE_EQUAL(termimpl::saturatingSubtract<Int>(Int{-120}, UInt{20}), std::numeric_limits<Int>::min());
        REQUIRE_EQUAL(termimpl::saturatingSubtract<UInt>(UInt{250}, Int{-10}), std::numeric_limits<UInt>::max());

        REQUIRE_EQUAL(termimpl::saturatingMultiply<Int>(Int{-100}, UInt{2}), std::numeric_limits<Int>::min());
        REQUIRE_EQUAL(termimpl::saturatingMultiply<UInt>(UInt{5}, Int{-2}), UInt{0});

        REQUIRE_EQUAL(termimpl::saturatingDivide<Int>(std::numeric_limits<Int>::min(), UInt{128}), Int{-1});
        REQUIRE_EQUAL(termimpl::saturatingDivide<Int>(std::numeric_limits<Int>::min(), UInt{129}), Int{0});
        REQUIRE_EQUAL(termimpl::saturatingDivide<UInt>(UInt{5}, Int{-2}), UInt{0});
    }

    void testMixedModuloUsesAbsoluteDivisor() {
        using Int = std::int8_t;
        using UInt = std::uint8_t;

        REQUIRE_EQUAL(termimpl::saturatingModulo<Int>(std::numeric_limits<Int>::min(), UInt{128}), Int{0});
        REQUIRE_EQUAL(termimpl::saturatingModulo<Int>(Int{-5}, UInt{200}), Int{-5});
        REQUIRE_EQUAL(termimpl::saturatingModulo<UInt>(UInt{5}, Int{-2}), UInt{1});
        REQUIRE_EQUAL(termimpl::saturatingModulo<UInt>(UInt{5}, std::int16_t{-300}), UInt{5});
    }

    void testOverflowChecks() {
        using Int = std::int16_t;
        using UInt = std::uint16_t;

        REQUIRE(termimpl::willAddOverflow<Int>(std::numeric_limits<Int>::max(), Int{1}));
        REQUIRE(termimpl::willSubtractOverflow<UInt>(UInt{0}, UInt{1}));
        REQUIRE(termimpl::willMultiplyOverflow<Int>(std::numeric_limits<Int>::min(), Int{2}));
        REQUIRE(termimpl::willDivideOverflow<Int>(std::numeric_limits<Int>::min(), Int{-1}));
        REQUIRE(termimpl::willModuloOverflow<Int>(std::numeric_limits<Int>::min(), Int{-1}));

        REQUIRE(termimpl::willAddOverflow<Int>(Int{120}, std::uint16_t{40000}));
        REQUIRE(termimpl::willSubtractOverflow<UInt>(UInt{10}, std::int32_t{-70000}));
        REQUIRE(termimpl::willMultiplyOverflow<UInt>(UInt{10}, Int{-1}));
        REQUIRE(!termimpl::willDivideOverflow<UInt>(UInt{0}, Int{-1}));
        REQUIRE(!termimpl::willModuloOverflow<UInt>(UInt{5}, Int{-2}));
    }
};
