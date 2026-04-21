// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../theme/Element.hpp"
#include "../theme/Tags.hpp"

namespace erbsland::cterm::ui {

/// Theme selector attributes configured on a surface.
class SurfaceThemeAttributes final {
public:
    /// Create default theme attributes for a generic surface.
    SurfaceThemeAttributes() = default;
    /// Create theme attributes with an element.
    /// @param element The theme element for the surface.
    explicit SurfaceThemeAttributes(theme::Element element) noexcept;
    ~SurfaceThemeAttributes() = default;

    // defaults
    SurfaceThemeAttributes(const SurfaceThemeAttributes &) = default;
    SurfaceThemeAttributes(SurfaceThemeAttributes &&) = default;
    auto operator=(const SurfaceThemeAttributes &) -> SurfaceThemeAttributes & = default;
    auto operator=(SurfaceThemeAttributes &&) -> SurfaceThemeAttributes & = default;

public:
    /// Access the theme element.
    [[nodiscard]] auto element() const noexcept -> theme::Element { return _element; }
    /// Replace the theme element without invalidating the surface.
    /// @param element The new theme element.
    void setElement(theme::Element element) noexcept { _element = element; }
    /// Access the theme tags.
    [[nodiscard]] auto tags() const noexcept -> const theme::Tags & { return _tags; }
    /// Replace the theme tags without invalidating the surface.
    /// @param tags The new theme tags.
    void setTags(theme::Tags tags) noexcept { _tags = tags; }

private:
    theme::Element _element{theme::Element::Surface}; ///< The theme element.
    theme::Tags _tags;                                ///< The theme tags.
};

}
