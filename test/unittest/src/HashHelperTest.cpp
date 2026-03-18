// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <concepts>
#include <cstdint>

class HashHelperTestTools final {
public:
    static constexpr auto manualHashCombineInput32(const uint32_t hash, const int inputValue) noexcept -> uint32_t {
        return static_cast<uint32_t>(inputValue) * 0xd8163841U + (hash << 6) + (hash >> 2);
    }

    static constexpr auto
    manualHashCreate32(const int inputValue1, const int inputValue2, const int inputValue3) noexcept -> uint32_t {
        auto hash = uint32_t{0};
        hash = manualHashCombineInput32(hash, inputValue1);
        hash = manualHashCombineInput32(hash, inputValue2);
        hash = manualHashCombineInput32(hash, inputValue3);
        hash ^= (hash >> 13);
        hash *= 0x85ebca6bU;
        hash ^= (hash >> 16);
        return hash;
    }

    static constexpr auto manualHashCombineInput64(const uint64_t hash, const int inputValue) noexcept -> uint64_t {
        return static_cast<uint64_t>(inputValue) + 0x9e3779b97f4a7c15ULL + (hash << 6) + (hash >> 2);
    }

    static constexpr auto
    manualHashCreate64(const int inputValue1, const int inputValue2, const int inputValue3) noexcept -> uint64_t {
        auto hash = uint64_t{0};
        hash = manualHashCombineInput64(hash, inputValue1);
        hash = manualHashCombineInput64(hash, inputValue2);
        hash = manualHashCombineInput64(hash, inputValue3);
        hash ^= (hash >> 30);
        hash *= 0xbf58476d1ce4e5b9ULL;
        hash ^= (hash >> 27);
        hash *= 0x94d049bb133111ebULL;
        hash ^= (hash >> 31);
        return hash;
    }
};

static_assert(std::same_as<decltype(term::impl::hashCreate(1, 2, 3)), std::size_t>);


TESTED_TARGETS(HashHelper)
class HashHelperTest final : public el::UnitTest {
public:
    void testHashCombineInput32UsesTheDocumentedMixingStep() {
        const auto expected = HashHelperTestTools::manualHashCombineInput32(0x12345678U, -0x1234);

        REQUIRE_EQUAL(term::impl::hashCombineInput32(0x12345678U, -0x1234), expected);
    }

    void testHashCreate32AccumulatesAllInputsAndFinalizesThem() {
        const auto expected = HashHelperTestTools::manualHashCreate32(17, -23, 99);
        const auto actual = term::impl::hashCreate32(17, -23, 99);

        REQUIRE_EQUAL(actual, expected);
        REQUIRE_NOT_EQUAL(actual, term::impl::hashCreate32(99, -23, 17));
    }

    void testHashCombineInput64UsesTheDocumentedMixingStep() {
        const auto expected = HashHelperTestTools::manualHashCombineInput64(0x0123456789abcdefULL, -0x1234);

        REQUIRE_EQUAL(term::impl::hashCombineInput64(0x0123456789abcdefULL, -0x1234), expected);
    }

    void testHashCreate64AccumulatesAllInputsAndFinalizesThem() {
        const auto expected = HashHelperTestTools::manualHashCreate64(17, -23, 99);
        const auto actual = term::impl::hashCreate64(17, -23, 99);

        REQUIRE_EQUAL(actual, expected);
        REQUIRE_NOT_EQUAL(actual, term::impl::hashCreate64(99, -23, 17));
    }

    void testHashCreateUsesTheNativeHashWidth() {
        const auto nativeHash = term::impl::hashCreate(17, -23, 99);

        if constexpr (sizeof(std::size_t) == sizeof(uint64_t)) {
            REQUIRE_EQUAL(nativeHash, static_cast<std::size_t>(term::impl::hashCreate64(17, -23, 99)));
        } else {
            REQUIRE_EQUAL(nativeHash, static_cast<std::size_t>(term::impl::hashCreate32(17, -23, 99)));
        }
    }
};
