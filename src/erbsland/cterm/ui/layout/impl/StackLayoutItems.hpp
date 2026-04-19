// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "StackLayoutItem.hpp"

#include <vector>

namespace erbsland::cterm::ui::layout::impl {

/// Ordered list of stack layout items with main-axis sizing logic.
class StackLayoutItems final {
public:
    /// Create layout items for the given stack children.
    /// @param children The stack children.
    /// @param orientation The stack orientation.
    /// @param availableSize The available stack size.
    /// @return The initialized item collection.
    [[nodiscard]] static auto
    fromChildren(const std::vector<SurfacePtr> &children, Orientation orientation, Size availableSize) noexcept
        -> StackLayoutItems;

public:
    /// Adjust all main-axis sizes to fit the available size.
    /// @param availableMainSize The available size on the main axis.
    void resolveMainSizes(Coordinate availableMainSize) noexcept;

    /// Apply the resolved rectangles to all items.
    /// @param orientation The stack orientation.
    void applyLayout(Orientation orientation) const noexcept;

private:
    /// Create a new item collection.
    explicit StackLayoutItems(std::vector<StackLayoutItem> items) noexcept;

    /// Get the currently assigned total size on the main axis.
    [[nodiscard]] auto totalMainSize() const noexcept -> Coordinate;

    /// Distribute free space to growing items.
    /// @param remainingSpace The remaining free space.
    void distributeExtraSpace(Coordinate remainingSpace) noexcept;

    /// Shrink items to fit the available main size.
    /// @param availableMainSize The available main size.
    void shrinkOverflow(Coordinate availableMainSize) noexcept;

    /// Shrink one policy group.
    /// @param overflow The remaining overflow.
    /// @param policyType The selected policy group.
    /// @param shrinkBelowMinimum If `true`, shrinking may continue down to zero.
    void shrinkPolicyGroup(Coordinate &overflow, DimensionPolicy::Type policyType, bool shrinkBelowMinimum) noexcept;

private:
    std::vector<StackLayoutItem> _items;
};

}
