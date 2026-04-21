// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "MeasureScope.hpp"

namespace erbsland::cterm::ui {

/// Gives a surface access to its assigned size plus framework-driven child placement.
class LayoutScope final {
public:
    /// The function used to place one child surface.
    using PlaceFunction = std::function<void(const SurfacePtr &, Rectangle)>;

public:
    /// Create an empty layout scope.
    LayoutScope() = default;
    /// Create a layout scope.
    /// @param size The assigned size of the surface being laid out.
    /// @param themeContext The resolved theme context.
    /// @param measureFunction The callback used by `measure()`.
    /// @param placeFunction The callback used by `place()`.
    LayoutScope(
        Size size,
        ThemeContext themeContext,
        MeasureScope::MeasureFunction measureFunction,
        PlaceFunction placeFunction) noexcept;

    // defaults
    ~LayoutScope() = default;
    LayoutScope(const LayoutScope &) = default;
    LayoutScope(LayoutScope &&) = default;
    auto operator=(const LayoutScope &) -> LayoutScope & = delete;
    auto operator=(LayoutScope &&) -> LayoutScope & = delete;

public:
    /// Access the assigned size of the current surface.
    [[nodiscard]] auto size() const noexcept -> Size { return _size; }
    /// Access the resolved theme context of the current surface.
    [[nodiscard]] auto themeContext() const noexcept -> const ThemeContext & { return _themeContext; }
    /// Access the theme of the current surface.
    [[nodiscard]] auto theme() const noexcept -> theme::ThemeAccessor { return _themeContext.theme(); }
    /// Measure a child surface for a proposal.
    [[nodiscard]] auto measure(const SurfacePtr &surface, const LayoutProposal &proposal) const noexcept
        -> LayoutMetrics;
    /// Place a child surface at a local rectangle.
    /// @param surface The child surface.
    /// @param rectangle The rectangle in the current surface's local coordinates.
    void place(const SurfacePtr &surface, Rectangle rectangle) const noexcept;

private:
    Size _size;                   ///< The assigned size of the current surface.
    ThemeContext _themeContext;   ///< The resolved theme context for the current surface.
    MeasureScope _measureScope;   ///< Child measurement access.
    PlaceFunction _placeFunction; ///< Child placement callback.
};

}
