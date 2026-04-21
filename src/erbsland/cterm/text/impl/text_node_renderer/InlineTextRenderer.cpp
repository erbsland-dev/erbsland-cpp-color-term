// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "InlineTextRenderer.hpp"

#include "StringBuilderWithWhitespaceState.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

auto InlineTextRenderer::render(const TextNode &node, const CharStyle &style, const WhitespaceMode whitespaceMode)
    -> String {
    auto text = StringBuilderWithWhitespaceState{};
    text.resetLineStart();
    appendTo(node, style, text, whitespaceMode);
    return text.takeString();
}

void InlineTextRenderer::appendTo(
    const TextNode &node,
    const CharStyle &style,
    StringBuilderWithWhitespaceState &text,
    const WhitespaceMode whitespaceMode) {
    switch (node.type().raw()) {
    case TextNode::Type::Text:
    case TextNode::Type::Unsupported:
    case TextNode::Type::Error:
        if (whitespaceMode == WhitespaceMode::TrimOuter) {
            text.appendTrimmed(node.text(), style);
        } else {
            text.appendStyled(node.text(), style);
        }
        return;
    case TextNode::Type::LineBreak:
        text.resetLineStart();
        text.append(Char{U'\n', style});
        return;
    default:
        if (node.type().renderClass() == TextNodeType::RenderClass::Inline) {
            const auto role = node.type().toStyleRole();
            if (!role.has_value()) {
                appendChildren(node, style, text, whitespaceMode);
                return;
            }
            const auto inlineRule = _styleResolver.blockRule(node, *role);
            const auto inlineStyle = style.withOverlay(inlineRule.textStyle());
            if (whitespaceMode == WhitespaceMode::TrimOuter) {
                text.appendDecorationTrimmed(inlineRule.prefix(), inlineStyle);
            } else {
                text.appendDecoration(inlineRule.prefix(), inlineStyle);
            }
            appendChildren(node, inlineStyle, text, whitespaceMode);
            if (whitespaceMode == WhitespaceMode::TrimOuter) {
                text.appendDecorationTrimmed(inlineRule.suffix(), inlineStyle);
            } else {
                text.appendDecoration(inlineRule.suffix(), inlineStyle);
            }
            return;
        }
        appendChildren(node, style, text, whitespaceMode);
        return;
    }
}

void InlineTextRenderer::appendChildren(
    const TextNode &node,
    const CharStyle &style,
    StringBuilderWithWhitespaceState &text,
    const WhitespaceMode whitespaceMode) {
    for (const auto &child : node.children()) {
        if (child) {
            appendTo(*child, style, text, whitespaceMode);
        }
    }
}

}
