// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "CombinedChar.hpp"

#include "U8Buffer.hpp"
#include "UnicodeWidth.hpp"

#include <stdexcept>

namespace erbsland::cterm::impl {

CombinedChar::CombinedChar(const std::string_view text) noexcept : _codePoints{decodeUtf8(text)} {
}

CombinedChar::CombinedChar(const std::u32string_view text) noexcept : _codePoints{decodeUtf32(text)} {
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

auto CombinedChar::withCombining(const char32_t codePoint, const EncodingErrors encodingErrors) const -> CombinedChar {
    if (codePointCount() == 0) {
        if (encodingErrors == EncodingErrors::Throw) {
            throw std::invalid_argument{"A base code point is required before combining code points can be appended."};
        }
        return *this;
    }
    if (!isValidUnicode(codePoint)) {
        if (encodingErrors == EncodingErrors::Throw) {
            throw std::invalid_argument{"CombinedChar requires a valid Unicode code point."};
        }
        return *this;
    }
    if (codePoint == 0) {
        if (encodingErrors == EncodingErrors::Throw) {
            throw std::invalid_argument{"CombinedChar does not support the Unicode code point U+0000."};
        }
        return *this;
    }
    if (isControlCode(codePoint)) {
        if (encodingErrors == EncodingErrors::Throw) {
            throw std::invalid_argument{"CombinedChar combining code points must not be control codes."};
        }
        return *this;
    }
    if (consoleCharacterWidth(codePoint) != 0) {
        if (encodingErrors == EncodingErrors::Throw) {
            throw std::invalid_argument{"CombinedChar combining code points must have zero display width."};
        }
        return *this;
    }
    auto result = *this;
    const auto index = countCodePoints(result._codePoints);
    if (index >= result._codePoints.size()) {
        if (encodingErrors == EncodingErrors::Throw) {
            throw std::invalid_argument{"CombinedChar supports at most three Unicode code points."};
        }
        return *this;
    }
    result._codePoints[index] = codePoint;
    return result;
}

auto CombinedChar::fromTextUtf8(const std::string_view text) noexcept -> std::optional<CombinedChar> {
    auto result = CombinedChar{};
    result._codePoints = decodeUtf8(text);
    return result;
}

auto CombinedChar::fromTextUtf32(const std::u32string_view text) noexcept -> std::optional<CombinedChar> {
    auto result = CombinedChar{};
    result._codePoints = decodeUtf32(text);
    return result;
}

auto CombinedChar::isTextCodePoint(const char32_t codePoint) noexcept -> bool {
    return codePoint == U'\t' || codePoint == U'\n' ||
        (!isControlCode(codePoint) && codePoint >= 0x20 && consoleCharacterWidth(codePoint) > 0);
}

auto CombinedChar::decodeUtf8(const std::string_view text) noexcept -> Storage {
    auto result = Storage{};
    auto combiningCount = std::size_t{0};
    auto hasBaseCodePoint = false;
    auto mustReplace = false;
    U8Buffer{text}.decodeAllReplacingErrors([&](const char32_t codePoint) noexcept -> void {
        normalizeDecodedTextCodePoint(result, combiningCount, hasBaseCodePoint, mustReplace, codePoint);
    });
    if (mustReplace || !hasBaseCodePoint) {
        return replacementStorage();
    }
    return result;
}

auto CombinedChar::decodeUtf32(const std::u32string_view text) noexcept -> Storage {
    return normalizeDecodedText(text);
}

auto CombinedChar::normalizeTextCodePoint(const char32_t codePoint) noexcept -> char32_t {
    if (!isValidUnicode(codePoint)) {
        return U8Buffer<const char>::cReplacementCharacter;
    }
    return codePoint;
}

auto CombinedChar::normalizeDecodedText(const std::u32string_view text) noexcept -> Storage {
    auto result = Storage{};
    auto combiningCount = std::size_t{0};
    auto hasBaseCodePoint = false;
    auto mustReplace = false;
    for (const auto codePoint : text) {
        normalizeDecodedTextCodePoint(
            result, combiningCount, hasBaseCodePoint, mustReplace, normalizeTextCodePoint(codePoint));
    }
    if (mustReplace || !hasBaseCodePoint) {
        return replacementStorage();
    }
    return result;
}

auto CombinedChar::replacementStorage() noexcept -> Storage {
    return {U8Buffer<const char>::cReplacementCharacter, 0, 0};
}

void CombinedChar::normalizeDecodedTextCodePoint(
    Storage &result,
    std::size_t &combiningCount,
    bool &hasBaseCodePoint,
    bool &mustReplace,
    const char32_t codePoint) noexcept {
    if (mustReplace) {
        return;
    }
    if (!hasBaseCodePoint) {
        if (codePoint == 0 || isControlCode(codePoint) || consoleCharacterWidth(codePoint) == 0) {
            mustReplace = true;
            return;
        }
        result[0] = codePoint;
        hasBaseCodePoint = true;
        return;
    }
    if (codePoint == 0 || isControlCode(codePoint)) {
        mustReplace = true;
        return;
    }
    if (consoleCharacterWidth(codePoint) != 0) {
        mustReplace = true;
        return;
    }
    if (combiningCount >= 2) {
        return;
    }
    result[1 + combiningCount] = codePoint;
    ++combiningCount;
}

}
