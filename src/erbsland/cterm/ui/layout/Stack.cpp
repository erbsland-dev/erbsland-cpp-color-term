// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Stack.hpp"

#include "impl/StackLayoutItems.hpp"

#include <algorithm>

namespace erbsland::cterm::ui::layout {

Stack::Stack(const Orientation orientation, ProtectedTag) noexcept : _orientation{orientation} {
}

auto Stack::create(Orientation orientation) -> StackPtr {
    auto result = std::make_shared<Stack>(orientation, ProtectedTag{});
    result->initializeUi();
    return result;
}

auto Stack::onMeasure(MeasureScope &scope, const LayoutProposal &proposal) noexcept -> LayoutMetrics {
    auto contentMinimum = Size{};
    auto contentPreferred = Size{};
    auto propagatedMargins = Margins{};
    auto previousMargins = Margins{};
    auto hasVisibleChild = false;
    const auto childProposal = LayoutProposal{proposal.width(), proposal.height()};
    for (const auto &child : surfaces()) {
        if (child == nullptr || !child->flags().isVisible()) {
            continue;
        }
        const auto childMetrics = scope.measure(child, childProposal);
        const auto childMargins = childMetrics.margins();
        if (!hasVisibleChild) {
            propagatedMargins.set(Margins::leadingSide(_orientation), childMargins.leading(_orientation));
        } else {
            const auto spacing = std::max(previousMargins.trailing(_orientation), childMargins.leading(_orientation));
            const auto spacingSize = Size{spacing, spacing};
            contentMinimum.add(spacingSize, _orientation);
            contentPreferred.add(spacingSize, _orientation);
        }
        propagatedMargins.set(Margins::trailingSide(_orientation), childMargins.trailing(_orientation));
        propagatedMargins.expandTo(childMargins, Margins::crossLeadingSide(_orientation));
        propagatedMargins.expandTo(childMargins, Margins::crossTrailingSide(_orientation));
        contentMinimum.add(childMetrics.minimum(), _orientation);
        contentPreferred.add(childMetrics.preferred(), _orientation);
        contentMinimum.expandTo(childMetrics.minimum(), _orientation.crossed());
        contentPreferred.expandTo(childMetrics.preferred(), _orientation.crossed());
        previousMargins = childMargins;
        hasVisibleChild = true;
    }
    auto result = layoutMetrics();
    result.setMinimum(result.minimum().expandedWith(contentMinimum).limitedWith(result.maximum()));
    result.setPreferred(
        result.preferred().expandedWith(result.minimum()).expandedWith(contentPreferred).limitedWith(result.maximum()));
    result.setMargins(result.margins().expandedWith(propagatedMargins));
    return result;
}

void Stack::onLayout(LayoutScope &scope) noexcept {
    auto items = impl::StackLayoutItems::fromSurfaces(surfaces(), _orientation, scope.size(), scope);
    items.resolveMainSizes(scope.size().coordinate(_orientation));
    items.applyLayout(_orientation, scope);
}

}
