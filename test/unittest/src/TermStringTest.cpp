// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/unittest/UnitTest.hpp>

#include "TestHelper.hpp"

#include <stdexcept>
#include <vector>

class TermStringTest : public el::UnitTest {
public:
    void testUtf8StringIsSplitIntoTerminalCharacters() {
        const auto text = term::String{"äöü"};
        REQUIRE_EQUAL(text.size(), std::size_t{3});
        REQUIRE_EQUAL(text[0].charStr(), std::string{"ä"});
        REQUIRE_EQUAL(text[1].charStr(), std::string{"ö"});
        REQUIRE_EQUAL(text[2].charStr(), std::string{"ü"});
    }

    void testDisplayWidth() {
        const auto text = term::String{"A中B"};
        REQUIRE_EQUAL(text.displayWidth(), 4);
    }

    void testWrapIntoLinesUsesTerminalWidth() {
        const auto text = term::String{"漢字テスト"};
        const auto lines = text.wrapIntoLines(4);
        REQUIRE_EQUAL(lines.size(), std::size_t{3});
        REQUIRE_EQUAL(lines[0][0].charStr(), std::string{"漢"});
        REQUIRE_EQUAL(lines[0][1].charStr(), std::string{"字"});
        REQUIRE_EQUAL(lines[1][0].charStr(), std::string{"テ"});
        REQUIRE_EQUAL(lines[1][1].charStr(), std::string{"ス"});
        REQUIRE_EQUAL(lines[2][0].charStr(), std::string{"ト"});
    }

    void testWrapIntoLinesPreservesParagraphBreaks() {
        const auto text = term::String{"alpha beta\ngamma"};
        const auto lines = text.wrapIntoLines(6);
        REQUIRE_EQUAL(lines.size(), std::size_t{4});
        REQUIRE_EQUAL(lines[0][0].charStr(), std::string{"a"});
        REQUIRE_EQUAL(lines[0][4].charStr(), std::string{"a"});
        REQUIRE_EQUAL(lines[1][0].charStr(), std::string{"b"});
        REQUIRE(lines[2].empty());
        REQUIRE_EQUAL(lines[3][0].charStr(), std::string{"g"});
    }

    void testWrapIntoLinesPreservesColoredSpacingBetweenWords() {
        auto text = term::String{};
        text.append(term::Char{"A", term::Color{term::fg::Red, term::bg::Black}});
        text.append(term::Char{" ", term::Color{term::fg::Default, term::bg::Blue}});
        text.append(term::Char{" ", term::Color{term::fg::Default, term::bg::Cyan}});
        text.append(term::Char{"B", term::Color{term::fg::Green, term::bg::Black}});

        const auto lines = text.wrapIntoLines(8);

        REQUIRE_EQUAL(lines.size(), std::size_t{1});
        REQUIRE_EQUAL(lines[0].size(), std::size_t{4});
        REQUIRE_EQUAL(lines[0][1].color(), term::Color(term::fg::Default, term::bg::Blue));
        REQUIRE_EQUAL(lines[0][2].color(), term::Color(term::fg::Default, term::bg::Cyan));
    }

    void testCombiningCharactersStayInOneCell() {
        const auto text = term::String{"e\xCC\x81"};
        REQUIRE_EQUAL(text.size(), std::size_t{1});
        REQUIRE_EQUAL(text.displayWidth(), 1);
        REQUIRE_EQUAL(text[0].charStr(), std::string{"e\xCC\x81"});
    }

    void testInvalidUtf8Fails() { REQUIRE_THROWS_AS(std::invalid_argument, term::String{"\xC3"}); }
};
