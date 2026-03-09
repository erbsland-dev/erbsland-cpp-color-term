// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/unittest/UnitTest.hpp>

#include "TestHelper.hpp"


TESTED_TARGETS(Char)
class CharTest final : public el::UnitTest {
public:
    void testDisplayWidthUsesUnicodeCellWidth() {
        const auto asciiChar = term::Char{"A", term::Color{}};
        const auto wideChar = term::Char{U'界', term::Color{}};
        const auto combiningChar = term::Char{"e\xCC\x81", term::Color{}};

        REQUIRE_EQUAL(asciiChar.displayWidth(), 1);
        REQUIRE_EQUAL(wideChar.displayWidth(), 2);
        REQUIRE_EQUAL(combiningChar.displayWidth(), 1);
    }

    void testConstructorsDecodeUtf8Utf32AndCodePoints() {
        const auto fromUtf8 = term::Char{"e\xCC\x81"};
        const auto fromUtf32Text = term::Char{U"e\u0301"};
        constexpr auto fromCodePoint = term::Char{U'★'};

        REQUIRE_EQUAL(fromUtf8.codePointCount(), std::size_t{2});
        REQUIRE_EQUAL(fromUtf32Text.codePointCount(), std::size_t{2});
        REQUIRE_EQUAL(fromCodePoint.codePointCount(), std::size_t{1});

        REQUIRE_EQUAL(fromUtf8.codePoints(), (std::array<char32_t, 3>{U'e', U'\u0301', 0}));
        REQUIRE_EQUAL(fromUtf32Text.codePoints(), (std::array<char32_t, 3>{U'e', U'\u0301', 0}));
        REQUIRE_EQUAL(fromCodePoint.codePoints(), (std::array<char32_t, 3>{U'★', 0, 0}));
        REQUIRE_EQUAL(fromUtf8.mainCodePoint(), U'e');
        REQUIRE_EQUAL(fromCodePoint.mainCodePoint(), U'★');
    }

    void testAppendToAndByteCountEncodeUtf8() {
        const auto character = term::Char{U"e\u0301", term::Color{term::fg::BrightWhite, term::bg::Blue}};
        auto buffer = std::string{"prefix:"};

        character.appendTo(buffer);

        REQUIRE_EQUAL(character.byteCount(), std::size_t{3});
        REQUIRE_EQUAL(character.charStr(), std::string{"e\xCC\x81"});
        REQUIRE_EQUAL(buffer, std::string{"prefix:e\xCC\x81"});
    }

    void testWithCombiningAppendsZeroWidthCodePoints() {
        const auto combined = term::Char{U'e'}.withCombining(U'\u0301');

        REQUIRE_EQUAL(combined.codePointCount(), std::size_t{2});
        REQUIRE_EQUAL(combined.codePoints(), (std::array<char32_t, 3>{U'e', U'\u0301', 0}));
        REQUIRE_EQUAL(combined.charStr(), std::string{"e\xCC\x81"});
    }

    void testWithCombiningRejectsInvalidCodePoints() {
        REQUIRE_THROWS_AS(std::invalid_argument, term::Char{}.withCombining(U'\u0301'));
        REQUIRE_THROWS_AS(std::invalid_argument, term::Char{U'e'}.withCombining(U'x'));
        REQUIRE_THROWS_AS(std::invalid_argument, term::Char{U'e'}.withCombining(U'\n'));
        REQUIRE_THROWS_AS(std::invalid_argument, term::Char{U"e\u0301\u0302"}.withCombining(U'\u0303'));
    }

    void testEqualityComparesCodePointsAndColors() {
        const auto left = term::Char{U'★', term::Color{term::fg::Yellow, term::bg::Blue}};
        const auto equal = term::Char{"★", term::Color{term::fg::Yellow, term::bg::Blue}};
        const auto differentColor = term::Char{U'★', term::Color{term::fg::Yellow, term::bg::Black}};
        const auto differentCodePoint = term::Char{U'☆', term::Color{term::fg::Yellow, term::bg::Blue}};

        REQUIRE(left == equal);
        REQUIRE_FALSE(left != equal);
        REQUIRE_FALSE(left == differentColor);
        REQUIRE_FALSE(left == differentCodePoint);
        REQUIRE(differentColor != left);
        REQUIRE(differentCodePoint != left);
    }

    void testWithColorOverlayPreservesInheritedComponents() {
        const auto base = term::Char{"X", term::Color{term::fg::Green, term::bg::Blue}};

        const auto changedForeground =
            base.withColorOverlay(term::Color{term::fg::BrightWhite, term::bg::Inherited});
        REQUIRE_EQUAL(changedForeground.color(), term::Color(term::fg::BrightWhite, term::bg::Blue));

        const auto changedBackground = base.withColorOverlay(term::Color{term::fg::Inherited, term::bg::Black});
        REQUIRE_EQUAL(changedBackground.color(), term::Color(term::fg::Green, term::bg::Black));
    }

    void testWithColorOverlayAppliesDefaultAsExplicitReset() {
        const auto base = term::Char{"X", term::Color{term::fg::Green, term::bg::Blue}};

        const auto resetForeground = base.withColorOverlay(term::Color{term::fg::Default, term::bg::Inherited});
        REQUIRE_EQUAL(resetForeground.color(), term::Color(term::fg::Default, term::bg::Blue));

        const auto resetBackground = base.withColorOverlay(term::Color{term::fg::Inherited, term::bg::Default});
        REQUIRE_EQUAL(resetBackground.color(), term::Color(term::fg::Green, term::bg::Default));
    }

    void testWithColorReplacedAndWithBaseColorUseExpectedRules() {
        const auto base = term::Char{U'X', term::Color{term::fg::Green, term::bg::Blue}};
        const auto overlaid =
            term::Char{U'X', term::Color{term::fg::Inherited, term::bg::Yellow}}
                .withBaseColor(term::Color{term::fg::BrightWhite, term::bg::Blue});

        REQUIRE_EQUAL(
            base.withColorReplaced(term::Color{term::fg::BrightWhite, term::bg::Black}).color(),
            term::Color(term::fg::BrightWhite, term::bg::Black));
        REQUIRE_EQUAL(
            base.withBaseColor(term::Color{term::fg::Inherited, term::bg::BrightBlack}).color(),
            term::Color(term::fg::Green, term::bg::Blue));
        REQUIRE_EQUAL(overlaid.color(), term::Color(term::fg::BrightWhite, term::bg::Yellow));
    }

    void testSpacingRecognizesSingleWhitespaceCodePoints() {
        REQUIRE(term::Char{U' '}.isSpacing());
        REQUIRE(term::Char{U'\t'}.isSpacing());
        REQUIRE_FALSE(term::Char{U"e\u0301"}.isSpacing());
        REQUIRE_FALSE(term::Char{}.isSpacing());
    }

    void testRenderedEqualsTreatsInheritedColorsLikeDefaults() {
        const auto inherited = term::Char{"X", term::Color{term::fg::Inherited, term::bg::Inherited}};
        const auto defaults = term::Char{"X", term::Color{term::fg::Default, term::bg::Default}};

        REQUIRE(inherited.renderedEquals(defaults));
        REQUIRE(defaults.renderedEquals(inherited));
    }

    void testRenderedEqualsCanIgnoreColorDifferences() {
        const auto left = term::Char{"X", term::Color{term::fg::Red, term::bg::Black}};
        const auto right = term::Char{"X", term::Color{term::fg::Blue, term::bg::Yellow}};

        REQUIRE_FALSE(left.renderedEquals(right));
        REQUIRE(left.renderedEquals(right, false));
    }

    void testRejectsTooManyCodePoints() {
        REQUIRE_THROWS_AS(std::invalid_argument, term::Char{U"ab\u0301\u0302"});
        REQUIRE_THROWS_AS(std::invalid_argument, term::Char{"a\xCC\x81\xCC\x82\xCC\x83"});
    }
};
