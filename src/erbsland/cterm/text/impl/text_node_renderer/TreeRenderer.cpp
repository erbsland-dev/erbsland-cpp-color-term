// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TreeRenderer.hpp"

#include "../../../impl/StringBuilder.hpp"
#include "../../../StringView.hpp"

#include <algorithm>
#include <utility>

namespace erbsland::cterm::text::impl::text_node_renderer {

TreeRenderer::TreeRenderer(StyleConstPtr style, BlockTarget &target) :
    _styleResolver{std::move(style)}, _inlineTextRenderer{_styleResolver}, _stream{target} {
}

void TreeRenderer::render(const TextNodeConstPtr &document) {
    if (document) {
        appendNode(*document, RenderContext{});
    }
    _stream.finish();
}

void TreeRenderer::appendNode(const TextNode &node, const RenderContext &context) {
    switch (node.type().raw()) {
    case TextNode::Type::Document:
        appendContainer(node, StyleRole::Document, context);
        return;
    case TextNode::Type::Section:
        appendContainer(node, StyleRole::Section, context);
        return;
    case TextNode::Type::Blockquote:
        appendContainer(node, StyleRole::Blockquote, context);
        return;
    case TextNode::Type::DefinitionList:
        appendContainer(node, StyleRole::DefinitionList, context);
        return;
    case TextNode::Type::ListItem:
        for (const auto &child : node.children()) {
            if (child) {
                appendNode(*child, context);
            }
        }
        return;
    case TextNode::Type::Paragraph:
        appendParagraphLikeNode(node, StyleRole::Paragraph, context);
        return;
    case TextNode::Type::Heading:
        emitBlock(heading(node, context));
        return;
    case TextNode::Type::BulletList:
        appendList(node, false, context);
        return;
    case TextNode::Type::NumberedList:
        appendList(node, true, context);
        return;
    case TextNode::Type::DefinitionTerm:
        appendParagraphLikeNode(node, StyleRole::DefinitionTerm, context);
        return;
    case TextNode::Type::DefinitionDescription:
        appendParagraphLikeNode(node, StyleRole::DefinitionDescription, context);
        return;
    case TextNode::Type::CodeBlock:
        appendParagraphLikeNode(node, StyleRole::CodeBlock, context);
        return;
    case TextNode::Type::HorizontalLine:
        emitBlock(horizontalRule(context));
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
        appendParagraphLikeNode(node, StyleRole::Paragraph, context);
        return;
    default:
        return;
    }
}

void TreeRenderer::appendContainer(const TextNode &node, const StyleRole role, const RenderContext &context) {
    const auto containerRule = _styleResolver.blockRule(node, role);
    openScope(containerRule.margins());
    const auto childContext = context.withContainer(containerRule);
    for (const auto &child : node.children()) {
        if (!child || child->type().renderClass() == TextNodeType::RenderClass::Inline ||
            child->type().renderClass() == TextNodeType::RenderClass::Empty) {
            continue;
        }
        appendNode(*child, childContext);
    }
    closeScope();
}

void TreeRenderer::appendList(const TextNode &node, const bool numbered, const RenderContext &context) {
    const auto listKind = numbered ? StyleListKind::Numbered : StyleListKind::Bullet;
    const auto listContainerRule = _styleResolver.blockRule(node, StyleRole::ListContainer, listKind);
    openScope(listContainerRule.margins());
    const auto childContext = context.withContainer(listContainerRule);

    auto number = std::size_t{1};
    for (const auto &child : node.children()) {
        if (!child || child->type() != TextNode::Type::ListItem) {
            continue;
        }
        const auto listItemRule =
            _styleResolver.blockRule(*child, StyleRole::ListItem, listKind, std::max(node.level(), 0));
        const auto listItemLayout = makeListItemLayout(listItemRule, childContext, number);
        appendListItem(*child, listItemRule, listItemLayout, childContext);
        number += 1;
    }
    closeScope();
}

void TreeRenderer::appendListItem(
    const TextNode &node,
    const StyleRule &listItemRule,
    const ListItemLayout &listItemLayout,
    const RenderContext &context) {
    openScope(listItemRule.margins(), listItemLayout);
    const auto childContext = context.withListItem(listItemRule);
    for (const auto &child : node.children()) {
        if (child) {
            appendListItemChild(*child, childContext);
        }
    }
    if (!currentListItemHasBlocks()) {
        emitBlock(emptyListItemBlock(childContext));
    }
    closeScope();
}

void TreeRenderer::appendListItemChild(const TextNode &node, const RenderContext &context) {
    if (TextNode::isListNodeType(node.type())) {
        if (!currentListItemHasBlocks()) {
            emitBlock(emptyListItemBlock(context));
        }
        appendNode(node, context);
        return;
    }
    if (node.type() == TextNode::Type::Paragraph) {
        emitBlock(paragraph(node, listItemParagraphRule(), context));
        return;
    }
    appendNode(node, context);
}

void TreeRenderer::appendParagraphLikeNode(const TextNode &node, const StyleRole role, const RenderContext &context) {
    const auto rule = _styleResolver.blockRule(node, role);
    emitBlock(paragraph(node, rule, context));
}

auto TreeRenderer::paragraph(const TextNode &node, const StyleRule &blockRule, const RenderContext &context)
    -> RenderBlock {
    const auto textStyle = context.resolvedTextStyle(_styleResolver.baseTextStyle(), blockRule);
    auto whitespaceMode = WhitespaceMode::TrimOuter;
    if (node.type() == TextNode::Type::CodeBlock) {
        whitespaceMode = WhitespaceMode::Preserve;
    }
    auto text = _inlineTextRenderer.render(node, textStyle, whitespaceMode);
    return paragraph(std::move(text), blockRule, context);
}

auto TreeRenderer::paragraph(String text, const StyleRule &blockRule, const RenderContext &context) -> RenderBlock {
    const auto textStyle = context.resolvedTextStyle(_styleResolver.baseTextStyle(), blockRule);
    auto builder = cterm::impl::StringBuilder{};
    builder.reserve(
        (blockRule.prefix().has_value() ? blockRule.prefix()->size() : 0) + text.size() +
        (blockRule.suffix().has_value() ? blockRule.suffix()->size() : 0));
    appendResolvedDecoration(builder, blockRule.prefix(), textStyle);
    builder.append(text);
    appendResolvedDecoration(builder, blockRule.suffix(), textStyle);
    return RenderBlock{BlockKind::Paragraph, builder.takeString(), context.resolvedBlockStyle(blockRule)};
}

auto TreeRenderer::heading(const TextNode &node, const RenderContext &context) -> RenderBlock {
    const auto rule = _styleResolver.blockRule(node, StyleRole::Heading);
    const auto headingTextStyle = context.resolvedTextStyle(_styleResolver.baseTextStyle(), rule);
    auto builder = cterm::impl::StringBuilder{};
    auto text = _inlineTextRenderer.render(node, headingTextStyle, WhitespaceMode::TrimOuter);
    builder.reserve(
        (rule.prefix().has_value() ? rule.prefix()->size() : 0) + text.size() +
        (rule.suffix().has_value() ? rule.suffix()->size() : 0));
    appendResolvedDecoration(builder, rule.prefix(), headingTextStyle);
    builder.append(text);
    appendResolvedDecoration(builder, rule.suffix(), headingTextStyle);
    auto blockStyle = context.resolvedBlockStyle(rule);
    if (rule.lineFill().has_value()) {
        return RenderBlock{
            BlockKind::FilledLine,
            builder.takeString(),
            std::move(blockStyle),
            rule.lineFill()->withBase(headingTextStyle)};
    }
    return RenderBlock{BlockKind::Paragraph, builder.takeString(), std::move(blockStyle)};
}

auto TreeRenderer::horizontalRule(const RenderContext &context) -> RenderBlock {
    const auto rule = _styleResolver.horizontalRuleRule();
    const auto ruleTextStyle = context.resolvedTextStyle(_styleResolver.baseTextStyle(), rule);
    return RenderBlock{
        resolvedDecoration(rule.prefix(), ruleTextStyle),
        resolvedDecoration(rule.suffix(), ruleTextStyle),
        context.resolvedBlockStyle(rule),
        rule.lineFill().has_value() ? rule.lineFill()->withBase(ruleTextStyle) : Char{U'-', ruleTextStyle}};
}

auto TreeRenderer::emptyListItemBlock(const RenderContext &context) -> RenderBlock {
    return paragraph(String{}, emptyListItemRule(), context);
}

auto TreeRenderer::makeListItemLayout(
    const StyleRule &listItemRule, const RenderContext &context, const std::size_t number) -> ListItemLayout {
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

void TreeRenderer::emitBlock(RenderBlock &&block) {
    for (auto &scope : _scopes) {
        const auto isFirstBlockInScope = !scope.hasBlocks;
        if (isFirstBlockInScope) {
            collapseVerticalMargin(block, Margins::Side::Top, scope.verticalMargins.top());
            scope.hasBlocks = true;
        }
        if (scope.listItemLayout.has_value()) {
            if (isFirstBlockInScope) {
                scope.listItemLayout->applyPrefixTo(block);
            } else {
                scope.listItemLayout->applyContinuationTo(block);
            }
        }
    }
    _stream.append(std::move(block));
}

void TreeRenderer::openScope(const Margins &verticalMargins, std::optional<ListItemLayout> listItemLayout) {
    _scopes.push_back(
        BlockScope{
            .verticalMargins = Margins{verticalMargins.top(), 0, verticalMargins.bottom(), 0},
            .listItemLayout = std::move(listItemLayout)});
}

void TreeRenderer::closeScope() {
    if (_scopes.empty()) {
        return;
    }
    const auto scope = std::move(_scopes.back());
    _scopes.pop_back();
    if (!scope.hasBlocks) {
        return;
    }
    if (auto *pendingBlock = _stream.pendingBlock(); pendingBlock != nullptr) {
        collapseVerticalMargin(*pendingBlock, Margins::Side::Bottom, scope.verticalMargins.bottom());
    }
}

auto TreeRenderer::currentListItemHasBlocks() const noexcept -> bool {
    for (auto iterator = _scopes.rbegin(); iterator != _scopes.rend(); ++iterator) {
        if (iterator->listItemLayout.has_value()) {
            return iterator->hasBlocks;
        }
    }
    return false;
}

void TreeRenderer::collapseVerticalMargin(RenderBlock &block, const Margins::Side side, const Coordinate margin) {
    if (margin == 0) {
        return;
    }
    auto margins = block.style().margins();
    margins.set(side, collapsedVerticalMarginValue(margins.at(side), margin));
    block.style().setMargins(margins);
}

void TreeRenderer::appendResolvedDecoration(
    cterm::impl::StringBuilder &builder, const std::optional<String> &decoration, const CharStyle &textStyle) {
    if (!decoration.has_value()) {
        return;
    }
    builder.appendWithBaseStyle(*decoration, textStyle);
}

auto TreeRenderer::resolvedDecoration(const std::optional<String> &decoration, const CharStyle &textStyle)
    -> std::optional<String> {
    if (!decoration.has_value()) {
        return std::nullopt;
    }
    auto result = cterm::impl::StringBuilder{};
    result.appendWithBaseStyle(*decoration, textStyle);
    return result.takeString();
}

auto TreeRenderer::collapsedVerticalMarginValue(const Coordinate first, const Coordinate second) noexcept
    -> Coordinate {
    if (first >= 0 && second >= 0) {
        return std::max(first, second);
    }
    if (first <= 0 && second <= 0) {
        return std::min(first, second);
    }
    return first + second;
}

auto TreeRenderer::listItemParagraphRule() -> StyleRule {
    auto rule = StyleRule{};
    rule.setIndents(ParagraphIndents{0, 0, 0, Margins{0}});
    return rule;
}

auto TreeRenderer::emptyListItemRule() -> StyleRule {
    auto rule = StyleRule{};
    rule.setIndents(ParagraphIndents{0, 0, 0, Margins{0}});
    return rule;
}

}
