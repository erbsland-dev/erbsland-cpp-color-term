// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Color.hpp"

#include <string>
#include <string_view>


namespace erbsland::cterm {


/// Represents a character string with foreground and background colors.
/// Used by the UI code to render colored text blocks on the console.
class Char {
public: // ctors/dtor/assign/move
    /// Construct an empty block character using default colors.
    Char() = default;
    /// Construct a block character with default colors.
    /// @param charStr The text to display.
    explicit Char(std::string_view charStr);
    /// Construct a block character with explicit text and colors.
    /// @param charStr The text to display.
    /// @param fgColor The foreground color.
    /// @param bgColor The background color.
    Char(std::string_view charStr, Foreground fgColor, Background bgColor);
    /// Construct a block character with explicit text and colors.
    /// @param charStr The text to display.
    /// @param color The color of the block.
    Char(std::string_view charStr, Color color);

public: // accessors
    /// Get the character string.
    [[nodiscard]] auto charStr() const noexcept -> const std::string & { return _charStr; }
    /// Get the character color.
    [[nodiscard]] auto color() const noexcept -> Color { return _color; }
    /// Get the display width on a terminal in cells.
    [[nodiscard]] auto displayWidth() const noexcept -> int { return _displayWidth; }

public: // modifiers
    /// Create a character with the given colors applied.
    /// @param color The color override to apply.
    /// @return A copy of this character with the adjusted colors.
    /// If the applied colors contain `Default`, the color of the character is kept unchanged.
    [[nodiscard]] auto withColor(Color color) const -> Char;

public: // tests
    /// Test if this character is spacing.
    /// Tests for space, tab, newline, and CR.
    [[nodiscard]] auto isSpacing() const noexcept -> bool;

private:
    std::string _charStr;     ///< The UTF-8 encoded, combined character.
    Color _color;             ///< The color for this character.
    uint8_t _displayWidth{0}; ///< The display width of the character in cells.
};


}
