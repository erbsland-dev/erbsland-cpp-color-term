// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(CombinedChar)
class CombinedCharTest final : public el::UnitTest {
public:
    void testConstructorsAndConversionsHandleCombinedUnicodeText() {
        const auto fromUtf8 = term::impl::CombinedChar{"e\xCC\x81"};
        const auto fromUtf32 = term::impl::CombinedChar{U"e\u0301"};
        const auto fromCodePoint = term::impl::CombinedChar{U'ä'};

        REQUIRE_EQUAL(fromUtf8.codePointCount(), std::size_t{2});
        REQUIRE_EQUAL(fromUtf8.codePoints(), (term::impl::CombinedChar::Storage{U'e', U'\u0301', 0}));
        REQUIRE_EQUAL(fromUtf8.utf8(), std::string{"e\xCC\x81"});
        REQUIRE_EQUAL(fromUtf32.utf32(), std::u32string{U"e\u0301"});
        REQUIRE_EQUAL(fromCodePoint.mainCodePoint(), U'ä');
    }

    void testDisplayWidthAndCombiningSupportMatchTerminalRules() {
        const auto wide = term::impl::CombinedChar{U'界'};
        const auto combined = term::impl::CombinedChar{U'e'}.withCombining(U'\u0301');

        REQUIRE_EQUAL(wide.displayWidth(), 2);
        REQUIRE_EQUAL(combined.displayWidth(), 1);
        REQUIRE_EQUAL(combined.utf32(), std::u32string{U"e\u0301"});
    }

    void testFromTextUtf8ParsesSingleVisibleCharacterOnly() {
        const auto ascii = term::impl::CombinedChar::fromTextUtf8("A");
        const auto unicode = term::impl::CombinedChar::fromTextUtf8("\xC3\xA4");
        const auto combined = term::impl::CombinedChar::fromTextUtf8("e\xCC\x81");

        REQUIRE(ascii.has_value());
        REQUIRE_EQUAL(ascii->mainCodePoint(), U'A');

        REQUIRE(unicode.has_value());
        REQUIRE_EQUAL(unicode->mainCodePoint(), U'\u00e4');

        REQUIRE(combined.has_value());
        REQUIRE_EQUAL(combined->utf32(), std::u32string{U"e\u0301"});

        REQUIRE_FALSE(term::impl::CombinedChar::fromTextUtf8("").has_value());
        REQUIRE_FALSE(term::impl::CombinedChar::fromTextUtf8("ab").has_value());
        REQUIRE_FALSE(term::impl::CombinedChar::fromTextUtf8("\x01").has_value());
    }
};
