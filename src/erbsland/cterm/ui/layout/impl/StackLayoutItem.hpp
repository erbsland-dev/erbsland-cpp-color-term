// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "DimensionConstraints.hpp"

#include "../../Surface.hpp"

namespace erbsland::cterm::ui::layout::impl {

/// One child surface together with its resolved stack-layout state.
class StackLayoutItem final {
public:
    /// Create one layout item from a surface and the available stack size.
    /// @param surface The child surface.
    /// @param orientation The stack orientation.
    /// @param availableSize The available stack size.
    /// @param scope The parent layout scope.
    /// @return The initialized layout item.
    [[nodiscard]] static auto
    fromSurface(const SurfacePtr &surface, Orientation orientation, Size availableSize, LayoutScope &scope) noexcept
        -> StackLayoutItem;

public:
    /// Get the assigned size on the main axis.
    [[nodiscard]] auto assignedMainSize() const noexcept -> Coordinate;

    /// Get the resolved spacing before this item.
    [[nodiscard]] auto spacingBefore() const noexcept -> Coordinate;

    /// Get the size on the cross axis.
    [[nodiscard]] auto assignedCrossSize() const noexcept -> Coordinate;

    /// Access the child margins.
    [[nodiscard]] auto margins() const noexcept -> Margins;

    /// Access the surface for this item.
    [[nodiscard]] auto surface() const noexcept -> const SurfacePtr &;

    /// Get the main-axis policy type.
    [[nodiscard]] auto policyType() const noexcept -> DimensionPolicy::Type;

    /// Get the main-axis policy factor.
    [[nodiscard]] auto factor() const noexcept -> int;

    /// Test if this item can still grow on the main axis.
    [[nodiscard]] auto canGrow() const noexcept -> bool;

    /// Test if this item can still shrink on the main axis.
    /// @param policyType The required policy type.
    /// @param shrinkBelowMinimum If `true`, shrinking may continue down to zero.
    [[nodiscard]] auto canShrink(DimensionPolicy::Type policyType, bool shrinkBelowMinimum) const noexcept -> bool;

    /// Grow the assigned main size.
    /// @param requestedGrowth The requested growth.
    /// @return The applied growth.
    [[nodiscard]] auto grow(Coordinate requestedGrowth) noexcept -> Coordinate;

    /// Shrink the assigned main size.
    /// @param requestedShrink The requested shrink amount.
    /// @param shrinkBelowMinimum If `true`, shrinking may continue down to zero.
    /// @return The applied shrink.
    [[nodiscard]] auto shrink(Coordinate requestedShrink, bool shrinkBelowMinimum) noexcept -> Coordinate;

    /// Shrink the spacing before this item.
    /// @param requestedShrink The requested shrink amount.
    /// @return The applied shrink.
    [[nodiscard]] auto shrinkSpacingBefore(Coordinate requestedShrink) noexcept -> Coordinate;

    /// Set the preferred spacing before this item.
    /// @param spacingBefore The collapsed margin spacing before this item.
    void setSpacingBefore(Coordinate spacingBefore) noexcept;

    /// Apply the resolved rectangle to the surface.
    /// @param orientation The stack orientation.
    /// @param mainOffset The position on the main axis.
    /// @param scope The parent layout scope.
    void applyLayout(Orientation orientation, Coordinate mainOffset, LayoutScope &scope) const noexcept;

private:
    /// Create an item with fully resolved constraints.
    StackLayoutItem(
        SurfacePtr surface,
        Margins margins,
        DimensionConstraints mainConstraints,
        DimensionConstraints crossConstraints,
        Coordinate assignedMainSize,
        Coordinate assignedCrossSize) noexcept;

private:
    SurfacePtr _surface;
    Margins _margins;
    DimensionConstraints _mainConstraints;
    DimensionConstraints _crossConstraints;
    Coordinate _spacingBefore{0};
    Coordinate _assignedMainSize{0};
    Coordinate _assignedCrossSize{0};
};

}
