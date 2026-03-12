// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <array>

class ColorTest : public el::UnitTest {
public:
    void testCodeBase() {
        REQUIRE_EQUAL(ColorPart<ColorRole::Foreground>::cCodeBase, 30);
        REQUIRE_EQUAL(ColorPart<ColorRole::Background>::cCodeBase, 40);
    }

    void testDefaultColors() {
        fg fgDefault;
        bg bgDefault;
        REQUIRE_EQUAL(fgDefault.ansiCode(), 39);
        REQUIRE_EQUAL(bgDefault.ansiCode(), 49);
        REQUIRE_EQUAL(fgDefault, fg(fg::Inherited));
        REQUIRE_EQUAL(bgDefault, bg(bg::Inherited));
    }

    void testAnsiCodesForeground() {
        struct Entry {
            fg value;
            int code;
        };
        constexpr std::array<Entry, 18> entries{{
            {fg::Black, 30},
            {fg::Red, 31},
            {fg::Green, 32},
            {fg::Yellow, 33},
            {fg::Blue, 34},
            {fg::Magenta, 35},
            {fg::Cyan, 36},
            {fg::White, 37},
            {fg::Default, 39},
            {fg::BrightBlack, 90},
            {fg::BrightRed, 91},
            {fg::BrightGreen, 92},
            {fg::BrightYellow, 93},
            {fg::BrightBlue, 94},
            {fg::BrightMagenta, 95},
            {fg::BrightCyan, 96},
            {fg::BrightWhite, 97},
            {fg::Inherited, 39},
        }};
        for (const auto &entry : entries) {
            REQUIRE_EQUAL(entry.value.ansiCode(), entry.code);
        }
    }

    void testAnsiCodesBackground() {
        struct Entry {
            bg value;
            int code;
        };
        constexpr std::array<Entry, 18> entries{{
            {bg::Black, 40},
            {bg::Red, 41},
            {bg::Green, 42},
            {bg::Yellow, 43},
            {bg::Blue, 44},
            {bg::Magenta, 45},
            {bg::Cyan, 46},
            {bg::White, 47},
            {bg::Default, 49},
            {bg::BrightBlack, 100},
            {bg::BrightRed, 101},
            {bg::BrightGreen, 102},
            {bg::BrightYellow, 103},
            {bg::BrightBlue, 104},
            {bg::BrightMagenta, 105},
            {bg::BrightCyan, 106},
            {bg::BrightWhite, 107},
            {bg::Inherited, 49},
        }};
        for (const auto &entry : entries) {
            REQUIRE_EQUAL(entry.value.ansiCode(), entry.code);
        }
    }

    void testEqualityOperators() {
        auto red = fg(fg::Red);
        auto blue = fg(fg::Blue);
        REQUIRE(red == fg(fg::Red));
        REQUIRE(red != blue);
    }

    void testDefaultConstructedColorUsesInheritedComponents() {
        const auto color = Color{};

        REQUIRE_EQUAL(color.fg(), fg(fg::Inherited));
        REQUIRE_EQUAL(color.bg(), bg(bg::Inherited));
    }

    void testOverlayWithKeepsInheritedOverlayComponentsTransparent() {
        const auto base = Color{fg::Green, bg::Blue};

        REQUIRE_EQUAL(base.overlayWith(Color{}), base);
        REQUIRE_EQUAL(base.overlayWith(Color{fg::BrightWhite, bg::Inherited}), Color(fg::BrightWhite, bg::Blue));
        REQUIRE_EQUAL(base.overlayWith(Color{fg::Inherited, bg::Black}), Color(fg::Green, bg::Black));
    }

    void testOverlayWithAppliesDefaultAsOpaqueColor() {
        const auto base = Color{fg::Green, bg::Blue};

        REQUIRE_EQUAL(base.overlayWith(Color{fg::Default, bg::Black}), Color(fg::Default, bg::Black));
        REQUIRE_EQUAL(base.overlayWith(Color{fg::BrightWhite, bg::Default}), Color(fg::BrightWhite, bg::Default));
    }
};
