// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StringData.hpp"

#include "TextUtil.hpp"
#include "U8Buffer.hpp"

namespace erbsland::cterm::impl {

StringData::StringData(Storage chars) noexcept : _chars{std::move(chars)} {
    for (const auto &character : _chars) {
        _displayWidth += character.displayWidth();
    }
}

StringData::StringData(Storage chars, const int displayWidth) noexcept :
    _chars{std::move(chars)}, _displayWidth{displayWidth} {
}

void StringData::reserve(const std::size_t size) {
    _chars.reserve(size);
}

void StringData::clear() noexcept {
    _chars.clear();
    _displayWidth = 0;
}

void StringData::append(const Char &character) noexcept {
    _chars.emplace_back(character);
    addDisplayWidth(character.displayWidth());
}

void StringData::addDisplayWidth(const int displayWidth) noexcept {
    if (hasDisplayWidthCache()) {
        _displayWidth += displayWidth;
    }
}

auto StringData::copyChars(IndexRange range) const -> Storage {
    range = range.clampedTo(_chars.size());
    return Storage{
        _chars.cbegin() + static_cast<Storage::difference_type>(range.startIndex()),
        _chars.cbegin() + static_cast<Storage::difference_type>(range.endIndex())};
}

void StringData::appendCodePoint(const char32_t codePoint, const Color color, const CharAttributes attributes) {
    if (codePoint == U'\t' || codePoint == U'\n') {
        append(Char{codePoint, color, attributes});
        return;
    }
    if (isControlCode(codePoint)) {
        return;
    }
    if (consoleCharacterWidth(codePoint) == 0) {
        if (!_chars.empty()) {
            _chars.back() = _chars.back().withCombining(codePoint);
        }
        return;
    }
    append(Char{codePoint, color, attributes});
}

void StringData::appendCharacters(
    const std::u32string_view text, const Color color, const CharAttributes attributes) noexcept {
    _chars.reserve(_chars.size() + text.size());
    for (const auto codePoint : text) {
        appendCodePoint(codePoint, color, attributes);
    }
}

void StringData::appendCharacters(
    const std::string_view text,
    const Color color,
    const CharAttributes attributes,
    const EncodingErrors encodingErrors) noexcept {

    _chars.reserve(_chars.size() + static_cast<std::size_t>(measureDisplayWidth(text, encodingErrors)));
    U8Buffer{text}.decodeAll(
        [&](const char32_t codePoint) -> void { appendCodePoint(codePoint, color, attributes); }, encodingErrors);
}

auto StringData::sharedEmpty() -> const std::shared_ptr<StringData> & {
    static const auto data = std::make_shared<StringData>(Storage{});
    return data;
}

auto StringData::measureDisplayWidth(const std::string_view text, const EncodingErrors encodingErrors) -> int {
    auto result = 0;
    U8Buffer{text}.decodeAll(
        [&](const char32_t codePoint) -> void {
            if (isStringCharacter(codePoint)) {
                result += Char{codePoint}.displayWidth();
            }
        },
        encodingErrors);
    return result;
}

auto StringData::measureDisplayWidth(const std::u32string_view text) -> int {
    auto result = 0;
    for (const auto codePoint : text) {
        if (isStringCharacter(codePoint)) {
            result += Char{codePoint}.displayWidth();
        }
    }
    return result;
}

}
