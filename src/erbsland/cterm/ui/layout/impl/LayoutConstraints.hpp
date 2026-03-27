// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "DimensionConstraints.hpp"


namespace erbsland::cterm::ui::layout::impl {

/// Layout constraints for resolving a child surface against the available parent size.
class LayoutConstraints final {
public:
    /// Create empty constraints.
    LayoutConstraints() = default;
    /// Create explicit constraints for both dimensions.
    /// @param widthConstraints The width constraints.
    /// @param heightConstraints The height constraints.
    constexpr LayoutConstraints(
        const DimensionConstraints widthConstraints, const DimensionConstraints heightConstraints) noexcept :
        _widthConstraints{widthConstraints}, _heightConstraints{heightConstraints} {}

    // defaults
    ~LayoutConstraints() = default;
    LayoutConstraints(const LayoutConstraints &) = default;
    LayoutConstraints(LayoutConstraints &&) = default;
    auto operator=(const LayoutConstraints &) -> LayoutConstraints & = default;
    auto operator=(LayoutConstraints &&) -> LayoutConstraints & = default;

public:
    /// Create layout constraints from a geometry description.
    /// @param geometry The source geometry.
    /// @return The extracted layout constraints.
    [[nodiscard]] static auto fromGeometry(const Geometry &geometry) noexcept -> LayoutConstraints;

    /// Resolve the final size for the available parent size.
    /// @param availableSize The available size inside the parent.
    /// @return The resolved child size.
    [[nodiscard]] auto resolve(Size availableSize) const noexcept -> Size;

private:
    DimensionConstraints _widthConstraints;
    DimensionConstraints _heightConstraints;
};

}
