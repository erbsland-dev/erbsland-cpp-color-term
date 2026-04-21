// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "HtmlParser.hpp"

#include "../../impl/TextUtil.hpp"
#include "../../impl/U8Buffer.hpp"

#include <cstddef>
#include <utility>

namespace erbsland::cterm::text::impl {

using namespace cterm::impl;

HtmlParser::HtmlParser(const std::string_view html) : _tokenizer(html) {
}

auto HtmlParser::parse() -> TextNodePtr {
    _root = TextNode::createDocument();
    _current = _root;
    _currentToken = {};
    _nextToken = {};
    _frames.clear();
    _suppressedFrameDepth = 0;
    _preservedWhitespaceFrameDepth = 0;
    _openListLevel = 0;
    auto generator = _tokenizer.tokenize();
    loadNextToken(generator);
    while (_nextToken.type != HtmlTokenType::End) {
        advanceToken(generator);
        switch (_currentToken.type) {
        case HtmlTokenType::Text: {
            handleText(std::move(_currentToken.value));
            break;
        }
        case HtmlTokenType::TagOpen: {
            auto tagName = toLowerAscii(_currentToken.value);
            auto attributes = parseAttributes(generator);
            handleOpenTag(std::move(tagName), attributes);
            break;
        }
        case HtmlTokenType::TagClose:
            handleCloseTag(toLowerAscii(_currentToken.value));
            break;
        case HtmlTokenType::End:
        case HtmlTokenType::DocType:
        case HtmlTokenType::AttributeName:
        case HtmlTokenType::AttributeValue:
            break;
        }
    }
    closeFramesTo(0);
    return _root;
}

void HtmlParser::loadNextToken(TokenGenerator &generator) {
    if (auto token = generator.next()) {
        _nextToken = std::move(*token);
        return;
    }
    _nextToken = {};
}

void HtmlParser::advanceToken(TokenGenerator &generator) {
    swap(_currentToken, _nextToken);
    loadNextToken(generator);
}

auto HtmlParser::parseAttributes(TokenGenerator &generator) -> Attributes {
    auto attributes = Attributes{};
    while (_nextToken.type == HtmlTokenType::AttributeName) {
        advanceToken(generator);
        auto attribute = Attribute{toLowerAscii(_currentToken.value)};
        if (_nextToken.type == HtmlTokenType::AttributeValue) {
            advanceToken(generator);
            attribute.setValue(std::move(_currentToken.value));
        }
        attributes.emplace_back(std::move(attribute));
    }
    return attributes;
}

void HtmlParser::handleText(std::u32string text) {
    if (isSuppressed()) {
        return;
    }

    const auto preserveWhitespace = isPreservingWhitespace() || currentNodeType() == TextNode::Type::CodeBlock;
    auto whitespaceOnly = false;
    if (!preserveWhitespace) {
        whitespaceOnly = normalizeWhitespaceInPlace(text);
    }
    if (text.empty()) {
        return;
    }
    if (!preserveWhitespace && whitespaceOnly) {
        if (!TextNode::isInlineNodeType(currentNodeType())) {
            if (!currentNodeHasChildren()) {
                return;
            }
            if (_nextToken.type == HtmlTokenType::TagClose) {
                return;
            }
            const auto *nextTagInfo =
                _nextToken.type == HtmlTokenType::TagOpen ? findTagInfo(toLowerAscii(_nextToken.value)) : nullptr;
            if (nextTagInfo != nullptr && nextTagInfo->isBlock()) {
                return;
            }
        }
    }

    ensureTextContainer();
    addNode(TextNode::createText(std::move(text)));
}

void HtmlParser::handleOpenTag(std::u32string tagName, const Attributes &attributes) {
    if (isSuppressed()) {
        pushTransparentFrame(std::move(tagName), true);
        return;
    }
    const auto *tagInfo = findTagInfo(tagName);
    const auto isBlock = tagInfo != nullptr && tagInfo->isBlock();
    const auto isPlaceholder = tagInfo != nullptr && !tagInfo->placeholderText().empty();
    const auto isTransparent = tagInfo != nullptr && tagInfo->transparent();
    const auto suppressSubtree = tagInfo != nullptr && tagInfo->suppressSubtree();

    if (tagName == U"li") {
        closeCurrentListItem();
        ensureImplicitListForItem();
        closeForBlockStart();
    } else if (tagName == U"dt" || tagName == U"dd") {
        closeCurrentDefinitionEntry();
        ensureImplicitDefinitionList();
        closeForBlockStart();
    } else if (isBlock) {
        closeForBlockStart();
    } else if (!isPlaceholder && !suppressSubtree && !isTransparent) {
        ensureTextContainer();
    }

    if (isPlaceholder) {
        auto node = TextNode::createUnsupported(std::u32string{tagInfo->placeholderText()});
        applyAttributes(node, tagName, attributes);
        addNode(node);
        if (suppressSubtree) {
            pushTransparentFrame(std::move(tagName), true);
        }
        return;
    }
    if (suppressSubtree) {
        pushTransparentFrame(std::move(tagName), true);
        return;
    }
    if (isTransparent) {
        pushTransparentFrame(std::move(tagName));
        return;
    }

    auto node = createNodeForTag(tagName, countOpenListLevels());
    if (!node) {
        pushTransparentFrame(std::move(tagName));
        return;
    }
    applyAttributes(node, tagName, attributes);

    if (node->type() == TextNode::Type::LineBreak || node->type() == TextNode::Type::HorizontalLine) {
        addNode(node);
        return;
    }
    pushNodeFrame(std::move(tagName), std::move(node), tagName == U"pre");
}

void HtmlParser::handleCloseTag(const std::u32string_view tagName) {
    for (auto index = _frames.size(); index > 0; --index) {
        if (_frames[index - 1].tagName() == tagName) {
            closeFramesTo(index - 1);
            return;
        }
    }
}

void HtmlParser::addNode(const TextNodePtr &node) {
    if (!node || !_current) {
        return;
    }
    node->setParent(_current);
    _current->addChild(node);
}

void HtmlParser::pushNodeFrame(std::u32string tagName, TextNodePtr node, const bool preserveWhitespace) {
    addNode(node);
    _frames.push_back(Frame{std::move(tagName), std::move(node), false, false, preserveWhitespace});
    updateFrameStateForPush(_frames.back());
    refreshCurrent();
}

void HtmlParser::pushTransparentFrame(
    std::u32string tagName, const bool suppressSubtree, const bool preserveWhitespace) {
    _frames.push_back(Frame{std::move(tagName), {}, true, suppressSubtree, preserveWhitespace});
    updateFrameStateForPush(_frames.back());
}

void HtmlParser::closeFramesTo(const std::size_t targetSize) {
    while (_frames.size() > targetSize) {
        updateFrameStateForPop(_frames.back());
        _frames.pop_back();
    }
    refreshCurrent();
}

void HtmlParser::refreshCurrent() noexcept {
    _current = _root;
    for (auto index = _frames.size(); index > 0; --index) {
        if (_frames[index - 1].node()) {
            _current = _frames[index - 1].node();
            return;
        }
    }
}

void HtmlParser::closeInlineFrames() {
    auto targetSize = _frames.size();
    while (targetSize > 0) {
        const auto &frame = _frames[targetSize - 1];
        if (!frame.node()) {
            targetSize -= 1;
            continue;
        }
        if (TextNode::isInlineNodeType(frame.node()->type())) {
            targetSize -= 1;
            continue;
        }
        break;
    }
    closeFramesTo(targetSize);
}

void HtmlParser::closeParagraphFrame() {
    closeInlineFrames();
    if (!_frames.empty() && _frames.back().node() && _frames.back().node()->type() == TextNode::Type::Paragraph) {
        closeFramesTo(_frames.size() - 1);
    }
}

void HtmlParser::closeCurrentListItem() {
    closeParagraphFrame();
    closeInlineFrames();
    if (!_frames.empty() && _frames.back().node() && _frames.back().node()->type() == TextNode::Type::ListItem) {
        closeFramesTo(_frames.size() - 1);
    }
}

void HtmlParser::closeCurrentDefinitionEntry() {
    closeParagraphFrame();
    closeInlineFrames();
    if (_frames.empty() || !_frames.back().node()) {
        return;
    }
    const auto type = _frames.back().node()->type();
    if (type == TextNode::Type::DefinitionTerm || type == TextNode::Type::DefinitionDescription) {
        closeFramesTo(_frames.size() - 1);
    }
}

void HtmlParser::closeForBlockStart() {
    closeParagraphFrame();
    closeInlineFrames();
}

void HtmlParser::ensureImplicitListForItem() {
    closeInlineFrames();
    if (currentNodeType() == TextNode::Type::BulletList || currentNodeType() == TextNode::Type::NumberedList) {
        return;
    }
    pushNodeFrame(U"ul", TextNode::createBulletList(countOpenListLevels()));
}

void HtmlParser::ensureImplicitDefinitionList() {
    closeInlineFrames();
    if (currentNodeType() == TextNode::Type::DefinitionList) {
        return;
    }
    pushNodeFrame(U"dl", TextNode::createDefinitionList());
}

void HtmlParser::ensureTextContainer() {
    while (true) {
        const auto type = currentNodeType();
        if (TextNode::isTextContainerType(type)) {
            return;
        }
        if (type == TextNode::Type::BulletList || type == TextNode::Type::NumberedList) {
            pushNodeFrame(U"li", TextNode::createListItem());
            continue;
        }
        if (type == TextNode::Type::DefinitionList) {
            pushNodeFrame(U"dd", TextNode::createDefinitionDescription());
            continue;
        }
        pushNodeFrame(U"p", TextNode::createParagraph());
        return;
    }
}

void HtmlParser::applyAttributes(
    const TextNodePtr &node, const std::u32string_view tagName, const Attributes &attributes) {
    if (!node) {
        return;
    }
    for (const auto &attribute : attributes) {
        if (attribute.name() == U"id") {
            node->setIdentifier(U8Buffer<const char>::encodeString(attribute.value()));
            continue;
        }
        if (attribute.name() == U"class") {
            node->setStyle(U8Buffer<const char>::encodeString(attribute.value()));
            continue;
        }
        if (attribute.name() == U"href" && tagName == U"a") {
            node->setData(U8Buffer<const char>::encodeString(attribute.value()));
        }
    }
}

auto HtmlParser::isSuppressed() const noexcept -> bool {
    return _suppressedFrameDepth > 0;
}

auto HtmlParser::isPreservingWhitespace() const noexcept -> bool {
    return _preservedWhitespaceFrameDepth > 0;
}

auto HtmlParser::currentNodeType() const noexcept -> TextNode::Type {
    if (!_current) {
        return TextNode::Type::Document;
    }
    return _current->type();
}

auto HtmlParser::currentNodeHasChildren() const noexcept -> bool {
    return _current && _current->hasChildren();
}

auto HtmlParser::countOpenListLevels() const noexcept -> TextNode::Level {
    return _openListLevel;
}

void HtmlParser::updateFrameStateForPush(const Frame &frame) noexcept {
    if (frame.suppressSubtree()) {
        _suppressedFrameDepth += 1;
    }
    if (frame.preserveWhitespace()) {
        _preservedWhitespaceFrameDepth += 1;
    }
    if (frame.node() && TextNode::isListNodeType(frame.node()->type())) {
        _openListLevel += 1;
    }
}

void HtmlParser::updateFrameStateForPop(const Frame &frame) noexcept {
    if (frame.suppressSubtree()) {
        _suppressedFrameDepth -= 1;
    }
    if (frame.preserveWhitespace()) {
        _preservedWhitespaceFrameDepth -= 1;
    }
    if (frame.node() && TextNode::isListNodeType(frame.node()->type())) {
        _openListLevel -= 1;
    }
}

auto HtmlParser::createNodeForTag(const std::u32string_view tagName, const TextNode::Level listLevel) -> TextNodePtr {
    if (const auto *tagInfo = findTagInfo(tagName)) {
        if (tagInfo->usesListLevel()) {
            return std::make_shared<TextNode>(tagInfo->nodeType(), listLevel);
        }
        if (tagInfo->hasNodeType()) {
            if (tagInfo->nodeType() == TextNode::Type::Heading) {
                return TextNode::createHeading(tagInfo->level());
            }
            return TextNode::create(tagInfo->nodeType());
        }
    }
    return {};
}

auto HtmlParser::findTagInfo(const std::u32string_view tagName) -> const TagInfo * {
    if (const auto entry = tagInfoMap().find(tagName); entry != tagInfoMap().end()) {
        return &entry->second;
    }
    return nullptr;
}

auto HtmlParser::tagInfoMap() -> const TagInfoMap & {
    using NB = TagInfo::NodeBehavior;
    using LM = TagInfo::LevelMode;
    using SB = TagInfo::SubtreeBehavior;
    using TR = TagInfo::Transparency;

    static const auto cTagInfoMap = TagInfoMap{
        {U"section", TagInfo{TextNode::Type::Section, NB::Block}},
        {U"div", TagInfo{TextNode::Type::Section, NB::Block}},
        {U"blockquote", TagInfo{TextNode::Type::Blockquote, NB::Block}},
        {U"p", TagInfo{TextNode::Type::Paragraph, NB::Block}},
        {U"span", TagInfo{TextNode::Type::Span}},
        {U"br", TagInfo{TextNode::Type::LineBreak}},
        {U"hr", TagInfo{TextNode::Type::HorizontalLine, NB::Block}},
        {U"ul", TagInfo{TextNode::Type::BulletList, LM::UseListLevel, NB::Block}},
        {U"ol", TagInfo{TextNode::Type::NumberedList, LM::UseListLevel, NB::Block}},
        {U"li", TagInfo{TextNode::Type::ListItem, NB::Block}},
        {U"a", TagInfo{TextNode::Type::Link}},
        {U"b", TagInfo{TextNode::Type::Strong}},
        {U"strong", TagInfo{TextNode::Type::Strong}},
        {U"i", TagInfo{TextNode::Type::Emphasis}},
        {U"em", TagInfo{TextNode::Type::Emphasis}},
        {U"u", TagInfo{TextNode::Type::Underline}},
        {U"code", TagInfo{TextNode::Type::Code}},
        {U"pre", TagInfo{TextNode::Type::CodeBlock, NB::Block}},
        {U"h1", TagInfo{TextNode::Type::Heading, 1, NB::Block}},
        {U"h2", TagInfo{TextNode::Type::Heading, 2, NB::Block}},
        {U"h3", TagInfo{TextNode::Type::Heading, 3, NB::Block}},
        {U"h4", TagInfo{TextNode::Type::Heading, 4, NB::Block}},
        {U"h5", TagInfo{TextNode::Type::Heading, 5, NB::Block}},
        {U"h6", TagInfo{TextNode::Type::Heading, 6, NB::Block}},
        {U"dl", TagInfo{TextNode::Type::DefinitionList, NB::Block}},
        {U"dt", TagInfo{TextNode::Type::DefinitionTerm, NB::Block}},
        {U"dd", TagInfo{TextNode::Type::DefinitionDescription, NB::Block}},
        {U"img", TagInfo{U"image"}},
        {U"table", TagInfo{U"table", SB::Suppress}},
        {U"form", TagInfo{U"form", SB::Suppress}},
        {U"svg", TagInfo{U"svg", SB::Suppress}},
        {U"head", TagInfo{TR::Transparent, SB::Suppress}},
        {U"title", TagInfo{TR::Transparent, SB::Suppress}},
        {U"script", TagInfo{TR::Transparent, SB::Suppress}},
        {U"style", TagInfo{TR::Transparent, SB::Suppress}},
        {U"link", TagInfo{TR::Transparent, SB::Suppress}},
        {U"meta", TagInfo{TR::Transparent, SB::Suppress}},
        {U"base", TagInfo{TR::Transparent, SB::Suppress}},
        {U"noscript", TagInfo{TR::Transparent, SB::Suppress}},
        {U"html", TagInfo{TR::Transparent}},
        {U"body", TagInfo{TR::Transparent}},
        {U"tr", TagInfo{TR::Transparent}},
        {U"td", TagInfo{TR::Transparent}},
        {U"th", TagInfo{TR::Transparent}},
        {U"thead", TagInfo{TR::Transparent}},
        {U"tbody", TagInfo{TR::Transparent}},
        {U"tfoot", TagInfo{TR::Transparent}},
        {U"caption", TagInfo{TR::Transparent}},
        {U"col", TagInfo{TR::Transparent}},
        {U"colgroup", TagInfo{TR::Transparent}},
        {U"input", TagInfo{TR::Transparent}},
        {U"label", TagInfo{TR::Transparent}},
        {U"select", TagInfo{TR::Transparent}},
        {U"option", TagInfo{TR::Transparent}},
        {U"textarea", TagInfo{TR::Transparent}},
        {U"button", TagInfo{TR::Transparent}},
        {U"fieldset", TagInfo{TR::Transparent}},
        {U"legend", TagInfo{TR::Transparent}},
        {U"optgroup", TagInfo{TR::Transparent}},
        {U"details", TagInfo{TR::Transparent}},
        {U"summary", TagInfo{TR::Transparent}},
        {U"figure", TagInfo{TR::Transparent}},
        {U"figcaption", TagInfo{TR::Transparent}},
        {U"mark", TagInfo{TR::Transparent}},
        {U"ruby", TagInfo{TR::Transparent}},
        {U"rt", TagInfo{TR::Transparent}},
        {U"rp", TagInfo{TR::Transparent}},
        {U"time", TagInfo{TR::Transparent}},
        {U"meter", TagInfo{TR::Transparent}},
        {U"progress", TagInfo{TR::Transparent}},
        {U"canvas", TagInfo{TR::Transparent}},
        {U"math", TagInfo{TR::Transparent}},
        {U"iframe", TagInfo{TR::Transparent}},
        {U"embed", TagInfo{TR::Transparent}},
        {U"object", TagInfo{TR::Transparent}},
        {U"video", TagInfo{TR::Transparent}},
        {U"audio", TagInfo{TR::Transparent}},
        {U"source", TagInfo{TR::Transparent}},
        {U"track", TagInfo{TR::Transparent}},
        {U"doctype", TagInfo{TR::Transparent}},
    };
    return cTagInfoMap;
}

}
