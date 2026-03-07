// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/unittest/UnitTest.hpp>

#include <array>

#include "TestHelper.hpp"

class ColorTest : public el::UnitTest {
public:
    void testCodeBase() {
        REQUIRE_EQUAL(term::ColorPart<term::ColorRole::Foreground>::cCodeBase, 30);
        REQUIRE_EQUAL(term::ColorPart<term::ColorRole::Background>::cCodeBase, 40);
    }

    void testDefaultColors() {
        term::fg fgDefault;
        term::bg bgDefault;
        REQUIRE_EQUAL(fgDefault.ansiCode(), 39);
        REQUIRE_EQUAL(bgDefault.ansiCode(), 49);
    }

    void testAnsiCodesForeground() {
        struct Entry {
            term::fg value;
            int code;
        };
        constexpr std::array<Entry, 17> entries{{
            {term::fg::Black, 30},
            {term::fg::Red, 31},
            {term::fg::Green, 32},
            {term::fg::Yellow, 33},
            {term::fg::Blue, 34},
            {term::fg::Magenta, 35},
            {term::fg::Cyan, 36},
            {term::fg::White, 37},
            {term::fg::Default, 39},
            {term::fg::BrightBlack, 90},
            {term::fg::BrightRed, 91},
            {term::fg::BrightGreen, 92},
            {term::fg::BrightYellow, 93},
            {term::fg::BrightBlue, 94},
            {term::fg::BrightMagenta, 95},
            {term::fg::BrightCyan, 96},
            {term::fg::BrightWhite, 97},
        }};
        for (const auto &entry : entries) {
            REQUIRE_EQUAL(entry.value.ansiCode(), entry.code);
        }
    }

    void testAnsiCodesBackground() {
        struct Entry {
            term::bg value;
            int code;
        };
        constexpr std::array<Entry, 17> entries{{
            {term::bg::Black, 40},
            {term::bg::Red, 41},
            {term::bg::Green, 42},
            {term::bg::Yellow, 43},
            {term::bg::Blue, 44},
            {term::bg::Magenta, 45},
            {term::bg::Cyan, 46},
            {term::bg::White, 47},
            {term::bg::Default, 49},
            {term::bg::BrightBlack, 100},
            {term::bg::BrightRed, 101},
            {term::bg::BrightGreen, 102},
            {term::bg::BrightYellow, 103},
            {term::bg::BrightBlue, 104},
            {term::bg::BrightMagenta, 105},
            {term::bg::BrightCyan, 106},
            {term::bg::BrightWhite, 107},
        }};
        for (const auto &entry : entries) {
            REQUIRE_EQUAL(entry.value.ansiCode(), entry.code);
        }
    }

    void testEqualityOperators() {
        auto red = term::fg(term::fg::Red);
        auto blue = term::fg(term::fg::Blue);
        REQUIRE(red == term::fg(term::fg::Red));
        REQUIRE(red != blue);
    }

    void testOverlayWithKeepsDefaultOverlayComponentsTransparent() {
        const auto base = term::Color{term::fg::Green, term::bg::Blue};

        REQUIRE_EQUAL(base.overlayWith(term::Color{}), base);
        REQUIRE_EQUAL(
            base.overlayWith(term::Color{term::fg::BrightWhite, term::bg::Default}),
            term::Color(term::fg::BrightWhite, term::bg::Blue));
        REQUIRE_EQUAL(
            base.overlayWith(term::Color{term::fg::Default, term::bg::Black}),
            term::Color(term::fg::Green, term::bg::Black));
    }
};
