// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ColorPart.hpp"

#include <string_view>
#include <vector>


namespace erbsland::cterm {


/// A foreground/background color pair for terminal rendering.
class Color final {
public: // ctors/dtor/assign/move
    /// Create the default terminal color.
    Color() = default;
    /// Create a color from explicit foreground and background parts.
    /// @param foreground The foreground color.
    /// @param background The background color.
    Color(Foreground foreground, Background background = Background::Default) noexcept;
    /// Destroy the color value.
    ~Color() = default;
    /// Copy construct a color value.
    Color(const Color &) noexcept = default;
    /// Copy-assign a color value.
    auto operator=(const Color &) noexcept -> Color & = default;

public: // operators
    /// Compare two colors for equality.
    auto operator==(const Color &other) const -> bool = default;
    /// Compare two colors for inequality.
    auto operator!=(const Color &other) const -> bool = default;

public: // accessors
    /// Get the foreground color.
    [[nodiscard]] auto fg() const noexcept -> Foreground { return _foreground; }
    /// Set the foreground color.
    /// @param foreground The new foreground color.
    void setFg(const Foreground foreground) noexcept { _foreground = foreground; }
    /// Get the background color.
    [[nodiscard]] auto bg() const noexcept -> Background { return _background; }
    /// Set the background color.
    /// @param background The new background color.
    void setBg(const Background background) noexcept { _background = background; }

public: // conversion and tools
    /// Combine this color with a new overlay color.
    /// For foreground or background color that is set to default, the new color will use this color.
    /// The default color acts like a transparent color.
    ///
    /// Examples:
    ///
    /// - [this.fg = default] + [new.fg = default] => [result.fg = default]
    /// - [this.fg = red] + [new.fg = default] => [result.fg = red]
    /// - [this.fg = red] + [new.fg = green] => [result.fg = green]
    /// - [this.fg = red] + [new.fg = green] => [result.fg = green]
    ///
    /// @param overlay The overlay color.
    /// @return The new color with the overlay applied.
    [[nodiscard]] auto overlayWith(const Color &overlay) const -> Color;
    /// Convert a color or color-pair into a block color.
    /// Format must be either `"fg"` or `"fg:bg"`.
    /// @param str The textual color specification.
    /// @return The parsed color.
    /// @throws std::invalid_argument if one of the colors does not exist.
    [[nodiscard]] static auto fromString(std::string_view str) -> Color;

public: // data
    /// Foreground component of the color.
    Foreground _foreground;
    /// Background component of the color.
    Background _background;
};


/// A list of terminal colors.
using ColorList = std::vector<Color>;


}
