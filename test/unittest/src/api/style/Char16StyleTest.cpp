// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/cterm/Char16Style.hpp>
#include <erbsland/cterm/FrameStyle.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <array>
#include <stdexcept>

TESTED_TARGETS(Char16Style)
class Char16StyleTest final : public el::UnitTest {
public:
    void testStringConstructorSplitsExactlySixteenTerminalCharacters() {
        const auto style = Char16Style{"0123456789ABCDEF"};

        REQUIRE_EQUAL(style.block(0), U'0');
        REQUIRE_EQUAL(style.block(10), U'A');
        REQUIRE_EQUAL(style.block(15), U'F');
    }

    void testUtf32StringConstructorSplitsExactlySixteenTerminalCharacters() {
        const auto style = Char16Style{U"0123456789ABCDEF"};

        REQUIRE_EQUAL(style.block(0), U'0');
        REQUIRE_EQUAL(style.block(10), U'A');
        REQUIRE_EQUAL(style.block(15), U'F');
    }

    void testStringConstructorRejectsInvalidTileCounts() {
        REQUIRE_THROWS_AS(std::invalid_argument, Char16Style{"123"});
        REQUIRE_THROWS_AS(std::invalid_argument, Char16Style{"0123456789ABCDEFG"});
    }

    void testCreateFactoryBuildsSharedStyles() {
        const auto utf8Style = Char16Style::create("0123456789ABCDEF");
        const auto utf32Style = Char16Style::create(U"0123456789ABCDEF");

        REQUIRE(utf8Style != nullptr);
        REQUIRE(utf32Style != nullptr);
        REQUIRE_EQUAL(utf8Style->block(1), U'1');
        REQUIRE_EQUAL(utf32Style->block(15), U'F');
    }

    void testBlockReturnsConfiguredTilesAndDefaultsForInvalidIndexes() {
        const auto style = Char16Style{std::array<Char, 16>{
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
            Char{U'A'},
            Char{U'B'},
            Char{U'C'},
            Char{U'D'},
            Char{U'E'},
            Char{U'F'},
        }};

        REQUIRE_EQUAL(style.block(0), U'0');
        REQUIRE_EQUAL(style.block(5), U'5');
        REQUIRE_EQUAL(style.block(15), U'F');
        REQUIRE(style.block(16).isEmpty());
    }

    void testPredefinedStylesExposeExpectedTiles() {
        REQUIRE_EQUAL(Char16Style::lightFrame()->block(3), U'┌');
        REQUIRE_EQUAL(Char16Style::lightDoubleDashFrame()->block(5), U'╌');
        REQUIRE_EQUAL(Char16Style::lightTripleDashFrame()->block(5), U'┄');
        REQUIRE_EQUAL(Char16Style::lightQuadrupleDashFrame()->block(5), U'┈');
        REQUIRE_EQUAL(Char16Style::lightRoundedFrame()->block(3), U'╭');
        REQUIRE_EQUAL(Char16Style::heavyFrame()->block(15), U'╋');
        REQUIRE_EQUAL(Char16Style::heavyDoubleDashFrame()->block(10), U'╏');
        REQUIRE_EQUAL(Char16Style::heavyTripleDashFrame()->block(5), U'┅');
        REQUIRE_EQUAL(Char16Style::heavyQuadrupleDashFrame()->block(5), U'┉');
        REQUIRE_EQUAL(Char16Style::doubleFrame()->block(10), U'║');
        REQUIRE_EQUAL(Char16Style::fullBlockFrame()->block(15), U'█');
        REQUIRE_EQUAL(Char16Style::fullBlockWithChamferFrame()->block(3), U'◢');
        REQUIRE_EQUAL(Char16Style::noneFrame()->block(15), U' ');
    }

    void testForStyleMapsKnownAndUnknownEnumValues() {
        REQUIRE_EQUAL(Char16Style::forStyle(FrameStyle::Light)->block(5), U'─');
        REQUIRE_EQUAL(Char16Style::forStyle(FrameStyle::LightDoubleDash)->block(5), U'╌');
        REQUIRE_EQUAL(Char16Style::forStyle(FrameStyle::LightTripleDash)->block(5), U'┄');
        REQUIRE_EQUAL(Char16Style::forStyle(FrameStyle::LightQuadrupleDash)->block(5), U'┈');
        REQUIRE_EQUAL(Char16Style::forStyle(FrameStyle::Heavy)->block(5), U'━');
        REQUIRE_EQUAL(Char16Style::forStyle(FrameStyle::HeavyDoubleDash)->block(5), U'╍');
        REQUIRE_EQUAL(Char16Style::forStyle(FrameStyle::HeavyTripleDash)->block(5), U'┅');
        REQUIRE_EQUAL(Char16Style::forStyle(FrameStyle::HeavyQuadrupleDash)->block(5), U'┉');
        REQUIRE_EQUAL(Char16Style::forStyle(FrameStyle::Double)->block(5), U'═');
        REQUIRE_EQUAL(Char16Style::forStyle(FrameStyle::FullBlock)->block(5), U'█');
        REQUIRE_EQUAL(Char16Style::forStyle(FrameStyle::FullBlockWithChamfer)->block(3), U'◢');
        REQUIRE(Char16Style::forStyle(FrameStyle::OuterHalfBlock) == nullptr);
        REQUIRE(Char16Style::forStyle(FrameStyle::InnerHalfBlock) == nullptr);
        REQUIRE_EQUAL(Char16Style::forStyle(FrameStyle::LightWithRoundedCorners)->block(12), U'╯');
        REQUIRE_EQUAL(Char16Style::forStyle(FrameStyle::None)->block(15), U' ');
        REQUIRE_EQUAL(Char16Style::forStyle(static_cast<FrameStyle>(255))->block(3), U'┌');
    }
};
