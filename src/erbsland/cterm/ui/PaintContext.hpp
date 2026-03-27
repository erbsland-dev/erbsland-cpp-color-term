// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../Rectangle.hpp"


namespace erbsland::cterm::ui {

/// The paint context for the `onPaint` method.
class PaintContext {
public:
    /// Create a paint context for a target rectangle.
    /// @param targetRect The rectangle that may be painted in this pass.
    explicit PaintContext(const Rectangle targetRect) noexcept : _targetRect{targetRect} {}

    // defaults
    ~PaintContext() = default;
    PaintContext(const PaintContext &) = default;
    PaintContext(PaintContext &&) = default;
    auto operator=(const PaintContext &) -> PaintContext & = default;
    auto operator=(PaintContext &&) -> PaintContext & = default;

public:
    /// Get the target rectangle for this paint pass.
    /// @return The target rectangle.
    [[nodiscard]] auto targetRect() const noexcept -> const Rectangle & { return _targetRect; }
    /// Set the target rectangle for this paint pass.
    /// @param targetRect The new target rectangle.
    void setTargetRect(const Rectangle targetRect) noexcept { _targetRect = targetRect; }

private:
    Rectangle _targetRect; ///< The target rectangle for this surface to paint on.
};

}
