// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Pages.hpp"

#include "../../theme/ThemePainter.hpp"

#include <algorithm>
#include <stdexcept>

namespace erbsland::cterm::ui::layout {

Pages::Pages(ProtectedTag) noexcept {
}

auto Pages::create() -> PagesPtr {
    auto result = std::make_shared<Pages>(ProtectedTag{});
    result->initializeUi();
    return result;
}

void Pages::initializeUi() {
    Layout::initializeUi();
}

void Pages::nextPage() {
    const auto count = pageCount();
    if (count <= 0) {
        _currentPage = 0;
        return;
    }
    const auto next = std::min(currentPage() + 1, count - 1);
    if (_currentPage == next) {
        return;
    }
    _currentPage = next;
    flags().setPaintOutdated();
}

void Pages::previousPage() {
    const auto count = pageCount();
    if (count <= 0) {
        _currentPage = 0;
        return;
    }
    const auto previous = std::max(currentPage() - 1, 0);
    if (_currentPage == previous) {
        return;
    }
    _currentPage = previous;
    flags().setPaintOutdated();
}

auto Pages::currentPage() const noexcept -> int {
    const auto pages = visiblePages();
    return clampedCurrentPage(pages);
}

auto Pages::pageCount() const noexcept -> int {
    return static_cast<int>(visiblePages().size());
}

void Pages::showPage(const int index) {
    const auto pages = visiblePages();
    if (index < 0 || index >= static_cast<int>(pages.size())) {
        throw std::out_of_range{"Page index is out of range."};
    }
    if (_currentPage == index) {
        return;
    }
    _currentPage = index;
    flags().setPaintOutdated();
}

void Pages::showPage(SurfacePtr surface) {
    const auto index = indexForPage(std::move(surface));
    if (index < 0) {
        throw std::out_of_range{"Surface is not a visible page."};
    }
    showPage(index);
}

auto Pages::indexForPage(SurfacePtr surface) const noexcept -> int {
    if (surface == nullptr || !surface->flags().isVisible()) {
        return -1;
    }
    auto index = 0;
    for (const auto &page : surfaces()) {
        if (page == nullptr || !page->flags().isVisible()) {
            continue;
        }
        if (page == surface) {
            return index;
        }
        index += 1;
    }
    return -1;
}

auto Pages::isOpaque() const noexcept -> bool {
    return true;
}

auto Pages::onMeasure([[maybe_unused]] MeasureScope &scope, [[maybe_unused]] const LayoutProposal &proposal) noexcept
    -> LayoutMetrics {
    return layoutMetrics();
}

void Pages::onLayout(LayoutScope &scope) noexcept {
    const auto pages = visiblePages();
    const auto pageIndex = clampedCurrentPage(pages);
    if (pageIndex < 0) {
        return;
    }
    const auto &page = pages[static_cast<std::size_t>(pageIndex)];
    const auto metrics = scope.measure(page, LayoutProposal::atMost(scope.size()));
    placeCentered(page, metrics, scope);
}

void Pages::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    theme::ThemePainter{buffer, context.theme().forPart(theme::Part::Background)}.fill(context.surfaceRect());
}

auto Pages::visiblePages() const -> std::vector<SurfacePtr> {
    auto result = std::vector<SurfacePtr>{};
    for (const auto &page : surfaces()) {
        if (page != nullptr && page->flags().isVisible()) {
            result.emplace_back(page);
        }
    }
    return result;
}

auto Pages::clampedCurrentPage(const std::vector<SurfacePtr> &pages) const noexcept -> int {
    if (pages.empty()) {
        return -1;
    }
    return std::clamp(_currentPage, 0, static_cast<int>(pages.size()) - 1);
}

void Pages::placeCentered(const SurfacePtr &surface, const LayoutMetrics &metrics, LayoutScope &scope) noexcept {
    const auto availableSize = scope.size();
    const auto margins = metrics.margins();
    const auto contentAvailableSize = availableSize - margins.extent();
    const auto childSize =
        metrics.resolvedSize(LayoutProposal::atMost(contentAvailableSize)).limitedWith(contentAvailableSize);
    const auto outerSize = (childSize + margins.extent()).limitedWith(availableSize);
    const auto offset = availableSize.alignmentOffset(outerSize, Alignment::Center);
    scope.place(surface, Rectangle{offset + Position{margins.left(), margins.top()}, childSize});
}

}
