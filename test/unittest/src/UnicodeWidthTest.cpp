// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/unittest/UnitTest.hpp>

#include "TestHelper.hpp"

class UnicodeWidthTest : public el::UnitTest {
public:
    void testAsciiCharacterWidth() { REQUIRE_EQUAL(cterm_impl::consoleCharacterWidth(U'A'), 1U); }

    void testWideCharacterWidth() { REQUIRE_EQUAL(cterm_impl::consoleCharacterWidth(U'界'), 2U); }

    void testCombiningCharacterWidth() { REQUIRE_EQUAL(cterm_impl::consoleCharacterWidth(U'\u0301'), 0U); }
};
