// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StackLayoutItem.hpp"

namespace erbsland::cterm::ui::layout::impl {

auto StackLayoutItem::fromSurface(
    const SurfacePtr &surface, const Orientation orientation, const Size availableSize) noexcept -> StackLayoutItem {
    const auto mainConstraints = DimensionConstraints::fromGeometry(surface->geometry(), orientation);
    const auto crossAxis = orientation.crossed();
    const auto crossConstraints = DimensionConstraints::fromGeometry(surface->geometry(), crossAxis);
    const auto availableMainSize = availableSize.coordinate(orientation);
    const auto availableCrossSize = availableSize.coordinate(crossAxis);
    return {
        surface,
        mainConstraints,
        crossConstraints,
        mainConstraints.initialSize(availableMainSize),
        crossConstraints.resolve(availableCrossSize)};
}

StackLayoutItem::StackLayoutItem(
    SurfacePtr surface,
    DimensionConstraints mainConstraints,
    DimensionConstraints crossConstraints,
    const Coordinate assignedMainSize,
    const Coordinate assignedCrossSize) noexcept :
    _surface(std::move(surface)),
    _mainConstraints(std::move(mainConstraints)),
    _crossConstraints(std::move(crossConstraints)),
    _assignedMainSize(assignedMainSize),
    _assignedCrossSize(assignedCrossSize) {
}

auto StackLayoutItem::assignedMainSize() const noexcept -> Coordinate {
    return _assignedMainSize;
}

auto StackLayoutItem::assignedCrossSize() const noexcept -> Coordinate {
    return _assignedCrossSize;
}

auto StackLayoutItem::policyType() const noexcept -> DimensionPolicy::Type {
    return _mainConstraints.policyType();
}

auto StackLayoutItem::factor() const noexcept -> int {
    return _mainConstraints.factor();
}

auto StackLayoutItem::canGrow() const noexcept -> bool {
    return _mainConstraints.canGrow(_assignedMainSize);
}

auto StackLayoutItem::canShrink(const DimensionPolicy::Type policyType, const bool shrinkBelowMinimum) const noexcept
    -> bool {
    return this->policyType() == policyType && _mainConstraints.canShrink(_assignedMainSize, shrinkBelowMinimum);
}

auto StackLayoutItem::grow(const Coordinate requestedGrowth) noexcept -> Coordinate {
    const auto appliedGrowth = std::min(requestedGrowth, _mainConstraints.growLimit(_assignedMainSize));
    _assignedMainSize += appliedGrowth;
    return appliedGrowth;
}

auto StackLayoutItem::shrink(const Coordinate requestedShrink, const bool shrinkBelowMinimum) noexcept -> Coordinate {
    const auto appliedShrink =
        std::min(requestedShrink, _mainConstraints.shrinkLimit(_assignedMainSize, shrinkBelowMinimum));
    _assignedMainSize -= appliedShrink;
    return appliedShrink;
}

void StackLayoutItem::applyLayout(const Orientation orientation, const Coordinate mainOffset) const noexcept {
    const auto childSize = orientation == Orientation::Horizontal ? Size{_assignedMainSize, _assignedCrossSize}
                                                                  : Size{_assignedCrossSize, _assignedMainSize};
    const auto childPos = orientation == Orientation::Horizontal ? Position{mainOffset, 0} : Position{0, mainOffset};
    _surface->setRectangle(Rectangle{childPos, childSize});
    _surface->onLayout(childSize);
}

}
