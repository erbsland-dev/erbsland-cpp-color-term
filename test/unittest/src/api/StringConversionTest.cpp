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

    void testFromLinesCanApplyBaseAttributes() {
        auto attributes = CharAttributes{};
        attributes.setBold(true);
        attributes.setUnderline(true);

        const auto text = String::fromLines({"A", "B"}, Color{fg::Red, bg::Blue}, attributes);

        REQUIRE_EQUAL(render(text), std::string{"A\nB"});
        REQUIRE(text[0].attributes().isBold());
        REQUIRE(text[0].attributes().isUnderline());
        REQUIRE(text[1].attributes().isBold());
        REQUIRE(text[1].attributes().isUnderline());
        REQUIRE(text[2].attributes().isBold());
        REQUIRE(text[2].attributes().isUnderline());
    }

    void testStringAndFromLinesAcceptCharStyleForUniformFormatting() {
        auto attributes = CharAttributes{};
        attributes.setItalic(true);
        const auto style = CharStyle{Color{fg::Green, bg::Black}, attributes};

        const auto text = String{"AB", style};
        const auto lines = String::fromLines({"A", "B"}, style);

        REQUIRE_EQUAL(text[0].color(), Color(fg::Green, bg::Black));
        REQUIRE(text[1].attributes().isItalic());
        REQUIRE_EQUAL(lines[0].color(), Color(fg::Green, bg::Black));
        REQUIRE(lines[2].attributes().isItalic());
    }

    void testBraceInitializedDefaultAndColorStylesAreUnambiguous() {
        static_assert(requires { String{"A", {}}; });
        static_assert(requires { String{U"A", {}}; });

        const auto inheritedUtf8 = String{"A", {}};
        const auto inheritedUtf32 = String{U"A", {}};
        const auto coloredUtf8 = String{"B", Color{fg::Yellow, bg::Blue}};

        REQUIRE_EQUAL(inheritedUtf8[0].color(), Color{});
        REQUIRE_EQUAL(inheritedUtf32[0].color(), Color{});
        REQUIRE_EQUAL(coloredUtf8[0].color(), Color(fg::Yellow, bg::Blue));
    }

    void testSplitWordsSkipsSpacingAndAppendUsesTheCurrentFormattingWithinOneCall() {
        auto text = String{};
        auto bold = CharAttributes{};
        bold.setBold(true);
        auto noBold = CharAttributes{};
        noBold.setBold(false);
        auto emphasis = CharStyle{Color{fg::Yellow, bg::Blue}, bold};
        text.append(emphasis, "alpha beta", String{"!"}, noBold, Char{U'?'});

        REQUIRE_EQUAL(text.size(), std::size_t{12});
        REQUIRE_EQUAL(text[0].color(), Color(fg::Yellow, bg::Blue));
        REQUIRE(text[0].attributes().isBold());
        REQUIRE_EQUAL(text[5].color(), Color(fg::Yellow, bg::Blue));
        REQUIRE(text[5].attributes().isBold());
        REQUIRE_EQUAL(text[9].color(), Color(fg::Yellow, bg::Blue));
        REQUIRE(text[9].attributes().isBold());
        REQUIRE_EQUAL(text[10].color(), Color(fg::Yellow, bg::Blue));
        REQUIRE(text[10].attributes().isBold());
        REQUIRE_EQUAL(text[10], U'!');
        REQUIRE_EQUAL(text[11].color(), Color(fg::Yellow, bg::Blue));
        REQUIRE_FALSE(text[11].attributes().isBold());
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
