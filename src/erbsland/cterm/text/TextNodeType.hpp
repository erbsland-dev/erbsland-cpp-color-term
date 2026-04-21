// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "StyleRole.hpp"

#include <cstdint>
#include <optional>
#include <string_view>

namespace erbsland::cterm::text {

/// The type of text node.
class TextNodeType {
public:
    /// The type of text node.
    enum Value : uint8_t {
        // structural nodes
        Document,              ///< The root node of the text tree (children).
        Paragraph,             ///< A paragraph of text (children).
        Section,               ///< A section of text (children).
        Blockquote,            ///< A blockquote (children).
        LineBreak,             ///< A line break (no content).
        Heading,               ///< A heading/title (inline nodes, level).
        BulletList,            ///< A bullet list (list items, level).
        NumberedList,          ///< A list with numbers (list items, level).
        ListItem,              ///< A single list item (children).
        DefinitionList,        ///< A list of definitions (definition terms, definition descriptions).
        DefinitionTerm,        ///< A definition term.
        DefinitionDescription, ///< A definition description.
        CodeBlock,             ///< A block of code (children, data=language).
        HorizontalLine,        ///< A horizontal line (no content).

        // inline nodes
        Text,      ///< Inline text (no children, text)
        Emphasis,  ///< Emphasized/Italic text (inline nodes).
        Strong,    ///< Strongly emphasized/Bold text (inline nodes).
        Underline, ///< Underlined text (inline nodes).
        Span,      ///< User defines inline text (inline nodes).
        Link,      ///< A link (inline nodes, data).
        Code,      ///< Inline code (inline nodes).

        // special
        Unsupported, ///< Node to indicate unsupported content (no children, text)
        Error,       ///< Error node (no children, text)
        None,        ///< No text node type.

        _count,
    };

    /// The render class of this type.
    enum class RenderClass : uint8_t {
        Empty,     ///< Does not render anything (e.g. a line break).
        Structure, ///< A structure that may or may not render output (depends on its content).
        Block,     ///< A structural block (e.g. a paragraph).
        Inline,    ///< An inline element (e.g. text, emphasis, ...).
    };

public:
    /// Create a new TextNodeType from a value.
    constexpr TextNodeType(const Value value) : _value{value} {}

    // defaults
    TextNodeType() = default;
    ~TextNodeType() = default;
    TextNodeType(const TextNodeType &) = default;
    auto operator=(const TextNodeType &) -> TextNodeType & = default;

public: // operators
    auto operator==(const TextNodeType &other) const noexcept -> bool { return _value == other._value; }
    auto operator!=(const TextNodeType &other) const noexcept -> bool { return !(*this == other); }
    auto operator==(const Value value) const noexcept -> bool { return _value == value; }
    auto operator!=(const Value value) const noexcept -> bool { return !(*this == value); }

public:
    /// Return the raw value of the text node type.
    [[nodiscard]] auto raw() const noexcept -> Value { return _value; }
    /// Return a string for this type.
    [[nodiscard]] auto toString() const -> std::string_view;
    /// Get the render class for this type.
    [[nodiscard]] auto renderClass() const noexcept -> RenderClass;
    /// Get the matching style role for this type, if any.
    [[nodiscard]] auto toStyleRole() const -> std::optional<StyleRole>;

private:
    Value _value{None}; ///< The value.
};

}
