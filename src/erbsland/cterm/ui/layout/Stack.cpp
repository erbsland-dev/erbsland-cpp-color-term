// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Stack.hpp"

#include "impl/StackLayoutItems.hpp"

#include <algorithm>

namespace erbsland::cterm::ui::layout {

Stack::Stack(const Orientation orientation, ProtectedTag) noexcept : _orientation{orientation} {
}

auto Stack::create(Orientation orientation) -> StackPtr {
    return std::make_shared<Stack>(orientation, ProtectedTag{});
}

auto Stack::onMeasure(MeasureScope &scope, const LayoutProposal &proposal) noexcept -> LayoutMetrics {
    auto contentMinimum = Size{};
    auto contentPreferred = Size{};
    const auto childProposal = LayoutProposal{proposal.width(), proposal.height()};
    for (const auto &child : surfaces()) {
        if (child == nullptr || !child->flags().isVisible()) {
            continue;
        }
        const auto childMetrics = scope.measure(child, childProposal);
        if (_orientation == Orientation::Horizontal) {
            contentMinimum.setWidth(saturatedAdd(contentMinimum.width(), childMetrics.minimum().width()));
            contentPreferred.setWidth(saturatedAdd(contentPreferred.width(), childMetrics.preferred().width()));
            contentMinimum.setHeight(std::max(contentMinimum.height(), childMetrics.minimum().height()));
            contentPreferred.setHeight(std::max(contentPreferred.height(), childMetrics.preferred().height()));
        } else {
            contentMinimum.setHeight(saturatedAdd(contentMinimum.height(), childMetrics.minimum().height()));
            contentPreferred.setHeight(saturatedAdd(contentPreferred.height(), childMetrics.preferred().height()));
            contentMinimum.setWidth(std::max(contentMinimum.width(), childMetrics.minimum().width()));
            contentPreferred.setWidth(std::max(contentPreferred.width(), childMetrics.preferred().width()));
        }
    }
    auto result = layoutMetrics();
    result.setMinimum(result.minimum().componentMax(contentMinimum).componentMin(result.maximum()));
    result.setPreferred(result.preferred()
                            .componentMax(result.minimum())
                            .componentMax(contentPreferred)
                            .componentMin(result.maximum()));
    return result;
}

void Stack::onLayout(LayoutScope &scope) noexcept {
    auto items = impl::StackLayoutItems::fromSurfaces(surfaces(), _orientation, scope.size(), scope);
    items.resolveMainSizes(scope.size().coordinate(_orientation));
    items.applyLayout(_orientation, scope);
}

auto Stack::saturatedAdd(const Coordinate left, const Coordinate right) noexcept -> Coordinate {
    const auto maximum = Size::maximum().width();
    if (left > maximum - right) {
        return maximum;
    }
    return left + right;
}

}
