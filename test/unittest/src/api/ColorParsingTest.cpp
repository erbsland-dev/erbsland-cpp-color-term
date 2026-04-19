// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(Color)
class ColorParsingTest final : public el::UnitTest {
public:
    void testColorPartsParseNormalizeAndBrightenAsExpected() {
        REQUIRE_EQUAL(Foreground::fromString("Bright Blue"), fg(fg::BrightBlue));
        REQUIRE_EQUAL(Background::fromString("bright blue"), bg(bg::BrightBlue));
        REQUIRE_EQUAL(Background::fromString("bright-blue"), bg(bg::BrightBlue));
        REQUIRE_EQUAL(Foreground::fromString(" inherited "), fg(fg::Inherited));
        REQUIRE_EQUAL(fg(fg::Red).toString(), std::string{"red"});
        REQUIRE_EQUAL(bg(bg::Default).toString(), std::string{"default"});
        REQUIRE_EQUAL(fg(fg::Green).brighter(), fg(fg::BrightGreen));
        REQUIRE_EQUAL(fg(fg::BrightGreen).brighter(), fg(fg::BrightGreen));
        REQUIRE_EQUAL(fg::fromIndex16(-1), fg(fg::Inherited));
        REQUIRE_EQUAL(bg::fromIndex16(16), bg(bg::Default));

        const auto baseColors = fg::allBaseColors();
        REQUIRE_EQUAL(baseColors.size(), std::size_t{8});
        REQUIRE_EQUAL(baseColors[0], fg(fg::Black));
        REQUIRE_EQUAL(baseColors[7], fg(fg::White));

        REQUIRE_THROWS_AS(std::invalid_argument, fg::fromString("unknown"));
    }

    void testColorsParseFromSingleAndPairedStrings() {
        REQUIRE_EQUAL(Color::fromString("green"), Color(fg::Green, bg::Inherited));
        REQUIRE_EQUAL(Color::fromString("bright white : blue"), Color(fg::BrightWhite, bg::Blue));
        REQUIRE_EQUAL(Color::fromString("default:bright_black"), Color(fg::Default, bg::BrightBlack));
        REQUIRE_EQUAL(Color::fromIndex16(-1, 99), Color(fg::Inherited, bg::Default));
        REQUIRE_EQUAL(Color::reset(), Color(fg::Default, bg::Default));

        REQUIRE_THROWS_AS(std::invalid_argument, Color::fromString("green:unknown"));
    }
};
