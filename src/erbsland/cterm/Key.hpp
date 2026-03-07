// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>
#include <string>


namespace erbsland::cterm {


/// A simple representation of a key press.
/// Supports alphanumeric characters and common special keys.
class Key {
public:
    /// Supported key kinds.
    enum Type : uint8_t {
        /// No supported key was decoded.
        None,
        /// A single alphanumeric character.
        Character,
        /// The Enter/Return key.
        Enter,
        /// The tab key.
        Tab,
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

public: // ctors/dtor/assign/move
    /// Create an invalid key.
    Key() = default;
    /// Create a key with explicit type and optional character payload.
    /// @param type The key type.
    /// @param ch The character value for `Type::Character`.
    Key(Type type, char ch = 0) noexcept;

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

public: // operators
    /// Compare two key events for equality.
    auto operator==(const Key &other) const noexcept -> bool = default;

public: // accessors
    /// Get the key type.
    [[nodiscard]] auto type() const noexcept -> Type { return _type; }
    /// Get the character payload.
    /// Returns `0` for non-character keys.
    [[nodiscard]] auto character() const noexcept -> char { return _ch; }
    /// Test if this object represents a supported key.
    [[nodiscard]] auto valid() const noexcept -> bool { return _type != None; }

private:
    Type _type{None};
    char _ch{0};
};


}
