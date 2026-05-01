// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "AbstractScrollArea.hpp"

#include "../impl/ScrollMetrics.hpp"

#include <algorithm>

namespace erbsland::cterm::ui::surface {

AbstractScrollArea::AbstractScrollArea(ProtectedTag) noexcept :
    Surface{LayoutMetrics{Size{}, Size::maximum(), Size{}, SizePolicy{SizePolicy::Grow}}} {
    childStorage().setManager(*this);
}

auto AbstractScrollArea::scrollOffset() const noexcept -> Position {
    return _scrollOffset;
}

void AbstractScrollArea::setScrollOffset(const Position scrollOffset) noexcept {
    const auto contentSize = contentSizeForViewport(_viewportRect.size());
    const auto clampedOffset = impl::clampedScrollOffset(scrollOffset, contentSize, _viewportRect.size());
    if (_scrollOffset == clampedOffset) {
        return;
    }
    _scrollOffset = clampedOffset;
    updateScrollBars();
    onScrollOffsetChanged(_scrollOffset);
}

void AbstractScrollArea::scrollBy(const Position delta) noexcept {
    setScrollOffset(_scrollOffset + delta);
}

void AbstractScrollArea::scrollUp(const Coordinate count) noexcept {
    scrollBy(Position{0, -std::max(count, Coordinate{0})});
}

void AbstractScrollArea::scrollDown(const Coordinate count) noexcept {
    scrollBy(Position{0, std::max(count, Coordinate{0})});
}

void AbstractScrollArea::scrollLeft(const Coordinate count) noexcept {
    scrollBy(Position{-std::max(count, Coordinate{0}), 0});
}

void AbstractScrollArea::scrollRight(const Coordinate count) noexcept {
    scrollBy(Position{std::max(count, Coordinate{0}), 0});
}

void AbstractScrollArea::pageUp() noexcept {
    scrollUp(std::max(_viewportRect.height(), Coordinate{1}));
}

void AbstractScrollArea::pageDown() noexcept {
    scrollDown(std::max(_viewportRect.height(), Coordinate{1}));
}

void AbstractScrollArea::pageLeft() noexcept {
    scrollLeft(std::max(_viewportRect.width(), Coordinate{1}));
}

void AbstractScrollArea::pageRight() noexcept {
    scrollRight(std::max(_viewportRect.width(), Coordinate{1}));
}

void AbstractScrollArea::scrollToTop() noexcept {
    setScrollOffset(Position{_scrollOffset.x(), 0});
}

void AbstractScrollArea::scrollToBottom() noexcept {
    setScrollOffset(Position{_scrollOffset.x(), Position::maximum().y()});
}

void AbstractScrollArea::scrollToLeftEdge() noexcept {
    setScrollOffset(Position{0, _scrollOffset.y()});
}

void AbstractScrollArea::scrollToRightEdge() noexcept {
    setScrollOffset(Position{Position::maximum().x(), _scrollOffset.y()});
}

void AbstractScrollArea::scrollIntoView(const Rectangle contentRect) noexcept {
    const auto contentSize = contentSizeForViewport(_viewportRect.size());
    setScrollOffset(impl::scrollOffsetForVisibleRect(_scrollOffset, contentRect, contentSize, _viewportRect.size()));
}

auto AbstractScrollArea::scrollBarMode(const Orientation orientation) const noexcept -> ScrollBarMode {
    return orientation == Orientation::Horizontal ? _horizontalScrollBarMode : _verticalScrollBarMode;
}

void AbstractScrollArea::setScrollBarMode(const Orientation orientation, const ScrollBarMode mode) noexcept {
    auto &scrollBarMode = orientation == Orientation::Horizontal ? _horizontalScrollBarMode : _verticalScrollBarMode;
    if (scrollBarMode == mode) {
        return;
    }
    scrollBarMode = mode;
    flags().setLayoutOutdated();
}

auto AbstractScrollArea::horizontalScrollBar() const noexcept -> const HorizontalScrollBarPtr & {
    return _horizontalScrollBar;
}

auto AbstractScrollArea::verticalScrollBar() const noexcept -> const VerticalScrollBarPtr & {
    return _verticalScrollBar;
}

auto AbstractScrollArea::scrollCorner() const noexcept -> const ScrollCornerPtr & {
    return _scrollCorner;
}

auto AbstractScrollArea::contentSize() const noexcept -> Size {
    return _contentSize;
}

auto AbstractScrollArea::viewportRect() const noexcept -> Rectangle {
    return _viewportRect;
}

auto AbstractScrollArea::visibleContentRect() const noexcept -> Rectangle {
    return Rectangle{_scrollOffset, _viewportRect.size()};
}

void AbstractScrollArea::onLayout(LayoutScope &scope) noexcept {
    const auto newParentSize = scope.size();
    const auto scrollBarVisibility = resolveScrollBarVisibility(newParentSize, scope);
    _viewportRect = Rectangle{Position{}, viewportSizeFor(newParentSize, scrollBarVisibility)};
    _contentSize = measureContentSizeForViewport(_viewportRect.size(), scope);
    _scrollOffset = impl::clampedScrollOffset(_scrollOffset, _contentSize, _viewportRect.size());
    layoutScrollBarSurfaces(scrollBarVisibility, scope);
    updateScrollBars();
}

void AbstractScrollArea::onPaint(WritableBuffer &buffer, [[maybe_unused]] const PaintContext &context) noexcept {
    if (!_viewportRect.size().isZero()) {
        onPaintArea(_scrollOffset, _viewportRect, buffer, context);
    }
}

void AbstractScrollArea::initializeUi() {
    Surface::initializeUi();
    if (_horizontalScrollBar != nullptr) {
        return;
    }
    _horizontalScrollBar = HorizontalScrollBar::create();
    _verticalScrollBar = VerticalScrollBar::create();
    _scrollCorner = ScrollCorner::create();
    childStorage().add(_horizontalScrollBar);
    childStorage().add(_verticalScrollBar);
    childStorage().add(_scrollCorner);
}

auto AbstractScrollArea::isManagedChild(const SurfacePtr &surface) const noexcept -> bool {
    return surface == _horizontalScrollBar || surface == _verticalScrollBar || surface == _scrollCorner;
}

void AbstractScrollArea::onScrollOffsetChanged([[maybe_unused]] Position scrollOffset) noexcept {
    flags().setPaintOutdated(_viewportRect);
}

auto AbstractScrollArea::resolveScrollBarVisibility(const Size availableSize, LayoutScope &scope) noexcept
    -> ScrollBarVisibility {
    const auto horizontalSizePermits = availableSize.height() > 0;
    const auto verticalSizePermits = availableSize.width() > 0;
    auto visibility = ScrollBarVisibility{
        .horizontal = _horizontalScrollBarMode == ScrollBarMode::Visible && horizontalSizePermits,
        .vertical = _verticalScrollBarMode == ScrollBarMode::Visible && verticalSizePermits};
    for (auto pass = 0; pass < 3; ++pass) {
        const auto viewportSize = viewportSizeFor(availableSize, visibility);
        const auto contentSize = measureContentSizeForViewport(viewportSize, scope);
        const auto nextVisibility = ScrollBarVisibility{
            .horizontal = horizontalSizePermits &&
                (_horizontalScrollBarMode == ScrollBarMode::Visible ||
                 (_horizontalScrollBarMode == ScrollBarMode::Automatic && contentSize.width() > viewportSize.width())),
            .vertical = verticalSizePermits &&
                (_verticalScrollBarMode == ScrollBarMode::Visible ||
                 (_verticalScrollBarMode == ScrollBarMode::Automatic && contentSize.height() > viewportSize.height()))};
        if (nextVisibility.horizontal == visibility.horizontal && nextVisibility.vertical == visibility.vertical) {
            return visibility;
        }
        visibility = nextVisibility;
    }
    return visibility;
}

auto AbstractScrollArea::viewportSizeFor(const Size availableSize, const ScrollBarVisibility visibility) noexcept
    -> Size {
    return Size{
        availableSize.width() - (visibility.vertical ? Coordinate{1} : Coordinate{0}),
        availableSize.height() - (visibility.horizontal ? Coordinate{1} : Coordinate{0})};
}

void AbstractScrollArea::layoutScrollBarSurfaces(const ScrollBarVisibility visibility, LayoutScope &scope) noexcept {
    const auto viewportSize = _viewportRect.size();
    _horizontalScrollBar->flags().setVisible(visibility.horizontal);
    _verticalScrollBar->flags().setVisible(visibility.vertical);
    _scrollCorner->flags().setVisible(visibility.horizontal && visibility.vertical);
    if (visibility.horizontal) {
        const auto horizontalRect = Rectangle{0, viewportSize.height(), viewportSize.width(), 1};
        scope.place(_horizontalScrollBar, horizontalRect);
    }
    if (visibility.vertical) {
        const auto verticalRect = Rectangle{viewportSize.width(), 0, 1, viewportSize.height()};
        scope.place(_verticalScrollBar, verticalRect);
    }
    if (visibility.horizontal && visibility.vertical) {
        const auto cornerRect = Rectangle{viewportSize.width(), viewportSize.height(), 1, 1};
        scope.place(_scrollCorner, cornerRect);
    }
}

void AbstractScrollArea::updateScrollBars() noexcept {
    _horizontalScrollBar->setScrollRegion(impl::scrollRegion(_contentSize.width()));
    _horizontalScrollBar->setVisibleRegion(
        impl::visibleRegion(_scrollOffset.x(), _viewportRect.width(), _contentSize.width()));
    _verticalScrollBar->setScrollRegion(impl::scrollRegion(_contentSize.height()));
    _verticalScrollBar->setVisibleRegion(
        impl::visibleRegion(_scrollOffset.y(), _viewportRect.height(), _contentSize.height()));
}

}
