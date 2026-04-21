// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "HtmlAttribute.hpp"
#include "HtmlParserFrame.hpp"
#include "HtmlTagInfo.hpp"
#include "HtmlTokenizer.hpp"

#include "../TextNode.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
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
    using Attribute = HtmlAttribute;
    using Frame = HtmlParserFrame;
    using TagInfo = HtmlTagInfo;
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
