// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include "erbsland/cterm/CharCombinationStyle.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>

TESTED_TARGETS(CharCombinationStyle)
class CharCombinationStyleTest final : public el::UnitTest {
public:
    void testMatrixCombinationStyleUsesIndexedLookup() {
        const auto style = MatrixCombinationStyle{
            U" xy",
            std::string_view{
                "\x00\x01\x02"
                "\x01\x01\x02"
                "\x02\x01\x02",
                9,
            },
        };

        REQUIRE_EQUAL(style.combine(Char{U' '}, Char{U'x'}), U'x');
        REQUIRE_EQUAL(style.combine(Char{U'x'}, Char{U'y'}), U'y');
        REQUIRE_EQUAL(style.combine(Char{U'?'}, Char{U'y'}), U'y');
    }

    void testOverwriteAndColorOverlayStylesUseExpectedColorRules() {
        const auto current = Char{U'A', fg::Green, bg::Blue};
        const auto overlay = Char{U'B', fg::Inherited, bg::Yellow};

        const auto overwritten = CharCombinationStyle::overwrite()->combine(current, overlay);
        REQUIRE_EQUAL(overwritten, U'B');
        REQUIRE_EQUAL(overwritten.color(), Color(fg::Inherited, bg::Yellow));

        const auto colorOverlaid = CharCombinationStyle::colorOverlay()->combine(current, overlay);
        REQUIRE_EQUAL(colorOverlaid, U'B');
        REQUIRE_EQUAL(colorOverlaid.color(), Color(fg::Green, bg::Yellow));
    }

    void testSimpleCombinationStyleUsesConfiguredMapAndColorOverlay() {
        auto style = SimpleCharCombinationStyle{};
        style.add("a", "b", "c");

        auto attributes = CharAttributes{};
        attributes.setUnderline(true);
        const auto result = style.combine(
            Char{U'a', fg::Green, bg::Blue}, Char{U'b', Color{fg::BrightWhite, bg::Inherited}, attributes});

        REQUIRE_EQUAL(result, U'c');
        REQUIRE_EQUAL(result.color(), Color(fg::BrightWhite, bg::Blue));
        REQUIRE(result.attributes().isUnderline());
    }

    void testMatrixCombinationStyleRejectsInvalidDefinitions() {
        REQUIRE_THROWS_AS(std::invalid_argument, MatrixCombinationStyle(U"ab", std::string_view{"\x00\x01", 2}));
        REQUIRE_THROWS_AS(std::invalid_argument, MatrixCombinationStyle(std::u32string(256, U'a'), ""));
    }

    void testCommonBoxFrameCombinesExactMatches() {
        const auto style = CharCombinationStyle::commonBoxFrame();

        REQUIRE_EQUAL(style->combine(Char{U'┌'}, Char{U'┛'}), U'╃');
        REQUIRE_EQUAL(style->combine(Char{U'┛'}, Char{U'┌'}), U'╃');
        REQUIRE_EQUAL(style->combine(Char{U'╴'}, Char{U'╶'}), U'─');
        REQUIRE_EQUAL(style->combine(Char{U'│'}, Char{U'═'}), U'╪');
        REQUIRE_EQUAL(style->combine(Char{U'║'}, Char{U'─'}), U'╫');
        REQUIRE_EQUAL(style->combine(Char{U'╱'}, Char{U'╲'}), U'╳');
    }

    void testCommonBoxFrameFallsBackToOverlayForUnsupportedBlend() {
        const auto style = CharCombinationStyle::commonBoxFrame();

        REQUIRE_EQUAL(style->combine(Char{U'╱'}, Char{U'┌'}), U'┌');
    }

    void testCommonBoxFrameOverwritesCenterOnlyCharactersWithLines() {
        const auto style = CharCombinationStyle::commonBoxFrame();

        REQUIRE_EQUAL(style->combine(Char{U'▫'}, Char{U'┌'}), U'┌');
        REQUIRE_EQUAL(style->combine(Char{U'┌'}, Char{U'▫'}), U'┌');
        REQUIRE_EQUAL(style->combine(Char{U'▪'}, Char{U'╱'}), U'╱');
    }

    void testCommonBoxFrameOverlaysColors() {
        const auto style = CharCombinationStyle::commonBoxFrame();

        const auto current = Char{U'│', fg::Green, bg::Blue};
        const auto overlay = Char{U'═', fg::BrightWhite, bg::Inherited};
        const auto result = style->combine(current, overlay);

        REQUIRE_EQUAL(result, U'╪');
        REQUIRE_EQUAL(result.color(), Color(fg::BrightWhite, bg::Blue));
    }

    void testCombinationStylesResolveCharacterAttributesLikeColors() {
        const auto style = SimpleCharCombinationStyle{};
        auto currentAttributes = CharAttributes{};
        currentAttributes.setBold(true);
        auto overlayAttributes = CharAttributes{};
        overlayAttributes.setBold(false);
        overlayAttributes.setItalic(true);

        const auto result = style.combine(
            Char{U'a', Color{fg::Green, bg::Blue}, currentAttributes},
            Char{U'b', Color{fg::BrightWhite, bg::Inherited}, overlayAttributes});

        REQUIRE_FALSE(result.attributes().isBold());
        REQUIRE(result.attributes().isItalic());
    }
};
