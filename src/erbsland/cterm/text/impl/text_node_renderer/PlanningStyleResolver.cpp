// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "PlanningStyleResolver.hpp"


#include <algorithm>


namespace erbsland::cterm::text::impl::text_node_renderer {


PlanningStyleResolver::PlanningStyleResolver(StyleConstPtr style) :
    _style{style != nullptr ? std::move(style) : Style::defaultStyle()} {
}


auto PlanningStyleResolver::baseTextStyle() const noexcept -> const CharStyle & {
    return _style->baseTextStyle();
}


auto PlanningStyleResolver::blockRule(
    const TextNode &node,
    const StyleRole role,
    const StyleListKind listKind,
    const std::optional<int> levelOverride) const -> StyleRule {
    auto level = levelOverride;
    if (!level.has_value() && (node.type() == TextNode::Type::Heading || TextNode::isListNodeType(node.type()))) {
        level = std::max(node.level(), 0);
    }
    if (!node.style().has_value() || node.style()->empty()) {
        return _style->resolve(StyleSelector{role, level, listKind}, StyleMatchContext{role, level, listKind});
    }
    const auto styleTokens = splitStyleTokens(*node.style());
    return _style->resolve(
        StyleSelector{role, level, listKind},
        StyleMatchContext{role, level, listKind, std::span<const std::string>{styleTokens}});
}


auto PlanningStyleResolver::horizontalRuleRule() const -> StyleRule {
    return _style->resolve(StyleSelector::horizontalRule(), StyleMatchContext{StyleRole::HorizontalRule});
}


auto PlanningStyleResolver::inlineRole(const TextNode::Type type) -> std::optional<StyleRole> {
    switch (type) {
    case TextNode::Type::Emphasis:
        return StyleRole::Emphasis;
    case TextNode::Type::Strong:
        return StyleRole::Strong;
    case TextNode::Type::Underline:
        return StyleRole::Underline;
    case TextNode::Type::Link:
        return StyleRole::Link;
    case TextNode::Type::Code:
        return StyleRole::Code;
    default:
        return std::nullopt;
    }
}


auto PlanningStyleResolver::splitStyleTokens(const std::string_view value) -> std::vector<std::string> {
    auto tokens = std::vector<std::string>{};
    auto remaining = value;
    while (!remaining.empty()) {
        const auto start = remaining.find_first_not_of(" \t\r\n");
        if (start == std::string_view::npos) {
            break;
        }
        remaining.remove_prefix(start);
        const auto end = remaining.find_first_of(" \t\r\n");
        if (end == std::string_view::npos) {
            tokens.emplace_back(remaining);
            break;
        }
        tokens.emplace_back(remaining.substr(0, end));
        remaining.remove_prefix(end);
    }
    std::ranges::sort(tokens);
    tokens.erase(std::ranges::unique(tokens).begin(), tokens.end());
    return tokens;
}


}
