// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/cterm/FrameStyle.hpp>
#include <erbsland/cterm/Tile9Style.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <array>
#include <stdexcept>

TESTED_TARGETS(Tile9Style)
class Tile9StyleTest final : public el::UnitTest {
public:
    void testStringConstructorRepeatsTheNineBaseTilesAcrossRectangles() {
        const auto style = Tile9Style{"ABCDEFGHI"};

        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{0, 0}), U'A');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{1, 0}), U'B');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{3, 0}), U'C');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{0, 1}), U'D');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{2, 1}), U'E');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{3, 1}), U'F');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{0, 2}), U'G');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{2, 2}), U'H');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{3, 2}), U'I');
    }

    void testDegenerateRectanglesUseFallbackTilesWhenOnlyNineTilesAreDefined() {
        const auto style = Tile9Style{"ABCDEFGHI"};

        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 1}, Position{0, 0}), U'A');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 1}, Position{1, 0}), U'B');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 1}, Position{3, 0}), U'C');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 1, 4}, Position{0, 0}), U'A');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 1, 4}, Position{0, 1}), U'D');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 1, 3}, Position{0, 2}), U'G');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 1, 1}, Position{0, 0}), U'A');
    }

    void testSixteenTilesProvideExplicitDegenerateVariants() {
        const auto style = Tile9Style{"ABCDEFGHIJKLMNOP"};

        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 1}, Position{0, 0}), U'J');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 1}, Position{1, 0}), U'K');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 1}, Position{3, 0}), U'L');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 1, 4}, Position{0, 0}), U'M');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 1, 1}, Position{0, 0}), U'P');
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 1, 4}, Position{0, 3}), U'O');
    }

    void testNamedElementsExposeAllSixteenTiles() {
        const auto style = Tile9Style{"ABCDEFGHIJKLMNOP"};

        REQUIRE_EQUAL(style.block(Tile9Style::Element::NorthWest), U'A');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::North), U'B');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::NorthEast), U'C');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::West), U'D');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::Center), U'E');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::East), U'F');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::SouthWest), U'G');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::South), U'H');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::SouthEast), U'I');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::HorizontalWest), U'J');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::HorizontalCenter), U'K');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::HorizontalEast), U'L');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::VerticalNorth), U'M');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::VerticalCenter), U'N');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::VerticalSouth), U'O');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::Single), U'P');
    }

    void testNamedExtendedElementsFallBackForNineTileStyles() {
        const auto style = Tile9Style{"ABCDEFGHI"};

        REQUIRE_EQUAL(style.block(Tile9Style::Element::HorizontalWest), U'A');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::HorizontalCenter), U'B');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::HorizontalEast), U'C');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::VerticalNorth), U'A');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::VerticalCenter), U'D');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::VerticalSouth), U'G');
        REQUIRE_EQUAL(style.block(Tile9Style::Element::Single), U'A');
    }

    void testStringConstructorRejectsInvalidTileCounts() {
        REQUIRE_THROWS_AS(std::invalid_argument, Tile9Style{"12345678"});
        REQUIRE_THROWS_AS(std::invalid_argument, Tile9Style{"1234567890"});
        REQUIRE_THROWS_AS(std::invalid_argument, Tile9Style{"123456789012345"});
        REQUIRE_THROWS_AS(std::invalid_argument, Tile9Style{"12345678901234567"});
    }

    void testCreateFactoryBuildsSharedStyles() {
        const auto utf8Style = Tile9Style::create("ABCDEFGHI");
        const auto utf32Style = Tile9Style::create(U"ABCDEFGHIJKLMNOP");

        REQUIRE(utf8Style != nullptr);
        REQUIRE(utf32Style != nullptr);
        REQUIRE_EQUAL(utf8Style->block(Rectangle{0, 0, 3, 3}, Position{1, 1}), U'E');
        REQUIRE_EQUAL(utf32Style->block(Rectangle{0, 0, 1, 1}, Position{0, 0}), U'P');
    }

    void testArrayConstructorsKeepConfiguredTiles() {
        const auto tiles9 = std::array<Char, 9>{
            Char{U'0'},
            Char{U'1'},
            Char{U'2'},
            Char{U'3'},
            Char{U'4'},
            Char{U'5'},
            Char{U'6'},
            Char{U'7'},
            Char{U'8'},
        };
        const auto tiles16 = std::array<Char, 16>{
            Char{U'0'},
            Char{U'1'},
            Char{U'2'},
            Char{U'3'},
            Char{U'4'},
            Char{U'5'},
            Char{U'6'},
            Char{U'7'},
            Char{U'8'},
            Char{U'9'},
            Char{"10"},
            Char{"11"},
            Char{"12"},
            Char{"13"},
            Char{"14"},
            Char{"15"},
        };
        const auto style9 = Tile9Style{tiles9};
        const auto style16 = Tile9Style{tiles16};

        REQUIRE_EQUAL(style9.block(Rectangle{0, 0, 3, 3}, Position{2, 2}), U'8');
        REQUIRE_EQUAL(style16.block(Rectangle{0, 0, 1, 1}, Position{0, 0}).charStr(), tiles16[15].charStr());
    }

    void testPredefinedStylesExposeExpectedTiles() {
        REQUIRE_EQUAL(
            Tile9Style::outerHalfBlockFrame()->block(Rectangle{0, 0, 4, 3}, Position{0, 0}).charStr(),
            std::string{"▛"});
        REQUIRE_EQUAL(
            Tile9Style::outerHalfBlockFrame()->block(Rectangle{0, 0, 1, 1}, Position{0, 0}).charStr(),
            std::string{"█"});
        REQUIRE_EQUAL(
            Tile9Style::innerHalfBlockFrame()->block(Rectangle{0, 0, 4, 3}, Position{1, 0}).charStr(),
            std::string{"▄"});
        REQUIRE_EQUAL(
            Tile9Style::innerHalfBlockFrame()->block(Rectangle{0, 0, 1, 2}, Position{0, 1}).charStr(),
            std::string{"█"});
    }

    void testForStyleMapsKnownAndUnknownEnumValues() {
        REQUIRE(Tile9Style::forStyle(FrameStyle::OuterHalfBlock) != nullptr);
        REQUIRE(Tile9Style::forStyle(FrameStyle::InnerHalfBlock) != nullptr);
        REQUIRE(Tile9Style::forStyle(FrameStyle::None) == nullptr);
        REQUIRE(Tile9Style::forStyle(FrameStyle::Light) == nullptr);
        REQUIRE(Tile9Style::forStyle(static_cast<FrameStyle>(255)) == nullptr);
    }
};
