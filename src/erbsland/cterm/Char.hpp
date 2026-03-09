// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Color.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>


namespace erbsland::cterm {


/// Represents a character string with foreground and background colors.
/// Used by the UI code to render colored text blocks on the console.
class Char {
public: // ctors/dtor/assign/move
    /// Construct an empty block character using inherited colors.
    constexpr Char() noexcept = default;
    /// Construct a block character from a single Unicode code point using inherited colors.
    /// @param codePoint The base Unicode code point.
    constexpr explicit Char(const char32_t codePoint) noexcept : _codePoints{codePoint, 0, 0} {}
    /// Construct a block character from a single Unicode code point and a color.
    /// @param codePoint The base Unicode code point.
    /// @param color The color for the character.
    constexpr Char(const char32_t codePoint, const Color color) noexcept :
        _codePoints{codePoint, 0, 0}, _color{color} {}
    /// Construct a block character from a single Unicode code point and explicit colors.
    /// @param codePoint The base Unicode code point.
    /// @param fgColor The foreground color.
    /// @param bgColor The background color.
    constexpr Char(const char32_t codePoint, const Foreground fgColor, const Background bgColor) noexcept :
        Char{codePoint, {fgColor, bgColor}} {}
    /// @overload
    constexpr Char(const char32_t codePoint, const Foreground::Hue fgColor, const Background::Hue bgColor) noexcept :
        Char{codePoint, {fgColor, bgColor}} {}
    /// @overload
    constexpr Char(const char32_t codePoint, const Foreground fgColor) noexcept :
        Char{codePoint, {fgColor, Background::Inherited}} {}
    /// @overload
    constexpr Char(const char32_t codePoint, const Foreground::Hue fgColor) noexcept :
        Char{codePoint, {fgColor, Background::Inherited}} {}
    /// @overload
    constexpr Char(const char32_t codePoint, const Background bgColor) noexcept :
        Char{codePoint, {Foreground::Inherited, bgColor}} {}
    /// @overload
    constexpr Char(const char32_t codePoint, const Background::Hue bgColor) noexcept :
        Char{codePoint, {Foreground::Inherited, bgColor}} {}
    /// Construct a block character with inherited colors.
    /// @param charStr The UTF-8 encoded text to display.
    /// @throws std::invalid_argument If the text is not valid UTF-8, contains U+0000, or uses more than three
    /// code points.
    explicit Char(std::string_view charStr);
    /// Construct a block character with inherited colors.
    /// @param charStr The UTF-32 encoded text to display.
    /// @throws std::invalid_argument If the text contains U+0000 or uses more than three code points.
    explicit Char(std::u32string_view charStr);
    /// Construct a block character with inherited colors.
    /// @param charStr The zero-terminated UTF-32 encoded text to display.
    /// @throws std::invalid_argument If the text contains U+0000 or uses more than three code points.
    explicit Char(const char32_t *charStr);
    /// Construct a block character with explicit text and colors.
    /// @param charStr The UTF-8 encoded text to display.
    /// @param fgColor The foreground color.
    /// @param bgColor The background color.
    Char(std::string_view charStr, Foreground fgColor, Background bgColor);
    /// @overload
    Char(std::string_view charStr, Foreground::Hue fgColor, Background::Hue bgColor);
    /// @overload
    Char(std::string_view charStr, Foreground fgColor);
    /// @overload
    Char(std::string_view charStr, Foreground::Hue fgColor);
    /// @overload
    Char(std::string_view charStr, Background bgColor);
    /// @overload
    Char(std::string_view charStr, Background::Hue bgColor);
    /// Construct a block character with explicit text and colors.
    /// @param charStr The UTF-8 encoded text to display.
    /// @param color The color of the block.
    /// @throws std::invalid_argument If the text is not valid UTF-8, contains U+0000, or uses more than three
    /// code points.
    Char(std::string_view charStr, Color color);
    /// Construct a block character with explicit text and colors.
    /// @param charStr The UTF-32 encoded text to display.
    /// @param fgColor The foreground color.
    /// @param bgColor The background color.
    Char(std::u32string_view charStr, Foreground fgColor, Background bgColor);
    /// @overload
    Char(std::u32string_view charStr, Foreground::Hue fgColor, Background::Hue bgColor);
    /// @overload
    Char(std::u32string_view charStr, Foreground fgColor);
    /// @overload
    Char(std::u32string_view charStr, Foreground::Hue fgColor);
    /// @overload
    Char(std::u32string_view charStr, Background bgColor);
    /// @overload
    Char(std::u32string_view charStr, Background::Hue bgColor);
    /// Construct a block character with explicit text and colors.
    /// @param charStr The UTF-32 encoded text to display.
    /// @param color The color of the block.
    /// @throws std::invalid_argument If the text contains U+0000 or uses more than three code points.
    Char(std::u32string_view charStr, Color color);
    /// Construct a block character with explicit text and colors.
    /// @param charStr The zero-terminated UTF-32 encoded text to display.
    /// @param fgColor The foreground color.
    /// @param bgColor The background color.
    Char(const char32_t *charStr, Foreground fgColor, Background bgColor);
    /// @overload
    Char(const char32_t *charStr, Foreground::Hue fgColor, Background::Hue bgColor);
    /// @overload
    Char(const char32_t *charStr, Foreground fgColor);
    /// @overload
    Char(const char32_t *charStr, Foreground::Hue fgColor);
    /// @overload
    Char(const char32_t *charStr, Background bgColor);
    /// @overload
    Char(const char32_t *charStr, Background::Hue bgColor);
    /// Construct a block character with explicit text and colors.
    /// @param charStr The zero-terminated UTF-32 encoded text to display.
    /// @param color The color of the block.
    /// @throws std::invalid_argument If the text contains U+0000 or uses more than three code points.
    Char(const char32_t *charStr, Color color);

public: // operators
    /// Compare two terminal characters for equality.
    [[nodiscard]] auto operator==(const Char &other) const noexcept -> bool = default;
    /// Compare two terminal characters for inequality.
    [[nodiscard]] auto operator!=(const Char &other) const noexcept -> bool = default;

public: // accessors
    /// Get the character string as UTF-8 text.
    /// @return A UTF-8 encoded copy of the stored character sequence.
    [[nodiscard]] auto charStr() const -> std::string;
    /// Get the leading Unicode code point.
    /// @return The base code point, or `0` if this character is empty.
    [[nodiscard]] constexpr auto mainCodePoint() const noexcept -> uint32_t {
        return static_cast<uint32_t>(_codePoints[0]);
    }
    /// Get the stored Unicode code points.
    /// Unused entries are set to `0`.
    [[nodiscard]] constexpr auto codePoints() const noexcept -> const std::array<char32_t, 3> & { return _codePoints; }
    /// Get the number of stored Unicode code points.
    [[nodiscard]] constexpr auto codePointCount() const noexcept -> std::size_t { return countCodePoints(_codePoints); }
    /// Get the character color.
    [[nodiscard]] auto color() const noexcept -> Color { return _color; }
    /// Get the display width on a terminal in cells.
    [[nodiscard]] auto displayWidth() const noexcept -> int;
    /// Get the number of UTF-8 bytes needed to encode this character.
    [[nodiscard]] auto byteCount() const noexcept -> std::size_t;
    /// Append the UTF-8 representation to an existing buffer.
    /// @param buffer The destination buffer.
    void appendTo(std::string &buffer) const noexcept;

public: // modifiers
    /// Create a character with an additional combining code point appended.
    /// @param codePoint The combining code point to append.
    /// @return A copy of this character with the combining code point appended.
    /// @throws std::invalid_argument If `codePoint` is a control code, is not zero-width, or if this character already
    /// stores three code points.
    [[nodiscard]] auto withCombining(char32_t codePoint) const -> Char;
    /// Create a character with the given colors applied as an overlay.
    /// @param color The color override to apply.
    /// @return A copy of this character with the adjusted colors.
    /// If the applied colors contain `Inherited`, the existing component from this character is kept unchanged.
    [[nodiscard]] auto withColorOverlay(Color color) const -> Char;
    /// Create a character with the given color replacing the stored color.
    /// @param color The replacement color.
    /// @return A copy of this character with exactly `color`.
    [[nodiscard]] auto withColorReplaced(Color color) const noexcept -> Char;
    /// Create a character with a base color underneath the stored color.
    /// @param color The base color.
    /// @return A copy of this character where this character color overlays `color`.
    [[nodiscard]] auto withBaseColor(Color color) const noexcept -> Char;

public: // tests
    /// Test if this character is spacing.
    /// Tests for space, tab, newline, and CR.
    [[nodiscard]] auto isSpacing() const noexcept -> bool;
    /// Compare how two characters would appear on screen.
    /// Code points must match exactly. When `colorEnabled` is `true`, inherited color components are treated as the
    /// terminal default color before comparing.
    /// @param other The character to compare with.
    /// @param colorEnabled `true` to include colors in the comparison.
    /// @return `true` if both characters render identically.
    [[nodiscard]] auto renderedEquals(const Char &other, bool colorEnabled = true) const noexcept -> bool;

private:
    [[nodiscard]] static auto decodeUtf8(std::string_view charStr) -> std::array<char32_t, 3>;
    [[nodiscard]] static auto decodeUtf32(std::u32string_view charStr) -> std::array<char32_t, 3>;
    [[nodiscard]] constexpr static auto isControlCode(const char32_t codePoint) noexcept -> bool {
        return codePoint < 0x20 || (codePoint >= 0x7FU && codePoint <= 0x9FU);
    }
    [[nodiscard]] constexpr static auto countCodePoints(const std::array<char32_t, 3> &codePoints) noexcept
        -> std::size_t {
        for (std::size_t index = 0; index < codePoints.size(); ++index) {
            if (codePoints[index] == 0) {
                return index;
            }
        }
        return codePoints.size();
    }
    [[nodiscard]] constexpr static auto utf8ByteCount(const char32_t codePoint) noexcept -> std::size_t {
        const auto value = static_cast<uint32_t>(codePoint);
        if (value <= 0x7FU) {
            return 1;
        }
        if (value <= 0x7FFU) {
            return 2;
        }
        if (value <= 0xFFFFU) {
            return 3;
        }
        return 4;
    }

private:
    /// Special value to mark that the display width was not yet calculated.
    constexpr static auto cNoDisplayWidth = std::numeric_limits<uint8_t>::max();

private:
    std::array<char32_t, 3> _codePoints{}; ///< The base code point followed by up to two combining code points.
    Color _color;                          ///< The color for this character.
    mutable uint8_t _displayWidthCache = cNoDisplayWidth; ///< The cached display width of this character.
};


}
