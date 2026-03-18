// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <string>
#include <vector>


TESTED_TARGETS(String)
class StringConversionTest final : public el::UnitTest {
public:
    void testFromLinesJoinsUtf8AndUtf32LinesAndPreservesColor() {
        const auto utf8 = String::fromLines({"A", "B"}, Color{fg::Red, bg::Blue});
        REQUIRE_EQUAL(render(utf8), std::string{"A\nB"});
        REQUIRE_EQUAL(utf8.size(), std::size_t{3});
        REQUIRE_EQUAL(utf8[0].color(), Color(fg::Red, bg::Blue));
        REQUIRE_EQUAL(utf8[1], U'\n');
        REQUIRE_EQUAL(utf8[2].color(), Color(fg::Red, bg::Blue));

        const auto utf32 = String::fromLines({U"ä", U"ö"}, Color{fg::Green, bg::Black});
        REQUIRE_EQUAL(render(utf32), std::string{"ä\nö"});
        REQUIRE_EQUAL(utf32.size(), std::size_t{3});
        REQUIRE_EQUAL(utf32[0].color(), Color(fg::Green, bg::Black));
        REQUIRE_EQUAL(utf32[2].color(), Color(fg::Green, bg::Black));

        REQUIRE(String::fromLines(std::initializer_list<std::string_view>{}).empty());
        REQUIRE(String::fromLines(std::initializer_list<std::u32string_view>{}).empty());
    }

    void testSplitWordsSkipsSpacingAndAppendUsesTheCurrentColorWithinOneCall() {
        auto text = String{};
        text.append(fg::Yellow, bg::Blue, "alpha beta", String{"!"}, Char{U'?'});

        REQUIRE_EQUAL(text.size(), std::size_t{12});
        REQUIRE_EQUAL(text[0].color(), Color(fg::Yellow, bg::Blue));
        REQUIRE_EQUAL(text[5].color(), Color(fg::Yellow, bg::Blue));
        REQUIRE_EQUAL(text[9].color(), Color(fg::Yellow, bg::Blue));
        REQUIRE_EQUAL(text[10].color(), Color(fg::Yellow, bg::Blue));
        REQUIRE_EQUAL(text[10], U'!');
        REQUIRE_EQUAL(text[11].color(), Color(fg::Yellow, bg::Blue));
        REQUIRE_EQUAL(text[11], U'?');

        const auto words = String{"  alpha\tbeta\n\ngamma  "}.splitWords();
        REQUIRE_EQUAL(renderWords(words), std::vector<std::string>({"alpha", "beta", "gamma"}));
    }

private:
    [[nodiscard]] static auto render(const String &text) -> std::string {
        auto result = std::string{};
        for (const auto &character : text) {
            character.appendTo(result);
        }
        return result;
    }

    [[nodiscard]] static auto renderWords(const std::vector<String> &words) -> std::vector<std::string> {
        auto result = std::vector<std::string>{};
        result.reserve(words.size());
        for (const auto &word : words) {
            result.push_back(render(word));
        }
        return result;
    }
};
