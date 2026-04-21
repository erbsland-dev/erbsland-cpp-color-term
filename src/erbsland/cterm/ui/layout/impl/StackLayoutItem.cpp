// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StackLayoutItem.hpp"

#include "../../../geometry/AxisMapper.hpp"

namespace erbsland::cterm::ui::layout::impl {

auto StackLayoutItem::fromSurface(
    const SurfacePtr &surface, const Orientation orientation, const Size availableSize, LayoutScope &scope) noexcept
    -> StackLayoutItem {
    const auto axis = AxisMapper{orientation};
    const auto crossAxis = orientation.crossed();
    const auto availableMainDimension = LayoutDimension::atMost(availableSize.coordinate(orientation));
    const auto availableCrossDimension = LayoutDimension::exact(availableSize.coordinate(crossAxis));
    const auto initialProposal = LayoutProposal{
        axis.horizontalValue(availableMainDimension, availableCrossDimension),
        axis.verticalValue(availableMainDimension, availableCrossDimension)};
    auto metrics = scope.measure(surface, initialProposal);
    auto crossConstraints = DimensionConstraints::fromMetrics(metrics, crossAxis);
    const auto availableCrossSize = availableSize.coordinate(crossAxis);
    const auto assignedCrossSize = crossConstraints.resolve(availableCrossSize);
    const auto assignedCrossDimension = LayoutDimension::exact(assignedCrossSize);
    const auto finalProposal = LayoutProposal{
        axis.horizontalValue(availableMainDimension, assignedCrossDimension),
        axis.verticalValue(availableMainDimension, assignedCrossDimension)};
    metrics = scope.measure(surface, finalProposal);
    const auto mainConstraints = DimensionConstraints::fromMetrics(metrics, orientation);
    crossConstraints = DimensionConstraints::fromMetrics(metrics, crossAxis);
    const auto availableMainSize = availableSize.coordinate(orientation);
    return {
        surface, mainConstraints, crossConstraints, mainConstraints.initialSize(availableMainSize), assignedCrossSize};
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

auto StackLayoutItem::surface() const noexcept -> const SurfacePtr & {
    return _surface;
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

void StackLayoutItem::applyLayout(
    const Orientation orientation, const Coordinate mainOffset, LayoutScope &scope) const noexcept {
    const auto axis = AxisMapper{orientation};
    scope.place(_surface, Rectangle{axis.position(mainOffset), axis.size(_assignedMainSize, _assignedCrossSize)});
}

}
