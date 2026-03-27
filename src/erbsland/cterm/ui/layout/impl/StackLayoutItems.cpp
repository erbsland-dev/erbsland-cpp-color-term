// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StackLayoutItems.hpp"


#include <algorithm>


namespace erbsland::cterm::ui::layout::impl {

auto StackLayoutItems::fromChildren(
    const std::vector<SurfacePtr> &children, const Orientation orientation, const Size availableSize) noexcept
    -> StackLayoutItems {
    auto items = std::vector<StackLayoutItem>{};
    items.reserve(children.size());
    for (const auto &child : children) {
        items.push_back(StackLayoutItem::fromSurface(child, orientation, availableSize));
    }
    return StackLayoutItems{std::move(items)};
}

StackLayoutItems::StackLayoutItems(std::vector<StackLayoutItem> items) noexcept : _items(std::move(items)) {
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

void StackLayoutItems::applyLayout(const Orientation orientation) const noexcept {
    auto mainOffset = Coordinate{0};
    for (const auto &item : _items) {
        item.applyLayout(orientation, mainOffset);
        mainOffset += item.assignedMainSize();
    }
}

auto StackLayoutItems::totalMainSize() const noexcept -> Coordinate {
    auto totalSize = Coordinate{0};
    for (const auto &item : _items) {
        totalSize += item.assignedMainSize();
    }
    return totalSize;
}

void StackLayoutItems::distributeExtraSpace(Coordinate remainingSpace) noexcept {
    while (remainingSpace > 0) {
        auto growFactorSum = 0;
        for (const auto &item : _items) {
            if (item.policyType() == DimensionPolicy::Type::Grow && item.canGrow()) {
                growFactorSum += item.factor();
            }
        }
        if (growFactorSum == 0) {
            return;
        }
        auto usedSpace = Coordinate{0};
        for (auto &item : _items) {
            if (item.policyType() != DimensionPolicy::Type::Grow || !item.canGrow()) {
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
    shrinkPolicyGroup(overflow, DimensionPolicy::Type::Shrink, false);
    shrinkPolicyGroup(overflow, DimensionPolicy::Type::Preferred, false);
    shrinkPolicyGroup(overflow, DimensionPolicy::Type::Grow, false);
    shrinkPolicyGroup(overflow, DimensionPolicy::Type::Grow, true);
    shrinkPolicyGroup(overflow, DimensionPolicy::Type::Preferred, true);
    shrinkPolicyGroup(overflow, DimensionPolicy::Type::Shrink, true);
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
