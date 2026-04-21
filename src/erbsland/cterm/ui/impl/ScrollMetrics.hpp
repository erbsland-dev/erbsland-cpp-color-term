// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../LayoutMetrics.hpp"

#include "../../IndexRange.hpp"

namespace erbsland::cterm::ui::impl {

/// Resolve a scroll content size from one surface layout metrics and viewport size.
[[nodiscard]] auto resolveScrollContentSize(const LayoutMetrics &metrics, Size viewportSize) noexcept -> Size;
/// Calculate the largest valid scroll offset for the given content and viewport sizes.
[[nodiscard]] auto maximumScrollOffset(Size contentSize, Size viewportSize) noexcept -> Position;
/// Clamp a scroll offset to the valid range for the given content and viewport sizes.
[[nodiscard]] auto clampedScrollOffset(Position offset, Size contentSize, Size viewportSize) noexcept -> Position;
/// Calculate the content origin inside one viewport.
[[nodiscard]] auto
alignedContentOrigin(Rectangle viewportRect, Size contentSize, Position scrollOffset, Alignment alignment) noexcept
    -> Position;
/// Calculate the total scroll region for one axis.
[[nodiscard]] auto scrollRegion(Coordinate contentExtent) noexcept -> IndexRange;
/// Calculate the visible scroll region for one axis.
[[nodiscard]] auto visibleRegion(Coordinate offset, Coordinate viewportExtent, Coordinate contentExtent) noexcept
    -> IndexRange;
/// Calculate the smallest scroll offset change needed to make a content rectangle visible.
[[nodiscard]] auto
scrollOffsetForVisibleRect(Position offset, Rectangle contentRect, Size contentSize, Size viewportSize) noexcept
    -> Position;

}
