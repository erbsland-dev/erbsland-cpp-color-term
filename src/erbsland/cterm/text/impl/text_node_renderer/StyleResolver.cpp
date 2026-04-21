// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StyleResolver.hpp"

#include <algorithm>
#include <utility>

namespace erbsland::cterm::text::impl::text_node_renderer {

StyleResolver::StyleResolver(StyleConstPtr style) : _style{std::move(style)} {
    if (!_style) {
        _style = Style::defaultStyle();
    }
}

auto StyleResolver::blockRule(
    const TextNode &node, const StyleRole role, const StyleListKind listKind, const std::optional<int> levelOverride)
    -> StyleRule {
    auto level = levelOverride;
    if (!level.has_value() && (node.type() == TextNode::Type::Heading || TextNode::isListNodeType(node.type()))) {
        level = std::max(node.level(), 0);
    }
    if (!node.style().has_value() || node.style()->empty()) {
        return resolveNoStyle(role, level, listKind);
    }
    return resolveWithStyleTokens(role, level, listKind, *node.style());
}

auto StyleResolver::splitStyleTokens(const std::string_view value) -> std::vector<std::string> {
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

auto StyleResolver::resolveNoStyle(const StyleRole role, const std::optional<int> level, const StyleListKind listKind)
    -> StyleRule {
    const auto existing = std::ranges::find_if(_ruleCache, [role, &level, listKind](const CacheEntry &entry) -> bool {
        return entry.role == role && entry.level == level && entry.listKind == listKind;
    });
    if (existing != _ruleCache.end()) {
        return existing->rule;
    }
    auto rule = _style->resolve(StyleSelector{role, level, listKind}, StyleMatchContext{role, level, listKind});
    _ruleCache.push_back(CacheEntry{.role = role, .level = level, .listKind = listKind, .rule = rule});
    return rule;
}

auto StyleResolver::resolveWithStyleTokens(
    const StyleRole role,
    const std::optional<int> level,
    const StyleListKind listKind,
    const std::string_view styleValue) const -> StyleRule {
    const auto styleTokens = splitStyleTokens(styleValue);
    return _style->resolve(
        StyleSelector{role, level, listKind}, StyleMatchContext{role, level, listKind, std::span{styleTokens}});
}

}
