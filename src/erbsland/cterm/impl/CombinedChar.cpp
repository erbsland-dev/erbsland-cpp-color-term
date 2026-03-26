// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "CombinedChar.hpp"


#include "U8Buffer.hpp"
#include "UnicodeWidth.hpp"

#include <stdexcept>


namespace erbsland::cterm::impl {


CombinedChar::CombinedChar(const std::string_view text) : _codePoints{decodeUtf8(text)} {
}

CombinedChar::CombinedChar(const std::u32string_view text) : _codePoints{decodeUtf32(text)} {
}

auto CombinedChar::operator==(const CombinedChar &other) const noexcept -> bool {
    return _codePoints == other._codePoints;
}

auto CombinedChar::operator==(const char32_t other) const noexcept -> bool {
    return _codePoints == Storage{other, 0, 0};
}

auto CombinedChar::operator!=(const char32_t other) const noexcept -> bool {
    return _codePoints != Storage{other, 0, 0};
}

auto CombinedChar::utf8() const -> std::string {
    auto result = std::string{};
    result.reserve(byteCount());
    appendTo(result);
    return result;
}

auto CombinedChar::utf32() const -> std::u32string {
    auto result = std::u32string{};
    result.reserve(codePointCount());
    for (const auto codePoint : _codePoints) {
        if (codePoint == 0) {
            break;
        }
        result.push_back(codePoint);
    }
    return result;
}

auto CombinedChar::displayWidth() const noexcept -> int {
    if (_displayWidthCache != cNoDisplayWidth) {
        return static_cast<int>(_displayWidthCache);
    }
    auto result = 0;
    for (const auto codePoint : _codePoints) {
        if (codePoint == 0) {
            break;
        }
        result += static_cast<int>(consoleCharacterWidth(codePoint));
    }
    _displayWidthCache = static_cast<uint8_t>(result);
    return result;
}

auto CombinedChar::byteCount() const noexcept -> std::size_t {
    auto result = std::size_t{0};
    for (const auto codePoint : _codePoints) {
        if (codePoint == 0) {
            break;
        }
        result += U8Buffer<const char>::encodedByteCount(codePoint);
    }
    return result;
}

void CombinedChar::appendTo(std::string &buffer) const noexcept {
    buffer.reserve(buffer.size() + byteCount());
    for (const auto codePoint : _codePoints) {
        if (codePoint == 0) {
            break;
        }
        U8Buffer<const char>::encodeChar(buffer, codePoint);
    }
}

auto CombinedChar::withCombining(const char32_t codePoint) const -> CombinedChar {
    if (codePointCount() == 0) {
        throw std::invalid_argument{"A base code point is required before combining code points can be appended."};
    }
    if (codePoint == 0) {
        throw std::invalid_argument{"CombinedChar does not support the Unicode code point U+0000."};
    }
    if (isControlCode(codePoint)) {
        throw std::invalid_argument{"CombinedChar combining code points must not be control codes."};
    }
    if (consoleCharacterWidth(codePoint) != 0) {
        throw std::invalid_argument{"CombinedChar combining code points must have zero display width."};
    }
    auto result = *this;
    const auto index = countCodePoints(result._codePoints);
    if (index >= result._codePoints.size()) {
        throw std::invalid_argument{"CombinedChar supports at most three Unicode code points."};
    }
    result._codePoints[index] = codePoint;
    result._displayWidthCache = cNoDisplayWidth;
    return result;
}

auto CombinedChar::isSpacing() const noexcept -> bool {
    if (codePointCount() != 1) {
        return false;
    }
    return _codePoints[0] == U' ' || _codePoints[0] == U'\t' || _codePoints[0] == U'\n' || _codePoints[0] == U'\r';
}

auto CombinedChar::isControl() const noexcept -> bool {
    return isControlCode(_codePoints[0]);
}

auto CombinedChar::fromTextUtf8(const std::string_view text) -> std::optional<CombinedChar> {
    auto decoded = std::u32string{};
    decoded.reserve(text.size());
    U8Buffer{text}.decodeAll([&](const char32_t codePoint) -> void { decoded.push_back(codePoint); });
    return parseSingleTextCharacter(decoded);
}

auto CombinedChar::fromTextUtf32(const std::u32string_view text) -> std::optional<CombinedChar> {
    return parseSingleTextCharacter(text);
}

auto CombinedChar::isTextCodePoint(const char32_t codePoint) noexcept -> bool {
    return codePoint == U'\t' || codePoint == U'\n' ||
        (!isControlCode(codePoint) && codePoint >= 0x20 && consoleCharacterWidth(codePoint) > 0);
}

auto CombinedChar::decodeUtf8(const std::string_view text) -> Storage {
    auto result = Storage{};
    auto index = std::size_t{0};
    U8Buffer{text}.decodeAll([&](const char32_t codePoint) -> void {
        if (codePoint == 0) {
            throw std::invalid_argument{"CombinedChar does not support the Unicode code point U+0000."};
        }
        if (isControlCode(codePoint) && codePoint != U'\t' && codePoint != U'\n' && codePoint != U'\r') {
            throw std::invalid_argument{
                "CombinedChar does not support control codes except tab, newline, and carriage return."};
        }
        if (index >= result.size()) {
            throw std::invalid_argument{"CombinedChar supports at most three Unicode code points."};
        }
        result[index++] = codePoint;
    });
    return result;
}

auto CombinedChar::decodeUtf32(const std::u32string_view text) -> Storage {
    auto result = Storage{};
    if (text.size() > result.size()) {
        throw std::invalid_argument{"CombinedChar supports at most three Unicode code points."};
    }
    for (std::size_t index = 0; index < text.size(); ++index) {
        if (text[index] == 0) {
            throw std::invalid_argument{"CombinedChar does not support the Unicode code point U+0000."};
        }
        if (isControlCode(text[index]) && text[index] != U'\t' && text[index] != U'\n' && text[index] != U'\r') {
            throw std::invalid_argument{
                "CombinedChar does not support control codes except tab, newline, and carriage return."};
        }
        result[index] = text[index];
    }
    return result;
}

auto CombinedChar::parseSingleTextCharacter(const std::u32string_view text) -> std::optional<CombinedChar> {
    auto result = std::optional<CombinedChar>{};
    for (const auto codePoint : text) {
        if (codePoint == 0 || isControlCode(codePoint) || codePoint == U'\t' || codePoint == U'\n' ||
            codePoint == U'\r') {
            return std::nullopt;
        }
        if (consoleCharacterWidth(codePoint) == 0) {
            if (result.has_value()) {
                result = result->withCombining(codePoint);
            }
            continue;
        }
        if (result.has_value()) {
            return std::nullopt;
        }
        result = CombinedChar{codePoint};
    }
    return result;
}


}
