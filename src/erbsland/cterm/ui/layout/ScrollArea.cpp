// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ScrollArea.hpp"

namespace erbsland::cterm::ui::layout {

ScrollArea::ScrollArea(ProtectedTag protectedTag) noexcept : surface::AbstractScrollArea{protectedTag} {
    setScrollBarMode(Orientation::Horizontal, ScrollBarMode::Automatic);
    setScrollBarMode(Orientation::Vertical, ScrollBarMode::Automatic);
}

auto ScrollArea::create() -> ScrollAreaPtr {
    auto scrollArea = std::make_shared<ScrollArea>(ProtectedTag{});
    scrollArea->initializeUi();
    return scrollArea;
}

void ScrollArea::setContentSurface(SurfacePtr contentSurface) {
    _viewport->setContentSurface(std::move(contentSurface));
    flags().setLayoutOutdated();
}

auto ScrollArea::contentSurface() const noexcept -> const SurfacePtr & {
    return _viewport->contentSurface();
}

auto ScrollArea::viewport() const noexcept -> const ViewportPtr & {
    return _viewport;
}

auto ScrollArea::alignment() const noexcept -> Alignment {
    return _viewport->alignment();
}

void ScrollArea::setAlignment(const Alignment alignment) noexcept {
    _viewport->setAlignment(alignment);
    flags().setLayoutOutdated();
}

void ScrollArea::onLayout(LayoutScope &scope) noexcept {
    surface::AbstractScrollArea::onLayout(scope);
    scope.place(_viewport, viewportRect());
    _viewport->setScrollOffset(scrollOffset());
}

auto ScrollArea::isManagedChild(const SurfacePtr &surface) const noexcept -> bool {
    return surface == _viewport || surface::AbstractScrollArea::isManagedChild(surface);
}

auto ScrollArea::contentSizeForViewport(const Size viewportSize) const noexcept -> Size {
    return _viewport->contentSizeForViewport(viewportSize);
}

auto ScrollArea::measureContentSizeForViewport(const Size viewportSize, LayoutScope &scope) noexcept -> Size {
    return _viewport->contentSizeForViewport(viewportSize, scope);
}

void ScrollArea::onPaintArea(
    [[maybe_unused]] const Position scrollOffset,
    [[maybe_unused]] const Rectangle targetRect,
    [[maybe_unused]] WritableBuffer &buffer,
    [[maybe_unused]] const PaintContext &context) noexcept {
}

void ScrollArea::onScrollOffsetChanged(const Position scrollOffset) noexcept {
    _viewport->setScrollOffset(scrollOffset);
    surface::AbstractScrollArea::onScrollOffsetChanged(scrollOffset);
}

void ScrollArea::initializeUi() {
    _viewport = Viewport::create();
    childStorage().add(_viewport);
    surface::AbstractScrollArea::initializeUi();
}

}
