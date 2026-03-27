// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "HtmlTokenizer.hpp"

#include "../TextNode.hpp"

#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>


namespace erbsland::cterm::text::impl {

/// A simple HTML parser to convert a text into a `TextNode` tree.
/// -   Supports a very limited subset of HTML and happily accepts HTML fragments.
/// -   If "<!doctype...>", "<html>", "<body>", or "<head>" are present, they are ignored.
/// -   Whitespace is normalized to a single space character.
/// -   Tag and attribute names are parsed case-insensitive.
/// -   Supports these tags: "section", "p", "div", "span", "br", "hr", "ul", "ol", "li", "a",
///     "b", "i", "u", "em", "strong", "code", "pre", "blockquote", "h1", "h2", "h3", "h4", "h5", "h6",
///     "dl", "dt", "dd".
/// -   Node Mappings:
///     -   "section", "div" -> `Section`
///     -   "blockquote" -> `Blockquote`
///     -   "p" -> `Paragraph`
///     -   "span" -> `Span`
///     -   "br" -> `LineBreak`
///     -   "hr" -> `HorizontalLine`
///     -   "ul" -> `BulletList`
///     -   "ol" -> `NumberedList`
///     -   "li" -> `ListItem`
///     -   "a" -> `Link`
///     -   "b", "strong" -> `Strong`
///     -   "i", "em" -> `Emphasis`
///     -   "u" -> `Underline`
///     -   "code" -> `Code`
///     -   "pre" -> `CodeBlock`
///     -   "h1", "h2", "h3", "h4", "h5", "h6" -> `Heading`
///     -   "dl" -> `DefinitionList`
///     -   "dt" -> `DefinitionTerm`
///     -   "dd" -> `DefinitionDescription`
/// -   Unsupported but mentioned: "img", "table", "form", "svg"
///     -   Insert an `Unsupported` placeholder like `image` as text node.
/// -   Unsupported ignored tags: "tr", "td", "th", "thead", "tbody", "tfoot", "caption", "col", "colgroup",
///     "input", "label", "select", "option", "textarea", "button", "fieldset", "legend", "optgroup", "details",
///     "summary", "figure", "figcaption", "mark", "ruby", "rt", "rp", "time", "meter", "progress", "canvas",
///     "math", "iframe", "embed", "object", "video", "audio", "source", "track", "canvas", "noscript", "script",
///     "style", "link", "meta", "base", "head", "title", "body", "html", "doctype".
///     -   Unsupported tags are ignored.
/// -   Comments `<!-- ... -->` are ignored.
/// -   Parsed attributes:
///     -   "id" (any tag) -> `identifier`
///     -   "href" ("a" tag) -> `data`
///     -   "class" (any tag) -> `style`
/// -   No errors are generated, or only as a last resort. Erroneous content is gracefully resolved.
class HtmlParser final {
public:
    explicit HtmlParser(std::string_view html);

    // defaults
    ~HtmlParser() = default;
    HtmlParser(const HtmlParser &) = delete;
    HtmlParser(HtmlParser &&) = delete;
    auto operator=(const HtmlParser &) -> HtmlParser & = delete;
    auto operator=(HtmlParser &&) -> HtmlParser & = delete;

public:
    /// Parse the given HTML into a text node tree.
    [[nodiscard]] auto parse() -> TextNodePtr;

private:
    class Attribute final {
    public:
        /// Create one parsed attribute.
        /// @param name The lower-case ASCII attribute name.
        /// @param value The attribute value.
        Attribute(std::u32string name = {}, std::u32string value = {}) :
            _name{std::move(name)}, _value{std::move(value)} {}

    public:
        /// Access the lower-case ASCII attribute name.
        [[nodiscard]] auto name() const noexcept -> const std::u32string & { return _name; }
        /// Access the attribute value.
        [[nodiscard]] auto value() const noexcept -> const std::u32string & { return _value; }
        /// Replace the attribute value.
        /// @param value The new attribute value.
        void setValue(std::u32string value) { _value = std::move(value); }

    private:
        std::u32string _name;  ///< The lower-case ASCII attribute name.
        std::u32string _value; ///< The attribute value.
    };

    class Frame final {
    public:
        /// Create one parser frame.
        /// @param tagName The lower-case ASCII tag name.
        /// @param node The created node, if this frame owns one.
        /// @param transparent `true` if the tag itself has no matching node.
        /// @param suppressSubtree `true` if all content below this frame is ignored.
        /// @param preserveWhitespace `true` if text below this frame is parsed verbatim.
        Frame(
            std::u32string tagName = {},
            TextNodePtr node = {},
            const bool transparent = false,
            const bool suppressSubtree = false,
            const bool preserveWhitespace = false) :
            _tagName{std::move(tagName)},
            _node{std::move(node)},
            _transparent{transparent},
            _suppressSubtree{suppressSubtree},
            _preserveWhitespace{preserveWhitespace} {}

    public:
        /// Access the lower-case ASCII tag name.
        [[nodiscard]] auto tagName() const noexcept -> const std::u32string & { return _tagName; }
        /// Access the created node, if this frame owns one.
        [[nodiscard]] auto node() const noexcept -> const TextNodePtr & { return _node; }
        /// Access the created node, if this frame owns one.
        [[nodiscard]] auto node() noexcept -> TextNodePtr & { return _node; }
        /// Test if the tag itself has no matching node.
        [[nodiscard]] auto transparent() const noexcept -> bool { return _transparent; }
        /// Test if all content below this frame is ignored.
        [[nodiscard]] auto suppressSubtree() const noexcept -> bool { return _suppressSubtree; }
        /// Test if text below this frame is parsed verbatim.
        [[nodiscard]] auto preserveWhitespace() const noexcept -> bool { return _preserveWhitespace; }

    private:
        std::u32string _tagName;         ///< The lower-case ASCII tag name.
        TextNodePtr _node;               ///< The created node, if this frame owns one.
        bool _transparent{false};        ///< `true` if the tag itself has no matching node.
        bool _suppressSubtree{false};    ///< `true` if all content below this frame is ignored.
        bool _preserveWhitespace{false}; ///< `true` if text below this frame is parsed verbatim.
    };

    class TagInfo final {
    public:
        enum class NodeBehavior : uint8_t { Inline, Block };
        enum class LevelMode : uint8_t { Fixed, UseListLevel };
        enum class SubtreeBehavior : uint8_t { Keep, Suppress };
        enum class Transparency : uint8_t { Transparent };

        TagInfo() = default;

        /// Create one node-producing tag description.
        /// @param nodeType The mapped node type.
        /// @param nodeBehavior Whether the tag starts a block node.
        TagInfo(const TextNode::Type nodeType, const NodeBehavior nodeBehavior = NodeBehavior::Inline) noexcept :
            _nodeType{nodeType}, _hasNodeType{true}, _isBlock{nodeBehavior == NodeBehavior::Block} {}

        /// Create one node-producing tag description with an explicit level.
        /// @param nodeType The mapped node type.
        /// @param level The configured heading level.
        /// @param nodeBehavior Whether the tag starts a block node.
        TagInfo(const TextNode::Type nodeType, const TextNode::Level level, const NodeBehavior nodeBehavior) noexcept :
            _nodeType{nodeType}, _level{level}, _hasNodeType{true}, _isBlock{nodeBehavior == NodeBehavior::Block} {}

        /// Create one node-producing tag description that uses the current list level.
        /// @param nodeType The mapped node type.
        /// @param levelMode Whether the node level is derived from the open list depth.
        /// @param nodeBehavior Whether the tag starts a block node.
        TagInfo(const TextNode::Type nodeType, const LevelMode levelMode, const NodeBehavior nodeBehavior) noexcept :
            _nodeType{nodeType},
            _hasNodeType{true},
            _usesListLevel{levelMode == LevelMode::UseListLevel},
            _isBlock{nodeBehavior == NodeBehavior::Block} {}

        /// Create one placeholder tag description.
        /// @param placeholderText The placeholder text for unsupported tags.
        /// @param subtreeBehavior Whether nested content is ignored.
        explicit TagInfo(
            std::u32string_view placeholderText, const SubtreeBehavior subtreeBehavior = SubtreeBehavior::Keep) noexcept
            :
            _placeholderText{placeholderText}, _suppressSubtree{subtreeBehavior == SubtreeBehavior::Suppress} {}

        /// Create one transparent tag description.
        /// @param transparency Marker that this tag is transparent.
        /// @param subtreeBehavior Whether nested content is ignored.
        explicit TagInfo(
            const Transparency transparency, const SubtreeBehavior subtreeBehavior = SubtreeBehavior::Keep) noexcept :
            _transparent{transparency == Transparency::Transparent},
            _suppressSubtree{subtreeBehavior == SubtreeBehavior::Suppress} {}

    public:
        /// Access the mapped node type.
        [[nodiscard]] auto nodeType() const noexcept -> TextNode::Type { return _nodeType; }
        /// Access the configured heading level.
        [[nodiscard]] auto level() const noexcept -> TextNode::Level { return _level; }
        /// Access the placeholder text for unsupported tags.
        [[nodiscard]] auto placeholderText() const noexcept -> std::u32string_view { return _placeholderText; }
        /// Test if the tag creates a node.
        [[nodiscard]] auto hasNodeType() const noexcept -> bool { return _hasNodeType; }
        /// Test if the list nesting defines the node level.
        [[nodiscard]] auto usesListLevel() const noexcept -> bool { return _usesListLevel; }
        /// Test if the tag starts a block node.
        [[nodiscard]] auto isBlock() const noexcept -> bool { return _isBlock; }
        /// Test if the tag itself is ignored.
        [[nodiscard]] auto transparent() const noexcept -> bool { return _transparent; }
        /// Test if all nested content is ignored.
        [[nodiscard]] auto suppressSubtree() const noexcept -> bool { return _suppressSubtree; }

    private:
        TextNode::Type _nodeType{TextNode::Type::Document}; ///< The mapped node type.
        TextNode::Level _level{0};                          ///< The configured heading level.
        std::u32string_view _placeholderText{};             ///< Placeholder text for unsupported tags.
        bool _hasNodeType{false};                           ///< `true` if the tag creates a node.
        bool _usesListLevel{false};                         ///< `true` if the list nesting defines the node level.
        bool _isBlock{false};                               ///< `true` if the tag starts a block node.
        bool _transparent{false};                           ///< `true` if the tag itself is ignored.
        bool _suppressSubtree{false};                       ///< `true` if all nested content is ignored.
    };

    using Attributes = std::vector<Attribute>;
    using FrameList = std::vector<Frame>;
    using TagInfoMap = std::unordered_map<std::u32string_view, TagInfo>;
    using TokenGenerator = HtmlTokenizer::TokenGenerator;

private:
    void updateFrameStateForPush(const Frame &frame) noexcept;
    void updateFrameStateForPop(const Frame &frame) noexcept;
    void loadNextToken(TokenGenerator &generator);
    void advanceToken(TokenGenerator &generator);
    auto parseAttributes(TokenGenerator &generator) -> Attributes;
    void handleText(std::u32string text);
    void handleOpenTag(std::u32string tagName, const Attributes &attributes);
    void handleCloseTag(std::u32string_view tagName);
    void addNode(const TextNodePtr &node);
    void pushNodeFrame(std::u32string tagName, TextNodePtr node, bool preserveWhitespace = false);
    void pushTransparentFrame(std::u32string tagName, bool suppressSubtree = false, bool preserveWhitespace = false);
    void closeFramesTo(std::size_t targetSize);
    void refreshCurrent() noexcept;
    void closeInlineFrames();
    void closeParagraphFrame();
    void closeCurrentListItem();
    void closeCurrentDefinitionEntry();
    void closeForBlockStart();
    void ensureImplicitListForItem();
    void ensureImplicitDefinitionList();
    void ensureTextContainer();
    void applyAttributes(const TextNodePtr &node, std::u32string_view tagName, const Attributes &attributes);
    [[nodiscard]] auto isSuppressed() const noexcept -> bool;
    [[nodiscard]] auto isPreservingWhitespace() const noexcept -> bool;
    [[nodiscard]] auto currentNodeType() const noexcept -> TextNode::Type;
    [[nodiscard]] auto currentNodeHasChildren() const noexcept -> bool;
    [[nodiscard]] auto countOpenListLevels() const noexcept -> TextNode::Level;

    [[nodiscard]] static auto createNodeForTag(std::u32string_view tagName, TextNode::Level listLevel) -> TextNodePtr;
    [[nodiscard]] static auto findTagInfo(std::u32string_view tagName) -> const TagInfo *;
    [[nodiscard]] static auto tagInfoMap() -> const TagInfoMap &;

private:
    HtmlTokenizer _tokenizer;               ///< The used tokenizer.
    HtmlToken _currentToken;                ///< The current token being processed.
    HtmlToken _nextToken;                   ///< The next token used for lookahead.
    TextNodePtr _root;                      ///< The root document node.
    TextNodePtr _current;                   ///< The current node.
    FrameList _frames;                      ///< The open HTML element frames.
    int _suppressedFrameDepth = 0;          ///< The number of frames that suppress their subtree.
    int _preservedWhitespaceFrameDepth = 0; ///< The number of frames that preserve whitespace.
    TextNode::Level _openListLevel = 0;     ///< The number of open list container frames.
};

}
