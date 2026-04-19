// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "CharAttributes.hpp"
#include "Color.hpp"

#include <functional>

namespace erbsland::cterm {

/// A combined terminal text style with color and character attributes.
class CharStyle final {
public:
    /// Create a style with inherited color and attributes.
    constexpr CharStyle() noexcept = default;
    /// Create a style from color and attributes.
    /// @param color The color for the style.
    /// @param attributes The character attributes for the style.
    constexpr CharStyle(const Color color, const CharAttributes attributes = {}) noexcept :
        _color{color}, _attributes{attributes} {}
    /// Create a style from character attributes and inherited color.
    /// @param attributes The character attributes for the style.
    constexpr CharStyle(const CharAttributes attributes) noexcept : _attributes{attributes} {}

    // defaults
    ~CharStyle() = default;
    CharStyle(const CharStyle &) = default;
    CharStyle(CharStyle &&) = default;
    auto operator=(const CharStyle &) -> CharStyle & = default;
    auto operator=(CharStyle &&) -> CharStyle & = default;

public: // operators
    /// Compare two character styles for equality.
    auto operator==(const CharStyle &) const noexcept -> bool = default;
    /// Compare two character styles for inequality.
    auto operator!=(const CharStyle &) const noexcept -> bool = default;

public: // accessors
    /// Get the color part of the style.
    [[nodiscard]] auto color() const noexcept -> Color { return _color; }
    /// Set the color part of the style.
    /// @param color The new color.
    void setColor(const Color color) noexcept { _color = color; }
    /// Get the foreground color.
    [[nodiscard]] auto fg() const noexcept -> Foreground { return _color.fg(); }
    /// Set the foreground color.
    /// @param foreground The new foreground color.
    void setFg(const Foreground foreground) noexcept { _color.setFg(foreground); }
    /// Get the background color.
    [[nodiscard]] auto bg() const noexcept -> Background { return _color.bg(); }
    /// Set the background color.
    /// @param background The new background color.
    void setBg(const Background background) noexcept { _color.setBg(background); }
    /// Get the character attributes.
    [[nodiscard]] auto attributes() const noexcept -> CharAttributes { return _attributes; }
    /// Set the character attributes.
    /// @param attributes The new character attributes.
    void setAttributes(const CharAttributes attributes) noexcept { _attributes = attributes; }

public: // tools
    /// Create a new style by overlaying another style onto this one.
    /// Inherited color components keep the existing color, and unspecified attributes keep the existing attributes.
    /// @param overlay The overlay style.
    /// @return The combined style.
    [[nodiscard]] auto withOverlay(const CharStyle overlay) const noexcept -> CharStyle {
        return CharStyle{_color.overlayWith(overlay._color), overlay._attributes.resolvedWith(_attributes)};
    }
    /// Create a new style by placing a base style underneath this one.
    /// The current style overwrites inherited or unspecified parts from the base style.
    /// @param base The base style.
    /// @return The resolved style.
    [[nodiscard]] auto withBase(const CharStyle base) const noexcept -> CharStyle {
        return CharStyle{base._color.overlayWith(_color), _attributes.resolvedWith(base._attributes)};
    }
    /// Get a stable hash for the character style.
    [[nodiscard]] constexpr auto hash() const noexcept -> std::size_t {
        return impl::hashCreate(_color.hash(), _attributes.hash());
    }

private:
    Color _color{};               ///< The foreground/background colors.
    CharAttributes _attributes{}; ///< The character attributes.
};

}

template <>
struct std::hash<erbsland::cterm::CharStyle> {
    auto operator()(const erbsland::cterm::CharStyle &style) const noexcept -> std::size_t { return style.hash(); }
};
