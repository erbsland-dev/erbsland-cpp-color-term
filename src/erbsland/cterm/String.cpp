// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "String.hpp"


#include "impl/StringWrapper.hpp"
#include "impl/TextUtil.hpp"
#include "impl/U8Buffer.hpp"
#include "impl/UnicodeWidth.hpp"

#include <algorithm>
#include <ranges>


namespace erbsland::cterm {


String::String(const std::string_view str, const EncodingErrors encodingErrors) :
    String{str, CharStyle{}, encodingErrors} {
}

String::String(const std::string_view str, const CharStyle style, const EncodingErrors encodingErrors) :
    _chars{splitCharacters(str, style.color(), style.attributes(), encodingErrors)} {
}

String::String(const std::u32string_view str) : String{str, CharStyle{}} {
}

String::String(const std::u32string_view str, const CharStyle style) :
    _chars{splitCharacters(str, style.color(), style.attributes())} {
}

String::String(const std::size_t count, Char character) noexcept :
    _chars(std::min(count, static_cast<std::size_t>(10'000'000U)), character) {
}


auto String::displayWidth() const noexcept -> int {
    auto result = 0;
    for (const auto &character : _chars) {
        result += character.displayWidth();
    }
    return result;
}

auto String::count(const Char character) const noexcept -> std::size_t {
    return static_cast<std::size_t>(
        std::ranges::count_if(_chars, [&](const Char &c) -> bool { return c == character; }));
}

auto String::count(const char32_t character) const noexcept -> std::size_t {
    return static_cast<std::size_t>(
        std::ranges::count_if(_chars, [&](const Char &c) -> bool { return c.singleCodePoint() == character; }));
}

auto String::indexOf(const Char character, const std::size_t startIndex) const noexcept -> std::size_t {
    if (startIndex >= _chars.size()) {
        return npos;
    }
    for (auto index = startIndex; index < _chars.size(); ++index) {
        if (_chars[index] == character) {
            return index;
        }
    }
    return npos;
}

auto String::indexOf(const char32_t character, const std::size_t startIndex) const noexcept -> std::size_t {
    if (startIndex >= _chars.size()) {
        return npos;
    }
    for (auto index = startIndex; index < _chars.size(); ++index) {
        if (_chars[index].singleCodePoint() == character) {
            return index;
        }
    }
    return npos;
}

auto String::substr(const std::size_t startIndex, std::size_t length) const noexcept -> String {
    if (startIndex >= _chars.size()) {
        return {};
    }
    if (length == 0) {
        return {};
    }
    if (length == npos || length > _chars.size() - startIndex) {
        length = _chars.size() - startIndex;
    }
    return String{Storage{
        _chars.cbegin() + static_cast<difference_type>(startIndex),
        _chars.cbegin() + static_cast<difference_type>(startIndex) + static_cast<difference_type>(length)}};
}

auto String::trimmed(const std::u32string_view characters) const noexcept -> String {
    auto start = std::size_t{0};
    auto end = size();
    while (start < end && _chars[start].isOneOf(characters)) {
        start += 1;
    }
    while (end > start && _chars[end - 1].isOneOf(characters)) {
        end -= 1;
    }
    return substr(start, end - start);
}

auto String::containsControlCharacters() const noexcept -> bool {
    return std::ranges::any_of(_chars, [](const Char &c) -> bool { return c.isControl(); });
}

void String::appendStyled(std::string_view text, CharStyle style) noexcept {
    appendCharacters(_chars, text, style.color(), style.attributes(), EncodingErrors::Replace);
}

void String::appendStyled(const std::u32string_view text, const CharStyle style) noexcept {
    _chars.reserve(_chars.size() + text.size());
    appendCharacters(_chars, text, style.color(), style.attributes());
}

void String::appendRange(const String &other, const std::size_t startIndex, std::size_t length) noexcept {
    if (startIndex >= other._chars.size() || length == 0) {
        return;
    }
    if (length == npos || length > other._chars.size() - startIndex) {
        length = other._chars.size() - startIndex;
    }
    _chars.insert(
        _chars.end(),
        other._chars.cbegin() + static_cast<difference_type>(startIndex),
        other._chars.cbegin() + static_cast<difference_type>(startIndex) + static_cast<difference_type>(length));
}

void String::appendRangeWithBaseStyle(
    const String &other, const std::size_t startIndex, std::size_t length, const CharStyle style) noexcept {
    if (startIndex >= other._chars.size() || length == 0) {
        return;
    }
    if (length == npos || length > other._chars.size() - startIndex) {
        length = other._chars.size() - startIndex;
    }
    _chars.reserve(_chars.size() + length);
    const auto color = style.color();
    const auto attributes = style.attributes();
    for (auto index = startIndex; index < startIndex + length; ++index) {
        _chars.emplace_back(other._chars[index].withBase(color, attributes));
    }
}

auto String::splitWords() const noexcept -> std::vector<String> {
    std::vector<String> words;
    String word;
    word.reserve(100);
    for (const auto &character : *this) {
        if (character.isSpacing()) {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word.append(character);
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

auto String::wrapIntoLines(const int width, const ParagraphSpacing paragraphSpacing) const noexcept
    -> std::vector<String> {
    return impl::StringWrapper{*this}.wrapIntoLines(width, paragraphSpacing);
}

auto String::terminalLines(const int width) const noexcept -> int {
    if (width <= 0) {
        return 0;
    }
    if (width == 1) {
        return static_cast<int>(size());
    }
    auto renderedLines = 0;
    auto currentWidth = 0;
    for (const auto &character : *this) {
        if (character == U'\n') {
            renderedLines += 1;
            currentWidth = 0;
            continue;
        }
        const auto characterWidth = character.displayWidth();
        currentWidth += characterWidth;
        if (currentWidth >= width) {
            renderedLines += 1;
            currentWidth = currentWidth > width ? characterWidth : 0;
        }
    }
    if (currentWidth > 0) {
        renderedLines += 1;
    }
    return renderedLines;
}

auto String::splitLines() const noexcept -> std::vector<String> {
    return impl::StringWrapper{*this}.splitLines();
}

auto String::fromLines(
    const std::initializer_list<std::string_view> lines, const Color color, const CharAttributes attributes) noexcept
    -> String {
    if (lines.size() == 0) {
        return {};
    }
    std::size_t characterCount = lines.size() - 1; // count newlines
    for (auto &line : lines) {
        characterCount += countCharacters(line, EncodingErrors::Replace);
    }
    Storage result;
    result.reserve(characterCount);
    bool isFirstLine = true;
    for (const auto &line : lines) {
        if (isFirstLine) {
            isFirstLine = false;
        } else {
            result.emplace_back(Char{U'\n', color, attributes});
        }
        impl::U8Buffer{line}.decodeAllReplacingErrors(
            [&](const char32_t codePoint) -> void { appendCodePoint(result, codePoint, color, attributes); });
    }
    return String{std::move(result)};
}

auto String::fromLines(const std::initializer_list<std::string_view> lines, const CharStyle style) noexcept -> String {
    return fromLines(lines, style.color(), style.attributes());
}

auto String::fromLines(
    const std::initializer_list<std::u32string_view> lines, const Color color, const CharAttributes attributes) noexcept
    -> String {
    if (lines.size() == 0) {
        return {};
    }
    std::size_t characterCount = lines.size() - 1; // count newlines
    for (auto &line : lines) {
        characterCount += countCharacters(line);
    }
    Storage result;
    result.reserve(characterCount);
    bool isFirstLine = true;
    for (const auto &line : lines) {
        if (isFirstLine) {
            isFirstLine = false;
        } else {
            result.emplace_back(Char{U'\n', color, attributes});
        }
        for (const auto codePoint : line) {
            appendCodePoint(result, codePoint, color, attributes);
        }
    }
    return String{std::move(result)};
}

auto String::fromLines(const std::initializer_list<std::u32string_view> lines, const CharStyle style) noexcept
    -> String {
    return fromLines(lines, style.color(), style.attributes());
}

auto String::countCharacters(const std::string_view str, const EncodingErrors encodingErrors) -> std::size_t {
    std::size_t displayCharCount = 0;
    impl::U8Buffer{str}.decodeAll(
        [&](const char32_t codePoint) -> void {
            if (isStringCharacter(codePoint)) {
                displayCharCount += static_cast<std::size_t>(Char{codePoint}.displayWidth());
            }
        },
        encodingErrors);
    return displayCharCount;
}

auto String::countCharacters(const std::u32string_view str) -> std::size_t {
    std::size_t displayCharCount = 0;
    for (const auto codePoint : str) {
        if (isStringCharacter(codePoint)) {
            displayCharCount += static_cast<std::size_t>(Char{codePoint}.displayWidth());
        }
    }
    return displayCharCount;
}

auto String::splitCharacters(
    const std::string_view str, const Color color, const CharAttributes attributes, const EncodingErrors encodingErrors)
    -> Storage {
    auto result = Storage{};
    result.reserve(countCharacters(str, encodingErrors));
    impl::U8Buffer{str}.decodeAll(
        [&](const char32_t codePoint) -> void { appendCodePoint(result, codePoint, color, attributes); },
        encodingErrors);
    return result;
}

auto String::splitCharacters(const std::u32string_view str, const Color color, const CharAttributes attributes)
    -> Storage {
    auto result = Storage{};
    result.reserve(str.size());
    appendCharacters(result, str, color, attributes);
    return result;
}

auto String::isStringCharacter(const char32_t codePoint) noexcept -> bool {
    return codePoint == U'\t' || codePoint == U'\n' ||
        (!isControlCode(codePoint) && codePoint >= 0x20 && impl::consoleCharacterWidth(codePoint) > 0);
}

void String::appendCodePoint(
    Storage &chars, const char32_t codePoint, const Color color, const CharAttributes attributes) {
    if (codePoint == U'\t' || codePoint == U'\n') {
        chars.emplace_back(codePoint, color, attributes);
        return;
    }
    if (isControlCode(codePoint)) {
        return;
    }
    if (impl::consoleCharacterWidth(codePoint) == 0) {
        if (!chars.empty()) { // drop combining characters at the beginning of a text.
            chars.back() = chars.back().withCombining(codePoint);
        }
        return;
    }
    chars.emplace_back(codePoint, color, attributes);
}

void String::appendCharacters(
    Storage &chars, const std::u32string_view text, const Color color, const CharAttributes attributes) noexcept {
    for (const auto codePoint : text) {
        appendCodePoint(chars, codePoint, color, attributes);
    }
}

void String::appendCharacters(
    Storage &chars,
    const std::string_view text,
    const Color color,
    const CharAttributes attributes,
    const EncodingErrors encodingErrors) noexcept {
    chars.reserve(chars.size() + countCharacters(text, encodingErrors));
    impl::U8Buffer{text}.decodeAll(
        [&](const char32_t codePoint) -> void { appendCodePoint(chars, codePoint, color, attributes); },
        encodingErrors);
}


}
