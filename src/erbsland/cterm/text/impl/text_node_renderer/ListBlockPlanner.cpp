// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ListBlockPlanner.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

ListBlockPlanner::ListBlockPlanner(
    const PlanningStyleResolver &styleResolver,
    const RenderBlockFactory &blockFactory,
    const TextNodePlanningQueries &queries) :
    _styleResolver{styleResolver}, _blockFactory{blockFactory}, _queries{queries} {
}

void ListBlockPlanner::appendList(
    const TextNode &node,
    const bool numbered,
    const PlanningContext &context,
    RenderBlocks &blocks,
    const PlannedNodeAppender &appender) const {
    const auto listKind = numbered ? StyleListKind::Numbered : StyleListKind::Bullet;
    const auto listContainerRule = _styleResolver.blockRule(node, StyleRole::ListContainer, listKind);
    const auto childContext = context.withHorizontalMargins(listContainerRule.margins());

    const auto childRange = _queries.findChildRange(node, TextNodePlanningQueries::ChildSelection::ListItems);
    if (!childRange.has_value()) {
        return;
    }

    auto number = std::size_t{1};
    for (auto index = childRange->first; index <= childRange->last; ++index) {
        const auto &child = node.children()[index];
        if (!child || !_queries.childMatchesSelection(*child, TextNodePlanningQueries::ChildSelection::ListItems)) {
            continue;
        }
        const auto effectiveContext = childContext.withEdgeMargins(
            listContainerRule.margins(), index == childRange->first, index == childRange->last);
        const auto listItemRule =
            _styleResolver.blockRule(*child, StyleRole::ListItem, listKind, std::max(node.level(), 0));
        auto listItemLayout = makeListItemLayout(listItemRule, effectiveContext, number);
        appendListItem(*child, listItemRule, effectiveContext, listItemLayout, blocks, appender);
        number += 1;
    }
}

auto ListBlockPlanner::listItemParagraphRule() -> StyleRule {
    auto rule = StyleRule{};
    rule.setIndents(ParagraphIndents{0, 0, 0, Margins{0}});
    return rule;
}

void ListBlockPlanner::appendListItem(
    const TextNode &node,
    const StyleRule &listItemRule,
    const PlanningContext &context,
    ListItemLayout &listItemLayout,
    RenderBlocks &blocks,
    const PlannedNodeAppender &appender) const {
    auto itemBlocks = collectListItemBlocks(node, context.withListItem(listItemRule), appender);
    applyListItemMargins(itemBlocks, listItemRule, context);
    appendListItemBlocks(itemBlocks, listItemLayout, blocks);
}

auto ListBlockPlanner::collectListItemBlocks(
    const TextNode &node, const PlanningContext &context, const PlannedNodeAppender &appender) const -> RenderBlocks {
    auto itemBlocks = RenderBlocks{};
    itemBlocks.reserve(node.children().size());
    for (const auto &child : node.children()) {
        if (child) {
            appendListItemChild(*child, context, itemBlocks, appender);
        }
    }
    if (itemBlocks.empty()) {
        itemBlocks.push_back(emptyListItemBlock(context));
    }
    return itemBlocks;
}

void ListBlockPlanner::appendListItemChild(
    const TextNode &node,
    const PlanningContext &context,
    RenderBlocks &blocks,
    const PlannedNodeAppender &appender) const {
    if (TextNode::isListNodeType(node.type())) {
        if (blocks.empty()) {
            blocks.push_back(emptyListItemBlock(context));
        }
        appender.appendPlannedNode(node, context, blocks);
        return;
    }
    if (node.type() == TextNode::Type::Paragraph) {
        const auto contentRule = listItemParagraphRule();
        blocks.push_back(_blockFactory.paragraph(node, contentRule, context));
        return;
    }
    appender.appendPlannedNode(node, context, blocks);
}

void ListBlockPlanner::applyListItemMargins(
    RenderBlocks &itemBlocks, const StyleRule &listItemRule, const PlanningContext &context) const {
    auto firstMargins = itemBlocks.front().style().margins();
    firstMargins = Margins{
        firstMargins.top() + context.margins.top() + listItemRule.margins().top(),
        firstMargins.right(),
        firstMargins.bottom(),
        firstMargins.left()};
    itemBlocks.front().style().setMargins(firstMargins);

    auto lastMargins = itemBlocks.back().style().margins();
    lastMargins = Margins{
        lastMargins.top(),
        lastMargins.right(),
        lastMargins.bottom() + context.margins.bottom() + listItemRule.margins().bottom(),
        lastMargins.left()};
    itemBlocks.back().style().setMargins(lastMargins);
}

void ListBlockPlanner::appendListItemBlocks(
    RenderBlocks &itemBlocks, const ListItemLayout &listItemLayout, RenderBlocks &blocks) const {
    auto firstBlock = true;
    for (auto &itemBlock : itemBlocks) {
        if (firstBlock) {
            listItemLayout.applyPrefixTo(itemBlock);
        } else {
            listItemLayout.applyContinuationTo(itemBlock);
        }
        blocks.push_back(std::move(itemBlock));
        firstBlock = false;
    }
}

auto ListBlockPlanner::emptyListItemRule() -> StyleRule {
    auto rule = StyleRule{};
    rule.setIndents(ParagraphIndents{0, 0, 0, Margins{0}});
    return rule;
}

auto ListBlockPlanner::emptyListItemBlock(const PlanningContext &context) const -> RenderBlock {
    return _blockFactory.paragraph(String{}, emptyListItemRule(), context);
}

auto ListBlockPlanner::makeListItemLayout(
    const StyleRule &listItemRule, const PlanningContext &context, const std::size_t number) const -> ListItemLayout {
    auto renderedMarker = listItemRule.marker().render(
        number,
        context.resolvedTextStyle(_styleResolver.baseTextStyle(), listItemRule),
        listItemRule.indents().wrappedLineIndent());
    return ListItemLayout{
        ListPrefix{std::move(renderedMarker.text), std::move(renderedMarker.terminalText), renderedMarker.width},
        listItemRule.indents().firstLineIndent(),
        listItemRule.indents().wrappedLineIndent(),
        listItemRule.indents().firstLineIndent(),
        listItemRule.indents().wrappedLineIndent()};
}

}
