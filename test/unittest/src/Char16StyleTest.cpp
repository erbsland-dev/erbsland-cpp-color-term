// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/cterm/Char16Style.hpp>
#include <erbsland/cterm/FrameStyle.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <array>
#include <stdexcept>

#include "TestHelper.hpp"


TESTED_TARGETS(Char16Style)
class Char16StyleTest final : public el::UnitTest {
public:
    void testStringConstructorSplitsExactlySixteenTerminalCharacters() {
        const auto style = term::Char16Style{"0123456789ABCDEF"};

        REQUIRE_EQUAL(style.block(0).charStr(), std::string{"0"});
        REQUIRE_EQUAL(style.block(10).charStr(), std::string{"A"});
        REQUIRE_EQUAL(style.block(15).charStr(), std::string{"F"});
    }

    void testStringConstructorRejectsInvalidTileCounts() {
        REQUIRE_THROWS_AS(std::invalid_argument, term::Char16Style{"123"});
        REQUIRE_THROWS_AS(std::invalid_argument, term::Char16Style{"0123456789ABCDEFG"});
    }

    void testBlockReturnsConfiguredTilesAndDefaultsForInvalidIndexes() {
        const auto style = term::Char16Style{std::array<term::Char, 16>{
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
        }};

        REQUIRE_EQUAL(style.block(0).charStr(), std::string{"0"});
        REQUIRE_EQUAL(style.block(5).charStr(), std::string{"5"});
        REQUIRE_EQUAL(style.block(15).charStr(), std::string{"15"});
        REQUIRE_EQUAL(style.block(16).charStr(), std::string{});
    }

    void testPredefinedStylesExposeExpectedTiles() {
        REQUIRE_EQUAL(term::Char16Style::lightFrame()->block(3).charStr(), std::string{"┌"});
        REQUIRE_EQUAL(term::Char16Style::lightDoubleDashFrame()->block(5).charStr(), std::string{"╌"});
        REQUIRE_EQUAL(term::Char16Style::lightTripleDashFrame()->block(5).charStr(), std::string{"┄"});
        REQUIRE_EQUAL(term::Char16Style::lightQuadrupleDashFrame()->block(5).charStr(), std::string{"┈"});
        REQUIRE_EQUAL(term::Char16Style::lightRoundedFrame()->block(3).charStr(), std::string{"╭"});
        REQUIRE_EQUAL(term::Char16Style::heavyFrame()->block(15).charStr(), std::string{"╋"});
        REQUIRE_EQUAL(term::Char16Style::heavyDoubleDashFrame()->block(10).charStr(), std::string{"╏"});
        REQUIRE_EQUAL(term::Char16Style::heavyTripleDashFrame()->block(5).charStr(), std::string{"┅"});
        REQUIRE_EQUAL(term::Char16Style::heavyQuadrupleDashFrame()->block(5).charStr(), std::string{"┉"});
        REQUIRE_EQUAL(term::Char16Style::doubleFrame()->block(10).charStr(), std::string{"║"});
        REQUIRE_EQUAL(term::Char16Style::fullBlockFrame()->block(15).charStr(), std::string{"█"});
        REQUIRE_EQUAL(term::Char16Style::fullBlockWithChamferFrame()->block(3).charStr(), std::string{"◢"});
    }

    void testForStyleMapsKnownAndUnknownEnumValues() {
        REQUIRE_EQUAL(term::Char16Style::forStyle(term::FrameStyle::Light)->block(5).charStr(), std::string{"─"});
        REQUIRE_EQUAL(term::Char16Style::forStyle(term::FrameStyle::LightDoubleDash)->block(5).charStr(), std::string{"╌"});
        REQUIRE_EQUAL(term::Char16Style::forStyle(term::FrameStyle::LightTripleDash)->block(5).charStr(), std::string{"┄"});
        REQUIRE_EQUAL(term::Char16Style::forStyle(term::FrameStyle::LightQuadrupleDash)->block(5).charStr(), std::string{"┈"});
        REQUIRE_EQUAL(term::Char16Style::forStyle(term::FrameStyle::Heavy)->block(5).charStr(), std::string{"━"});
        REQUIRE_EQUAL(term::Char16Style::forStyle(term::FrameStyle::HeavyDoubleDash)->block(5).charStr(), std::string{"╍"});
        REQUIRE_EQUAL(term::Char16Style::forStyle(term::FrameStyle::HeavyTripleDash)->block(5).charStr(), std::string{"┅"});
        REQUIRE_EQUAL(term::Char16Style::forStyle(term::FrameStyle::HeavyQuadrupleDash)->block(5).charStr(), std::string{"┉"});
        REQUIRE_EQUAL(term::Char16Style::forStyle(term::FrameStyle::Double)->block(5).charStr(), std::string{"═"});
        REQUIRE_EQUAL(term::Char16Style::forStyle(term::FrameStyle::FullBlock)->block(5).charStr(), std::string{"█"});
        REQUIRE_EQUAL(term::Char16Style::forStyle(term::FrameStyle::FullBlockWithChamfer)->block(3).charStr(), std::string{"◢"});
        REQUIRE(term::Char16Style::forStyle(term::FrameStyle::OuterHalfBlock) != nullptr);
        REQUIRE(term::Char16Style::forStyle(term::FrameStyle::InnerHalfBlock) != nullptr);
        REQUIRE_EQUAL(
            term::Char16Style::forStyle(term::FrameStyle::LightWithRoundedCorners)->block(12).charStr(),
            std::string{"╯"});
        REQUIRE_EQUAL(term::Char16Style::forStyle(static_cast<term::FrameStyle>(255))->block(3).charStr(), std::string{"┌"});
    }
};
