// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace erbsland::cterm::text {

class TextNode;
using TextNodePtr = std::shared_ptr<TextNode>;
using TextNodeConstPtr = std::shared_ptr<const TextNode>;
using TextNodeWeakPtr = std::weak_ptr<TextNode>;

/// A node in the text tree.
class TextNode : public std::enable_shared_from_this<TextNode> {
public:
    /// The node type.
    enum class Type : uint8_t {
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
    };

    /// The nesting or heading level used by structural nodes.
    using Level = int;

public:
    /// Create a node with explicit metadata.
    /// @param type The node type.
    /// @param text Optional UTF-32 text content.
    /// @param identifier Optional identifier or anchor.
    /// @param style Optional style or class information.
    /// @param data Optional node-specific payload such as a link target or language.
    /// @param level The heading or nesting level.
    TextNode(
        Type type,
        std::optional<std::u32string> text,
        std::optional<std::string> identifier,
        std::optional<std::string> style,
        std::optional<std::string> data,
        Level level) noexcept;
    /// Create a node with a type and otherwise empty metadata.
    /// @param type The node type.
    explicit TextNode(const Type type) noexcept :
        TextNode(type, std::nullopt, std::nullopt, std::nullopt, std::nullopt, 0) {}
    /// Create a node with inline text content.
    /// @param type The node type.
    /// @param text The UTF-32 text content.
    /// @param level The heading or nesting level.
    TextNode(const Type type, std::u32string text, const Level level = 0) noexcept :
        TextNode(type, std::move(text), std::nullopt, std::nullopt, std::nullopt, level) {}
    /// Create a node with a type and explicit level.
    /// @param type The node type.
    /// @param level The heading or nesting level.
    TextNode(const Type type, const Level level) noexcept :
        TextNode(type, std::nullopt, std::nullopt, std::nullopt, std::nullopt, level) {}

    // defaults
    ~TextNode() = default;
    TextNode(const TextNode &) = delete;
    TextNode(TextNode &&) = delete;
    auto operator=(const TextNode &) -> TextNode & = delete;
    auto operator=(TextNode &&) -> TextNode & = delete;

public:
    /// Create a node with the given type using default metadata, empty text, and level `0`.
    /// @param type The node type to create.
    /// @return The created node.
    [[nodiscard]] static auto create(Type type) noexcept -> TextNodePtr;
    /// Create a document root node.
    /// @return The created node.
    [[nodiscard]] static auto createDocument() noexcept -> TextNodePtr;
    /// Create a paragraph node.
    /// @return The created node.
    [[nodiscard]] static auto createParagraph() noexcept -> TextNodePtr;
    /// Create a section node.
    /// @return The created node.
    [[nodiscard]] static auto createSection() noexcept -> TextNodePtr;
    /// Create a blockquote node.
    /// @return The created node.
    [[nodiscard]] static auto createBlockquote() noexcept -> TextNodePtr;
    /// Create an explicit line-break node.
    /// @return The created node.
    [[nodiscard]] static auto createLineBreak() noexcept -> TextNodePtr;
    /// Create a heading node.
    /// @param level The heading level.
    /// @return The created node.
    [[nodiscard]] static auto createHeading(Level level) noexcept -> TextNodePtr;
    /// Create a bullet list node.
    /// @param level The nesting level.
    /// @return The created node.
    [[nodiscard]] static auto createBulletList(Level level) noexcept -> TextNodePtr;
    /// Create a numbered list node.
    /// @param level The nesting level.
    /// @return The created node.
    [[nodiscard]] static auto createNumberedList(Level level) noexcept -> TextNodePtr;
    /// Create a list item node.
    /// @return The created node.
    [[nodiscard]] static auto createListItem() noexcept -> TextNodePtr;
    /// Create a definition list node.
    /// @return The created node.
    [[nodiscard]] static auto createDefinitionList() noexcept -> TextNodePtr;
    /// Create a definition term node.
    /// @return The created node.
    [[nodiscard]] static auto createDefinitionTerm() noexcept -> TextNodePtr;
    /// Create a definition description node.
    /// @return The created node.
    [[nodiscard]] static auto createDefinitionDescription() noexcept -> TextNodePtr;
    /// Create a code block node.
    /// @param language The optional language identifier.
    /// @return The created node.
    [[nodiscard]] static auto createCodeBlock(std::string language) noexcept -> TextNodePtr;
    /// Create a horizontal rule node.
    /// @return The created node.
    [[nodiscard]] static auto createHorizontalLine() noexcept -> TextNodePtr;
    /// Create a plain text node.
    /// @param text The UTF-32 text content.
    /// @return The created node.
    [[nodiscard]] static auto createText(std::u32string text) noexcept -> TextNodePtr;
    /// Create an emphasis node.
    /// @return The created node.
    [[nodiscard]] static auto createEmphasis() noexcept -> TextNodePtr;
    /// Create a strong-emphasis node.
    /// @return The created node.
    [[nodiscard]] static auto createStrong() noexcept -> TextNodePtr;
    /// Create an underline node.
    /// @return The created node.
    [[nodiscard]] static auto createUnderline() noexcept -> TextNodePtr;
    /// Create a generic span node.
    /// @return The created node.
    [[nodiscard]] static auto createSpan() noexcept -> TextNodePtr;
    /// Create a link node.
    /// @param url The link target.
    /// @return The created node.
    [[nodiscard]] static auto createLink(std::string url) noexcept -> TextNodePtr;
    /// Create an inline code node.
    /// @return The created node.
    [[nodiscard]] static auto createCode() noexcept -> TextNodePtr;
    /// Create a placeholder node for unsupported content.
    /// @param text The placeholder text to render.
    /// @return The created unsupported node.
    [[nodiscard]] static auto createUnsupported(std::u32string text) noexcept -> TextNodePtr;
    /// Test if the given node type is an inline container node.
    /// @param type The node type.
    /// @return `true` for inline container nodes, otherwise `false`.
    [[nodiscard]] static auto isInlineNodeType(Type type) noexcept -> bool;
    /// Test if the given node type can directly contain parsed text.
    /// @param type The node type.
    /// @return `true` if parsed text can be appended directly, otherwise `false`.
    [[nodiscard]] static auto isTextContainerType(Type type) noexcept -> bool;
    /// Test if the given node type is a list container.
    /// @param type The node type.
    /// @return `true` for bullet or numbered lists, otherwise `false`.
    [[nodiscard]] static auto isListNodeType(Type type) noexcept -> bool;
    /// Render this node tree into a human-readable diagnostic tree.
    /// Each entry contains one node with metadata, indented by the given number of spaces per depth level.
    /// @param indent The number of spaces for each nesting level.
    /// @return The rendered diagnostic lines.
    [[nodiscard]] auto toDiagnosticTree(std::size_t indent = 4) const noexcept -> std::vector<std::string>;
    /// Estimate the number of inline terminal characters produced by this node subtree.
    /// This is useful for reserving output storage before rendering inline content.
    /// @return The estimated inline character count.
    [[nodiscard]] auto estimatedInlineTextCapacity() const noexcept -> std::size_t;

public:
    /// Get the node type.
    /// @return The node type.
    [[nodiscard]] auto type() const noexcept -> Type { return _type; }
    /// Test if this node currently owns child nodes.
    /// @return `true` if child nodes are present.
    [[nodiscard]] auto hasChildren() const noexcept -> bool;
    /// Access the child nodes.
    /// @return The child node list.
    [[nodiscard]] auto children() const noexcept -> const std::vector<TextNodePtr> &;
    /// Test if this node has a parent.
    /// @return `true` if a parent node is set.
    [[nodiscard]] auto hasParent() const noexcept -> bool;
    /// Access the parent node.
    /// @return The parent node, or `nullptr` if there is none.
    [[nodiscard]] auto parent() const noexcept -> TextNodePtr;
    /// Set the parent node reference.
    /// @param parent The new parent node.
    void setParent(const TextNodePtr &parent) noexcept;
    /// Append one child node and update its parent reference.
    /// @param child The child node to add.
    void addChild(TextNodePtr child) noexcept;
    /// Access the stored text payload.
    /// @return The UTF-32 text content, or an empty string if none is stored.
    [[nodiscard]] auto text() const noexcept -> const std::u32string &;
    /// Access the optional identifier.
    /// @return The identifier, if set.
    [[nodiscard]] auto identifier() const noexcept -> const std::optional<std::string> &;
    /// Replace the identifier.
    /// @param identifier The new identifier.
    void setIdentifier(std::string identifier) noexcept;
    /// Access the optional style information.
    /// @return The style or class value, if set.
    [[nodiscard]] auto style() const noexcept -> const std::optional<std::string> &;
    /// Replace the optional style information.
    /// @param style The new style or class value.
    void setStyle(std::string style) noexcept;
    /// Access the optional node-specific data payload.
    /// @return The data payload, if set.
    [[nodiscard]] auto data() const noexcept -> const std::optional<std::string> &;
    /// Replace the optional node-specific data payload.
    /// @param data The new data payload.
    void setData(std::string data) noexcept;
    /// Get the heading or nesting level.
    /// @return The current level.
    [[nodiscard]] auto level() const noexcept -> Level;
    /// Set the heading or nesting level.
    /// @param level The new level.
    void setLevel(Level level) noexcept;

private:
    static void appendDiagnosticTree(
        std::vector<std::string> &lines, const TextNode &node, std::size_t depth, std::size_t indent) noexcept;
    [[nodiscard]] static auto renderDiagnosticLine(const TextNode &node, std::size_t depth, std::size_t indent) noexcept
        -> std::string;
    [[nodiscard]] static auto typeName(Type type) noexcept -> std::string_view;
    [[nodiscard]] static auto renderTextValue(std::u32string_view text) noexcept -> std::string;
    [[nodiscard]] static auto escapeText(std::string_view text) noexcept -> std::string;

private:
    Type _type{Type::Text};                            ///< The node type.
    std::optional<std::vector<TextNodePtr>> _children; ///< Child nodes.
    TextNodeWeakPtr _parent;                           ///< The parent node.
    std::optional<std::u32string> _text;               ///< The UTF-32 text content.
    std::optional<std::string> _identifier;            ///< Optional identifier/anchor.
    std::optional<std::string> _style;                 ///< Optional style/class.
    std::optional<std::string> _data;                  ///< Target URL for links/code language/etc.
    Level _level{0};                                   ///< Level of a heading/list
};

}
