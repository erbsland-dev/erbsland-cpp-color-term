// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "BufferTestHelper.hpp"

#include <erbsland/cterm/text/HtmlRenderer.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <format>
#include <string>
#include <vector>

namespace text = erbsland::cterm::text;

TESTED_TARGETS(HtmlRenderer)
class HtmlRendererTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testRenderStringPreservesInlineStylesAndHeadingParagraphStructure() {
        const auto rendered = text::HtmlRenderer{"<h1>Title</h1><p>Hello <strong>world</strong>.</p>"}.renderString();
        const auto expected = std::vector<std::string>{"Title", "Hello world."};

        REQUIRE_EQUAL_LINES(erbsland::unittest::th::splitLines(toPlainText(rendered)), expected);
        REQUIRE(rendered[12].attributes().isBold());
    }

    void testRenderStringRendersListsDefinitionsCodeBlocksAndLinks() {
        const auto rendered = text::HtmlRenderer{"<ul><li>One</li><li>Two</li></ul>"
                                                 "<ol><li>First</li><li>Second</li></ol>"
                                                 "<dl><dt>Term</dt><dd>Meaning</dd></dl>"
                                                 "<pre>  a\n  b</pre>"
                                                 "<p><a href=\"/target\">Go</a></p>"
                                                 "<hr>"}
                                  .renderString();
        const auto expected = std::vector<std::string>{
            "•   One", "•   Two", "1.  First", "2.  Second", "Term", "Meaning", "  a", "  b", "Go", "--------"};

        REQUIRE_EQUAL_LINES(erbsland::unittest::th::splitLines(toPlainText(rendered)), expected);
    }

    void testRenderStringUsesStyleTokensFromNodeClassAttributes() {
        auto style = std::make_shared<text::Style>();
        style->edit(text::StyleSelector{text::StyleRole::Paragraph, {"warning"}}).setPrefix(U"[warning] ");

        const auto rendered = text::HtmlRenderer{"<p class=\"warning\">Careful</p>", style}.renderString();

        REQUIRE_EQUAL(toPlainText(rendered), std::string{"[warning] Careful"});
    }

    void testRenderStringAppliesConfiguredBlockPrefixesAndSuffixes() {
        auto style = std::make_shared<text::Style>();
        style->edit(text::StyleSelector::paragraph()).setPrefix(U"(").setSuffix(U")");

        const auto rendered = text::HtmlRenderer{"<p>Careful</p>", style}.renderString();

        REQUIRE_EQUAL(toPlainText(rendered), std::string{"(Careful)"});
    }

    void testRenderStringUsesDedicatedSpanSelectorsForClassedInlineSpans() {
        auto style = std::make_shared<text::Style>();
        style->edit(text::StyleSelector{text::StyleRole::Span, {"warning"}}).setTextStyle(CharStyle{fg::Yellow});

        const auto rendered =
            text::HtmlRenderer{"<p>Hello <span class=\"warning\">there</span>.</p>", style}.renderString();

        REQUIRE_EQUAL(toPlainText(rendered), std::string{"Hello there."});
        REQUIRE_EQUAL(rendered[6].color().fg(), fg::Yellow);
        REQUIRE_EQUAL(rendered[10].color().fg(), fg::Yellow);
        REQUIRE_NOT_EQUAL(rendered[0].color().fg(), fg::Yellow);
        REQUIRE_NOT_EQUAL(rendered[11].color().fg(), fg::Yellow);
    }

    void testRenderStringComposesSpanSelectorsWithExistingInlineRoles() {
        auto style = std::make_shared<text::Style>();
        style->edit(text::StyleSelector::strong()).setTextStyle(CharStyle{CharAttributes::Bold});
        style->edit(text::StyleSelector{text::StyleRole::Span, {"warning"}}).setTextStyle(CharStyle{fg::BrightRed});

        const auto rendered =
            text::HtmlRenderer{"<p><strong><span class=\"warning\">Alert</span></strong></p>", style}.renderString();

        REQUIRE_EQUAL(toPlainText(rendered), std::string{"Alert"});
        for (const auto &character : rendered) {
            REQUIRE(character.attributes().isBold());
            REQUIRE_EQUAL(character.color().fg(), fg::BrightRed);
        }
    }

    void testRenderStringUsesConfiguredListItemIndentAndMarker() {
        auto style = std::make_shared<text::Style>();
        style->edit(text::StyleSelector::listItem(text::StyleListKind::Bullet, 0))
            .setIndents(ParagraphIndents{0, 2, 2, Margins{0}})
            .setLiteralMarker(U"◆ ");

        const auto rendered = text::HtmlRenderer{"<ul><li><p>One</p><p>Two</p></li></ul>", style}.renderString();
        const auto expected = std::vector<std::string>{"  ◆ One", "  Two"};

        REQUIRE_EQUAL_LINES(erbsland::unittest::th::splitLines(toPlainText(rendered)), expected);
    }

    void testRenderStringUsesTabAlignedNumberedPrefixesAndOverflowFallback() {
        auto style = std::make_shared<text::Style>();
        style->edit(text::StyleSelector::listItem(text::StyleListKind::Numbered, 0))
            .setIndents(ParagraphIndents{0, 0, 4, Margins{0}});

        const auto rendered = text::HtmlRenderer{makeOrderedListHtml(10'000), style}.renderString();
        const auto lines = erbsland::unittest::th::splitLines(toPlainText(rendered));

        REQUIRE(lines.size() >= std::size_t{9'999});
        REQUIRE_EQUAL(lines[0], std::string{"1.  Item 1"});
        REQUIRE_EQUAL(lines[98], std::string{"99. Item 99"});
        REQUIRE_EQUAL(lines[9'998], std::string{"9999. Item 9999"});
    }

    void testRenderToWrapsParagraphsWithinConfiguredMargins() {
        auto style = std::make_shared<text::Style>();
        style->edit(text::StyleSelector::paragraph()).setMargins(1, 0);

        auto buffer = std::make_shared<CursorBuffer>(Size{6, 4});
        text::HtmlRenderer{"<p>AB CD</p>", style}.renderTo(buffer);

        requireRowsEqual(*buffer, {" AB   ", " CD   ", "      ", "      "});
    }

    void testRenderToCollapsesVerticalMarginsAndFilledHeadings() {
        auto style = std::make_shared<text::Style>();
        style->edit(text::StyleSelector::heading(1))
            .setIndents(ParagraphIndents{0, 0, 0, Margins{0, 0, 1, 0}})
            .setLineFill(U'-');
        style->edit(text::StyleSelector::paragraph()).setIndents(ParagraphIndents{0, 0, 0, Margins{0, 0, 0, 0}});

        auto buffer = std::make_shared<CursorBuffer>(Size{8, 5});
        text::HtmlRenderer{"<h1>Title</h1><p>Text</p>", style}.renderTo(buffer);

        requireRowsEqual(*buffer, {"Title---", "        ", "Text    ", "        ", "        "});
    }

    void testRenderToAppliesDocumentMargins() {
        auto style = std::make_shared<text::Style>();
        style->edit(text::StyleSelector::document())
            .setIndents(
                ParagraphIndents{
                    0, ParagraphIndents::cUseLineIndent, ParagraphIndents::cUseLineIndent, Margins{1, 1, 1, 2}});

        auto buffer = std::make_shared<CursorBuffer>(Size{6, 5});
        text::HtmlRenderer{"<p>AB</p>", style}.renderTo(buffer);

        requireRowsEqual(*buffer, {"      ", "  AB  ", "      ", "      ", "      "});
    }

    void testRenderToComposesContinuationAndNestedItemIndentation() {
        auto buffer = std::make_shared<CursorBuffer>(Size{12, 5});
        text::HtmlRenderer{"<ul><li>Outer<ul><li>Inner</li></ul></li></ul>"}.renderTo(buffer);

        requireRowsEqual(*buffer, {"•   Outer   ", "    ‣       ", "         In-", "        ner ", "            "});
    }

    void testRenderToKeepsPrefixLinesForListItemsThatStartWithNestedLists() {
        auto style = std::make_shared<text::Style>();
        style->edit(text::StyleSelector::listItem(text::StyleListKind::Bullet, 0))
            .setIndents(ParagraphIndents{0, 0, 0, Margins{1, 0, 1, 0}});
        style->edit(text::StyleSelector::listItem(text::StyleListKind::Bullet, 1))
            .setIndents(ParagraphIndents{0, 0, 0, Margins{0}});

        auto buffer = std::make_shared<CursorBuffer>(Size{12, 5});
        text::HtmlRenderer{"<ul><li><ul><li>Inner</li></ul></li></ul>", style}.renderTo(buffer);

        requireRowsEqual(*buffer, {"            ", "•           ", "‣ Inner     ", "            ", "            "});
    }

    void testRenderToAppliesBlockquoteMarginsToDescendantBlocks() {
        auto style = std::make_shared<text::Style>();
        style->edit(text::StyleSelector::blockquote())
            .setIndents(
                ParagraphIndents{
                    0, ParagraphIndents::cUseLineIndent, ParagraphIndents::cUseLineIndent, Margins{1, 0, 1, 2}});

        auto buffer = std::make_shared<CursorBuffer>(Size{8, 6});
        text::HtmlRenderer{"<blockquote><p>Quote</p></blockquote><p>After</p>", style}.renderTo(buffer);

        requireRowsEqual(*buffer, {"        ", "  Quote ", "        ", "After   ", "        ", "        "});
    }

    void testConfiguredHorizontalRulesApplyDecorationAndFill() {
        auto style = std::make_shared<text::Style>();
        style->edit(text::StyleSelector::horizontalRule())
            .setLineFill(U'─', fg::Magenta)
            .setPrefix(U"◆◂")
            .setSuffix(U"▸◆");

        const auto rendered = text::HtmlRenderer{"<hr>", style}.renderString();
        REQUIRE_EQUAL(toPlainText(rendered), std::string{"◆◂────────▸◆"});

        auto buffer = std::make_shared<CursorBuffer>(Size{10, 2});
        text::HtmlRenderer{"<hr>", style}.renderTo(buffer);

        requireRowsEqual(*buffer, {"◆◂──────▸◆", "          "});
    }

private:
    [[nodiscard]] static auto toPlainText(const String &textValue) -> std::string {
        auto result = std::string{};
        for (const auto &character : textValue) {
            result += character.charStr();
        }
        return result;
    }
    [[nodiscard]] static auto makeOrderedListHtml(const std::size_t itemCount) -> std::string {
        auto html = std::string{"<ol>"};
        html.reserve(itemCount * 20 + 16);
        for (auto index = std::size_t{1}; index <= itemCount; ++index) {
            html += std::format("<li>Item {}</li>", index);
        }
        html += "</ol>";
        return html;
    }
};
