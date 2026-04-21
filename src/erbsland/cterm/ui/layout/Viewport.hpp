// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "SingleContentLayout.hpp"

namespace erbsland::cterm::ui::layout {

class Viewport;
using ViewportPtr = std::shared_ptr<Viewport>;

/// A layout surface that clips, positions, and scrolls one content surface.
class Viewport final : public SingleContentLayout {
public:
    /// Create a viewport.
    explicit Viewport(ProtectedTag) noexcept;
    ~Viewport() override = default;

public:
    /// Create a viewport.
    /// @return The new viewport.
    [[nodiscard]] static auto create() noexcept -> ViewportPtr;

public:
    /// Get the content alignment for undersized content.
    /// @return The content alignment.
    [[nodiscard]] auto alignment() const noexcept -> Alignment;
    /// Replace the content alignment for undersized content.
    /// @param alignment The new alignment.
    void setAlignment(Alignment alignment) noexcept;
    /// Get the current scroll offset.
    /// @return The clamped scroll offset.
    [[nodiscard]] auto scrollOffset() const noexcept -> Position;
    /// Replace the scroll offset.
    /// @param scrollOffset The new scroll offset.
    void setScrollOffset(Position scrollOffset) noexcept;
    /// Scroll by the given delta.
    /// @param delta The scroll delta.
    void scrollBy(Position delta) noexcept;
    /// Scroll one or more rows up.
    /// @param count The number of rows.
    void scrollUp(Coordinate count = 1) noexcept;
    /// Scroll one or more rows down.
    /// @param count The number of rows.
    void scrollDown(Coordinate count = 1) noexcept;
    /// Scroll one or more columns left.
    /// @param count The number of columns.
    void scrollLeft(Coordinate count = 1) noexcept;
    /// Scroll one or more columns right.
    /// @param count The number of columns.
    void scrollRight(Coordinate count = 1) noexcept;
    /// Scroll one page up.
    void pageUp() noexcept;
    /// Scroll one page down.
    void pageDown() noexcept;
    /// Scroll one page left.
    void pageLeft() noexcept;
    /// Scroll one page right.
    void pageRight() noexcept;
    /// Scroll to the top edge.
    void scrollToTop() noexcept;
    /// Scroll to the bottom edge.
    void scrollToBottom() noexcept;
    /// Scroll to the left edge.
    void scrollToLeftEdge() noexcept;
    /// Scroll to the right edge.
    void scrollToRightEdge() noexcept;
    /// Scroll by the smallest amount needed to make a content rectangle visible.
    /// @param contentRect The rectangle in content coordinates.
    void scrollIntoView(Rectangle contentRect) noexcept;
    /// Resolve the content size for a candidate viewport.
    /// @param viewportSize The candidate viewport size.
    /// @return The content size.
    [[nodiscard]] auto contentSizeForViewport(Size viewportSize) const noexcept -> Size;
    /// Resolve the content size for a candidate viewport using measurement.
    /// @param viewportSize The candidate viewport size.
    /// @param scope The measurement scope.
    /// @return The content size.
    [[nodiscard]] auto contentSizeForViewport(Size viewportSize, LayoutScope &scope) noexcept -> Size;
    /// Get the current content size.
    /// @return The current content size.
    [[nodiscard]] auto contentSize() const noexcept -> Size;

public: // implement Surface
    void onLayout(LayoutScope &scope) noexcept override;

private:
    /// Clamp the current scroll offset to the current content and viewport size.
    void clampScrollOffset() noexcept;

private:
    Alignment _alignment{Alignment::TopLeft}; ///< Alignment for undersized content.
    Position _scrollOffset;                   ///< Current scroll offset.
    Size _contentSize;                        ///< Current content size.
};

}
