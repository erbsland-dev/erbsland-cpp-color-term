// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Viewport.hpp"

#include "../impl/ScrollMetrics.hpp"

#include <algorithm>

namespace erbsland::cterm::ui::layout {

Viewport::Viewport(ProtectedTag) noexcept {
}

auto Viewport::create() -> ViewportPtr {
    auto result = std::make_shared<Viewport>(ProtectedTag{});
    result->initializeUi();
    return result;
}

auto Viewport::alignment() const noexcept -> Alignment {
    return _alignment;
}

void Viewport::setAlignment(const Alignment alignment) noexcept {
    if (_alignment == alignment) {
        return;
    }
    _alignment = alignment;
    flags().setLayoutOutdated();
}

auto Viewport::scrollOffset() const noexcept -> Position {
    return _scrollOffset;
}

void Viewport::setScrollOffset(const Position scrollOffset) noexcept {
    const auto contentSize = contentSizeForViewport(rectangle().size());
    const auto clampedOffset = ui::impl::clampedScrollOffset(scrollOffset, contentSize, rectangle().size());
    if (_scrollOffset == clampedOffset) {
        return;
    }
    _scrollOffset = clampedOffset;
    flags().setLayoutOutdated();
}

void Viewport::scrollBy(const Position delta) noexcept {
    setScrollOffset(_scrollOffset + delta);
}

void Viewport::scrollUp(const Coordinate count) noexcept {
    scrollBy(Position{0, -std::max(count, Coordinate{0})});
}

void Viewport::scrollDown(const Coordinate count) noexcept {
    scrollBy(Position{0, std::max(count, Coordinate{0})});
}

void Viewport::scrollLeft(const Coordinate count) noexcept {
    scrollBy(Position{-std::max(count, Coordinate{0}), 0});
}

void Viewport::scrollRight(const Coordinate count) noexcept {
    scrollBy(Position{std::max(count, Coordinate{0}), 0});
}

void Viewport::pageUp() noexcept {
    scrollUp(std::max(rectangle().height(), Coordinate{1}));
}

void Viewport::pageDown() noexcept {
    scrollDown(std::max(rectangle().height(), Coordinate{1}));
}

void Viewport::pageLeft() noexcept {
    scrollLeft(std::max(rectangle().width(), Coordinate{1}));
}

void Viewport::pageRight() noexcept {
    scrollRight(std::max(rectangle().width(), Coordinate{1}));
}

void Viewport::scrollToTop() noexcept {
    setScrollOffset(Position{_scrollOffset.x(), 0});
}

void Viewport::scrollToBottom() noexcept {
    setScrollOffset(Position{_scrollOffset.x(), Position::maximum().y()});
}

void Viewport::scrollToLeftEdge() noexcept {
    setScrollOffset(Position{0, _scrollOffset.y()});
}

void Viewport::scrollToRightEdge() noexcept {
    setScrollOffset(Position{Position::maximum().x(), _scrollOffset.y()});
}

void Viewport::scrollIntoView(const Rectangle contentRect) noexcept {
    const auto contentSize = contentSizeForViewport(rectangle().size());
    setScrollOffset(ui::impl::scrollOffsetForVisibleRect(_scrollOffset, contentRect, contentSize, rectangle().size()));
}

auto Viewport::contentSizeForViewport(const Size viewportSize) const noexcept -> Size {
    const auto &content = contentSurface();
    if (content == nullptr || !content->flags().isVisible()) {
        return {};
    }
    return ui::impl::resolveScrollContentSize(content->layoutMetrics(), viewportSize);
}

auto Viewport::contentSizeForViewport(const Size viewportSize, LayoutScope &scope) noexcept -> Size {
    const auto &content = contentSurface();
    if (content == nullptr || !content->flags().isVisible()) {
        return {};
    }
    const auto metrics = measureContentForViewport(content, viewportSize, scope);
    return ui::impl::resolveScrollContentSize(metrics, viewportSize);
}

auto Viewport::contentSize() const noexcept -> Size {
    return _contentSize;
}

void Viewport::onLayout(LayoutScope &scope) noexcept {
    const auto newParentSize = scope.size();
    const auto &content = contentSurface();
    if (content == nullptr || !content->flags().isVisible()) {
        _contentSize = {};
        _scrollOffset = {};
        return;
    }
    const auto viewportRect = Rectangle{Position{}, newParentSize};
    const auto metrics = measureContentForViewport(content, newParentSize, scope);
    const auto margins = metrics.margins();
    _contentSize = ui::impl::resolveScrollContentSize(metrics, newParentSize);
    clampScrollOffset();
    const auto contentOrigin = ui::impl::alignedContentOrigin(viewportRect, _contentSize, _scrollOffset, _alignment);
    const auto childSize = _contentSize - margins.extent();
    scope.place(content, Rectangle{contentOrigin + Position{margins.left(), margins.top()}, childSize});
}

auto Viewport::measureContentForViewport(
    const SurfacePtr &content, const Size viewportSize, LayoutScope &scope) noexcept -> LayoutMetrics {
    auto metrics = scope.measure(content, LayoutProposal::atMost(viewportSize));
    metrics = scope.measure(content, LayoutProposal::atMost(viewportSize - metrics.margins().extent()));
    return metrics;
}

void Viewport::clampScrollOffset() noexcept {
    _scrollOffset = ui::impl::clampedScrollOffset(_scrollOffset, _contentSize, rectangle().size());
}

}
