// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>

#include "TestHelper.hpp"
#include "erbsland/cterm/CharCombinationStyle.hpp"


TESTED_TARGETS(CharCombinationStyle)
class CharCombinationStyleTest final : public el::UnitTest {
public:
    void testMatrixCombinationStyleUsesIndexedLookup() {
        const auto style = term::MatrixCombinationStyle{
            U" xy",
            std::string_view{
                "\x00\x01\x02"
                "\x01\x01\x02"
                "\x02\x01\x02",
                9,
            },
        };

        REQUIRE_EQUAL(style.combine(term::Char{" "}, term::Char{"x"}).charStr(), "x");
        REQUIRE_EQUAL(style.combine(term::Char{"x"}, term::Char{"y"}).charStr(), "y");
        REQUIRE_EQUAL(style.combine(term::Char{"?"}, term::Char{"y"}).charStr(), "y");
    }

    void testOverwriteAndColorOverlayStylesUseExpectedColorRules() {
        const auto current = term::Char{"A", term::Color{term::fg::Green, term::bg::Blue}};
        const auto overlay = term::Char{"B", term::Color{term::fg::Inherited, term::bg::Yellow}};

        const auto overwritten = term::CharCombinationStyle::overwrite()->combine(current, overlay);
        REQUIRE_EQUAL(overwritten.charStr(), "B");
        REQUIRE_EQUAL(overwritten.color(), term::Color(term::fg::Inherited, term::bg::Yellow));

        const auto colorOverlaid = term::CharCombinationStyle::colorOverlay()->combine(current, overlay);
        REQUIRE_EQUAL(colorOverlaid.charStr(), "B");
        REQUIRE_EQUAL(colorOverlaid.color(), term::Color(term::fg::Green, term::bg::Yellow));
    }

    void testSimpleCombinationStyleUsesConfiguredMapAndColorOverlay() {
        auto style = term::SimpleCharCombinationStyle{};
        style.add("a", "b", "c");

        const auto result = style.combine(
            term::Char{"a", term::Color{term::fg::Green, term::bg::Blue}},
            term::Char{"b", term::Color{term::fg::BrightWhite, term::bg::Inherited}});

        REQUIRE_EQUAL(result.charStr(), "c");
        REQUIRE_EQUAL(result.color(), term::Color(term::fg::BrightWhite, term::bg::Blue));
    }

    void testMatrixCombinationStyleRejectsInvalidDefinitions() {
        REQUIRE_THROWS_AS(std::invalid_argument, term::MatrixCombinationStyle(U"ab", std::string_view{"\x00\x01", 2}));
        REQUIRE_THROWS_AS(std::invalid_argument, term::MatrixCombinationStyle(std::u32string(256, U'a'), ""));
    }

    void testCommonBoxFrameCombinesExactMatches() {
        const auto style = term::CharCombinationStyle::commonBoxFrame();

        REQUIRE_EQUAL(style->combine(term::Char{"┌"}, term::Char{"┛"}).charStr(), "╃");
        REQUIRE_EQUAL(style->combine(term::Char{"┛"}, term::Char{"┌"}).charStr(), "╃");
        REQUIRE_EQUAL(style->combine(term::Char{"╴"}, term::Char{"╶"}).charStr(), "─");
        REQUIRE_EQUAL(style->combine(term::Char{"│"}, term::Char{"═"}).charStr(), "╪");
        REQUIRE_EQUAL(style->combine(term::Char{"║"}, term::Char{"─"}).charStr(), "╫");
        REQUIRE_EQUAL(style->combine(term::Char{"╱"}, term::Char{"╲"}).charStr(), "╳");
    }

    void testCommonBoxFrameFallsBackToOverlayForUnsupportedBlend() {
        const auto style = term::CharCombinationStyle::commonBoxFrame();

        REQUIRE_EQUAL(style->combine(term::Char{"╱"}, term::Char{"┌"}).charStr(), "┌");
    }

    void testCommonBoxFrameOverwritesCenterOnlyCharactersWithLines() {
        const auto style = term::CharCombinationStyle::commonBoxFrame();

        REQUIRE_EQUAL(style->combine(term::Char{"▫"}, term::Char{"┌"}).charStr(), "┌");
        REQUIRE_EQUAL(style->combine(term::Char{"┌"}, term::Char{"▫"}).charStr(), "┌");
        REQUIRE_EQUAL(style->combine(term::Char{"▪"}, term::Char{"╱"}).charStr(), "╱");
    }

    void testCommonBoxFrameOverlaysColors() {
        const auto style = term::CharCombinationStyle::commonBoxFrame();

        const auto current = term::Char{"│", term::Color{term::fg::Green, term::bg::Blue}};
        const auto overlay = term::Char{"═", term::Color{term::fg::BrightWhite, term::bg::Inherited}};
        const auto result = style->combine(current, overlay);

        REQUIRE_EQUAL(result.charStr(), "╪");
        REQUIRE_EQUAL(result.color(), term::Color(term::fg::BrightWhite, term::bg::Blue));
    }
};
