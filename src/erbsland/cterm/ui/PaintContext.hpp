// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "ThemeContext.hpp"

#include "../geometry/Rectangle.hpp"

#include <utility>

namespace erbsland::cterm::ui {

/// The paint context for the `onPaint` method.
class PaintContext {
public:
    /// Create a paint context for a clipped paint operation.
    /// @param surfaceRect The full local rectangle of the surface.
    /// @param visibleRect The visible part of the surface in local coordinates.
    /// @param dirtyRect The dirty part of the surface in local coordinates.
    /// @param themeContext The theme scope for this paint pass.
    PaintContext(
        const Rectangle surfaceRect,
        const Rectangle visibleRect,
        const Rectangle dirtyRect,
        ThemeContext themeContext) noexcept :
        _surfaceRect{surfaceRect},
        _visibleRect{visibleRect},
        _dirtyRect{dirtyRect},
        _themeContext{std::move(themeContext)} {}

    // defaults
    ~PaintContext() = default;
    PaintContext(const PaintContext &) = default;
    PaintContext(PaintContext &&) = default;
    auto operator=(const PaintContext &) -> PaintContext & = delete;
    auto operator=(PaintContext &&) -> PaintContext & = delete;

public:
    /// Get the full local rectangle of the surface.
    /// @return The full local rectangle of the surface.
    [[nodiscard]] auto surfaceRect() const noexcept -> const Rectangle & { return _surfaceRect; }
    /// Get the visible part of the surface in local coordinates.
    /// @return The visible local rectangle.
    [[nodiscard]] auto visibleRect() const noexcept -> const Rectangle & { return _visibleRect; }
    /// Get the dirty part of the surface in local coordinates.
    /// @return The dirty local rectangle.
    [[nodiscard]] auto dirtyRect() const noexcept -> const Rectangle & { return _dirtyRect; }
    /// Get the theme context used for this paint pass.
    /// @return The theme context.
    [[nodiscard]] auto themeContext() const noexcept -> const ThemeContext & { return _themeContext; }
    /// Access the theme for this paint pass.
    /// @return The read-only theme accessor.
    [[nodiscard]] auto theme() const noexcept -> theme::ThemeAccessor { return _themeContext.theme(); }

private:
    const Rectangle _surfaceRect;     ///< The full local rectangle of the surface.
    const Rectangle _visibleRect;     ///< The visible part of the surface in local coordinates.
    const Rectangle _dirtyRect;       ///< The dirty part of the surface in local coordinates.
    const ThemeContext _themeContext; ///< The theme scope for this paint pass.
};

}
