// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/unittest/UnitTest.hpp>

#include "TestHelper.hpp"


class CharTest final : public el::UnitTest {
public:
    void testDisplayWidthUsesUnicodeCellWidth() {
        const auto asciiChar = term::Char{"A", term::Color{}};
        const auto wideChar = term::Char{"界", term::Color{}};
        const auto combiningChar = term::Char{"e\xCC\x81", term::Color{}};

        REQUIRE_EQUAL(asciiChar.displayWidth(), 1);
        REQUIRE_EQUAL(wideChar.displayWidth(), 2);
        REQUIRE_EQUAL(combiningChar.displayWidth(), 1);
    }

    void testWithColorPreservesDefaultComponents() {
        const auto base = term::Char{"X", term::Color{term::fg::Green, term::bg::Blue}};

        const auto changedForeground = base.withColor(term::Color{term::fg::BrightWhite, term::bg::Default});
        REQUIRE_EQUAL(changedForeground.color(), term::Color(term::fg::BrightWhite, term::bg::Blue));

        const auto changedBackground = base.withColor(term::Color{term::fg::Default, term::bg::Black});
        REQUIRE_EQUAL(changedBackground.color(), term::Color(term::fg::Green, term::bg::Black));
    }
};
