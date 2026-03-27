// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "HashHelper.hpp"
#include "UnicodeWidth.hpp"

#include "../EncodingErrors.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <string_view>


namespace erbsland::cterm::impl {


/// Internal representation of one terminal character with optional combining marks.
class CombinedChar final {
public:
    using Storage = std::array<char32_t, 3>; ///< Fixed storage for one base code point and up to two combining marks.

public:
    /// Construct an empty character.
    constexpr CombinedChar() noexcept = default;
    /// Construct a character from a single code point.
    /// @param codePoint The Unicode code point to store as the base character.
    constexpr explicit CombinedChar(const char32_t codePoint) noexcept : _codePoints{codePoint, 0, 0} {}
    /// Construct a character from UTF-8 text.
    /// @param text The UTF-8 text for exactly one terminal character.
    /// Invalid UTF-8 bytes are replaced per byte, and unsupported text normalizes to `U+FFFD`.
    /// Empty input, control codes, and leading zero-width code points normalize to `U+FFFD`.
    /// Later visible code points also collapse the result to `U+FFFD`, while a third combining mark is ignored.
    explicit CombinedChar(std::string_view text) noexcept;
    /// Construct a character from UTF-32 text.
    /// @param text The UTF-32 text for exactly one terminal character.
    /// Invalid Unicode scalar values and unsupported text normalize to `U+FFFD`.
    /// Empty input, control codes, and leading zero-width code points normalize to `U+FFFD`.
    /// Later visible code points also collapse the result to `U+FFFD`, while a third combining mark is ignored.
    explicit CombinedChar(std::u32string_view text) noexcept;

    // defaults
    CombinedChar(const CombinedChar &) = default;
    CombinedChar(CombinedChar &&) = default;
    auto operator=(const CombinedChar &) -> CombinedChar & = default;
    auto operator=(CombinedChar &&) -> CombinedChar & = default;

public: // operators
    /// Compare two stored character sequences.
    auto operator==(const CombinedChar &other) const noexcept -> bool { return _codePoints == other._codePoints; }
    /// Compare two stored character sequences.
    auto operator!=(const CombinedChar &other) const noexcept -> bool { return _codePoints != other._codePoints; }
    /// Compare against a single code point.
    [[nodiscard]] auto operator==(char32_t other) const noexcept -> bool { return singleCodePoint() == other; }
    /// Compare against a single code point.
    [[nodiscard]] auto operator!=(char32_t other) const noexcept -> bool { return !operator==(other); }

public: // accessors
    /// Convert the stored sequence to UTF-8.
    [[nodiscard]] auto utf8() const -> std::string;
    /// Convert the stored sequence to UTF-32.
    [[nodiscard]] auto utf32() const -> std::u32string;
    /// Get the leading code point.
    [[nodiscard]] constexpr auto mainCodePoint() const noexcept -> char32_t { return _codePoints[0]; }
    /// Get a single Unicode code point or zero for combined or empty characters.
    /// This is a fast-path method for comparing a single-code point character, without the color.
    /// @return The single code point, or `0` if this character is combined or empty.
    [[nodiscard]] constexpr auto singleCodePoint() const noexcept -> char32_t {
        return _codePoints[1] != 0 ? 0 : _codePoints[0];
    }
    /// Get the raw stored code points.
    [[nodiscard]] constexpr auto codePoints() const noexcept -> const Storage & { return _codePoints; }
    /// Get the number of stored code points.
    [[nodiscard]] constexpr auto codePointCount() const noexcept -> std::size_t { return countCodePoints(_codePoints); }
    /// Get the terminal display width in cells.
    [[nodiscard]] auto displayWidth() const noexcept -> int {
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
    /// Get the UTF-8 byte count for this sequence.
    [[nodiscard]] auto byteCount() const noexcept -> std::size_t;
    /// Append the UTF-8 representation to a buffer.
    /// @param buffer The destination buffer.
    void appendTo(std::string &buffer) const noexcept;

public: // modifiers
    /// Create a copy with one additional combining code point.
    /// @param codePoint The combining code point to append.
    /// @param encodingErrors How invalid combining code points are handled.
    /// @return The updated character.
    /// `EncodingErrors::Replace` and `EncodingErrors::Ignore` keep the original character unchanged for invalid
    /// combining code points or when the storage is already full.
    /// @throws std::invalid_argument If `encodingErrors` is `Throw` and the code point is unsupported.
    [[nodiscard]] auto withCombining(char32_t codePoint, EncodingErrors encodingErrors = EncodingErrors::Replace) const
        -> CombinedChar;

public: // tests
    /// Test if the character is empty.
    [[nodiscard]] constexpr auto isEmpty() const noexcept -> bool { return _codePoints[0] == 0; }
    /// Test if the character is spacing.
    [[nodiscard]] auto isSpacing() const noexcept -> bool {
        if (codePointCount() != 1) {
            return false;
        }
        return _codePoints[0] == U' ' || _codePoints[0] == U'\t' || _codePoints[0] == U'\n' || _codePoints[0] == U'\r';
    }
    /// Test if the leading code point is a control code.
    [[nodiscard]] auto isControl() const noexcept -> bool { return isControlCode(_codePoints[0]); }
    /// Get a stable hash for the stored code points.
    [[nodiscard]] constexpr auto hash() const noexcept -> std::size_t {
        return hashCreate(_codePoints[0], _codePoints[1], _codePoints[2]);
    }

public: // tools
    /// Parse exactly one visible text character from UTF-8 input.
    /// Invalid or unsupported input normalizes to `U+FFFD`.
    /// @param text The UTF-8 text to parse.
    /// @return The parsed character wrapped in `std::optional` for API compatibility.
    /// This function currently always returns a value and uses normalization instead of `std::nullopt`.
    [[nodiscard]] static auto fromTextUtf8(std::string_view text) noexcept -> std::optional<CombinedChar>;
    /// Parse exactly one visible text character from UTF-32 input.
    /// Invalid or unsupported input normalizes to `U+FFFD`.
    /// @param text The UTF-32 text to parse.
    /// @return The parsed character wrapped in `std::optional` for API compatibility.
    /// This function currently always returns a value and uses normalization instead of `std::nullopt`.
    [[nodiscard]] static auto fromTextUtf32(std::u32string_view text) noexcept -> std::optional<CombinedChar>;
    /// Test if a code point is a control code.
    [[nodiscard]] constexpr static auto isControlCode(char32_t codePoint) noexcept -> bool;
    /// Test whether the code point is encoded as a standalone terminal character in text parsing.
    [[nodiscard]] static auto isTextCodePoint(char32_t codePoint) noexcept -> bool;

private:
    [[nodiscard]] static auto decodeUtf8(std::string_view text) noexcept -> Storage;
    [[nodiscard]] static auto decodeUtf32(std::u32string_view text) noexcept -> Storage;
    [[nodiscard]] static auto normalizeTextCodePoint(char32_t codePoint) noexcept -> char32_t;
    [[nodiscard]] static auto normalizeDecodedText(std::u32string_view text) noexcept -> Storage;
    [[nodiscard]] static auto replacementStorage() noexcept -> Storage;
    static void normalizeDecodedTextCodePoint(
        Storage &result,
        std::size_t &combiningCount,
        bool &hasBaseCodePoint,
        bool &mustReplace,
        char32_t codePoint) noexcept;
    [[nodiscard]] constexpr static auto countCodePoints(const Storage &codePoints) noexcept -> std::size_t;

private:
    constexpr static auto cNoDisplayWidth = std::numeric_limits<uint8_t>::max();

private:
    Storage _codePoints{};                                ///< The stored base and combining code points.
    mutable uint8_t _displayWidthCache = cNoDisplayWidth; ///< Cached terminal width.
};


constexpr auto CombinedChar::isControlCode(const char32_t codePoint) noexcept -> bool {
    return codePoint < 0x20 || (codePoint >= 0x7FU && codePoint <= 0x9FU);
}

constexpr auto CombinedChar::countCodePoints(const Storage &codePoints) noexcept -> std::size_t {
    for (std::size_t index = 0; index < codePoints.size(); ++index) {
        if (codePoints[index] == 0) {
            return index;
        }
    }
    return codePoints.size();
}

}


template <>
struct std::hash<erbsland::cterm::impl::CombinedChar> {
    auto operator()(const erbsland::cterm::impl::CombinedChar &character) const noexcept -> std::size_t {
        return character.hash();
    }
};
