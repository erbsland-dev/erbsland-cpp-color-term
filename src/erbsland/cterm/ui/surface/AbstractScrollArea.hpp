// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "HorizontalScrollBar.hpp"
#include "ScrollCorner.hpp"
#include "VerticalScrollBar.hpp"

#include "../ExclusiveSurfaceManager.hpp"
#include "../ScrollBarMode.hpp"
#include "../Surface.hpp"

namespace erbsland::cterm::ui::surface {

class AbstractScrollArea;
using AbstractScrollAreaPtr = std::shared_ptr<AbstractScrollArea>;

/// Shared base for scrollable surfaces that paint custom content into a viewport.
class AbstractScrollArea : public Surface, protected ExclusiveSurfaceManager {
public:
    /// Create a scroll area base with grow/fill geometry.
    explicit AbstractScrollArea(ProtectedTag) noexcept;
    ~AbstractScrollArea() override = default;

public:
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

public:
    /// Get one scroll bar mode.
    /// @param orientation The scroll bar orientation.
    /// @return The configured scroll bar mode.
    [[nodiscard]] auto scrollBarMode(Orientation orientation) const noexcept -> ScrollBarMode;
    /// Replace one scroll bar mode.
    /// @param orientation The scroll bar orientation.
    /// @param mode The new scroll bar mode.
    void setScrollBarMode(Orientation orientation, ScrollBarMode mode) noexcept;
    /// Access the horizontal scroll bar.
    /// @return The horizontal scroll bar.
    [[nodiscard]] auto horizontalScrollBar() const noexcept -> const HorizontalScrollBarPtr &;
    /// Access the vertical scroll bar.
    /// @return The vertical scroll bar.
    [[nodiscard]] auto verticalScrollBar() const noexcept -> const VerticalScrollBarPtr &;
    /// Access the scroll corner.
    /// @return The scroll corner.
    [[nodiscard]] auto scrollCorner() const noexcept -> const ScrollCornerPtr &;
    /// Get the current content size.
    /// @return The content size used for scrolling.
    [[nodiscard]] auto contentSize() const noexcept -> Size;
    /// Get the current viewport rectangle.
    /// @return The viewport rectangle in this surface's local coordinates.
    [[nodiscard]] auto viewportRect() const noexcept -> Rectangle;
    /// Get the visible content rectangle.
    /// @return The visible rectangle in content coordinates.
    [[nodiscard]] auto visibleContentRect() const noexcept -> Rectangle;

public: // implement Surface
    void onLayout(LayoutScope &scope) noexcept override;
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

protected:
    /// Create and add the scroll bar surfaces to the child storage.
    void initializeUi() override;

protected: // implement ExclusiveSurfaceManager
    [[nodiscard]] auto isManagedChild(const SurfacePtr &surface) const noexcept -> bool override;

protected:
    /// Calculate the content size for one viewport size.
    /// @param viewportSize The candidate viewport size.
    /// @return The resulting content size.
    [[nodiscard]] virtual auto contentSizeForViewport(Size viewportSize) const noexcept -> Size = 0;
    /// Measure the content size for one viewport size.
    /// @param viewportSize The candidate viewport size.
    /// @param scope The layout scope.
    /// @return The resulting content size.
    [[nodiscard]] virtual auto measureContentSizeForViewport(Size viewportSize, LayoutScope &scope) noexcept
        -> Size = 0;
    /// Paint the custom scrollable area.
    /// @param scrollOffset The current scroll offset.
    /// @param targetRect The target viewport rectangle.
    /// @param buffer The target buffer.
    /// @param context The paint context.
    virtual void onPaintArea(
        Position scrollOffset, Rectangle targetRect, WritableBuffer &buffer, const PaintContext &context) noexcept = 0;
    /// React to a scroll offset change.
    /// @param scrollOffset The new scroll offset.
    virtual void onScrollOffsetChanged(Position scrollOffset) noexcept;

private:
    struct ScrollBarVisibility final {
        bool horizontal{false}; ///< Whether the horizontal scroll bar is visible.
        bool vertical{false};   ///< Whether the vertical scroll bar is visible.
    };

private:
    /// Resolve scrollbar visibility for one available size.
    [[nodiscard]] auto resolveScrollBarVisibility(Size availableSize, LayoutScope &scope) noexcept
        -> ScrollBarVisibility;
    /// Calculate the viewport size after reserving visible scroll bars.
    [[nodiscard]] static auto viewportSizeFor(Size availableSize, ScrollBarVisibility visibility) noexcept -> Size;
    /// Apply the current scrollbar state to all scrollbar child surfaces.
    void layoutScrollBarSurfaces(ScrollBarVisibility visibility, LayoutScope &scope) noexcept;
    /// Synchronize scroll bar ranges with the current metrics.
    void updateScrollBars() noexcept;

private:
    HorizontalScrollBarPtr _horizontalScrollBar;                   ///< The horizontal scroll bar.
    VerticalScrollBarPtr _verticalScrollBar;                       ///< The vertical scroll bar.
    ScrollCornerPtr _scrollCorner;                                 ///< The scroll corner.
    ScrollBarMode _horizontalScrollBarMode{ScrollBarMode::Hidden}; ///< The horizontal scroll bar mode.
    ScrollBarMode _verticalScrollBarMode{ScrollBarMode::Hidden};   ///< The vertical scroll bar mode.
    Position _scrollOffset;                                        ///< The current scroll offset.
    Rectangle _viewportRect;                                       ///< The current viewport rectangle.
    Size _contentSize;                                             ///< The current content size.
};

}
