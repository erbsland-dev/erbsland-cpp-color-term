// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TextNode.hpp"

#include "../impl/U8Buffer.hpp"

#include <format>
#include <string_view>

namespace erbsland::cterm::text {

TextNode::TextNode(
    const Type type,
    std::optional<std::u32string> text,
    std::optional<std::string> identifier,
    std::optional<std::string> style,
    std::optional<std::string> data,
    const Level level) noexcept :
    _type{type},
    _text{std::move(text)},
    _identifier{std::move(identifier)},
    _style{std::move(style)},
    _data{std::move(data)},
    _level{level} {
}

auto TextNode::create(const Type type) noexcept -> TextNodePtr {
    switch (type) {
    case Type::Heading:
    case Type::BulletList:
    case Type::NumberedList:
        return std::make_shared<TextNode>(type, 0);
    case Type::Link:
    case Type::CodeBlock:
        return std::make_shared<TextNode>(type, std::nullopt, std::nullopt, std::nullopt, std::string{}, 0);
    case Type::Text:
    case Type::Unsupported:
    case Type::Error:
        return std::make_shared<TextNode>(type, std::u32string{});
    default:
        return std::make_shared<TextNode>(type);
    }
}

auto TextNode::createText(std::u32string text) noexcept -> TextNodePtr {
    return std::make_shared<TextNode>(Type::Text, std::move(text));
}

auto TextNode::createDocument() noexcept -> TextNodePtr {
    return create(Type::Document);
}
auto TextNode::createSection() noexcept -> TextNodePtr {
    return create(Type::Section);
}

auto TextNode::createBlockquote() noexcept -> TextNodePtr {
    return create(Type::Blockquote);
}

auto TextNode::createHorizontalLine() noexcept -> TextNodePtr {
    return create(Type::HorizontalLine);
}

auto TextNode::createEmphasis() noexcept -> TextNodePtr {
    return create(Type::Emphasis);
}

auto TextNode::createStrong() noexcept -> TextNodePtr {
    return create(Type::Strong);
}

auto TextNode::createUnderline() noexcept -> TextNodePtr {
    return create(Type::Underline);
}

auto TextNode::createSpan() noexcept -> TextNodePtr {
    return create(Type::Span);
}

auto TextNode::createLink(std::string url) noexcept -> TextNodePtr {
    return std::make_shared<TextNode>(Type::Link, std::nullopt, std::nullopt, std::nullopt, std::move(url), 0);
}

auto TextNode::createCode() noexcept -> TextNodePtr {
    return create(Type::Code);
}

auto TextNode::createUnsupported(std::u32string text) noexcept -> TextNodePtr {
    return std::make_shared<TextNode>(Type::Unsupported, std::move(text));
}

auto TextNode::isInlineNodeType(const Type type) noexcept -> bool {
    return type == Type::Emphasis || type == Type::Strong || type == Type::Underline || type == Type::Span ||
        type == Type::Link || type == Type::Code;
}

auto TextNode::isTextContainerType(const Type type) noexcept -> bool {
    return type == Type::Paragraph || type == Type::Heading || type == Type::CodeBlock || isInlineNodeType(type);
}

auto TextNode::isListNodeType(const Type type) noexcept -> bool {
    return type == Type::BulletList || type == Type::NumberedList;
}

auto TextNode::toDiagnosticTree(const std::size_t indent) const noexcept -> std::vector<std::string> {
    auto result = std::vector<std::string>{};
    appendDiagnosticTree(result, *this, 0, indent);
    return result;
}

auto TextNode::estimatedInlineTextCapacity() const noexcept -> std::size_t {
    switch (type()) {
    case Type::Text:
    case Type::Unsupported:
    case Type::Error:
        return text().size();
    case Type::LineBreak:
        return 1;
    default:
        break;
    }
    auto result = std::size_t{0};
    for (const auto &child : children()) {
        if (child) {
            result += child->estimatedInlineTextCapacity();
        }
    }
    return result;
}

auto TextNode::createParagraph() noexcept -> TextNodePtr {
    return create(Type::Paragraph);
}

auto TextNode::createLineBreak() noexcept -> TextNodePtr {
    return create(Type::LineBreak);
}

auto TextNode::createHeading(const Level level) noexcept -> TextNodePtr {
    return std::make_shared<TextNode>(Type::Heading, level);
}

auto TextNode::createBulletList(Level level) noexcept -> TextNodePtr {
    return std::make_shared<TextNode>(Type::BulletList, level);
}

auto TextNode::createNumberedList(Level level) noexcept -> TextNodePtr {
    return std::make_shared<TextNode>(Type::NumberedList, level);
}

auto TextNode::createListItem() noexcept -> TextNodePtr {
    return create(Type::ListItem);
}

auto TextNode::createDefinitionList() noexcept -> TextNodePtr {
    return create(Type::DefinitionList);
}

auto TextNode::createDefinitionTerm() noexcept -> TextNodePtr {
    return create(Type::DefinitionTerm);
}

auto TextNode::createDefinitionDescription() noexcept -> TextNodePtr {
    return create(Type::DefinitionDescription);
}

auto TextNode::createCodeBlock(std::string language) noexcept -> TextNodePtr {
    return std::make_shared<TextNode>(
        Type::CodeBlock, std::nullopt, std::nullopt, std::nullopt, std::move(language), 0);
}

auto TextNode::text() const noexcept -> const std::u32string & {
    if (!_text.has_value()) {
        static const auto cEmptyText = std::u32string{};
        return cEmptyText;
    }
    return _text.value();
}

auto TextNode::identifier() const noexcept -> const std::optional<std::string> & {
    return _identifier;
}

void TextNode::setIdentifier(std::string identifier) noexcept {
    _identifier = std::move(identifier);
}

auto TextNode::style() const noexcept -> const std::optional<std::string> & {
    return _style;
}

void TextNode::setStyle(std::string style) noexcept {
    _style = std::move(style);
}

auto TextNode::data() const noexcept -> const std::optional<std::string> & {
    return _data;
}

void TextNode::setData(std::string data) noexcept {
    _data = std::move(data);
}

auto TextNode::level() const noexcept -> Level {
    return _level;
}

void TextNode::setLevel(Level level) noexcept {
    _level = level;
}

auto TextNode::children() const noexcept -> const std::vector<TextNodePtr> & {
    if (_children.has_value()) {
        return _children.value();
    }
    static const std::vector<TextNodePtr> emptyChildren;
    return emptyChildren;
}

auto TextNode::hasChildren() const noexcept -> bool {
    return _children.has_value() && !_children.value().empty();
}

auto TextNode::hasParent() const noexcept -> bool {
    return !_parent.expired();
}

auto TextNode::parent() const noexcept -> TextNodePtr {
    if (_parent.expired()) {
        return {};
    }
    return _parent.lock();
}

void TextNode::setParent(const TextNodePtr &parent) noexcept {
    _parent = parent;
}

void TextNode::addChild(TextNodePtr child) noexcept {
    if (!_children.has_value()) {
        _children = std::vector<TextNodePtr>{};
    }
    _children.value().emplace_back(std::move(child));
}

void TextNode::appendDiagnosticTree(
    std::vector<std::string> &lines, const TextNode &node, const std::size_t depth, const std::size_t indent) noexcept {
    lines.push_back(renderDiagnosticLine(node, depth, indent));
    for (const auto &child : node.children()) {
        if (!child) {
            continue;
        }
        appendDiagnosticTree(lines, *child, depth + 1, indent);
    }
}

auto TextNode::renderDiagnosticLine(const TextNode &node, const std::size_t depth, const std::size_t indent) noexcept
    -> std::string {
    auto line = std::string(depth * indent, ' ');
    line += typeName(node.type());
    if (node.level() != 0) {
        line += std::format(" level={}", node.level());
    }
    if (node.identifier().has_value() && !node.identifier()->empty()) {
        line += std::format(" id=\"{}\"", escapeText(node.identifier().value()));
    }
    if (node.style().has_value() && !node.style()->empty()) {
        line += std::format(" style=\"{}\"", escapeText(node.style().value()));
    }
    if (node.data().has_value() && !node.data()->empty()) {
        line += std::format(" data=\"{}\"", escapeText(node.data().value()));
    }
    if (!node.text().empty()) {
        line += std::format(" text=\"{}\"", escapeText(renderTextValue(node.text())));
    }
    return line;
}

auto TextNode::typeName(const Type type) noexcept -> std::string_view {
    switch (type) {
    case Type::Document:
        return "Document";
    case Type::Paragraph:
        return "Paragraph";
    case Type::Section:
        return "Section";
    case Type::Blockquote:
        return "Blockquote";
    case Type::LineBreak:
        return "LineBreak";
    case Type::Heading:
        return "Heading";
    case Type::BulletList:
        return "BulletList";
    case Type::NumberedList:
        return "NumberedList";
    case Type::ListItem:
        return "ListItem";
    case Type::DefinitionList:
        return "DefinitionList";
    case Type::DefinitionTerm:
        return "DefinitionTerm";
    case Type::DefinitionDescription:
        return "DefinitionDescription";
    case Type::CodeBlock:
        return "CodeBlock";
    case Type::HorizontalLine:
        return "HorizontalLine";
    case Type::Text:
        return "Text";
    case Type::Emphasis:
        return "Emphasis";
    case Type::Strong:
        return "Strong";
    case Type::Underline:
        return "Underline";
    case Type::Span:
        return "Span";
    case Type::Link:
        return "Link";
    case Type::Code:
        return "Code";
    case Type::Unsupported:
        return "Unsupported";
    case Type::Error:
        return "Error";
    }
    return "Unknown";
}

auto TextNode::renderTextValue(const std::u32string_view text) noexcept -> std::string {
    return impl::U8Buffer<const char>::encodeString(text);
}

auto TextNode::escapeText(const std::string_view text) noexcept -> std::string {
    auto result = std::string{};
    for (const auto character : text) {
        if (character == '\\') {
            result += "\\\\";
        } else if (character == '"') {
            result += "\\\"";
        } else if (character == '\n') {
            result += "\\n";
        } else if (character == '\t') {
            result += "\\t";
        } else {
            result.push_back(character);
        }
    }
    return result;
}

}
