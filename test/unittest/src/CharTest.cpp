// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <functional>

TESTED_TARGETS(Char)
class CharTest final : public el::UnitTest {
public:
    void testDisplayWidthUsesUnicodeCellWidth() {
        const auto asciiChar = Char{"A", Color{}};
        const auto wideChar = Char{U'界', Color{}};
        const auto combiningChar = Char{"e\xCC\x81", Color{}};

        REQUIRE_EQUAL(asciiChar.displayWidth(), 1);
        REQUIRE_EQUAL(wideChar.displayWidth(), 2);
        REQUIRE_EQUAL(combiningChar.displayWidth(), 1);
    }

    void testConstructorsDecodeUtf8Utf32AndCodePoints() {
        const auto fromUtf8 = Char{"e\xCC\x81"};
        const auto fromUtf32Text = Char{U"e\u0301"};
        constexpr auto fromCodePoint = Char{U'★'};

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
        const auto character = Char{U"e\u0301", fg::BrightWhite, bg::Blue};
        auto buffer = std::string{"prefix:"};

        character.appendTo(buffer);

        REQUIRE_EQUAL(character.byteCount(), std::size_t{3});
        REQUIRE_EQUAL(character.charStr(), std::string{"e\xCC\x81"});
        REQUIRE_EQUAL(buffer, std::string{"prefix:e\xCC\x81"});
    }

    void testWithCombiningAppendsZeroWidthCodePoints() {
        const auto combined = Char{U'e'}.withCombining(U'\u0301');

        REQUIRE_EQUAL(combined.codePointCount(), std::size_t{2});
        REQUIRE_EQUAL(combined.codePoints(), (std::array<char32_t, 3>{U'e', U'\u0301', 0}));
        REQUIRE_EQUAL(combined.charStr(), std::string{"e\xCC\x81"});
    }

    void testWithCombiningRejectsInvalidCodePoints() {
        REQUIRE_THROWS_AS(std::invalid_argument, Char{}.withCombining(U'\u0301'));
        REQUIRE_THROWS_AS(std::invalid_argument, Char{U'e'}.withCombining(U'x'));
        REQUIRE_THROWS_AS(std::invalid_argument, Char{U'e'}.withCombining(U'\n'));
        REQUIRE_THROWS_AS(std::invalid_argument, Char{U"e\u0301\u0302"}.withCombining(U'\u0303'));
    }

    void testEqualityComparesCodePointsAndColors() {
        auto attributes = CharAttributes{};
        attributes.setBold(true);
        const auto left = Char{U'★', Color{fg::Yellow, bg::Blue}, attributes};
        const auto equal = Char{U'★', Color{fg::Yellow, bg::Blue}, attributes};
        const auto differentColor = Char{U'★', fg::Yellow, bg::Black};
        auto differentAttributes = attributes;
        differentAttributes.setUnderline(true);
        const auto differentAttributeChar = Char{U'★', Color{fg::Yellow, bg::Blue}, differentAttributes};
        const auto differentCodePoint = Char{U'☆', fg::Yellow, bg::Blue};

        REQUIRE(left == equal);
        REQUIRE_FALSE(left != equal);
        REQUIRE_FALSE(left == differentColor);
        REQUIRE_FALSE(left == differentAttributeChar);
        REQUIRE_FALSE(left == differentCodePoint);
        REQUIRE(differentColor != left);
        REQUIRE(differentCodePoint != left);
    }

    void testEqualityIgnoresTheDisplayWidthCache() {
        auto attributes = CharAttributes{};
        attributes.setItalic(true);
        auto left = Char{U'界', Color{fg::Yellow, bg::Blue}, attributes};
        auto right = Char{U'界', Color{fg::Yellow, bg::Blue}, attributes};

        REQUIRE_EQUAL(left.displayWidth(), 2);
        REQUIRE(left == right);

        REQUIRE_EQUAL(right.displayWidth(), 2);
        REQUIRE(left == right);
    }

    void testSingleCodePointComparisonsIgnoreColorButRejectMultiCodePointCharacters() {
        const auto colored = Char{U'★', fg::Yellow, bg::Blue};
        const auto combined = Char{U"e\u0301"};

        REQUIRE(colored == U'★');
        REQUIRE_FALSE(colored != U'★');
        REQUIRE(colored != U'☆');
        REQUIRE_FALSE(combined == U'e');
        REQUIRE(combined != U'e');
    }

    void testWithColorOverlayPreservesInheritedComponents() {
        const auto base = Char{U'X', fg::Green, bg::Blue};

        const auto changedForeground = base.withColorOverlay(Color{fg::BrightWhite, bg::Inherited});
        REQUIRE_EQUAL(changedForeground.color(), Color(fg::BrightWhite, bg::Blue));

        const auto changedBackground = base.withColorOverlay(Color{fg::Inherited, bg::Black});
        REQUIRE_EQUAL(changedBackground.color(), Color(fg::Green, bg::Black));
    }

    void testWithColorOverlayAppliesDefaultAsExplicitReset() {
        const auto base = Char{U'X', fg::Green, bg::Blue};

        const auto resetForeground = base.withColorOverlay(Color{fg::Default, bg::Inherited});
        REQUIRE_EQUAL(resetForeground.color(), Color(fg::Default, bg::Blue));

        const auto resetBackground = base.withColorOverlay(Color{fg::Inherited, bg::Default});
        REQUIRE_EQUAL(resetBackground.color(), Color(fg::Green, bg::Default));
    }

    void testWithColorReplacedAndWithBaseColorUseExpectedRules() {
        const auto base = Char{U'X', fg::Green, bg::Blue};
        const auto overlaid = Char{U'X', fg::Inherited, bg::Yellow}.withBaseColor(Color{fg::BrightWhite, bg::Blue});

        REQUIRE_EQUAL(
            base.withColorReplaced(Color{fg::BrightWhite, bg::Black}).color(), Color(fg::BrightWhite, bg::Black));
        REQUIRE_EQUAL(base.withBaseColor(Color{fg::Inherited, bg::BrightBlack}).color(), Color(fg::Green, bg::Blue));
        REQUIRE_EQUAL(overlaid.color(), Color(fg::BrightWhite, bg::Yellow));
    }

    void testConstructorsAndWithAttributesPreserveCharacterAttributes() {
        auto attributes = CharAttributes{};
        attributes.setBold(true);
        attributes.setUnderline(true);

        const auto coloredCharacter = Char{U'X', Color{fg::Green, bg::Blue}, attributes};
        REQUIRE(coloredCharacter.attributes().isBold());
        REQUIRE(coloredCharacter.attributes().isUnderline());

        auto replacedAttributes = CharAttributes::reset();
        replacedAttributes.setItalic(true);
        const auto updatedCharacter = coloredCharacter.withAttributes(replacedAttributes);
        REQUIRE_FALSE(updatedCharacter.attributes().isBold());
        REQUIRE(updatedCharacter.attributes().isItalic());
    }

    void testStyleConstructorAccessorAndSetterExposeTheCombinedStyle() {
        auto attributes = CharAttributes{};
        attributes.setItalic(true);
        auto style = CharStyle{Color{fg::Green, bg::Blue}, attributes};

        auto character = Char{U'X', style};

        REQUIRE_EQUAL(character.style(), style);
        REQUIRE_EQUAL(character.color(), Color(fg::Green, bg::Blue));
        REQUIRE(character.attributes().isItalic());

        auto replacementAttributes = CharAttributes{};
        replacementAttributes.setBold(true);
        style = CharStyle{Color{fg::BrightWhite, bg::Black}, replacementAttributes};
        character.setStyle(style);

        REQUIRE_EQUAL(character.style(), style);
        REQUIRE(character.attributes().isBold());
        REQUIRE_FALSE(character.attributes().isItalic());
    }

    void testWithOverlayAndWithBaseResolveColorAndAttributesTogether() {
        auto currentAttributes = CharAttributes{};
        currentAttributes.setBold(true);
        auto overlayAttributes = CharAttributes{};
        overlayAttributes.setBold(false);
        overlayAttributes.setUnderline(true);
        auto baseAttributes = CharAttributes{};
        baseAttributes.setItalic(true);
        const auto character = Char{U'X', Color{fg::Green, bg::Blue}, currentAttributes};

        const auto overlaid = character.withOverlay(Color{fg::Inherited, bg::Black}, overlayAttributes);
        REQUIRE_EQUAL(overlaid.color(), Color(fg::Green, bg::Black));
        REQUIRE_FALSE(overlaid.attributes().isBold());
        REQUIRE(overlaid.attributes().isUnderline());

        const auto based = character.withBase(Color{fg::BrightWhite, bg::Default}, baseAttributes);
        REQUIRE_EQUAL(based.color(), Color(fg::Green, bg::Blue));
        REQUIRE(based.attributes().isBold());
        REQUIRE(based.attributes().isItalic());

        const auto fromBaseCharacter = Char{U'Y', Color{fg::BrightWhite, bg::Default}, baseAttributes};
        REQUIRE_EQUAL(character.withBase(fromBaseCharacter), based);
    }

    void testSpacingRecognizesSingleWhitespaceCodePoints() {
        REQUIRE(Char{U' '}.isSpacing());
        REQUIRE(Char{U'\t'}.isSpacing());
        REQUIRE_FALSE(Char{U"e\u0301"}.isSpacing());
        REQUIRE_FALSE(Char{}.isSpacing());
    }

    void testIsEmptySpaceAndIsOneOfHelpersUseSingleCodePoints() {
        const auto empty = Char{};
        const auto space = Char::space();
        const auto symbol = Char{U'X', fg::Red, bg::Black};
        const auto combined = Char{U"e\u0301"};

        REQUIRE(empty.isEmpty());
        REQUIRE_FALSE(space.isEmpty());
        REQUIRE(space == U' ');
        REQUIRE_EQUAL(space.color(), Color{});
        REQUIRE(symbol.isOneOf(U"ABCX"));
        REQUIRE(symbol.isOneOf({U'A', U'X', U'Z'}));
        REQUIRE(symbol.isOneOf(U'A', U'X', U'Z'));
        REQUIRE_FALSE(symbol.isOneOf(U"ABC"));
        REQUIRE_FALSE(combined.isOneOf(U"e"));
    }

    void testRenderedEqualsTreatsInheritedColorsLikeDefaults() {
        const auto inherited = Char{U'X', fg::Inherited, bg::Inherited};
        const auto defaults = Char{U'X', fg::Default, bg::Default};

        REQUIRE(inherited.renderedEquals(defaults));
        REQUIRE(defaults.renderedEquals(inherited));
    }

    void testRenderedEqualsCanIgnoreColorDifferences() {
        const auto left = Char{U'X', fg::Red, bg::Black};
        const auto right = Char{U'X', fg::Blue, bg::Yellow};

        REQUIRE_FALSE(left.renderedEquals(right));
        REQUIRE(left.renderedEquals(right, false));
    }

    void testRenderedEqualsTreatsInheritedAttributesLikeDisabled() {
        auto disabledAttributes = CharAttributes::reset();
        disabledAttributes.setBold(false);
        disabledAttributes.setUnderline(false);
        const auto inherited = Char{U'X', Color{}, CharAttributes{}};
        const auto disabled = Char{U'X', Color{}, disabledAttributes};

        REQUIRE(inherited.renderedEquals(disabled));
        REQUIRE(disabled.renderedEquals(inherited));

        auto boldAttributes = CharAttributes{};
        boldAttributes.setBold(true);
        const auto bold = Char{U'X', Color{}, boldAttributes};
        REQUIRE_FALSE(inherited.renderedEquals(bold));
        REQUIRE(inherited.renderedEquals(bold, true, false));
    }

    void testRejectsTooManyCodePoints() {
        REQUIRE_THROWS_AS(std::invalid_argument, Char{U"ab\u0301\u0302"});
        REQUIRE_THROWS_AS(std::invalid_argument, Char{"a\xCC\x81\xCC\x82\xCC\x83"});
    }

    void testHashMatchesStdHashAndReflectsTextAndColor() {
        auto attributes = CharAttributes{};
        attributes.setStrikethrough(true);
        const auto base = Char{U'★', Color{fg::Yellow, bg::Blue}, attributes};
        const auto differentText = Char{U'☆', fg::Yellow, bg::Blue};
        const auto differentColor = Char{U'★', fg::Yellow, bg::Black};
        auto differentAttributes = attributes;
        differentAttributes.setStrikethrough(false);
        const auto differentAttributeChar = Char{U'★', Color{fg::Yellow, bg::Blue}, differentAttributes};

        REQUIRE_EQUAL(base.hash(), std::hash<Char>{}(base));
        REQUIRE_NOT_EQUAL(base.hash(), differentText.hash());
        REQUIRE_NOT_EQUAL(base.hash(), differentColor.hash());
        REQUIRE_NOT_EQUAL(base.hash(), differentAttributeChar.hash());
    }
};
