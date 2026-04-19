// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "InlineTextPlanner.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

InlineTextPlanner::InlineTextPlanner(const PlanningStyleResolver &styleResolver) : _styleResolver{styleResolver} {
}

auto InlineTextPlanner::render(const TextNode &node, const CharStyle &style) const -> String {
    auto text = cterm::impl::StringBuilder{};
    text.reserve(node.estimatedInlineTextCapacity());
    appendTo(node, style, text);
    return text.takeString();
}

void InlineTextPlanner::appendTo(const TextNode &node, const CharStyle &style, cterm::impl::StringBuilder &text) const {
    switch (node.type()) {
    case TextNode::Type::Text:
    case TextNode::Type::Unsupported:
    case TextNode::Type::Error:
        text.appendStyled(node.text(), style);
        return;
    case TextNode::Type::LineBreak:
        text.append(Char{U'\n', style});
        return;
    default:
        if (const auto role = PlanningStyleResolver::inlineRole(node.type()); role.has_value()) {
            const auto inlineRule = _styleResolver.blockRule(node, *role);
            appendChildren(node, style.withOverlay(inlineRule.textStyle()), text);
            return;
        }
        appendChildren(node, style, text);
        return;
    }
}

void InlineTextPlanner::appendChildren(
    const TextNode &node, const CharStyle &style, cterm::impl::StringBuilder &text) const {
    for (const auto &child : node.children()) {
        if (child) {
            appendTo(*child, style, text);
        }
    }
}

}
