// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/StringTestHelper.hpp"

#include <erbsland/cterm/text/impl/text_node_renderer/InlineTextRenderer.hpp>
#include <erbsland/cterm/text/impl/text_node_renderer/StyleResolver.hpp>
#include <erbsland/cterm/text/Style.hpp>
#include <erbsland/cterm/text/TextNode.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <string>

TESTED_TARGETS(InlineTextRenderer)
class InlineTextRendererTest final : public UNITTEST_SUBCLASS(StringTestHelper) {
public:
    void testInlineRendererPreservesNestedStyleOverlaysAndLineBreaks() {
        auto paragraph = text::TextNode::createParagraph();
        paragraph->addChild(text::TextNode::createText(U"A"));

        auto strong = text::TextNode::createStrong();
        strong->addChild(text::TextNode::createText(U"B"));
        paragraph->addChild(strong);

        paragraph->addChild(text::TextNode::createLineBreak());

        auto emphasis = text::TextNode::createEmphasis();
        emphasis->addChild(text::TextNode::createText(U"C"));
        paragraph->addChild(emphasis);

        auto styleResolver = tnr::StyleResolver{text::Style::defaultStyle()};
        auto renderer = tnr::InlineTextRenderer{styleResolver};
        const auto rendered = renderer.render(*paragraph, CharStyle{fg::White});

        REQUIRE_EQUAL(toPlainText(rendered), std::string{"AB\nC"});
        REQUIRE_FALSE(rendered[0].attributes().isBold());
        REQUIRE(rendered[1].attributes().isBold());
        REQUIRE(rendered[3].attributes().isItalic());
    }

    void testInlineRendererAppliesInlinePrefixesAndSuffixes() {
        auto style = std::make_shared<text::Style>();
        style->edit(text::StyleSelector::strong())
            .setTextStyle(CharStyle{CharAttributes::Bold})
            .setPrefix(U"[")
            .setSuffix(U"]");
        style->edit(text::StyleSelector::emphasis())
            .setTextStyle(CharStyle{fg::Yellow, CharAttributes::Italic})
            .setPrefix(U"<")
            .setSuffix(U">");

        auto paragraph = text::TextNode::createParagraph();
        paragraph->addChild(text::TextNode::createText(U"A"));

        auto strong = text::TextNode::createStrong();
        strong->addChild(text::TextNode::createText(U"B"));

        auto emphasis = text::TextNode::createEmphasis();
        emphasis->addChild(text::TextNode::createText(U"C"));
        strong->addChild(emphasis);
        strong->addChild(text::TextNode::createText(U"D"));
        paragraph->addChild(strong);

        auto styleResolver = tnr::StyleResolver{style};
        auto renderer = tnr::InlineTextRenderer{styleResolver};
        const auto rendered = renderer.render(*paragraph, CharStyle{fg::White});

        REQUIRE_EQUAL(toPlainText(rendered), std::string{"A[B<C>D]"});
        REQUIRE(rendered[1].attributes().isBold());
        REQUIRE(rendered[3].attributes().isBold());
        REQUIRE(rendered[3].attributes().isItalic());
        REQUIRE_EQUAL(rendered[3].color().fg(), fg::Yellow);
        REQUIRE(rendered[5].attributes().isBold());
        REQUIRE_EQUAL(rendered[5].color().fg(), fg::Yellow);
        REQUIRE(rendered[7].attributes().isBold());
        REQUIRE_FALSE(rendered[0].attributes().isBold());
    }

    void testInlineRendererTrimOuterDoesNotReuseFlushedWhitespace() {
        auto style = std::make_shared<text::Style>();
        style->edit(text::StyleSelector::strong())
            .setTextStyle(CharStyle{CharAttributes::Bold})
            .setPrefix(U"[")
            .setSuffix(U"]");
        style->edit(text::StyleSelector::span({"warning"}))
            .setTextStyle(CharStyle{fg::Yellow})
            .setPrefix(U"<")
            .setSuffix(U">");

        auto paragraph = text::TextNode::createParagraph();
        paragraph->addChild(text::TextNode::createText(U"Hello "));

        auto strong = text::TextNode::createStrong();
        auto warningSpan = text::TextNode::createSpan();
        warningSpan->setStyle("warning");
        warningSpan->addChild(text::TextNode::createText(U"Alert"));
        strong->addChild(warningSpan);
        paragraph->addChild(strong);

        paragraph->addChild(text::TextNode::createText(U"."));

        auto styleResolver = tnr::StyleResolver{style};
        auto renderer = tnr::InlineTextRenderer{styleResolver};
        const auto rendered = renderer.render(*paragraph, CharStyle{fg::White}, tnr::WhitespaceMode::TrimOuter);

        REQUIRE_EQUAL(toPlainText(rendered), std::string{"Hello [<Alert>]."});
        REQUIRE(rendered[7].color().fg() == fg::Yellow);
        REQUIRE(rendered[13].color().fg() == fg::Yellow);
    }

    void testInlineRendererDoesNotApplyBlockDecorationsForParagraphNodes() {
        auto style = std::make_shared<text::Style>();
        style->edit(text::StyleSelector::paragraph()).setPrefix(U"(").setSuffix(U")");
        style->edit(text::StyleSelector::strong()).setPrefix(U"[").setSuffix(U"]");

        auto paragraph = text::TextNode::createParagraph();
        paragraph->addChild(text::TextNode::createText(U"A"));

        auto strong = text::TextNode::createStrong();
        strong->addChild(text::TextNode::createText(U"B"));
        paragraph->addChild(strong);

        auto styleResolver = tnr::StyleResolver{style};
        auto renderer = tnr::InlineTextRenderer{styleResolver};
        const auto rendered = renderer.render(*paragraph, CharStyle{fg::White}, tnr::WhitespaceMode::TrimOuter);

        REQUIRE_EQUAL(toPlainText(rendered), std::string{"A[B]"});
    }
};
