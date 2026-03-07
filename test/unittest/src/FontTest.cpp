// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/Font.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <array>
#include <string_view>
#include <vector>


TESTED_TARGETS(Font)
class FontTest final : public el::UnitTest {
public:
    void testDefaultAsciiContainsLatinAndPunctuationGlyphs() {
        const auto font = term::Font::defaultAscii();
        REQUIRE_EQUAL(font->height(), 8);
        REQUIRE(font->glyph("A") != nullptr);
        REQUIRE(font->glyph("\u00c4") != nullptr);
        const auto punctuation =
            std::array<std::string_view, 23>{"!", "?", "<", ">", ":", ";", ".", ",", "'", "\"", "/", "[",
                                             "]", "-", "+", "=", "_", "(", ")", "*", "%", "$",  "#"};
        for (const auto name : punctuation) {
            WITH_CONTEXT(std::string{name});
            REQUIRE(font->glyph(name) != nullptr);
        }
        REQUIRE(font->glyph("\u3042") == nullptr);
    }

    void testDefaultFontFactoriesReturnIndependentInstances() {
        const auto first = term::Font::defaultAscii();
        const auto second = term::Font::defaultAscii();

        first->addGlyph("~", term::FontGlyph{std::vector<uint64_t>{0x00, 0x00, 0x14, 0x2a, 0x00, 0x00, 0x00, 0x00}});

        REQUIRE(first->glyph("~") != nullptr);
        REQUIRE(second->glyph("~") == nullptr);
    }
};
