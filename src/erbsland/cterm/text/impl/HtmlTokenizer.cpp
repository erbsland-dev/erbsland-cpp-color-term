// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "HtmlTokenizer.hpp"


#include "../../impl/TextUtil.hpp"

#include <array>
#include <cstdint>
#include <string_view>
#include <utility>
#include <vector>


namespace erbsland::cterm::text::impl {

HtmlTokenizer::HtmlTokenizer(const std::string_view text) : BufferedTokenizer{text, 1024} {
}

auto HtmlTokenizer::tokenize() -> TokenGenerator {
    auto tokens = std::vector<HtmlTokenRef>{};
    while (!isAtEnd()) {
        if (current() == U'<') {
            resetBuffer();
            tokens.clear();
            if (tokenizeTag(tokens)) {
                for (const auto &token : tokens) {
                    co_yield materializeToken(token);
                }
                tokens.clear();
            } else {
                co_yield tokenizeBufferedLiteralTagText();
            }
        } else {
            co_yield tokenizeText();
        }
    }
    co_return;
}

auto HtmlTokenizer::tokenizeTag(std::vector<HtmlTokenRef> &tokens) -> bool {
    if (!consumeBufferedIf(U'<')) {
        return false;
    }
    if (consumeBufferedIf(U'!')) {
        return tokenizeDeclaration(tokens);
    }
    if (consumeBufferedIf(U'/')) {
        return tokenizeClosingTag(tokens);
    }
    return tokenizeOpeningTag(tokens);
}

auto HtmlTokenizer::tokenizeDeclaration(std::vector<HtmlTokenRef> &tokens) -> bool {
    const auto savedState = save();
    const auto bufferSize = _buffer.size();
    if (tokenizeComment()) {
        return true;
    }
    load(savedState);
    _buffer.resize(bufferSize);
    return tokenizeDocType(tokens);
}

auto HtmlTokenizer::tokenizeClosingTag(std::vector<HtmlTokenRef> &tokens) -> bool {
    const auto tagName = parseName();
    if (!tagName.has_value()) {
        return false;
    }
    skipWhitespace();
    if (!consumeBufferedIf(U'>')) {
        return false;
    }
    tokens.emplace_back(HtmlTokenRef{.type = HtmlTokenType::TagClose, .range = *tagName});
    return true;
}

auto HtmlTokenizer::tokenizeOpeningTag(std::vector<HtmlTokenRef> &tokens) -> bool {
    const auto tagName = parseName();
    if (!tagName.has_value()) {
        return false;
    }
    tokens.emplace_back(HtmlTokenRef{.type = HtmlTokenType::TagOpen, .range = *tagName});
    while (!isAtEnd()) {
        skipWhitespace();
        if (consumeBufferedIf(U'>')) {
            return true;
        }
        if (consumeBufferedIf(U'/')) {
            if (!consumeBufferedIf(U'>')) {
                return false;
            }
            tokens.emplace_back(HtmlTokenRef{.type = HtmlTokenType::TagClose, .range = *tagName});
            return true;
        }

        const auto attributeName = parseName();
        if (!attributeName.has_value()) {
            return false;
        }
        tokens.emplace_back(HtmlTokenRef{.type = HtmlTokenType::AttributeName, .range = *attributeName});

        skipWhitespace();
        if (!consumeBufferedIf(U'=')) {
            continue;
        }
        skipWhitespace();

        auto attributeValue = IndexRange{};
        if (!parseAttributeValue(attributeValue)) {
            return false;
        }
        tokens.emplace_back(HtmlTokenRef{.type = HtmlTokenType::AttributeValue, .range = attributeValue});
    }
    return false;
}

auto HtmlTokenizer::tokenizeText() -> HtmlToken {
    resetBuffer();
    while (!isAtEnd() && current() != U'<') {
        char32_t decodedCharacter = 0;
        if (current() == U'&' && decodeEntity(decodedCharacter)) {
            append(decodedCharacter);
            continue;
        }
        appendCurrentAndAdvance();
    }
    return bufferTextToken();
}

auto HtmlTokenizer::tokenizeBufferedLiteralTagText() -> HtmlToken {
    while (!isAtEnd() && current() != U'<') {
        char32_t decodedCharacter = 0;
        if (current() == U'&' && decodeEntity(decodedCharacter)) {
            append(decodedCharacter);
            continue;
        }
        appendCurrentAndAdvance();
    }
    return bufferTextToken();
}

auto HtmlTokenizer::tokenizeComment() -> bool {
    if (!consumeBufferedIf(U'-') || !consumeBufferedIf(U'-')) {
        return false;
    }

    while (!isAtEnd()) {
        if (current() == U'-' && peek() == U'-') {
            appendCurrentAndAdvance();
            appendCurrentAndAdvance();
            if (!consumeBufferedIf(U'>')) {
                return false;
            }
            return true;
        }
        appendCurrentAndAdvance();
    }
    return false;
}

auto HtmlTokenizer::tokenizeDocType(std::vector<HtmlTokenRef> &tokens) -> bool {
    constexpr auto cDocType = std::u32string_view{U"doctype"};
    for (const auto expectedCharacter : cDocType) {
        if (cterm::impl::toLowerAscii(current()) != expectedCharacter) {
            return false;
        }
        appendCurrentAndAdvance();
    }
    if (!isAtEnd() && !cterm::impl::isAsciiWhitespace(current()) && current() != U'>') {
        return false;
    }

    skipWhitespace();
    const auto textStart = _buffer.size();
    while (!isAtEnd() && current() != U'>') {
        appendCurrentAndAdvance();
    }
    const auto textEnd = _buffer.size();
    if (!consumeBufferedIf(U'>')) {
        return false;
    }
    tokens.emplace_back(
        HtmlTokenRef{
            .type = HtmlTokenType::DocType,
            .range = trimRange(IndexRange{.start = textStart, .length = textEnd - textStart})});
    return true;
}

auto HtmlTokenizer::decodeEntity(char32_t &decodedCharacter) -> bool {
    const auto savedState = save();
    if (!consumeIf(U'&')) {
        return false;
    }

    if (consumeIf(U'#')) {
        auto base = 10U;
        if (current() == U'x' || current() == U'X') {
            base = 16U;
            next();
        }

        auto hasDigits = false;
        auto value = uint32_t{0};
        while (!isAtEnd()) {
            auto digit = int{-1};
            if (current() >= U'0' && current() <= U'9') {
                digit = static_cast<int>(current() - U'0');
            } else if (base == 16U && current() >= U'a' && current() <= U'f') {
                digit = 10 + static_cast<int>(current() - U'a');
            } else if (base == 16U && current() >= U'A' && current() <= U'F') {
                digit = 10 + static_cast<int>(current() - U'A');
            }

            if (digit < 0) {
                break;
            }
            if (value > ((0x10FFFFU - static_cast<uint32_t>(digit)) / base)) {
                load(savedState);
                return false;
            }
            hasDigits = true;
            value = (value * base) + static_cast<uint32_t>(digit);
            next();
        }
        if (!hasDigits || !consumeIf(U';') || !cterm::impl::isValidUnicode(static_cast<char32_t>(value))) {
            load(savedState);
            return false;
        }
        decodedCharacter = static_cast<char32_t>(value);
        return true;
    }

    constexpr auto cAmp = std::u32string_view{U"amp"};
    constexpr auto cLt = std::u32string_view{U"lt"};
    constexpr auto cGt = std::u32string_view{U"gt"};
    constexpr auto cQuot = std::u32string_view{U"quot"};
    constexpr auto cApos = std::u32string_view{U"apos"};
    auto matches = std::array{true, true, true, true, true};
    auto length = std::size_t{0};
    while (!isAtEnd() && current() != U';' && !cterm::impl::isAsciiWhitespace(current()) && current() != U'<' &&
           current() != U'&') {
        const auto character = current();
        if (length >= cAmp.size() || cAmp[length] != character) {
            matches[0] = false;
        }
        if (length >= cLt.size() || cLt[length] != character) {
            matches[1] = false;
        }
        if (length >= cGt.size() || cGt[length] != character) {
            matches[2] = false;
        }
        if (length >= cQuot.size() || cQuot[length] != character) {
            matches[3] = false;
        }
        if (length >= cApos.size() || cApos[length] != character) {
            matches[4] = false;
        }
        next();
        length += 1;
    }
    if (!consumeIf(U';')) {
        load(savedState);
        return false;
    }

    if (matches[0] && length == cAmp.size()) {
        decodedCharacter = U'&';
        return true;
    }
    if (matches[1] && length == cLt.size()) {
        decodedCharacter = U'<';
        return true;
    }
    if (matches[2] && length == cGt.size()) {
        decodedCharacter = U'>';
        return true;
    }
    if (matches[3] && length == cQuot.size()) {
        decodedCharacter = U'"';
        return true;
    }
    if (matches[4] && length == cApos.size()) {
        decodedCharacter = U'\'';
        return true;
    }

    load(savedState);
    return false;
}

auto HtmlTokenizer::parseAttributeValue(IndexRange &value) -> bool {
    if (isAtEnd()) {
        return false;
    }

    if (current() == U'"' || current() == U'\'') {
        const auto quoteCharacter = current();
        appendCurrentAndAdvance();
        const auto start = _buffer.size();
        while (!isAtEnd() && current() != quoteCharacter) {
            char32_t decodedCharacter = 0;
            if (current() == U'&' && decodeEntity(decodedCharacter)) {
                append(decodedCharacter);
                continue;
            }
            appendCurrentAndAdvance();
        }
        const auto end = _buffer.size();
        if (!consumeBufferedIf(quoteCharacter)) {
            return false;
        }
        value = IndexRange{.start = start, .length = end - start};
        return true;
    }

    if (isAttributeValueTerminator(current())) {
        return false;
    }
    const auto start = _buffer.size();
    while (!isAtEnd() && !isAttributeValueTerminator(current())) {
        char32_t decodedCharacter = 0;
        if (current() == U'&' && decodeEntity(decodedCharacter)) {
            append(decodedCharacter);
            continue;
        }
        appendCurrentAndAdvance();
    }
    value = makeRange(start);
    return value.length > 0;
}

auto HtmlTokenizer::parseName() -> std::optional<IndexRange> {
    const auto start = _buffer.size();
    while (!isAtEnd() && !cterm::impl::isAsciiWhitespace(current()) && !isNameTerminator(current())) {
        appendCurrentAndAdvance();
    }
    const auto range = makeRange(start);
    if (range.length == 0) {
        return std::nullopt;
    }
    return range;
}

void HtmlTokenizer::skipWhitespace() {
    while (!isAtEnd() && cterm::impl::isAsciiWhitespace(current())) {
        appendCurrentAndAdvance();
    }
}

auto HtmlTokenizer::materializeToken(const HtmlTokenRef &token) const -> HtmlToken {
    return HtmlToken{token.type, _buffer.substr(token.range.start, token.range.length)};
}

auto HtmlTokenizer::bufferTextToken() const -> HtmlToken {
    return HtmlToken{HtmlTokenType::Text, _buffer};
}

auto HtmlTokenizer::isNameTerminator(const char32_t character) noexcept -> bool {
    return character == U'>' || character == U'/' || character == U'=';
}

auto HtmlTokenizer::isAttributeValueTerminator(const char32_t character) noexcept -> bool {
    return cterm::impl::isAsciiWhitespace(character) || character == U'>' || character == U'/';
}

}
