// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "CharStyle.hpp"
#include "EncodingErrors.hpp"

#include "impl/CombinedChar.hpp"
#include "impl/TypeTraits.hpp"

#include <cstddef>
#include <functional>
#include <string>
#include <string_view>


namespace erbsland::cterm {


/// Represents a character string with combined terminal style information.
/// Used by the UI code to render colored text blocks on the console.
class Char {
public:
    /// Construct an empty block character using inherited colors.
    constexpr Char() noexcept = default;
    /// Construct a block character from a single Unicode code point using inherited colors.
    /// @param codePoint The base Unicode code point.
    constexpr explicit Char(const char32_t codePoint) noexcept : _character{codePoint} {}
    /// Construct a block character with inherited colors.
    /// @param charStr The UTF-8 encoded text to display.
    /// Invalid or unsupported text normalizes deterministically to a single renderable character.
    /// Empty input, control codes, and leading zero-width code points normalize to `U+FFFD`.
    /// Later visible code points also collapse the result to `U+FFFD`, while a third combining mark is ignored.
    explicit Char(std::string_view charStr) noexcept : Char{charStr, CharStyle{}} {}
    /// Construct a block character with inherited colors.
    /// @param charStr The UTF-32 encoded text to display.
    /// Invalid or unsupported text normalizes deterministically to a single renderable character.
    /// Empty input, control codes, invalid Unicode scalar values, and leading zero-width code points normalize to
    /// `U+FFFD`. Later visible code points also collapse the result to `U+FFFD`, while a third combining mark is
    /// ignored.
    explicit Char(std::u32string_view charStr) noexcept : Char{charStr, CharStyle{}} {}
    /// Construct a block character from a single Unicode code point with explicit style.
    /// @param codePoint The base Unicode code point.
    /// @param style The style for the character.
    constexpr Char(const char32_t codePoint, const CharStyle style) noexcept : _character{codePoint}, _style{style} {}
    /// Construct a block character from a single Unicode code point with explicit color and attributes.
    /// @param codePoint The base Unicode code point.
    /// @param color The color for the character.
    /// @param attributes The character attributes.
    constexpr Char(const char32_t codePoint, const Color color, const CharAttributes attributes) noexcept :
        Char{codePoint, CharStyle{color, attributes}} {}
    /// Construct a block character with explicit text and style.
    /// @param charStr The UTF-8 encoded text to display.
    /// @param style The style for the character.
    /// Invalid or unsupported text normalizes deterministically to a single renderable character.
    /// Empty input, control codes, and leading zero-width code points normalize to `U+FFFD`.
    /// Later visible code points also collapse the result to `U+FFFD`, while a third combining mark is ignored.
    explicit Char(std::string_view charStr, CharStyle style) noexcept : _character{charStr}, _style{style} {}
    /// Construct a block character with explicit text, color, and attributes.
    /// @param charStr The UTF-8 encoded text to display.
    /// @param color The color for the character.
    /// @param attributes The character attributes.
    /// Invalid or unsupported text normalizes deterministically to a single renderable character.
    /// Empty input, control codes, and leading zero-width code points normalize to `U+FFFD`.
    /// Later visible code points also collapse the result to `U+FFFD`, while a third combining mark is ignored.
    explicit Char(const std::string_view charStr, const Color color, const CharAttributes attributes) noexcept :
        Char{charStr, CharStyle{color, attributes}} {}
    /// Construct a block character with explicit text and style.
    /// @param charStr The UTF-32 encoded text to display.
    /// @param style The style for the character.
    /// Invalid or unsupported text normalizes deterministically to a single renderable character.
    /// Empty input, control codes, invalid Unicode scalar values, and leading zero-width code points normalize to
    /// `U+FFFD`. Later visible code points also collapse the result to `U+FFFD`, while a third combining mark is
    /// ignored.
    explicit Char(std::u32string_view charStr, CharStyle style) noexcept : _character{charStr}, _style{style} {}
    /// Construct a block character with explicit text, color, and attributes.
    /// @param charStr The UTF-32 encoded text to display.
    /// @param color The color for the character.
    /// @param attributes The character attributes.
    /// Invalid or unsupported text normalizes deterministically to a single renderable character.
    /// Empty input, control codes, invalid Unicode scalar values, and leading zero-width code points normalize to
    /// `U+FFFD`. Later visible code points also collapse the result to `U+FFFD`, while a third combining mark is
    /// ignored.
    explicit Char(const std::u32string_view charStr, const Color color, const CharAttributes attributes) noexcept :
        Char{charStr, CharStyle{color, attributes}} {}
    /// Construct a block character from a single Unicode code point and a color.
    /// @param codePoint The base Unicode code point.
    /// @param color The color for the character.
    template <typename... tColorArgs>
        requires CharColorConstructorArgs<tColorArgs...>
    constexpr Char(const char32_t codePoint, tColorArgs... color) noexcept :
        _character{codePoint}, _style{Color{color...}} {}
    /// Construct a block character with explicit text and colors.
    /// @param charStr The UTF-8 encoded text to display.
    /// @param color The color for the character.
    /// Invalid or unsupported text normalizes deterministically to a single renderable character.
    /// Empty input, control codes, and leading zero-width code points normalize to `U+FFFD`.
    /// Later visible code points also collapse the result to `U+FFFD`, while a third combining mark is ignored.
    template <typename... tColorArgs>
        requires CharColorConstructorArgs<tColorArgs...>
    explicit Char(const std::string_view charStr, tColorArgs... color) noexcept :
        _character{charStr}, _style{Color{color...}} {}
    /// Construct a block character with explicit text and colors.
    /// @param charStr The UTF-32 encoded text to display.
    /// @param color The color for the character.
    /// Invalid or unsupported text normalizes deterministically to a single renderable character.
    /// Empty input, control codes, invalid Unicode scalar values, and leading zero-width code points normalize to
    /// `U+FFFD`. Later visible code points also collapse the result to `U+FFFD`, while a third combining mark is
    /// ignored.
    template <typename... tColorArgs>
        requires CharColorConstructorArgs<tColorArgs...>
    explicit Char(const std::u32string_view charStr, tColorArgs... color) noexcept :
        _character{charStr}, _style{Color{color...}} {}

    // defaults
    Char(const Char &) = default;
    Char(Char &&) = default;
    auto operator=(const Char &) -> Char & = default;
    auto operator=(Char &&) -> Char & = default;

public: // operators
    /// Compare two terminal characters for equality.
    [[nodiscard]] auto operator==(const Char &other) const noexcept -> bool {
        return _character == other._character && _style == other._style;
    }
    /// Compare two terminal characters for inequality.
    [[nodiscard]] auto operator!=(const Char &other) const noexcept -> bool { return !(*this == other); }
    /// Compare just a single-code point character, without the color.
    [[nodiscard]] auto operator==(char32_t other) const noexcept -> bool { return _character.operator==(other); }
    /// Compare just a single-code point character, without the color.
    [[nodiscard]] auto operator!=(char32_t other) const noexcept -> bool { return !operator==(other); }

public: // accessors
    /// Get the character string as UTF-8 text.
    /// @return A UTF-8 encoded copy of the stored character sequence.
    [[nodiscard]] auto charStr() const -> std::string { return _character.utf8(); }
    /// Get the leading Unicode code point.
    /// @return The base code point, or `0` if this character is empty.
    [[nodiscard]] constexpr auto mainCodePoint() const noexcept -> char32_t { return _character.mainCodePoint(); }
    /// Get a single Unicode code point or zero for combined or empty characters.
    /// This is a fast-path method for comparing a single-code point character, without the color.
    /// @return The single code point, or `0` if this character is combined or empty.
    [[nodiscard]] constexpr auto singleCodePoint() const noexcept -> char32_t { return _character.singleCodePoint(); }
    /// Get the stored Unicode code points.
    /// Unused entries are set to `0`.
    [[nodiscard]] constexpr auto codePoints() const noexcept -> const impl::CombinedChar::Storage & {
        return _character.codePoints();
    }
    /// Get the number of stored Unicode code points.
    [[nodiscard]] constexpr auto codePointCount() const noexcept -> std::size_t { return _character.codePointCount(); }
    /// Get the character color.
    [[nodiscard]] auto color() const noexcept -> Color { return _style.color(); }
    /// Get the character attributes.
    [[nodiscard]] auto attributes() const noexcept -> CharAttributes { return _style.attributes(); }
    /// Get the combined character style.
    [[nodiscard]] auto style() const noexcept -> const CharStyle & { return _style; }
    /// Get the display width on a terminal in cells.
    [[nodiscard]] auto displayWidth() const noexcept -> int { return _character.displayWidth(); }
    /// Get the number of UTF-8 bytes needed to encode this character.
    [[nodiscard]] auto byteCount() const noexcept -> std::size_t { return _character.byteCount(); }
    /// Append the UTF-8 representation to an existing buffer.
    /// @param buffer The destination buffer.
    void appendTo(std::string &buffer) const noexcept { _character.appendTo(buffer); }

public: // modifiers
    /// Replace the full style of this character.
    /// @param style The new style.
    void setStyle(const CharStyle style) noexcept { _style = style; }
    /// Create a character with an additional combining code point appended.
    /// @param codePoint The combining code point to append.
    /// @param encodingErrors How invalid combining code points are handled.
    /// @return A copy of this character with the combining code point appended.
    /// `EncodingErrors::Replace` and `EncodingErrors::Ignore` keep the original character unchanged for invalid
    /// combining code points or when the storage is already full.
    /// @throws std::invalid_argument If `encodingErrors` is `Throw` and the code point is unsupported.
    [[nodiscard]] auto withCombining(char32_t codePoint, EncodingErrors encodingErrors = EncodingErrors::Replace) const
        -> Char;
    /// Create a character with the given colors applied as an overlay.
    /// @param color The color override to apply.
    /// @return A copy of this character with the adjusted colors.
    /// If the applied colors contain `Inherited`, the existing component from this character is kept unchanged.
    [[nodiscard]] auto withColorOverlay(Color color) const -> Char;
    /// Create a character with style applied on top of the stored style.
    /// `Inherited` color components keep the current color component, and unspecified attributes keep the current
    /// attribute state.
    /// @param color The color override to apply.
    /// @param attributes The attribute override to apply.
    /// @return A copy of this character with the overlaid style.
    [[nodiscard]] auto withOverlay(Color color, CharAttributes attributes) const noexcept -> Char;
    /// Create a character with the given color replacing the stored color.
    /// @param color The replacement color.
    /// @return A copy of this character with exactly `color`.
    [[nodiscard]] auto withColorReplaced(Color color) const noexcept -> Char;
    /// Create a character with the given attributes replacing the stored attributes.
    /// @param attributes The replacement attributes.
    /// @return A copy of this character with exactly `attributes`.
    [[nodiscard]] auto withAttributes(CharAttributes attributes) const noexcept -> Char;
    /// Create a character with a base color underneath the stored color.
    /// @param color The base color.
    /// @return A copy of this character where this character color overlays `color`.
    [[nodiscard]] auto withBaseColor(Color color) const noexcept -> Char;
    /// Create a character with style used as the base underneath the stored style.
    /// The stored color and stored attributes overwrite the base style.
    /// @param color The base color.
    /// @param attributes The base attributes.
    /// @return A copy of this character resolved against the base style.
    [[nodiscard]] auto withBase(Color color, CharAttributes attributes) const noexcept -> Char;
    /// Create a character with another character used as the style base.
    /// Only the style is used from `base`; the stored Unicode character is preserved.
    /// @param base The character providing the base color and attributes.
    /// @return A copy of this character resolved against `base`.
    [[nodiscard]] auto withBase(const Char &base) const noexcept -> Char;

public: // tests
    /// Test if this character is empty (has no code-point).
    [[nodiscard]] constexpr auto isEmpty() const noexcept -> bool { return _character.isEmpty(); }
    /// Test if this character is spacing.
    /// Tests for space, tab, newline, and CR.
    [[nodiscard]] auto isSpacing() const noexcept -> bool;
    /// Test if this character is a control character.
    [[nodiscard]] auto isControl() const noexcept -> bool;
    /// Test if this character is one of the given characters.
    /// Tests only the character, not the color. Only tests one code-point characters.
    [[nodiscard]] auto isOneOf(std::u32string_view characters) const noexcept -> bool;
    /// @overload
    [[nodiscard]] auto isOneOf(std::initializer_list<char32_t> characters) const noexcept -> bool;
    /// @overload
    template <typename... tCharacters>
        requires(sizeof...(tCharacters) >= 1 && (std::convertible_to<tCharacters, char32_t> && ...))
    [[nodiscard]] auto isOneOf(tCharacters... characters) const noexcept -> bool {
        return ((*this == characters) || ...);
    }
    /// Compare how two characters would appear on screen.
    /// Code points must match exactly. When `colorEnabled` is `true`, inherited color components are treated as the
    /// terminal default color before comparing. When `attributeEnabled` is `true`, inherited attributes are treated as
    /// disabled before comparing.
    /// @param other The character to compare with.
    /// @param colorEnabled `true` to include colors in the comparison.
    /// @param attributeEnabled `true` to include character attributes in the comparison.
    /// @return `true` if both characters render identically.
    [[nodiscard]] auto
    renderedEquals(const Char &other, bool colorEnabled = true, bool attributeEnabled = true) const noexcept -> bool;
    /// Get a hash for this character and its color.
    [[nodiscard]] constexpr auto hash() const noexcept -> std::size_t {
        return impl::hashCreate(_character.hash(), _style.hash());
    }

public: // predefined characters.
    /// A space with inherited colors.
    [[nodiscard]] static auto space() noexcept -> const Char &;
    /// Create an empty render cell that carries only the given style.
    /// This is mainly used for wide-character continuation cells, which must stay logically empty while preserving
    /// the visible style of the leading cell.
    /// @param style The style to store on the empty block.
    /// @return An empty block with `style`.
    [[nodiscard]] static auto emptyBlock(CharStyle style) noexcept -> Char;

private:
    Char(const impl::CombinedChar character, const CharStyle style) noexcept : _character{character}, _style{style} {}

private:
    impl::CombinedChar _character; ///< The Unicode character and combining code points.
    CharStyle _style;              ///< The style for this character.
};


}


template <>
struct std::hash<erbsland::cterm::Char> {
    auto operator()(const erbsland::cterm::Char &character) const noexcept -> std::size_t { return character.hash(); }
};
