// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "StackLayoutItem.hpp"

#include "../../AbstractSurfaceContainer.hpp"

#include <vector>

namespace erbsland::cterm::ui::layout::impl {

/// Ordered list of stack layout items with main-axis sizing logic.
class StackLayoutItems final {
public:
    /// Create layout items for the given stack surfaces.
    /// @param surfaces The stack child surfaces.
    /// @param orientation The stack orientation.
    /// @param availableSize The available stack size.
    /// @param scope The parent layout scope.
    /// @return The initialized item collection.
    [[nodiscard]] static auto fromSurfaces(
        const AbstractSurfaceContainer &surfaces,
        Orientation orientation,
        Size availableSize,
        LayoutScope &scope) noexcept -> StackLayoutItems;

public:
    /// Adjust all main-axis sizes to fit the available size.
    /// @param availableMainSize The available size on the main axis.
    void resolveMainSizes(Coordinate availableMainSize) noexcept;

    /// Access the resolved items.
    /// @return The resolved layout items.
    [[nodiscard]] auto items() const noexcept -> const std::vector<StackLayoutItem> & { return _items; }

    /// Apply the resolved rectangles to all items.
    /// @param orientation The stack orientation.
    /// @param scope The parent layout scope.
    void applyLayout(Orientation orientation, LayoutScope &scope) const noexcept;

private:
    /// Create a new item collection.
    StackLayoutItems(Orientation orientation, std::vector<StackLayoutItem> items) noexcept;

    /// Get the currently assigned total size on the main axis.
    [[nodiscard]] auto totalMainSize() const noexcept -> Coordinate;

    /// Distribute free space to growing items.
    /// @param remainingSpace The remaining free space.
    void distributeExtraSpace(Coordinate remainingSpace) noexcept;

    /// Shrink items to fit the available main size.
    /// @param availableMainSize The available main size.
    void shrinkOverflow(Coordinate availableMainSize) noexcept;

    /// Shrink internal margin spacing before reducing child sizes.
    /// @param overflow The remaining overflow.
    void shrinkSpacing(Coordinate &overflow) noexcept;

    /// Shrink one policy group.
    /// @param overflow The remaining overflow.
    /// @param policyType The selected policy group.
    /// @param shrinkBelowMinimum If `true`, shrinking may continue down to zero.
    void shrinkPolicyGroup(Coordinate &overflow, DimensionPolicy::Type policyType, bool shrinkBelowMinimum) noexcept;

private:
    Orientation _orientation{Orientation::Vertical};
    std::vector<StackLayoutItem> _items;
};

}
