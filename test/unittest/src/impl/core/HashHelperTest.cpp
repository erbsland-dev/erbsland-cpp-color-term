// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "HashHelperTestSupport.hpp"

#include "support/TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <concepts>
#include <cstdint>

static_assert(std::same_as<decltype(impl::hashCreate(1, 2, 3)), std::size_t>);

TESTED_TARGETS(HashHelper)
class HashHelperTest final : public UNITTEST_SUBCLASS(HashHelperTestSupport) {
public:
    void testHashCombineInput32UsesTheDocumentedMixingStep() {
        const auto expected = manualHashCombineInput32(0x12345678U, -0x1234);

        REQUIRE_EQUAL(impl::hashCombineInput32(0x12345678U, -0x1234), expected);
    }

    void testHashCreate32AccumulatesAllInputsAndFinalizesThem() {
        const auto expected = manualHashCreate32(17, -23, 99);
        const auto actual = impl::hashCreate32(17, -23, 99);

        REQUIRE_EQUAL(actual, expected);
        REQUIRE_NOT_EQUAL(actual, impl::hashCreate32(99, -23, 17));
    }

    void testHashCombineInput64UsesTheDocumentedMixingStep() {
        const auto expected = manualHashCombineInput64(0x0123456789abcdefULL, -0x1234);

        REQUIRE_EQUAL(impl::hashCombineInput64(0x0123456789abcdefULL, -0x1234), expected);
    }

    void testHashCreate64AccumulatesAllInputsAndFinalizesThem() {
        const auto expected = manualHashCreate64(17, -23, 99);
        const auto actual = impl::hashCreate64(17, -23, 99);

        REQUIRE_EQUAL(actual, expected);
        REQUIRE_NOT_EQUAL(actual, impl::hashCreate64(99, -23, 17));
    }

    void testHashCreateUsesTheNativeHashWidth() {
        const auto nativeHash = impl::hashCreate(17, -23, 99);

        if constexpr (sizeof(std::size_t) == sizeof(uint64_t)) {
            REQUIRE_EQUAL(nativeHash, static_cast<std::size_t>(impl::hashCreate64(17, -23, 99)));
        } else {
            REQUIRE_EQUAL(nativeHash, static_cast<std::size_t>(impl::hashCreate32(17, -23, 99)));
        }
    }
};
