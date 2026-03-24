// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>


TESTED_TARGETS(ParagraphOptions)
class ParagraphOptionsTest final : public el::UnitTest {
public:
    void testLineBreakMarksAcceptValidShortStrings() {
        auto options = ParagraphOptions{};

        options.setLineBreakEndMark(String{U"->"});
        options.setLineBreakStartMark(String{U"<-"});

        REQUIRE_EQUAL(options.lineBreakEndMark().size(), std::size_t{2});
        REQUIRE_EQUAL(options.lineBreakEndMark()[0], U'-');
        REQUIRE_EQUAL(options.lineBreakEndMark()[1], U'>');
        REQUIRE_EQUAL(options.lineBreakStartMark().size(), std::size_t{2});
        REQUIRE_EQUAL(options.lineBreakStartMark()[0], U'<');
        REQUIRE_EQUAL(options.lineBreakStartMark()[1], U'-');
    }

    void testLineBreakEndMarkRejectsTooLongOrControlText() {
        auto options = ParagraphOptions{};

        REQUIRE_THROWS_AS(std::invalid_argument, options.setLineBreakEndMark(String{U"abc"}));
        REQUIRE_THROWS_AS(std::invalid_argument, options.setLineBreakEndMark(String{U"\n"}));
    }

    void testLineBreakStartMarkRejectsTooLongOrControlText() {
        auto options = ParagraphOptions{};

        REQUIRE_THROWS_AS(std::invalid_argument, options.setLineBreakStartMark(String{U"abc"}));
        REQUIRE_THROWS_AS(std::invalid_argument, options.setLineBreakStartMark(String{U"\t"}));
    }
};
