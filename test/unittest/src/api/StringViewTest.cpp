// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

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
