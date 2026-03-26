// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "impl/CombinedChar.hpp"
#include "impl/HashHelper.hpp"

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>


namespace erbsland::cterm {


/// A simple representation of a key press.
/// Supports Unicode text input and common special keys.
class Key {
public:
    /// Supported key kinds.
    enum Type : uint8_t {
        /// No supported key was decoded.
        None,
        /// A single Unicode code point.
        Character,
        /// Multiple code points that form one combined text input.
        Combined,
        /// The Enter/Return key.
        Enter,
        /// The tab key.
        Tab,
        /// Reverse tab / Shift+Tab.
        BackTab,
        /// The space key.
        Space,
        /// The escape key.
        Escape,
        /// The backspace key.
        Backspace,
        /// The insert key.
        Insert,
        /// The delete key.
        Delete,
        /// The home key.
        Home,
        /// The end key.
        End,
        /// The page up key.
        PageUp,
        /// The page down key.
        PageDown,
        /// The left cursor key.
        Left,
        /// The right cursor key.
        Right,
        /// The up cursor key.
        Up,
        /// The down cursor key.
        Down,
        /// The function key F1.
        F1,
        /// The function key F2.
        F2,
        /// The function key F3.
        F3,
        /// The function key F4.
        F4,
        /// The function key F5.
        F5,
        /// The function key F6.
        F6,
        /// The function key F7.
        F7,
        /// The function key F8.
        F8,
        /// The function key F9.
        F9,
        /// The function key F10.
        F10,
        /// The function key F11.
        F11,
        /// The function key F12.
        F12,
    };

public:
    /// Create an invalid key.
    Key() = default;
    /// Create a key with explicit type and optional Unicode payload.
    /// @param type The key type.
    /// @param codePoint The Unicode value for `Type::Character`.
    Key(Type type, char32_t codePoint = 0) noexcept;
    /// Create a key with explicit combined Unicode payload.
    /// @param type The key type.
    /// @param character The combined Unicode text for `Type::Character` or `Type::Combined`.
    /// @throws std::invalid_argument If `character` is not a supported Unicode character sequence.
    Key(Type type, std::u32string_view character);

public: // operators
    /// Compare two key events for equality.
    auto operator==(const Key &other) const noexcept -> bool = default;

public: // accessors
    /// Get the key type.
    [[nodiscard]] auto type() const noexcept -> Type { return _type; }
    /// Legacy ASCII accessor for `Type::Character`.
    /// @deprecated Use `unicode()` or `combined()` to support full Unicode input.
    /// @return The ASCII character for single-code-point character input, otherwise `0`.
    [[nodiscard]] auto character() const noexcept -> char;
    /// Get the Unicode code point for `Type::Character`.
    /// @return The single Unicode code point, or `0` if this key does not store exactly one code point.
    [[nodiscard]] auto unicode() const noexcept -> char32_t;
    /// Get the full combined Unicode payload for character input.
    /// @return The stored Unicode text, or an empty string for non-character keys.
    [[nodiscard]] auto combined() const -> std::u32string;
    /// Test if this object represents a supported key.
    [[nodiscard]] auto valid() const noexcept -> bool { return _type != None; }
    /// Get a hash for this key.
    [[nodiscard]] constexpr auto hash() const noexcept -> std::size_t {
        return impl::hashCreate(static_cast<uint8_t>(_type), _character.hash());
    }

public: // conversion
    /// Decode a key from configuration text.
    /// @param text The textual key name.
    /// @return The decoded key, or `Type::None` if the text is unsupported.
    [[nodiscard]] static auto fromString(std::string text) noexcept -> Key;
    /// Decode a key from console input text.
    /// @param text The input text or escape sequence.
    /// @return The decoded key, or `Type::None` if the input is unsupported.
    [[nodiscard]] static auto fromConsoleInput(const std::string &text) noexcept -> Key;
    /// Convert the key to configuration text.
    /// @return The canonical textual key name.
    [[nodiscard]] auto toString() const -> std::string;
    /// Convert the key to human-readable display text.
    /// @param useBrackets If `true`, wrap the text in `[` and `]`.
    /// @return The display text for prompts and help texts.
    [[nodiscard]] auto toDisplayText(bool useBrackets = true) const -> std::string;

private:
    Type _type{None};
    impl::CombinedChar _character;
};


}


template <>
struct std::hash<erbsland::cterm::Key> {
    auto operator()(const erbsland::cterm::Key &key) const noexcept -> std::size_t { return key.hash(); }
};
