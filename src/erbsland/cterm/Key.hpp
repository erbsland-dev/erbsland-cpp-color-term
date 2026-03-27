// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "impl/CombinedChar.hpp"
#include "impl/HashHelper.hpp"

#include <array>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>


namespace erbsland::cterm {


/// A simple representation of a key press.
/// Supports Unicode text input and common special keys.
class Key {
public:
    /// Supported key kinds.
    enum Type : uint8_t {
        None,      ///< No supported key was decoded.
        Character, ///< A single Unicode code point.
        Combined,  ///< Multiple code points that form one combined text input.
        Enter,     ///< The Enter/Return key.
        Tab,       ///< The tab key.
        BackTab,   ///< Reverse tab / Shift+Tab.
        Space,     ///< The space key.
        Escape,    ///< The escape key.
        Backspace, ///< The backspace key.
        Insert,    ///< The insert key.
        Delete,    ///< The delete key.
        Home,      ///< The home key.
        End,       ///< The end key.
        PageUp,    ///< The page up key.
        PageDown,  ///< The page down key.
        Left,      ///< The left cursor key.
        Right,     ///< The right cursor key.
        Up,        ///< The up cursor key.
        Down,      ///< The down cursor key.
        F1,        ///< The function key F1.
        F2,        ///< The function key F2.
        F3,        ///< The function key F3.
        F4,        ///< The function key F4.
        F5,        ///< The function key F5.
        F6,        ///< The function key F6.
        F7,        ///< The function key F7.
        F8,        ///< The function key F8.
        F9,        ///< The function key F9.
        F10,       ///< The function key F10.
        F11,       ///< The function key F11.
        F12,       ///< The function key F12.
    };

public:
    /// Create an invalid key.
    Key() = default;
    /// Create a key with an explicit type and optional Unicode payload.
    /// @param type The key type.
    /// @param codePoint The Unicode value for `Type::Character`.
    explicit Key(Type type, char32_t codePoint = 0) noexcept;
    /// Create a key with an explicit combined Unicode payload.
    /// @param type The key type.
    /// @param character The combined Unicode text for `Type::Character` or `Type::Combined`.
    /// @throws std::invalid_argument If `character` is not a supported Unicode character sequence.
    Key(Type type, std::u32string_view character);

public: // operators
    /// Compare two key events for equality.
    auto operator==(const Key &other) const noexcept -> bool = default;
    auto operator!=(const Key &other) const noexcept -> bool = default;
    /// Compare against a single code point.
    /// This requires `type()` == `Character` and `unicode()` == `other`.
    [[nodiscard]] auto operator==(char32_t other) const noexcept -> bool;
    [[nodiscard]] auto operator!=(char32_t other) const noexcept -> bool;
    /// Compare against a combined key
    /// This requires `type()` == `Combined` and `combined()` == `other`.
    [[nodiscard]] auto operator==(std::u32string_view other) const noexcept -> bool;
    [[nodiscard]] auto operator!=(std::u32string_view other) const noexcept -> bool;
    /// Compare against a special key.
    /// This requires `type()` == `type` and `type` != `Character`|`Combined`.
    [[nodiscard]] auto operator==(Type type) const noexcept -> bool;
    [[nodiscard]] auto operator!=(Type type) const noexcept -> bool;

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
    /// Decode a key from the configuration text.
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
    struct KeyTextDefinition final {
        Type type;
        std::string_view text;
        std::string_view displayText;
    };

    struct KeyAliasDefinition final {
        std::string_view text;
        Type type;
    };

    /// Get the canonical text definitions for special keys.
    [[nodiscard]] static auto keyTextDefinitions() noexcept -> const std::array<KeyTextDefinition, 28> &;
    /// Get the accepted alias definitions for parsing key names.
    [[nodiscard]] static auto keyAliasDefinitions() noexcept -> const std::array<KeyAliasDefinition, 39> &;
    /// Find the display and serialization text for a special key.
    [[nodiscard]] static auto findKeyTextDefinition(Type type) noexcept -> const KeyTextDefinition *;
    /// Normalize a key name for case-insensitive alias matching.
    [[nodiscard]] static auto normalizeKeyText(std::string text) noexcept -> std::string;
    /// Wrap display text in square brackets when requested.
    [[nodiscard]] static auto wrapDisplayText(std::string_view text, bool useBrackets) -> std::string;
    /// Create a character or combined key from normalized Unicode input.
    [[nodiscard]] static auto createCharacterKey(const impl::CombinedChar &character) noexcept -> Key;
    /// Parse Unicode text into a character key when possible.
    [[nodiscard]] static auto parseCharacterKeyText(std::string_view text) -> std::optional<Key>;

private:
    Type _type{None};
    impl::CombinedChar _character;
};


}


template <>
struct std::hash<erbsland::cterm::Key> {
    auto operator()(const erbsland::cterm::Key &key) const noexcept -> std::size_t { return key.hash(); }
};
