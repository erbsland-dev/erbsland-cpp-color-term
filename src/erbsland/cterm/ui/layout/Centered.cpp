// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Centered.hpp"

#include <algorithm>
#include <utility>

namespace erbsland::cterm::ui::layout {

Centered::Centered(ProtectedTag) noexcept {
}

auto Centered::create() -> CenteredPtr {
    auto result = std::make_shared<Centered>(ProtectedTag{});
    result->initializeUi();
    return result;
}

auto Centered::padding() const noexcept -> Margins {
    return _padding;
}

void Centered::setPadding(const Margins padding) noexcept {
    if (_padding == padding) {
        return;
    }
    _padding = padding;
    flags().setLayoutOutdated();
    flags().setPaintOutdated();
}

auto Centered::onMeasure(MeasureScope &scope, const LayoutProposal &proposal) noexcept -> LayoutMetrics {
    const auto &content = contentSurface();
    if (content == nullptr || !content->flags().isVisible()) {
        return layoutMetrics();
    }
    const auto contentMetrics = scope.measure(content, contentProposal(proposal, _padding));
    auto result = paddedMetrics(contentMetrics, _padding);
    if (proposal.width().hasBound() || proposal.height().hasBound()) {
        const auto proposalMaximum = proposal.maximumSize();
        result.setMinimum(result.minimum().limitedWith(proposalMaximum));
        result.setPreferred(result.preferred().limitedWith(proposalMaximum));
    }
    return result;
}

void Centered::onLayout(LayoutScope &scope) noexcept {
    const auto &content = contentSurface();
    if (content == nullptr || !content->flags().isVisible()) {
        return;
    }
    auto effectivePadding = _padding;
    auto availableSize = contentSize(scope.size(), effectivePadding);
    if (availableSize.isZero() && !scope.size().isZero()) {
        effectivePadding = Margins{1};
        availableSize = contentSize(scope.size(), effectivePadding);
    }
    if (availableSize.isZero() && !scope.size().isZero()) {
        effectivePadding = {};
        availableSize = scope.size();
    }
    const auto childMetrics = scope.measure(content, LayoutProposal::atMost(availableSize));
    const auto childSize = childMetrics.resolvedSize(LayoutProposal::atMost(availableSize)).limitedWith(availableSize);
    const auto offset = availableSize.alignmentOffset(childSize, Alignment::Center);
    const auto childPosition =
        Position{std::max(effectivePadding.left(), 0) + offset.x(), std::max(effectivePadding.top(), 0) + offset.y()};
    scope.place(content, Rectangle{childPosition, childSize});
}

auto Centered::contentSize(const Size size, const Margins padding) noexcept -> Size {
    return size - padding.extent();
}

auto Centered::contentProposal(const LayoutProposal proposal, const Margins padding) noexcept -> LayoutProposal {
    const auto paddingExtent = padding.extent();
    auto result = proposal;
    if (proposal.width().hasBound()) {
        result = result.withWidth(
            LayoutDimension{proposal.width().type(), proposal.width().value() - paddingExtent.width()});
    }
    if (proposal.height().hasBound()) {
        result = result.withHeight(
            LayoutDimension{proposal.height().type(), proposal.height().value() - paddingExtent.height()});
    }
    return result;
}

auto Centered::paddedMetrics(const LayoutMetrics &contentMetrics, const Margins contentInset) const noexcept
    -> LayoutMetrics {
    const auto padding = contentInset.extent();
    auto result = layoutMetrics();
    result.setMinimum((contentMetrics.minimum() + padding).limitedWith(result.maximum()));
    result.setPreferred(
        (contentMetrics.preferred() + padding).expandedWith(result.minimum()).limitedWith(result.maximum()));
    return result;
}

}
