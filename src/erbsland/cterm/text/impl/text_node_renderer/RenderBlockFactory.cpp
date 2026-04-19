// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "RenderBlockFactory.hpp"

#include "../../../impl/StringBuilder.hpp"
#include "../../../StringView.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

RenderBlockFactory::RenderBlockFactory(
    const PlanningStyleResolver &styleResolver, const InlineTextPlanner &inlineTextPlanner) :
    _styleResolver{styleResolver}, _inlineTextPlanner{inlineTextPlanner} {
}

auto RenderBlockFactory::paragraph(
    const TextNode &node, const StyleRule &blockStyle, const PlanningContext &context) const -> RenderBlock {
    const auto textStyle = context.resolvedTextStyle(_styleResolver.baseTextStyle(), blockStyle);
    return paragraph(_inlineTextPlanner.render(node, textStyle), blockStyle, context);
}

auto RenderBlockFactory::paragraph(String text, const StyleRule &blockStyle, const PlanningContext &context) const
    -> RenderBlock {
    const auto textStyle = context.resolvedTextStyle(_styleResolver.baseTextStyle(), blockStyle);
    return RenderBlock{
        BlockKind::Paragraph,
        decorateText(std::move(text), blockStyle, textStyle),
        context.resolvedBlockStyle(blockStyle)};
}

auto RenderBlockFactory::heading(const TextNode &node, const PlanningContext &context) const -> RenderBlock {
    const auto rule = _styleResolver.blockRule(node, StyleRole::Heading);
    const auto headingTextStyle = context.resolvedTextStyle(_styleResolver.baseTextStyle(), rule);
    auto text = decorateText(_inlineTextPlanner.render(node, headingTextStyle), rule, headingTextStyle);
    const auto blockStyle = context.resolvedBlockStyle(rule);
    if (rule.lineFill().has_value()) {
        return RenderBlock{
            BlockKind::FilledLine,
            std::move(text),
            std::move(blockStyle),
            rule.lineFill()->withBase(headingTextStyle.color(), headingTextStyle.attributes())};
    }
    return RenderBlock{BlockKind::Paragraph, std::move(text), std::move(blockStyle)};
}

auto RenderBlockFactory::horizontalRule(const PlanningContext &context) const -> RenderBlock {
    const auto rule = _styleResolver.horizontalRuleRule();
    const auto ruleTextStyle = context.resolvedTextStyle(_styleResolver.baseTextStyle(), rule);
    return RenderBlock{
        resolvedDecoration(rule.prefix(), ruleTextStyle),
        resolvedDecoration(rule.suffix(), ruleTextStyle),
        context.resolvedBlockStyle(rule),
        rule.lineFill().has_value() ? rule.lineFill()->withBase(ruleTextStyle.color(), ruleTextStyle.attributes())
                                    : Char{U'-', ruleTextStyle}};
}

auto RenderBlockFactory::decorateText(String text, const StyleRule &blockStyle, const CharStyle &textStyle) const
    -> String {
    if (!blockStyle.prefix().has_value() && !blockStyle.suffix().has_value()) {
        return text;
    }
    auto decorated = cterm::impl::StringBuilder{};
    auto reservedSize = text.size();
    if (blockStyle.prefix().has_value()) {
        reservedSize += blockStyle.prefix()->size();
    }
    if (blockStyle.suffix().has_value()) {
        reservedSize += blockStyle.suffix()->size();
    }
    decorated.reserve(reservedSize);
    if (blockStyle.prefix().has_value()) {
        decorated.appendWithBaseStyle(*blockStyle.prefix(), textStyle);
    }
    decorated.append(text);
    if (blockStyle.suffix().has_value()) {
        decorated.appendWithBaseStyle(*blockStyle.suffix(), textStyle);
    }
    return decorated.takeString();
}

auto RenderBlockFactory::resolvedDecoration(const std::optional<String> &decoration, const CharStyle &textStyle) const
    -> std::optional<String> {
    if (!decoration.has_value()) {
        return std::nullopt;
    }
    auto result = cterm::impl::StringBuilder{};
    result.appendWithBaseStyle(*decoration, textStyle);
    return result.takeString();
}

}
