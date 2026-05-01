// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/cterm/StringView.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <string>
#include <vector>

TESTED_TARGETS(StringView)
class StringViewTest final : public el::UnitTest {
public:
    void testStringImplicitlyConvertsToStringView() {
        auto text = String{};
        text.append(Char{U'A', fg::Red, bg::Black});
        text.append(Char{U'B', fg::Green, bg::Black});

        const StringView view = text;

        REQUIRE_EQUAL(view.size(), std::size_t{2});
        REQUIRE_EQUAL(view[0], U'A');
        REQUIRE_EQUAL(view[1], U'B');
        REQUIRE_EQUAL(view[0].color(), Color(fg::Red, bg::Black));
        REQUIRE_EQUAL(view[1].color(), Color(fg::Green, bg::Black));
    }

    void testStringCanBeExplicitlyConstructedFromAView() {
        const auto source = String{"AB界D"};
        const auto view = StringView{source}.substr(1, 2);
        const auto copy = String{view};

        REQUIRE_EQUAL(render(copy), std::string{"B界"});
        REQUIRE_EQUAL(copy.size(), std::size_t{2});
    }

    void testSubstrAndTrimmedReturnViews() {
        const auto source = String{"  alpha beta  "};
        const auto trimmed = StringView{source}.trimmed(U" ");
        const auto middle = trimmed.substr(2, 5);

        REQUIRE_EQUAL(render(trimmed), std::string{"alpha beta"});
        REQUIRE_EQUAL(render(middle), std::string{"pha b"});
    }

    void testIndexedAccessAndAtRespectCroppedBounds() {
        const auto source = String{"012345"};
        const auto view = StringView{source}.substr(2, 3);

        REQUIRE_EQUAL(view[0], U'2');
        REQUIRE_EQUAL(view[2], U'4');
        REQUIRE(view[3].isEmpty());
        REQUIRE_THROWS_AS(std::out_of_range, view.at(3));
    }

    void testTrimmedUsesDefaultWhitespaceCharacters() {
        const auto source = String{"xx \talpha\n yy"};
        const auto view = StringView{source}.substr(2, 9);

        REQUIRE_EQUAL(render(view), std::string{" \talpha\n "});
        REQUIRE_EQUAL(render(view.trimmed()), std::string{"alpha"});
    }

    void testTrimmedRemovesNonWhitespaceCharactersFromCroppedView() {
        const auto source = String{"00xyalpha yx11"};
        const auto view = StringView{source}.substr(2, 10);

        REQUIRE_EQUAL(render(view.trimmed(U"xy")), std::string{"alpha "});
    }

    void testIndexOfCharacterSetCoversBoundsAndCroppedViews() {
        const auto source = String{"xxabc-yy"};
        const auto view = StringView{source}.substr(2, 4);

        REQUIRE_EQUAL(view.indexOf(U"-x"), std::size_t{3});
        REQUIRE_EQUAL(view.indexOf(U"-x", 4), StringView::npos);
        REQUIRE_EQUAL(view.indexOf(U"x"), StringView::npos);
        REQUIRE_EQUAL(view.indexOf(U"-", 99), StringView::npos);
    }

    void testIndexNotOfCharacterSetCoversBoundsAndCroppedViews() {
        const auto source = String{"xxabc-yy"};
        const auto view = StringView{source}.substr(2, 4);

        REQUIRE_EQUAL(view.indexNotOf(U"abc"), std::size_t{3});
        REQUIRE_EQUAL(view.indexNotOf(U"abc-"), StringView::npos);
        REQUIRE_EQUAL(view.indexNotOf(U"a", 99), StringView::npos);
    }

    void testSplitWordsReturnsOnlyWordViews() {
        const auto words = StringView{String{"  alpha\tbeta\n\ngamma  "}}.splitWords();

        REQUIRE_EQUAL(renderWords(words), std::vector<std::string>({"alpha", "beta", "gamma"}));
    }

    void testSplitLinesPreservesEmptyLinesWithoutTrailingEmptyLine() {
        const auto lines = StringView{String{"alpha\n\nbeta\n"}}.splitLines();

        REQUIRE_EQUAL(renderWords(lines), std::vector<std::string>({"alpha", "", "beta"}));
    }

    void testWrapIntoLinesMaterializesStrings() {
        const auto lines = StringView{String{"alpha beta\ngamma"}}.wrapIntoLines(6, ParagraphSpacing::DoubleLine);

        REQUIRE_EQUAL(renderStringLines(lines), std::vector<std::string>({"alpha", "beta", "", "gamma"}));
    }

    void testTextMeasurementWorksOnViews() {
        const auto source = String{"A界\nBC"};
        const auto view = StringView{source};
        auto options = TextOptions{};

        REQUIRE_EQUAL(view.naturalTextSize(), (Size{3, 2}));
        REQUIRE_EQUAL(view.wrappedTextHeight(3, options), 2);
    }

    void testCroppedViewsUseTheSameRangeAlgorithmsAsStrings() {
        const auto source = String{U"xxA界 e\u0301\nB yy"};
        const auto stringRange = source.substr(2, 6);
        const auto view = StringView{source}.substr(2, 6);

        REQUIRE_EQUAL(render(view), render(stringRange));
        REQUIRE_EQUAL(view.displayWidth(), stringRange.displayWidth());
        REQUIRE_EQUAL(view.count(U'e'), stringRange.count(U'e'));
        REQUIRE_EQUAL(view.indexOf(U'\n'), stringRange.indexOf(U'\n'));
        REQUIRE_EQUAL(view.indexNotOf(U"A界 e"), stringRange.indexNotOf(U"A界 e"));
        REQUIRE_EQUAL(
            render(view.croppedToDisplayWidth(4, Alignment::Left)),
            render(stringRange.croppedToDisplayWidth(4, Alignment::Left)));
        REQUIRE_EQUAL(
            render(view.croppedToDisplayWidth(4, Alignment::Right)),
            render(stringRange.croppedToDisplayWidth(4, Alignment::Right)));
        REQUIRE_EQUAL(renderWords(view.splitWords()), std::vector<std::string>({"A界", "é", "B"}));
        REQUIRE_EQUAL(renderWords(view.splitLines()), std::vector<std::string>({"A界 é", "B"}));
        REQUIRE_EQUAL(view.naturalTextSize(), stringRange.naturalTextSize());
    }

    void testEmptyViewRangesStayEmpty() {
        const auto view = StringView{String{"xxx"}}.trimmed(U"x");

        REQUIRE(view.empty());
        REQUIRE(view.substr(0).empty());
        REQUIRE(view.croppedToDisplayWidth(3, Alignment::Left).empty());
        REQUIRE(view.splitWords().empty());
        REQUIRE(view.splitLines().empty());
    }

private:
    [[nodiscard]] static auto render(const StringView text) -> std::string {
        auto result = std::string{};
        for (const auto &character : text) {
            character.appendTo(result);
        }
        return result;
    }

    [[nodiscard]] static auto renderWords(const std::vector<StringView> &words) -> std::vector<std::string> {
        auto result = std::vector<std::string>{};
        result.reserve(words.size());
        for (const auto word : words) {
            result.push_back(render(word));
        }
        return result;
    }

    [[nodiscard]] static auto renderStringLines(const StringLines &lines) -> std::vector<std::string> {
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
