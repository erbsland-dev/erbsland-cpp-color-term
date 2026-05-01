// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "UnicodeWidth.hpp"

#include <cctype>
#include <cstdint>
#include <string>
#include <string_view>

namespace erbsland::cterm::impl {

/// Convert one ASCII uppercase character to lowercase.
/// @param character The character to normalize.
/// @return The lowercase ASCII character, or the original character.
[[nodiscard]] constexpr auto toLowerAscii(const char32_t character) noexcept -> char32_t {
    if (character >= U'A' && character <= U'Z') {
        return character - U'A' + U'a';
    }
    return character;
}

/// Convert ASCII uppercase characters in a UTF-32 string to lowercase.
/// @param text The text to normalize.
/// @return The lower-case ASCII copy of the text.
[[nodiscard]] inline auto toLowerAscii(const std::u32string_view text) -> std::u32string {
    auto result = std::u32string{};
    result.reserve(text.size());
    for (const auto character : text) {
        result.push_back(toLowerAscii(character));
    }
    return result;
}

/// Test if one code point is in the valid Unicode range.
[[nodiscard]] constexpr auto isValidUnicode(const char32_t unicode) noexcept -> bool {
    return unicode <= 0x10FFFFU && (unicode < 0xD800U || unicode > 0xDFFFU);
}

/// Test if one code point is an ISO control code.
/// @param codePoint The code point to test.
/// @return `true` if the code point is a control code.
[[nodiscard]] constexpr auto isControlCode(const char32_t codePoint) noexcept -> bool {
    return codePoint < 0x20 || (codePoint >= 0x7FU && codePoint <= 0x9FU);
}

/// Test if one code point is safe for `Char` construction.
/// This accepts any valid Unicode code point except control characters.
/// New-line and tabs are allowed control-characters.
[[nodiscard]] constexpr auto isSafeCodePoint(const char32_t codePoint) noexcept -> bool {
    return isValidUnicode(codePoint) &&
        (codePoint == U'\t' || codePoint == U'\n' || (codePoint >= 0x20 && codePoint <= 0x7E) || codePoint >= 0xA0);
}

/// Return a safe replacement for invalid code-points.
/// @return Space for all valid Unicode code points, otherwise the Unicode replacement character.
[[nodiscard]] constexpr auto safeCodePointReplacement(const char32_t codePoint) noexcept -> char32_t {
    return isValidUnicode(codePoint) ? U' ' : 0xFFFDU;
}

/// Return a safe code point for `Char`.
/// Valid Unicode code points are preserved, control codes are replaced with a space - to represent an
/// "invisible" character. Invalid Unicode code points are replaced with the Unicode replacement character.
/// New-line and tabs are allowed control-characters.
/// @return The safe code point for use in terminal strings.
[[nodiscard]] constexpr auto safeCodePoint(const char32_t codePoint) noexcept -> char32_t {
    return isSafeCodePoint(codePoint) ? codePoint : safeCodePointReplacement(codePoint);
}

/// Test whether one code point is accepted as a visible string character.
/// @param codePoint The code point to test.
/// @return `true` if the code point is preserved in a terminal string.
[[nodiscard]] inline auto isStringCharacter(const char32_t codePoint) noexcept -> bool {
    return codePoint == U'\t' || codePoint == U'\n' ||
        (!isControlCode(codePoint) && codePoint >= 0x20 && consoleCharacterWidth(codePoint) > 0);
}

[[nodiscard]] constexpr auto isAsciiWhitespace(const char32_t character) noexcept -> bool {
    return character == U' ' || character == U'\t' || character == U'\n' || character == U'\r' || character == U'\f';
}

[[nodiscard]] inline auto trimWhitespace(std::u32string text) -> std::u32string {
    while (!text.empty() && isAsciiWhitespace(text.front())) {
        text.erase(text.begin());
    }
    while (!text.empty() && isAsciiWhitespace(text.back())) {
        text.pop_back();
    }
    return text;
}

/// Normalize ASCII whitespace runs to single spaces.
/// @param text The text to normalize.
/// @return The text with collapsed ASCII whitespace.
[[nodiscard]] inline auto normalizeWhitespace(const std::u32string_view text) -> std::u32string {
    auto result = std::u32string{};
    result.reserve(text.size());
    auto pendingSpace = false;
    for (const auto character : text) {
        if (isAsciiWhitespace(character)) {
            pendingSpace = true;
            continue;
        }
        if (pendingSpace) {
            result.push_back(U' ');
            pendingSpace = false;
        }
        result.push_back(character);
    }
    if (pendingSpace) {
        result.push_back(U' ');
    }
    return result;
}

/// Normalize ASCII whitespace runs to single spaces in-place.
/// @param text The text to normalize.
/// @return `true` if the original text contained only ASCII whitespace, otherwise `false`.
[[nodiscard]] inline auto normalizeWhitespaceInPlace(std::u32string &text) -> bool {
    auto writeIndex = std::size_t{0};
    auto pendingSpace = false;
    auto hasNonWhitespace = false;
    for (const auto character : text) {
        if (isAsciiWhitespace(character)) {
            pendingSpace = true;
            continue;
        }
        if (pendingSpace) {
            text[writeIndex] = U' ';
            writeIndex += 1;
            pendingSpace = false;
        }
        text[writeIndex] = character;
        writeIndex += 1;
        hasNonWhitespace = true;
    }
    if (pendingSpace) {
        text[writeIndex] = U' ';
        writeIndex += 1;
    }
    text.resize(writeIndex);
    return !hasNonWhitespace;
}

/// Test if the given UTF-32 string contains only ASCII whitespace.
/// @param text The text to examine.
/// @return `true` if the text contains only ASCII whitespace.
[[nodiscard]] inline auto isWhitespaceOnly(const std::u32string_view text) noexcept -> bool {
    for (const auto character : text) {
        if (!isAsciiWhitespace(character)) {
            return false;
        }
    }
    return true;
}

[[nodiscard]] inline auto toNormalizedIdentifier(const std::string_view text) -> std::string {
    std::string result;
    result.reserve(text.size());
    bool seenContent = false;
    bool pendingSpace = false;
    for (char ch : text) {
        const auto unsignedCharacter = static_cast<unsigned char>(ch);
        if (unsignedCharacter < 128U) {
            ch = static_cast<char>(std::tolower(unsignedCharacter));
        }
        if (ch == ' ' || ch == '\t' || ch == '-') {
            if (seenContent) {
                pendingSpace = true;
            }
            continue;
        }
        if (pendingSpace && !result.empty()) {
            result += '_';
            pendingSpace = false;
        }
        result += ch;
        seenContent = true;
    }
    return result;
}

}
