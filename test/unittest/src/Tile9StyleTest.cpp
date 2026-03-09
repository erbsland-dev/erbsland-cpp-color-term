// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/cterm/FrameStyle.hpp>
#include <erbsland/cterm/Tile9Style.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <array>
#include <stdexcept>

#include "TestHelper.hpp"


TESTED_TARGETS(Tile9Style)
class Tile9StyleTest final : public el::UnitTest {
public:
    void testStringConstructorRepeatsTheNineBaseTilesAcrossRectangles() {
        const auto style = term::Tile9Style{"ABCDEFGHI"};

        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{0, 0}).charStr(), std::string{"A"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{1, 0}).charStr(), std::string{"B"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{3, 0}).charStr(), std::string{"C"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{0, 1}).charStr(), std::string{"D"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{2, 1}).charStr(), std::string{"E"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{3, 1}).charStr(), std::string{"F"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{0, 2}).charStr(), std::string{"G"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{2, 2}).charStr(), std::string{"H"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 3}, Position{3, 2}).charStr(), std::string{"I"});
    }

    void testDegenerateRectanglesUseFallbackTilesWhenOnlyNineTilesAreDefined() {
        const auto style = term::Tile9Style{"ABCDEFGHI"};

        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 1}, Position{0, 0}).charStr(), std::string{"A"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 1}, Position{1, 0}).charStr(), std::string{"B"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 1}, Position{3, 0}).charStr(), std::string{"C"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 1, 4}, Position{0, 0}).charStr(), std::string{"A"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 1, 4}, Position{0, 1}).charStr(), std::string{"D"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 1, 3}, Position{0, 2}).charStr(), std::string{"G"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 1, 1}, Position{0, 0}).charStr(), std::string{"A"});
    }

    void testSixteenTilesProvideExplicitDegenerateVariants() {
        const auto style = term::Tile9Style{"ABCDEFGHIJKLMNOP"};

        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 1}, Position{0, 0}).charStr(), std::string{"J"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 1}, Position{1, 0}).charStr(), std::string{"K"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 4, 1}, Position{3, 0}).charStr(), std::string{"L"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 1, 4}, Position{0, 0}).charStr(), std::string{"M"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 1, 1}, Position{0, 0}).charStr(), std::string{"P"});
        REQUIRE_EQUAL(style.block(Rectangle{0, 0, 1, 4}, Position{0, 3}).charStr(), std::string{"O"});
    }

    void testStringConstructorRejectsInvalidTileCounts() {
        REQUIRE_THROWS_AS(std::invalid_argument, term::Tile9Style{"12345678"});
        REQUIRE_THROWS_AS(std::invalid_argument, term::Tile9Style{"1234567890"});
        REQUIRE_THROWS_AS(std::invalid_argument, term::Tile9Style{"123456789012345"});
        REQUIRE_THROWS_AS(std::invalid_argument, term::Tile9Style{"12345678901234567"});
    }

    void testCreateFactoryBuildsSharedStyles() {
        const auto utf8Style = term::Tile9Style::create("ABCDEFGHI");
        const auto utf32Style = term::Tile9Style::create(U"ABCDEFGHIJKLMNOP");

        REQUIRE(utf8Style != nullptr);
        REQUIRE(utf32Style != nullptr);
        REQUIRE_EQUAL(utf8Style->block(Rectangle{0, 0, 3, 3}, Position{1, 1}).charStr(), std::string{"E"});
        REQUIRE_EQUAL(utf32Style->block(Rectangle{0, 0, 1, 1}, Position{0, 0}).charStr(), std::string{"P"});
    }

    void testArrayConstructorsKeepConfiguredTiles() {
        const auto tiles9 = std::array<term::Char, 9>{
            term::Char{"0"},
            term::Char{"1"},
            term::Char{"2"},
            term::Char{"3"},
            term::Char{"4"},
            term::Char{"5"},
            term::Char{"6"},
            term::Char{"7"},
            term::Char{"8"},
        };
        const auto tiles16 = std::array<term::Char, 16>{
            term::Char{"0"},
            term::Char{"1"},
            term::Char{"2"},
            term::Char{"3"},
            term::Char{"4"},
            term::Char{"5"},
            term::Char{"6"},
            term::Char{"7"},
            term::Char{"8"},
            term::Char{"9"},
            term::Char{"10"},
            term::Char{"11"},
            term::Char{"12"},
            term::Char{"13"},
            term::Char{"14"},
            term::Char{"15"},
        };
        const auto style9 = term::Tile9Style{tiles9};
        const auto style16 = term::Tile9Style{tiles16};

        REQUIRE_EQUAL(style9.block(Rectangle{0, 0, 3, 3}, Position{2, 2}).charStr(), std::string{"8"});
        REQUIRE_EQUAL(style16.block(Rectangle{0, 0, 1, 1}, Position{0, 0}).charStr(), std::string{"15"});
    }

    void testPredefinedStylesExposeExpectedTiles() {
        REQUIRE_EQUAL(
            term::Tile9Style::outerHalfBlockFrame()->block(Rectangle{0, 0, 4, 3}, Position{0, 0}).charStr(),
            std::string{"▛"});
        REQUIRE_EQUAL(
            term::Tile9Style::outerHalfBlockFrame()->block(Rectangle{0, 0, 1, 1}, Position{0, 0}).charStr(),
            std::string{"█"});
        REQUIRE_EQUAL(
            term::Tile9Style::innerHalfBlockFrame()->block(Rectangle{0, 0, 4, 3}, Position{1, 0}).charStr(),
            std::string{"▄"});
        REQUIRE_EQUAL(
            term::Tile9Style::innerHalfBlockFrame()->block(Rectangle{0, 0, 1, 2}, Position{0, 1}).charStr(),
            std::string{"█"});
    }

    void testForStyleMapsKnownAndUnknownEnumValues() {
        REQUIRE(term::Tile9Style::forStyle(term::FrameStyle::OuterHalfBlock) != nullptr);
        REQUIRE(term::Tile9Style::forStyle(term::FrameStyle::InnerHalfBlock) != nullptr);
        REQUIRE(term::Tile9Style::forStyle(term::FrameStyle::Light) == nullptr);
        REQUIRE(term::Tile9Style::forStyle(static_cast<term::FrameStyle>(255)) == nullptr);
    }
};
