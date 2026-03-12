// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>
#include <string>
#include <vector>

TESTED_TARGETS(String)
class TermStringTest final : public el::UnitTest {
public:
    void testUtf8StringIsSplitIntoTerminalCharacters() {
        const auto text = String{"äöü"};
        REQUIRE_EQUAL(text.size(), std::size_t{3});
        REQUIRE_EQUAL(text[0], U'ä');
        REQUIRE_EQUAL(text[1], U'ö');
        REQUIRE_EQUAL(text[2], U'ü');
    }

    void testUtf32StringIsSplitIntoTerminalCharacters() {
        const auto text = String{U"äöü"};
        REQUIRE_EQUAL(text.size(), std::size_t{3});
        REQUIRE_EQUAL(text[0], U'ä');
        REQUIRE_EQUAL(text[1], U'ö');
        REQUIRE_EQUAL(text[2], U'ü');
    }

    void testDisplayWidth() {
        const auto text = String{"A中B"};
        REQUIRE_EQUAL(text.displayWidth(), 4);
    }

    void testCountAndIndexOfRespectColorSpecificAndColorAgnosticSearches() {
        auto text = String{};
        text.append(Char{U'A', fg::Red, bg::Black});
        text.append(Char{U'B', fg::Blue, bg::Black});
        text.append(Char{U'A', fg::Green, bg::Black});
        text.append(Char{U"e\u0301"});

        REQUIRE_EQUAL(text.count(Char{U'A', fg::Red, bg::Black}), std::size_t{1});
        REQUIRE_EQUAL(text.count(Char{U'A', fg::Green, bg::Black}), std::size_t{1});
        REQUIRE_EQUAL(text.count(Char{U'A', fg::White, bg::Black}), std::size_t{0});
        REQUIRE_EQUAL(text.count(U'A'), std::size_t{2});
        REQUIRE_EQUAL(text.count(U'e'), std::size_t{0});

        REQUIRE_EQUAL(text.indexOf(Char{U'A', fg::Green, bg::Black}), std::size_t{2});
        REQUIRE_EQUAL(text.indexOf(Char{U'A', fg::Red, bg::Black}, 1), String::npos);
        REQUIRE_EQUAL(text.indexOf(U'A'), std::size_t{0});
        REQUIRE_EQUAL(text.indexOf(U'A', 1), std::size_t{2});
        REQUIRE_EQUAL(text.indexOf(U'Z'), String::npos);
        REQUIRE_EQUAL(text.indexOf(U'B', 99), String::npos);
    }

    void testSubstrReturnsRequestedWindowAndClampsBounds() {
        const auto text = String{"AB界D"};

        const auto middle = text.substr(1, 2);
        const auto tail = text.substr(2);

        REQUIRE_EQUAL(middle.size(), std::size_t{2});
        REQUIRE_EQUAL(middle[0], U'B');
        REQUIRE_EQUAL(middle[1], U'界');
        REQUIRE_EQUAL(tail.size(), std::size_t{2});
        REQUIRE_EQUAL(tail[0], U'界');
        REQUIRE_EQUAL(tail[1], U'D');
        REQUIRE(text.substr(2, 0).empty());
        REQUIRE(text.substr(99).empty());
    }

    void testWrapIntoLinesUsesTerminalWidth() {
        const auto text = String{"漢字テスト"};
        const auto lines = text.wrapIntoLines(4);
        REQUIRE_EQUAL(lines.size(), std::size_t{3});
        REQUIRE_EQUAL(lines[0][0], U'漢');
        REQUIRE_EQUAL(lines[0][1], U'字');
        REQUIRE_EQUAL(lines[1][0], U'テ');
        REQUIRE_EQUAL(lines[1][1], U'ス');
        REQUIRE_EQUAL(lines[2][0], U'ト');
    }

    void testWrapIntoLinesUsesSingleParagraphSpacingByDefault() {
        const auto text = String{"alpha beta\ngamma"};
        const auto lines = text.wrapIntoLines(6);
        REQUIRE_EQUAL(lines.size(), std::size_t{3});
        REQUIRE_EQUAL(lines[0][0], U'a');
        REQUIRE_EQUAL(lines[0][4], U'a');
        REQUIRE_EQUAL(lines[1][0], U'b');
        REQUIRE_EQUAL(lines[2][0], U'g');
    }

    void testWrapIntoLinesAddsExtraSpacingForDoubleParagraphSpacing() {
        const auto text = String{"alpha beta\ngamma"};
        const auto lines = text.wrapIntoLines(6, ParagraphSpacing::DoubleLine);

        REQUIRE_EQUAL(lines.size(), std::size_t{4});
        REQUIRE_EQUAL(lines[0][0], U'a');
        REQUIRE_EQUAL(lines[1][0], U'b');
        REQUIRE(lines[2].empty());
        REQUIRE_EQUAL(lines[3][0], U'g');
    }

    void testWrapIntoLinesPreservesExplicitEmptyParagraphs() {
        const auto lines = String{"alpha\n\nbeta"}.wrapIntoLines(10);

        REQUIRE_EQUAL(renderLines(lines), std::vector<std::string>({"alpha", "", "beta"}));
    }

    void testSplitLinesPreservesEmptyLinesWithoutTrailingEmptyLine() {
        const auto lines = String{"alpha\n\nbeta\n"}.splitLines();

        REQUIRE_EQUAL(lines.size(), std::size_t{3});
        REQUIRE_EQUAL(lines[0].size(), std::size_t{5});
        REQUIRE_EQUAL(lines[0][0], U'a');
        REQUIRE(lines[1].empty());
        REQUIRE_EQUAL(lines[2].size(), std::size_t{4});
        REQUIRE_EQUAL(lines[2][0], U'b');
    }

    void testSplitLinesHandlesEmptyLeadingAndSingleNewlineStrings() {
        REQUIRE(String{}.splitLines().empty());

        const auto leadingEmptyLine = String{"\nalpha"}.splitLines();
        REQUIRE_EQUAL(leadingEmptyLine.size(), std::size_t{2});
        REQUIRE(leadingEmptyLine[0].empty());
        REQUIRE_EQUAL(leadingEmptyLine[1][0], U'a');

        const auto singleNewline = String{"\n"}.splitLines();
        REQUIRE_EQUAL(singleNewline.size(), std::size_t{1});
        REQUIRE(singleNewline[0].empty());
    }

    void testWrapIntoLinesPreservesColoredSpacingBetweenWords() {
        auto text = String{};
        text.append(Char{U'A', fg::Red, bg::Black});
        text.append(Char{U' ', fg::Inherited, bg::Blue});
        text.append(Char{U' ', fg::Inherited, bg::Cyan});
        text.append(Char{U'B', fg::Green, bg::Black});

        const auto lines = text.wrapIntoLines(8);

        REQUIRE_EQUAL(lines.size(), std::size_t{1});
        REQUIRE_EQUAL(lines[0].size(), std::size_t{4});
        REQUIRE_EQUAL(lines[0][1].color(), Color(fg::Inherited, bg::Blue));
        REQUIRE_EQUAL(lines[0][2].color(), Color(fg::Inherited, bg::Cyan));
    }

    void testWrapIntoLinesTrimsOuterSpacingAndKeepsInternalSpacingThatFits() {
        const auto lines = String{"  alpha   beta  "}.wrapIntoLines(12);

        REQUIRE_EQUAL(renderLines(lines), std::vector<std::string>({"alpha   beta"}));
    }

    void testWrapIntoLinesSplitsOversizedWordsIntoStandaloneChunks() {
        const auto lines = String{"supercalifragilistic test"}.wrapIntoLines(5);

        REQUIRE_EQUAL(renderLines(lines), std::vector<std::string>({"super", "calif", "ragil", "istic", "test"}));
    }

    void testWrapIntoLinesSplitsWideWordsAtCharacterBoundaries() {
        const auto lines = String{"漢字abcde"}.wrapIntoLines(5);

        REQUIRE_EQUAL(renderLines(lines), std::vector<std::string>({"漢字a", "bcde"}));
    }

    void testWrapIntoLinesDoesNotDuplicateWordsBetweenTokens() {
        const auto lines =
            String{"Text Gallery  |  alignment, wrapping, wide characters, and bitmap fonts"}.wrapIntoLines(200);

        REQUIRE_EQUAL(
            renderLines(lines),
            std::vector<std::string>({"Text Gallery  |  alignment, wrapping, wide characters, and bitmap fonts"}));
    }

    void testCombiningCharactersStayInOneCell() {
        const auto text = String{"e\xCC\x81"};
        REQUIRE_EQUAL(text.size(), std::size_t{1});
        REQUIRE_EQUAL(text.displayWidth(), 1);
        REQUIRE_EQUAL(text[0].charStr(), std::string{"e\xCC\x81"});
    }

    void testControlCodesAreFilteredExceptTabAndNewline() {
        const auto text = String{"A\r\x01\t\nB"};

        REQUIRE_EQUAL(text.size(), std::size_t{4});
        REQUIRE_EQUAL(text[0], U'A');
        REQUIRE_EQUAL(text[1].charStr(), std::string{"\t"});
        REQUIRE_EQUAL(text[2].charStr(), std::string{"\n"});
        REQUIRE_EQUAL(text[3], U'B');
    }

    void testInvalidUtf8Fails() { REQUIRE_THROWS_AS(std::invalid_argument, String{"\xC3"}); }

private:
    [[nodiscard]] static auto renderLines(const StringLines &lines) -> std::vector<std::string> {
        auto result = std::vector<std::string>{};
        result.reserve(lines.size());
        for (const auto &line : lines) {
            auto text = std::string{};
            for (const auto &character : line) {
                character.appendTo(text);
            }
            result.push_back(text);
        }
        return result;
    }
};
