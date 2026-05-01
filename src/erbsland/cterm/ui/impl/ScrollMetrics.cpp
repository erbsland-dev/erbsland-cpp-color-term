// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ScrollMetrics.hpp"

#include <algorithm>

namespace erbsland::cterm::ui::impl {

auto resolvedScrollDimension(
    const LayoutMetrics &metrics, const Orientation orientation, const Coordinate viewportExtent) noexcept
    -> Coordinate {
    const auto minimum = std::max(Coordinate{0}, metrics.minimum().coordinate(orientation));
    const auto maximum = std::max(minimum, metrics.maximum().coordinate(orientation));
    const auto preferred = std::clamp(metrics.preferred().coordinate(orientation), minimum, maximum);
    const auto policy =
        orientation == Orientation::Horizontal ? metrics.sizePolicy().width() : metrics.sizePolicy().height();
    if (policy.type() == DimensionPolicy::Grow) {
        return std::clamp(viewportExtent, minimum, maximum);
    }
    return preferred;
}

auto resolveScrollContentSize(const LayoutMetrics &metrics, const Size viewportSize) noexcept -> Size {
    const auto margins = metrics.margins();
    const auto contentViewportSize = (viewportSize - margins.extent()).expandedWith(Size{});
    return Size{
        resolvedScrollDimension(metrics, Orientation::Horizontal, contentViewportSize.width()) +
            margins.horizontalExtent(),
        resolvedScrollDimension(metrics, Orientation::Vertical, contentViewportSize.height()) +
            margins.verticalExtent()};
}

auto maximumScrollOffset(const Size contentSize, const Size viewportSize) noexcept -> Position {
    return Position{
        std::max(contentSize.width() - std::max(viewportSize.width(), Coordinate{1}), Coordinate{0}),
        std::max(contentSize.height() - std::max(viewportSize.height(), Coordinate{1}), Coordinate{0})};
}

auto clampedScrollOffset(const Position offset, const Size contentSize, const Size viewportSize) noexcept -> Position {
    const auto maximumOffset = maximumScrollOffset(contentSize, viewportSize);
    return Position{
        std::clamp(offset.x(), Coordinate{0}, maximumOffset.x()),
        std::clamp(offset.y(), Coordinate{0}, maximumOffset.y())};
}

auto alignedContentOrigin(
    const Rectangle viewportRect,
    const Size contentSize,
    const Position scrollOffset,
    const Alignment alignment) noexcept -> Position {
    auto origin = viewportRect.topLeft();
    if (contentSize.width() <= viewportRect.width()) {
        origin.setX(viewportRect.alignmentOffset(contentSize, alignment).x());
    } else {
        origin.setX(viewportRect.x1() - scrollOffset.x());
    }
    if (contentSize.height() <= viewportRect.height()) {
        origin.setY(viewportRect.alignmentOffset(contentSize, alignment).y());
    } else {
        origin.setY(viewportRect.y1() - scrollOffset.y());
    }
    return origin;
}

auto scrollRegion(const Coordinate contentExtent) noexcept -> IndexRange {
    return IndexRange{0, static_cast<std::size_t>(std::max(contentExtent, Coordinate{0}))};
}

auto visibleRegion(const Coordinate offset, const Coordinate viewportExtent, const Coordinate contentExtent) noexcept
    -> IndexRange {
    const auto boundedContentExtent = std::max(contentExtent, Coordinate{0});
    const auto boundedOffset = std::clamp(offset, Coordinate{0}, boundedContentExtent);
    const auto boundedViewportExtent = std::max(viewportExtent, Coordinate{0});
    const auto length = std::min(boundedViewportExtent, boundedContentExtent - boundedOffset);
    return IndexRange{static_cast<std::size_t>(boundedOffset), static_cast<std::size_t>(std::max(length, 0))};
}

auto scrollAxisOffset(
    Coordinate offset,
    const Coordinate contentStart,
    const Coordinate contentEnd,
    const Coordinate viewportExtent) noexcept -> Coordinate {
    if (contentStart < offset || contentEnd - contentStart > viewportExtent) {
        offset = contentStart;
    } else if (contentEnd > offset + viewportExtent) {
        offset = contentEnd - viewportExtent;
    }
    return offset;
}

auto scrollOffsetForVisibleRect(
    const Position offset, const Rectangle contentRect, const Size contentSize, const Size viewportSize) noexcept
    -> Position {
    const auto newOffset = Position{
        scrollAxisOffset(offset.x(), contentRect.x1(), contentRect.x2(), viewportSize.width()),
        scrollAxisOffset(offset.y(), contentRect.y1(), contentRect.y2(), viewportSize.height())};
    return clampedScrollOffset(newOffset, contentSize, viewportSize);
}

}
