// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../../../geometry/Orientation.hpp"
#include "../../LayoutMetrics.hpp"

namespace erbsland::cterm::ui::layout::impl {

/// Constraints and policy for a single layout dimension.
class DimensionConstraints final {
public:
    /// Create empty constraints with the default policy.
    DimensionConstraints() = default;
    /// Create explicit constraints for one dimension.
    /// @param minimumSize The minimum size.
    /// @param maximumSize The maximum size.
    /// @param preferredSize The preferred size.
    /// @param policy The size policy.
    constexpr DimensionConstraints(
        const Coordinate minimumSize,
        const Coordinate maximumSize,
        const Coordinate preferredSize,
        const DimensionPolicy policy) noexcept :
        _minimumSize{minimumSize}, _maximumSize{maximumSize}, _preferredSize{preferredSize}, _policy{policy} {}

    // defaults
    ~DimensionConstraints() = default;
    DimensionConstraints(const DimensionConstraints &) = default;
    DimensionConstraints(DimensionConstraints &&) = default;
    auto operator=(const DimensionConstraints &) -> DimensionConstraints & = default;
    auto operator=(DimensionConstraints &&) -> DimensionConstraints & = default;

public:
    /// Create dimension constraints from one axis of layout metrics.
    /// @param metrics The source metrics.
    /// @param orientation The selected axis.
    /// @return The extracted constraints.
    [[nodiscard]] static auto fromMetrics(const LayoutMetrics &metrics, Orientation orientation) noexcept
        -> DimensionConstraints;

    /// Resolve a dimension against an available size.
    /// `Grow` policies use the full available size within the configured range.
    /// `Preferred` and `Shrink` policies start from the preferred size.
    /// @param availableSize The available size.
    /// @return The resolved size.
    [[nodiscard]] auto resolve(Coordinate availableSize) const noexcept -> Coordinate;

    /// Resolve the initial size that stack layout starts with on its main axis.
    /// This always starts from the preferred size within the configured range.
    /// @param availableSize The available size.
    /// @return The initial resolved size.
    [[nodiscard]] auto initialSize(Coordinate availableSize) const noexcept -> Coordinate;

    /// Get the policy type.
    /// @return The configured policy type.
    [[nodiscard]] auto policyType() const noexcept -> DimensionPolicy::Type;

    /// Get the policy factor.
    /// @return The configured factor, clamped to at least `1`.
    [[nodiscard]] auto factor() const noexcept -> int;

    /// Test if the current size can still grow.
    /// @param currentSize The currently assigned size.
    /// @return `true` if more size may be assigned.
    [[nodiscard]] auto canGrow(Coordinate currentSize) const noexcept -> bool;

    /// Get the maximum growth for the current size.
    /// @param currentSize The currently assigned size.
    /// @return The remaining growth capacity.
    [[nodiscard]] auto growLimit(Coordinate currentSize) const noexcept -> Coordinate;

    /// Test if the current size can shrink.
    /// @param currentSize The currently assigned size.
    /// @param shrinkBelowMinimum If `true`, shrinking may continue down to zero.
    /// @return `true` if the size can shrink.
    [[nodiscard]] auto canShrink(Coordinate currentSize, bool shrinkBelowMinimum) const noexcept -> bool;

    /// Get the maximum shrink amount for the current size.
    /// @param currentSize The currently assigned size.
    /// @param shrinkBelowMinimum If `true`, shrinking may continue down to zero.
    /// @return The remaining shrink capacity.
    [[nodiscard]] auto shrinkLimit(Coordinate currentSize, bool shrinkBelowMinimum) const noexcept -> Coordinate;

private:
    /// Get the normalized minimum size.
    [[nodiscard]] auto boundedMinimumSize() const noexcept -> Coordinate;
    /// Get the normalized maximum size.
    [[nodiscard]] auto boundedMaximumSize() const noexcept -> Coordinate;

private:
    Coordinate _minimumSize{0};
    Coordinate _maximumSize{0};
    Coordinate _preferredSize{0};
    DimensionPolicy _policy;
};

}
