// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "BufferTestHelper.hpp"

#include <erbsland/cterm/text/impl/HtmlParser.hpp>
#include <erbsland/cterm/text/impl/TextNodeRenderer.hpp>
#include <erbsland/unittest/UnitTest.hpp>

namespace textimpl = erbsland::cterm::text::impl;

TESTED_TARGETS(TextNodeRenderer)
class TextNodeRendererTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testRendererRenderStringCombinesBlocksWithExplicitLineBreaks() {
        const auto document = textimpl::HtmlParser{"<h1>Title</h1><p>Hello</p><hr>"}.parse();
        const auto rendered = textimpl::TextNodeRenderer{document}.renderString();

        REQUIRE_EQUAL(toPlainText(rendered), std::string{"Title\nHello\n--------"});
    }

    void testRendererRendersHorizontalRulesAcrossTheAvailableWidth() {
        const auto document = textimpl::HtmlParser{"<hr>"}.parse();
        auto buffer = std::make_shared<CursorBuffer>(Size{16, 2});

        textimpl::TextNodeRenderer{document}.renderTo(buffer);

        requireRowsEqual(*buffer, {"----------------", "                "});
    }

private:
    [[nodiscard]] static auto toPlainText(const String &text) -> std::string {
        auto result = std::string{};
        for (const auto &character : text) {
            result += character.charStr();
        }
        return result;
    }
};
