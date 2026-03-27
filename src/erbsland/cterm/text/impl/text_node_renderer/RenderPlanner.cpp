// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "RenderPlanner.hpp"


namespace erbsland::cterm::text::impl::text_node_renderer {


RenderPlanner::RenderPlanner(StyleConstPtr style) :
    _style{style != nullptr ? std::move(style) : Style::defaultStyle()},
    _styleResolver{_style},
    _queries{},
    _inlineTextPlanner{_styleResolver},
    _blockFactory{_styleResolver, _inlineTextPlanner},
    _listBlockPlanner{_styleResolver, _blockFactory, _queries} {
}


auto RenderPlanner::build(const TextNodeConstPtr &document) const -> RenderBlocks {
    auto blocks = RenderBlocks{};
    if (document) {
        appendNode(*document, PlanningContext{}, blocks);
    }
    return blocks;
}


void RenderPlanner::appendNode(const TextNode &node, const PlanningContext &context, RenderBlocks &blocks) const {
    switch (node.type()) {
    case TextNode::Type::Document:
        appendContainer(node, StyleRole::Document, context, blocks);
        return;
    case TextNode::Type::Section:
        appendContainer(node, StyleRole::Section, context, blocks);
        return;
    case TextNode::Type::Blockquote:
        appendContainer(node, StyleRole::Blockquote, context, blocks);
        return;
    case TextNode::Type::DefinitionList:
        appendContainer(node, StyleRole::DefinitionList, context, blocks);
        return;
    case TextNode::Type::ListItem:
        for (const auto &child : node.children()) {
            if (child) {
                appendNode(*child, context, blocks);
            }
        }
        return;
    case TextNode::Type::Paragraph:
        appendParagraphLikeNode(node, StyleRole::Paragraph, context, blocks);
        return;
    case TextNode::Type::Heading:
        blocks.push_back(_blockFactory.heading(node, context));
        return;
    case TextNode::Type::BulletList:
        _listBlockPlanner.appendList(node, false, context, blocks, *this);
        return;
    case TextNode::Type::NumberedList:
        _listBlockPlanner.appendList(node, true, context, blocks, *this);
        return;
    case TextNode::Type::DefinitionTerm:
        appendParagraphLikeNode(node, StyleRole::DefinitionTerm, context, blocks);
        return;
    case TextNode::Type::DefinitionDescription:
        appendParagraphLikeNode(node, StyleRole::DefinitionDescription, context, blocks);
        return;
    case TextNode::Type::CodeBlock:
        appendParagraphLikeNode(node, StyleRole::CodeBlock, context, blocks);
        return;
    case TextNode::Type::HorizontalLine:
        blocks.push_back(_blockFactory.horizontalRule(context));
        return;
    case TextNode::Type::Unsupported:
    case TextNode::Type::Error:
    case TextNode::Type::LineBreak:
    case TextNode::Type::Text:
    case TextNode::Type::Emphasis:
    case TextNode::Type::Strong:
    case TextNode::Type::Underline:
    case TextNode::Type::Span:
    case TextNode::Type::Link:
    case TextNode::Type::Code:
        appendParagraphLikeNode(node, StyleRole::Paragraph, context, blocks);
        return;
    }
}


void RenderPlanner::appendParagraphLikeNode(
    const TextNode &node, const StyleRole role, const PlanningContext &context, RenderBlocks &blocks) const {
    const auto rule = _styleResolver.blockRule(node, role);
    blocks.push_back(_blockFactory.paragraph(node, rule, context));
}


void RenderPlanner::appendContainer(
    const TextNode &node, const StyleRole role, const PlanningContext &context, RenderBlocks &blocks) const {
    const auto containerRule = _styleResolver.blockRule(node, role);
    const auto childContext = context.withContainer(containerRule);

    const auto childRange = _queries.findChildRange(node, TextNodePlanningQueries::ChildSelection::RenderableBlocks);
    if (!childRange.has_value()) {
        return;
    }

    for (auto index = childRange->first; index <= childRange->last; ++index) {
        const auto &child = node.children()[index];
        if (!child ||
            !_queries.childMatchesSelection(*child, TextNodePlanningQueries::ChildSelection::RenderableBlocks)) {
            continue;
        }
        const auto effectiveContext = childContext.withEdgeMargins(
            containerRule.margins(), index == childRange->first, index == childRange->last);
        appendNode(*child, effectiveContext, blocks);
    }
}


void RenderPlanner::appendPlannedNode(
    const TextNode &node, const PlanningContext &context, RenderBlocks &blocks) const {
    appendNode(node, context, blocks);
}


}
