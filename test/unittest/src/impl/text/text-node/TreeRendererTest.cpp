// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/StringTestHelper.hpp"

#include <erbsland/cterm/text/impl/text_node_renderer/StringBlockTarget.hpp>
#include <erbsland/cterm/text/impl/text_node_renderer/TreeRenderer.hpp>
#include <erbsland/cterm/text/TextNode.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <string>

TESTED_TARGETS(TreeRenderer)
class TreeRendererTest final : public UNITTEST_SUBCLASS(StringTestHelper) {
public:
    void testRendererIgnoresDocumentLevelInlineChildren() {
        auto document = text::TextNode::createDocument();
        auto span = text::TextNode::createSpan();
        span->addChild(text::TextNode::createText(U"inline only"));
        document->addChild(span);
        auto paragraph = text::TextNode::createParagraph();
        paragraph->addChild(text::TextNode::createText(U"text"));
        document->addChild(paragraph);

        auto target = tnr::StringBlockTarget{};
        tnr::TreeRenderer{text::Style::defaultStyle(), target}.render(document);

        REQUIRE_EQUAL(toPlainText(target.takeString()), std::string{"text"});
    }

    void testRendererTreatsDirectListItemTextAsParagraphContent() {
        auto document = text::TextNode::createDocument();
        auto list = text::TextNode::createBulletList(0);
        auto item = text::TextNode::createListItem();
        item->addChild(text::TextNode::createText(U"direct"));
        list->addChild(item);
        document->addChild(list);

        auto target = tnr::StringBlockTarget{};
        tnr::TreeRenderer{text::Style::defaultStyle(), target}.render(document);

        REQUIRE_EQUAL(toPlainText(target.takeString()), std::string{"•   direct"});
    }
};
