// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(Font)
class FontAccessTest final : public el::UnitTest {
public:
    void testConstructorsAndAccessorsExposeTheConfiguredGlyphs() {
        auto glyphs = Font::GlyphMap{};
        glyphs.emplace("A", FontGlyph{Size{1, 1}});
        auto font = Font{3, glyphs};

        REQUIRE_EQUAL(font.height(), 3);
        REQUIRE_EQUAL(font.glyphs().size(), std::size_t{1});
        REQUIRE(font.glyph("A") != nullptr);
        REQUIRE(font.glyph("B") == nullptr);

        font.setHeight(5);
        font.addGlyph("B", FontGlyph{Size{2, 1}});
        const auto expectedGlyphSize = Size{2, 1};

        REQUIRE_EQUAL(font.height(), 5);
        REQUIRE_EQUAL(font.glyphs().size(), std::size_t{2});
        REQUIRE(font.glyph("B") != nullptr);
        REQUIRE_EQUAL(font.glyph("B")->size(), expectedGlyphSize);
    }
};
