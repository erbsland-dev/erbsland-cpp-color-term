// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <array>

TESTED_TARGETS(CombinedChar)
class CombinedCharTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testConstructorsAndConversionsHandleCombinedUnicodeText() {
        const auto fromUtf8 = term::impl::CombinedChar{bytes({0x65, 0xCC, 0x81})};
        const auto fromUtf32 = term::impl::CombinedChar{U"e\u0301"};
        const auto fromCodePoint = term::impl::CombinedChar{U'ä'};

        REQUIRE_EQUAL(fromUtf8.codePointCount(), std::size_t{2});
        REQUIRE_EQUAL(fromUtf8.codePoints(), (term::impl::CombinedChar::Storage{U'e', U'\u0301', 0}));
        REQUIRE_EQUAL(fromUtf8.utf8(), bytes({0x65, 0xCC, 0x81}));
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

    void testFromTextUtf8NormalizesToOneDeterministicCharacter() {
        const auto ascii = term::impl::CombinedChar::fromTextUtf8("A");
        const auto unicode = term::impl::CombinedChar::fromTextUtf8(bytes({0xC3, 0xA4}));
        const auto combined = term::impl::CombinedChar::fromTextUtf8(bytes({0x65, 0xCC, 0x81}));
        const auto empty = term::impl::CombinedChar::fromTextUtf8("");
        const auto multiple = term::impl::CombinedChar::fromTextUtf8("ab");
        const auto control = term::impl::CombinedChar::fromTextUtf8("\x01");

        REQUIRE(ascii.has_value());
        REQUIRE_EQUAL(ascii->mainCodePoint(), U'A');

        REQUIRE(unicode.has_value());
        REQUIRE_EQUAL(unicode->mainCodePoint(), U'\u00e4');

        REQUIRE(combined.has_value());
        REQUIRE_EQUAL(combined->utf32(), std::u32string{U"e\u0301"});
        REQUIRE(empty.has_value());
        REQUIRE_EQUAL(empty->mainCodePoint(), U'\uFFFD');
        REQUIRE(multiple.has_value());
        REQUIRE_EQUAL(multiple->mainCodePoint(), U'\uFFFD');
        REQUIRE(control.has_value());
        REQUIRE_EQUAL(control->mainCodePoint(), U'\uFFFD');
    }

    void testTextNormalizationHandlesMalformedUtf8AndUtf32() {
        const auto replacement = term::impl::CombinedChar::fromTextUtf8(bytes({0xC3}));
        constexpr auto invalidUtf32 = std::array<char32_t, 1>{0x110000U};
        const auto invalidUtf32Replacement =
            term::impl::CombinedChar::fromTextUtf32(std::u32string_view{invalidUtf32.data(), invalidUtf32.size()});
        const auto leadingCombining = term::impl::CombinedChar::fromTextUtf32(U"\u0301");
        const auto tooManyCombining =
            term::impl::CombinedChar::fromTextUtf8(bytes({0x61, 0xCC, 0x81, 0xCC, 0x82, 0xCC, 0x83}));
        const auto malformedThenVisible = term::impl::CombinedChar::fromTextUtf8(bytes({0xC3, 0x42}));

        REQUIRE(replacement.has_value());
        REQUIRE_EQUAL(replacement->mainCodePoint(), U'\uFFFD');
        REQUIRE(invalidUtf32Replacement.has_value());
        REQUIRE_EQUAL(invalidUtf32Replacement->mainCodePoint(), U'\uFFFD');
        REQUIRE(leadingCombining.has_value());
        REQUIRE_EQUAL(leadingCombining->mainCodePoint(), U'\uFFFD');
        REQUIRE(tooManyCombining.has_value());
        REQUIRE_EQUAL(tooManyCombining->utf32(), std::u32string{U"a\u0301\u0302"});
        REQUIRE(malformedThenVisible.has_value());
        REQUIRE_EQUAL(malformedThenVisible->mainCodePoint(), U'\uFFFD');
    }

    void testWithCombiningUsesEncodingErrorPolicy() {
        const auto base = term::impl::CombinedChar{U"e\u0301\u0302"};

        REQUIRE_THROWS_AS(
            std::invalid_argument, term::impl::CombinedChar{}.withCombining(U'\u0301', term::EncodingErrors::Throw));
        REQUIRE_THROWS_AS(
            std::invalid_argument, term::impl::CombinedChar{U'e'}.withCombining(U'x', term::EncodingErrors::Throw));
        REQUIRE_EQUAL(term::impl::CombinedChar{}.withCombining(U'\u0301'), term::impl::CombinedChar{});
        REQUIRE_EQUAL(term::impl::CombinedChar{U'e'}.withCombining(U'x'), term::impl::CombinedChar{U'e'});
        REQUIRE_EQUAL(base.withCombining(U'\u0303'), base);
        REQUIRE_EQUAL(base.withCombining(U'\u0303', term::EncodingErrors::Ignore), base);
    }
};
