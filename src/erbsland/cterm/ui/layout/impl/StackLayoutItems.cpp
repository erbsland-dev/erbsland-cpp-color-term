// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StackLayoutItems.hpp"

#include <algorithm>

namespace erbsland::cterm::ui::layout::impl {

auto StackLayoutItems::fromSurfaces(
    const AbstractSurfaceContainer &surfaces,
    const Orientation orientation,
    const Size availableSize,
    LayoutScope &scope) noexcept -> StackLayoutItems {
    auto items = std::vector<StackLayoutItem>{};
    items.reserve(surfaces.size());
    auto previousMargins = Margins{};
    auto first = true;
    for (const auto &surface : surfaces) {
        if (!surface->flags().isVisible()) {
            continue;
        }
        auto item = StackLayoutItem::fromSurface(surface, orientation, availableSize, scope);
        if (!first) {
            item.setSpacingBefore(std::max(previousMargins.trailing(orientation), item.margins().leading(orientation)));
        }
        previousMargins = item.margins();
        first = false;
        items.push_back(std::move(item));
    }
    return StackLayoutItems{orientation, std::move(items)};
}

void StackLayoutItems::resolveMainSizes(const Coordinate availableMainSize) noexcept {
    const auto totalSize = totalMainSize();
    if (totalSize < availableMainSize) {
        distributeExtraSpace(availableMainSize - totalSize);
        return;
    }
    if (totalSize > availableMainSize) {
        shrinkOverflow(availableMainSize);
    }
}

StackLayoutItems::StackLayoutItems(const Orientation orientation, std::vector<StackLayoutItem> items) noexcept :
    _orientation(orientation), _items(std::move(items)) {
}

void StackLayoutItems::applyLayout(const Orientation orientation, LayoutScope &scope) const noexcept {
    auto mainOffset = Coordinate{0};
    for (const auto &item : _items) {
        mainOffset += item.spacingBefore();
        item.applyLayout(orientation, mainOffset, scope);
        mainOffset += item.assignedMainSize();
    }
}

auto StackLayoutItems::totalMainSize() const noexcept -> Coordinate {
    auto totalSize = Coordinate{0};
    for (const auto &item : _items) {
        totalSize += item.spacingBefore() + item.assignedMainSize();
    }
    return totalSize;
}

void StackLayoutItems::distributeExtraSpace(Coordinate remainingSpace) noexcept {
    while (remainingSpace > 0) {
        auto growFactorSum = 0;
        for (const auto &item : _items) {
            if (item.policyType() == DimensionPolicy::Grow && item.canGrow()) {
                growFactorSum += item.factor();
            }
        }
        if (growFactorSum == 0) {
            return;
        }
        auto usedSpace = Coordinate{0};
        for (auto &item : _items) {
            if (item.policyType() != DimensionPolicy::Grow || !item.canGrow()) {
                continue;
            }
            const auto desiredGrowth = std::max<Coordinate>(1, remainingSpace * item.factor() / growFactorSum);
            usedSpace += item.grow(desiredGrowth);
            if (usedSpace >= remainingSpace) {
                break;
            }
        }
        if (usedSpace == 0) {
            return;
        }
        remainingSpace -= usedSpace;
    }
}

void StackLayoutItems::shrinkOverflow(const Coordinate availableMainSize) noexcept {
    auto overflow = totalMainSize() - availableMainSize;
    shrinkSpacing(overflow);
    shrinkPolicyGroup(overflow, DimensionPolicy::Shrink, false);
    shrinkPolicyGroup(overflow, DimensionPolicy::Preferred, false);
    shrinkPolicyGroup(overflow, DimensionPolicy::Grow, false);
    shrinkPolicyGroup(overflow, DimensionPolicy::Grow, true);
    shrinkPolicyGroup(overflow, DimensionPolicy::Preferred, true);
    shrinkPolicyGroup(overflow, DimensionPolicy::Shrink, true);
}

void StackLayoutItems::shrinkSpacing(Coordinate &overflow) noexcept {
    for (auto &item : _items) {
        if (overflow <= 0) {
            return;
        }
        overflow -= item.shrinkSpacingBefore(overflow);
    }
}

void StackLayoutItems::shrinkPolicyGroup(
    Coordinate &overflow, const DimensionPolicy::Type policyType, const bool shrinkBelowMinimum) noexcept {
    while (overflow > 0) {
        auto shrinkFactorSum = 0;
        for (const auto &item : _items) {
            if (item.canShrink(policyType, shrinkBelowMinimum)) {
                shrinkFactorSum += item.factor();
            }
        }
        if (shrinkFactorSum == 0) {
            return;
        }
        auto reducedSpace = Coordinate{0};
        for (auto &item : _items) {
            if (!item.canShrink(policyType, shrinkBelowMinimum)) {
                continue;
            }
            const auto desiredShrink = std::max<Coordinate>(1, overflow * item.factor() / shrinkFactorSum);
            reducedSpace += item.shrink(desiredShrink, shrinkBelowMinimum);
            if (reducedSpace >= overflow) {
                break;
            }
        }
        if (reducedSpace == 0) {
            return;
        }
        overflow -= reducedSpace;
    }
}

}
