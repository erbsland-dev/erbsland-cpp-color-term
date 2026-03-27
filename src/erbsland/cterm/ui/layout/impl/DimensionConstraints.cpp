// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "DimensionConstraints.hpp"


#include <algorithm>


namespace erbsland::cterm::ui::layout::impl {

auto DimensionConstraints::fromGeometry(const Geometry &geometry, const Orientation orientation) noexcept
    -> DimensionConstraints {
    return {
        geometry.minimum().coordinate(orientation),
        geometry.maximum().coordinate(orientation),
        geometry.preferred().coordinate(orientation),
        orientation == Orientation::Horizontal ? geometry.sizePolicy().width() : geometry.sizePolicy().height()};
}

auto DimensionConstraints::resolve(const Coordinate availableSize) const noexcept -> Coordinate {
    const auto boundedAvailableSize = std::max(Coordinate{0}, availableSize);
    if (policyType() == DimensionPolicy::Type::Grow) {
        return std::min(
            std::clamp(boundedAvailableSize, boundedMinimumSize(), boundedMaximumSize()), boundedAvailableSize);
    }
    return initialSize(boundedAvailableSize);
}

auto DimensionConstraints::initialSize(const Coordinate availableSize) const noexcept -> Coordinate {
    const auto boundedAvailableSize = std::max(Coordinate{0}, availableSize);
    return std::min(std::clamp(_preferredSize, boundedMinimumSize(), boundedMaximumSize()), boundedAvailableSize);
}

auto DimensionConstraints::policyType() const noexcept -> DimensionPolicy::Type {
    return _policy.type();
}

auto DimensionConstraints::factor() const noexcept -> int {
    return std::max(_policy.factor(), 1);
}

auto DimensionConstraints::canGrow(const Coordinate currentSize) const noexcept -> bool {
    return currentSize < boundedMaximumSize();
}

auto DimensionConstraints::growLimit(const Coordinate currentSize) const noexcept -> Coordinate {
    return std::max(Coordinate{0}, boundedMaximumSize() - currentSize);
}

auto DimensionConstraints::canShrink(const Coordinate currentSize, const bool shrinkBelowMinimum) const noexcept
    -> bool {
    return currentSize > (shrinkBelowMinimum ? Coordinate{0} : boundedMinimumSize());
}

auto DimensionConstraints::shrinkLimit(const Coordinate currentSize, const bool shrinkBelowMinimum) const noexcept
    -> Coordinate {
    return std::max(Coordinate{0}, currentSize - (shrinkBelowMinimum ? Coordinate{0} : boundedMinimumSize()));
}

auto DimensionConstraints::boundedMinimumSize() const noexcept -> Coordinate {
    return std::max(Coordinate{0}, std::min(_minimumSize, _maximumSize));
}

auto DimensionConstraints::boundedMaximumSize() const noexcept -> Coordinate {
    return std::max(boundedMinimumSize(), _maximumSize);
}

}
