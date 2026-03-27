// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TextNodePlanningQueries.hpp"


namespace erbsland::cterm::text::impl::text_node_renderer {


auto TextNodePlanningQueries::emitsBlocks(const TextNode &node) const -> bool {
    switch (node.type()) {
    case TextNode::Type::Document:
    case TextNode::Type::Section:
    case TextNode::Type::Blockquote:
    case TextNode::Type::DefinitionList:
    case TextNode::Type::ListItem:
        for (const auto &child : node.children()) {
            if (child && emitsBlocks(*child)) {
                return true;
            }
        }
        return false;
    case TextNode::Type::BulletList:
    case TextNode::Type::NumberedList:
        for (const auto &child : node.children()) {
            if (child && child->type() == TextNode::Type::ListItem) {
                return true;
            }
        }
        return false;
    case TextNode::Type::Paragraph:
    case TextNode::Type::Heading:
    case TextNode::Type::DefinitionTerm:
    case TextNode::Type::DefinitionDescription:
    case TextNode::Type::CodeBlock:
    case TextNode::Type::HorizontalLine:
    case TextNode::Type::Unsupported:
    case TextNode::Type::Error:
        return true;
    default:
        return false;
    }
}


auto TextNodePlanningQueries::childMatchesSelection(const TextNode &node, const ChildSelection selection) const
    -> bool {
    switch (selection) {
    case ChildSelection::RenderableBlocks:
        return emitsBlocks(node);
    case ChildSelection::ListItems:
        return node.type() == TextNode::Type::ListItem;
    }
    return false;
}


auto TextNodePlanningQueries::findChildRange(const TextNode &node, const ChildSelection selection) const
    -> std::optional<ChildRange> {
    auto first = std::optional<std::size_t>{};
    auto last = std::optional<std::size_t>{};
    for (auto index = std::size_t{0}; index < node.children().size(); ++index) {
        const auto &child = node.children()[index];
        if (!child || !childMatchesSelection(*child, selection)) {
            continue;
        }
        if (!first.has_value()) {
            first = index;
        }
        last = index;
    }
    if (!first.has_value() || !last.has_value()) {
        return std::nullopt;
    }
    return ChildRange{.first = *first, .last = *last};
}


}
