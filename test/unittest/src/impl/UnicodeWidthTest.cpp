// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(UnicodeWidth)
class UnicodeWidthTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testAsciiCharacterWidth() { REQUIRE_EQUAL(impl::consoleCharacterWidth(U'A'), 1U); }

    void testFastPathLatinRangeBoundaries() {
        REQUIRE_EQUAL(impl::consoleCharacterWidth(U'\0'), 0U);
        REQUIRE_EQUAL(impl::consoleCharacterWidth(U'\x1f'), 0U);
        REQUIRE_EQUAL(impl::consoleCharacterWidth(U'\x20'), 1U);
        REQUIRE_EQUAL(impl::consoleCharacterWidth(U'\x7f'), 0U);
        REQUIRE_EQUAL(impl::consoleCharacterWidth(U'\u00ad'), 0U);
        REQUIRE_EQUAL(impl::consoleCharacterWidth(U'\u00ae'), 1U);
        REQUIRE_EQUAL(impl::consoleCharacterWidth(U'\u024f'), 1U);
    }

    void testFastPathBoxDrawingRangeBoundaries() {
        REQUIRE_EQUAL(impl::consoleCharacterWidth(U'\u2500'), 1U);
        REQUIRE_EQUAL(impl::consoleCharacterWidth(U'\u25fb'), 1U);
        REQUIRE_EQUAL(impl::consoleCharacterWidth(U'\u25fc'), 1U);
        REQUIRE_EQUAL(impl::consoleCharacterWidth(U'\u25fd'), 2U);
    }

    void testWideCharacterWidth() { REQUIRE_EQUAL(impl::consoleCharacterWidth(U'界'), 2U); }

    void testCombiningCharacterWidth() { REQUIRE_EQUAL(impl::consoleCharacterWidth(U'\u0301'), 0U); }

    void testCalculateDisplayWidthIsNoexceptAndReplacesInvalidUtf8() {
        static_assert(noexcept(impl::calculateDisplayWidth(std::string_view{})));
        const auto text = bytes({0x41, 0xC3, 0x42});
        REQUIRE_EQUAL(impl::calculateDisplayWidth(std::string_view{text}), 3U);
    }
};
