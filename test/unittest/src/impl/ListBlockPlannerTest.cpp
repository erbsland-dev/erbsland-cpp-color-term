// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/text/impl/text_node_renderer/InlineTextPlanner.hpp>
#include <erbsland/cterm/text/impl/text_node_renderer/ListBlockPlanner.hpp>
#include <erbsland/cterm/text/impl/text_node_renderer/PlanningContext.hpp>
#include <erbsland/cterm/text/impl/text_node_renderer/PlanningStyleResolver.hpp>
#include <erbsland/cterm/text/impl/text_node_renderer/RenderBlockFactory.hpp>
#include <erbsland/cterm/text/impl/text_node_renderer/TextNodePlanningQueries.hpp>
#include <erbsland/cterm/text/Style.hpp>
#include <erbsland/cterm/text/TextNode.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <string>

namespace text = erbsland::cterm::text;
namespace tnr = erbsland::cterm::text::impl::text_node_renderer;

namespace {

auto toPlainText(const String &textValue) -> std::string {
    auto result = std::string{};
    for (const auto &character : textValue) {
        result += character.charStr();
    }
    return result;
}

class TestNodeAppender final : public tnr::PlannedNodeAppender {
public:
    TestNodeAppender(
        const tnr::PlanningStyleResolver &styleResolver,
        const tnr::RenderBlockFactory &blockFactory,
        const tnr::ListBlockPlanner &listBlockPlanner) :
        _styleResolver{styleResolver}, _blockFactory{blockFactory}, _listBlockPlanner{listBlockPlanner} {}

    void appendPlannedNode(
        const text::TextNode &node, const tnr::PlanningContext &context, tnr::RenderBlocks &blocks) const override {
        switch (node.type()) {
        case text::TextNode::Type::BulletList:
            _listBlockPlanner.appendList(node, false, context, blocks, *this);
            return;
        case text::TextNode::Type::NumberedList:
            _listBlockPlanner.appendList(node, true, context, blocks, *this);
            return;
        case text::TextNode::Type::Heading:
            blocks.push_back(_blockFactory.heading(node, context));
            return;
        case text::TextNode::Type::ListItem:
        case text::TextNode::Type::Document:
        case text::TextNode::Type::Section:
        case text::TextNode::Type::Blockquote:
        case text::TextNode::Type::DefinitionList:
            for (const auto &child : node.children()) {
                if (child) {
                    appendPlannedNode(*child, context, blocks);
                }
            }
            return;
        case text::TextNode::Type::DefinitionTerm:
            blocks.push_back(_blockFactory.paragraph(
                node, _styleResolver.blockRule(node, text::StyleRole::DefinitionTerm), context));
            return;
        case text::TextNode::Type::DefinitionDescription:
            blocks.push_back(_blockFactory.paragraph(
                node, _styleResolver.blockRule(node, text::StyleRole::DefinitionDescription), context));
            return;
        case text::TextNode::Type::CodeBlock:
            blocks.push_back(
                _blockFactory.paragraph(node, _styleResolver.blockRule(node, text::StyleRole::CodeBlock), context));
            return;
        case text::TextNode::Type::HorizontalLine:
            blocks.push_back(_blockFactory.horizontalRule(context));
            return;
        default:
            blocks.push_back(
                _blockFactory.paragraph(node, _styleResolver.blockRule(node, text::StyleRole::Paragraph), context));
            return;
        }
    }

private:
    const tnr::PlanningStyleResolver &_styleResolver;
    const tnr::RenderBlockFactory &_blockFactory;
    const tnr::ListBlockPlanner &_listBlockPlanner;
};

} // namespace

TESTED_TARGETS(ListBlockPlanner ListItemLayout)
class ListBlockPlannerTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testParagraphFirstListItemsAttachOnePrefixAndIndentContinuations() {
        auto list = text::TextNode::createBulletList(0);
        auto item = text::TextNode::createListItem();
        auto firstParagraph = text::TextNode::createParagraph();
        firstParagraph->addChild(text::TextNode::createText(U"One"));
        auto secondParagraph = text::TextNode::createParagraph();
        secondParagraph->addChild(text::TextNode::createText(U"Two"));
        item->addChild(firstParagraph);
        item->addChild(secondParagraph);
        list->addChild(item);

        const auto styleResolver = tnr::PlanningStyleResolver{text::Style::defaultStyle()};
        const auto queries = tnr::TextNodePlanningQueries{};
        const auto inlineTextPlanner = tnr::InlineTextPlanner{styleResolver};
        const auto blockFactory = tnr::RenderBlockFactory{styleResolver, inlineTextPlanner};
        const auto listBlockPlanner = tnr::ListBlockPlanner{styleResolver, blockFactory, queries};
        const auto appender = TestNodeAppender{styleResolver, blockFactory, listBlockPlanner};

        auto blocks = tnr::RenderBlocks{};
        listBlockPlanner.appendList(*list, false, tnr::PlanningContext{}, blocks, appender);

        REQUIRE_EQUAL(blocks.size(), std::size_t{2});
        REQUIRE_EQUAL(toPlainText(blocks[0].renderString()), std::string{"•   One"});
        REQUIRE(blocks[0].terminalText().has_value());
        REQUIRE_EQUAL(toPlainText(*blocks[0].terminalText()), std::string{"•\tOne"});
        REQUIRE_EQUAL(toPlainText(blocks[1].renderString()), std::string{"    Two"});
    }

    void testListPlannerKeepsPlaceholderPrefixLinesForNestedListFirstItems() {
        auto outerList = text::TextNode::createBulletList(0);
        auto outerItem = text::TextNode::createListItem();
        auto innerList = text::TextNode::createBulletList(1);
        auto innerItem = text::TextNode::createListItem();
        auto innerParagraph = text::TextNode::createParagraph();
        innerParagraph->addChild(text::TextNode::createText(U"Inner"));
        innerItem->addChild(innerParagraph);
        innerList->addChild(innerItem);
        outerItem->addChild(innerList);
        outerList->addChild(outerItem);

        const auto styleResolver = tnr::PlanningStyleResolver{text::Style::defaultStyle()};
        const auto queries = tnr::TextNodePlanningQueries{};
        const auto inlineTextPlanner = tnr::InlineTextPlanner{styleResolver};
        const auto blockFactory = tnr::RenderBlockFactory{styleResolver, inlineTextPlanner};
        const auto listBlockPlanner = tnr::ListBlockPlanner{styleResolver, blockFactory, queries};
        const auto appender = TestNodeAppender{styleResolver, blockFactory, listBlockPlanner};

        auto blocks = tnr::RenderBlocks{};
        listBlockPlanner.appendList(*outerList, false, tnr::PlanningContext{}, blocks, appender);

        REQUIRE_EQUAL(blocks.size(), std::size_t{2});
        REQUIRE(toPlainText(blocks[0].renderString()).starts_with("•"));
        REQUIRE(toPlainText(blocks[1].renderString()).find("Inner") != std::string::npos);
    }

    void testHeadingFirstListItemsAttachThePrefixToTheFirstRenderedBlock() {
        auto list = text::TextNode::createBulletList(0);
        auto item = text::TextNode::createListItem();
        auto heading = text::TextNode::createHeading(1);
        heading->addChild(text::TextNode::createText(U"Title"));
        item->addChild(heading);
        list->addChild(item);

        const auto styleResolver = tnr::PlanningStyleResolver{text::Style::defaultStyle()};
        const auto queries = tnr::TextNodePlanningQueries{};
        const auto inlineTextPlanner = tnr::InlineTextPlanner{styleResolver};
        const auto blockFactory = tnr::RenderBlockFactory{styleResolver, inlineTextPlanner};
        const auto listBlockPlanner = tnr::ListBlockPlanner{styleResolver, blockFactory, queries};
        const auto appender = TestNodeAppender{styleResolver, blockFactory, listBlockPlanner};

        auto blocks = tnr::RenderBlocks{};
        listBlockPlanner.appendList(*list, false, tnr::PlanningContext{}, blocks, appender);

        REQUIRE_EQUAL(blocks.size(), std::size_t{1});
        REQUIRE(toPlainText(blocks[0].renderString()).starts_with("•"));
        REQUIRE(toPlainText(blocks[0].renderString()).find("Title") != std::string::npos);
    }
};
