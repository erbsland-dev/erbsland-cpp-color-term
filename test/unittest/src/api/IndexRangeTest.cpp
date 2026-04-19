// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/IndexRange.hpp>
#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(IndexRange)
class IndexRangeTest final : public el::UnitTest {
public:
    void testEmptyRangeReportsNoContent() {
        const auto range = IndexRange{};

        REQUIRE(range.empty());
        REQUIRE_EQUAL(range.startIndex(), 0U);
        REQUIRE_EQUAL(range.length(), 0U);
    }

    void testEndIndexUsesExclusiveBounds() {
        const auto simpleRange = IndexRange{3, 5};
        const auto openRange = IndexRange{4, IndexRange::npos};

        REQUIRE_EQUAL(simpleRange.endIndex(), 8U);
        REQUIRE_EQUAL(openRange.endIndex(), IndexRange::npos);
    }

    void testClampedToKeepsRangesInsideTheContainer() {
        const auto unchanged = IndexRange{2, 3}.clampedTo(10);
        const auto shortened = IndexRange{2, 99}.clampedTo(6);
        const auto emptyTail = IndexRange{9, 1}.clampedTo(4);

        REQUIRE_EQUAL(unchanged.startIndex(), 2U);
        REQUIRE_EQUAL(unchanged.length(), 3U);
        REQUIRE_EQUAL(shortened.startIndex(), 2U);
        REQUIRE_EQUAL(shortened.length(), 4U);
        REQUIRE_EQUAL(emptyTail.startIndex(), 4U);
        REQUIRE_EQUAL(emptyTail.length(), 0U);
    }
};
