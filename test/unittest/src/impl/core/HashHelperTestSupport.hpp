// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/TestHelper.hpp"

#include <cstdint>

class HashHelperTestSupport : public TestHelper {
public:
    [[nodiscard]] constexpr auto manualHashCombineInput32(const uint32_t hash, const int inputValue) const noexcept
        -> uint32_t {
        return static_cast<uint32_t>(inputValue) * 0xd8163841U + (hash << 6) + (hash >> 2);
    }

    [[nodiscard]] constexpr auto
    manualHashCreate32(const int inputValue1, const int inputValue2, const int inputValue3) const noexcept -> uint32_t {
        auto hash = uint32_t{0};
        hash = manualHashCombineInput32(hash, inputValue1);
        hash = manualHashCombineInput32(hash, inputValue2);
        hash = manualHashCombineInput32(hash, inputValue3);
        hash ^= (hash >> 13);
        hash *= 0x85ebca6bU;
        hash ^= (hash >> 16);
        return hash;
    }

    [[nodiscard]] constexpr auto manualHashCombineInput64(const uint64_t hash, const int inputValue) const noexcept
        -> uint64_t {
        return static_cast<uint64_t>(inputValue) + 0x9e3779b97f4a7c15ULL + (hash << 6) + (hash >> 2);
    }

    [[nodiscard]] constexpr auto
    manualHashCreate64(const int inputValue1, const int inputValue2, const int inputValue3) const noexcept -> uint64_t {
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
