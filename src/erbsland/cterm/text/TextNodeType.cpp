// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TextNodeType.hpp"

#include "Text.hpp"
#include "TextNode.hpp"

#include <array>

namespace erbsland::cterm::text {

auto TextNodeType::toString() const -> std::string_view {
    const static auto names = std::array<std::string_view, static_cast<std::size_t>(_count)>{
        "Document",   "Paragraph",      "Section",  "Blockquote",     "LineBreak",      "Heading",
        "BulletList", "NumberedList",   "ListItem", "DefinitionList", "DefinitionTerm", "DefinitionDescription",
        "CodeBlock",  "HorizontalLine", "Text",     "Emphasis",       "Strong",         "Underline",
        "Span",       "Link",           "Code",     "Unsupported",    "Error",          "None",
    };
    if (static_cast<std::size_t>(_value) >= names.size()) {
        return "Unknown";
    }
    return names[static_cast<std::size_t>(_value)];
}

auto TextNodeType::renderClass() const noexcept -> RenderClass {
    switch (_value) {
    case Document:
    case Section:
    case Blockquote:
    case DefinitionList:
    case ListItem:
    case BulletList:
    case NumberedList:
        return RenderClass::Structure;
    case Paragraph:
    case Heading:
    case DefinitionTerm:
    case DefinitionDescription:
    case CodeBlock:
    case HorizontalLine:
    case Unsupported:
    case Error:
        return RenderClass::Block;
    case Text:
    case Emphasis:
    case Strong:
    case Underline:
    case Span:
    case Link:
    case Code:
        return RenderClass::Inline;
    case None:
    case LineBreak:
    case _count: // prevent undetected additions.
        break;
    }
    return RenderClass::Empty;
}

auto TextNodeType::toStyleRole() const -> std::optional<StyleRole> {
    switch (_value) {
    case Document:
        return StyleRole::Document;
    case Paragraph:
        return StyleRole::Paragraph;
    case Heading:
        return StyleRole::Heading;
    case NumberedList:
    case BulletList:
        return StyleRole::ListContainer;
    case ListItem:
        return StyleRole::ListItem;
    case DefinitionList:
        return StyleRole::DefinitionList;
    case DefinitionTerm:
        return StyleRole::DefinitionTerm;
    case DefinitionDescription:
        return StyleRole::DefinitionDescription;
    case Blockquote:
        return StyleRole::Blockquote;
    case CodeBlock:
        return StyleRole::CodeBlock;
    case HorizontalLine:
        return StyleRole::HorizontalRule;
    case Emphasis:
        return StyleRole::Emphasis;
    case Strong:
        return StyleRole::Strong;
    case Underline:
        return StyleRole::Underline;
    case Span:
        return StyleRole::Span;
    case Link:
        return StyleRole::Link;
    case Code:
        return StyleRole::Code;
    default:
        return std::nullopt;
    }
}

}
