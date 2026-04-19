// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>

TESTED_TARGETS(ParagraphOptions)
class ParagraphOptionsTest final : public el::UnitTest {
public:
    void testDefaultWordSeparatorsUseTheSharedSpaceAndTabSet() {
        const auto options = ParagraphOptions{};

        REQUIRE(options.wordSeparatorSet() == FastCharSet::spaceAndTab());
        REQUIRE_EQUAL(options.wordSeparators(), std::u32string{U"\t "});
    }

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

    void testIndentsAndMarginsCanBeConfiguredAsOneValueObject() {
        auto options = ParagraphOptions{};
        auto indents = ParagraphIndents{2, 4, 6, Margins{1, 3}};

        options.setIndents(indents);

        REQUIRE_EQUAL(options.indents(), indents);
        REQUIRE_EQUAL(options.lineIndent(), 2);
        REQUIRE_EQUAL(options.firstLineIndent(), 4);
        REQUIRE_EQUAL(options.wrappedLineIndent(), 6);
        REQUIRE_EQUAL(options.margins(), Margins(1, 3));
    }

    void testWordSeparatorsAreCanonicalizedAndReuseSharedDefaults() {
        auto options = ParagraphOptions{};

        options.setWordSeparators(U" \t\t ");

        REQUIRE(options.wordSeparatorSet() == FastCharSet::spaceAndTab());
        REQUIRE_EQUAL(options.wordSeparators(), std::u32string{U"\t "});

        options.setWordSeparators(U".,.,");

        REQUIRE(options.wordSeparatorSet() != FastCharSet::spaceAndTab());
        REQUIRE_EQUAL(options.wordSeparators(), std::u32string{U",."});
        REQUIRE(options.wordSeparatorSet()->contains(U','));
        REQUIRE(options.wordSeparatorSet()->contains(U'.'));
        REQUIRE(!options.wordSeparatorSet()->contains(U';'));
    }
};
