// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Surface.hpp"

#include "../../BufferView.hpp"

#include <memory>

namespace erbsland::cterm::ui::surface {

class ScrollingBufferView;
using ScrollingBufferViewPtr = std::shared_ptr<ScrollingBufferView>;

/// A surface that renders a scrollable viewport into one readable buffer.
class ScrollingBufferView final : public Surface {
public:
    /// Create a scrolling buffer view.
    /// @param source The optional source buffer.
    explicit ScrollingBufferView(ReadableBufferPtr source, ProtectedTag) noexcept;
    ~ScrollingBufferView() override = default;

public:
    /// Create a scrolling buffer view for one source buffer.
    /// @param source The optional source buffer.
    /// @return The new scrolling buffer view.
    [[nodiscard]] static auto create(ReadableBufferPtr source = {}) noexcept -> ScrollingBufferViewPtr;

public:
    /// Access the source buffer.
    /// @return The current source buffer.
    [[nodiscard]] auto source() const noexcept -> const ReadableBufferPtr &;
    /// Replace the source buffer and clamp the stored view offset to the new content.
    /// @param source The new source buffer.
    void setSource(ReadableBufferPtr source) noexcept;
    /// Access the current top-left content offset of the visible viewport.
    /// @return The current viewport offset.
    [[nodiscard]] auto viewOffset() const noexcept -> Position;
    /// Replace the current viewport offset.
    /// The offset is clamped to the visible content range for the current surface size.
    /// @param offset The new viewport offset.
    void setViewOffset(Position offset) noexcept;
    /// Access the current viewport rectangle in source-buffer coordinates.
    /// @return The current viewport rectangle.
    [[nodiscard]] auto viewRect() const noexcept -> Rectangle;
    /// Test whether crop indicator characters are shown.
    /// @return `true` if crop indicator characters are shown.
    [[nodiscard]] auto showCropCharacters() const noexcept -> bool;
    /// Enable or disable crop indicator characters.
    /// @param show `true` to show crop indicator characters.
    void setShowCropCharacters(bool show) noexcept;
    /// Get one crop indicator character.
    /// @param direction The crop direction to query.
    /// @return The configured crop indicator character.
    [[nodiscard]] auto cropCharacter(Direction direction) const noexcept -> Char;
    /// Replace one crop indicator character.
    /// @param direction The crop direction to update.
    /// @param character The new crop indicator character.
    void setCropCharacter(Direction direction, Char character) noexcept;

public:
    /// Scroll the viewport one or more rows up.
    /// @param count The number of rows to scroll.
    void scrollUp(Coordinate count = 1) noexcept;
    /// Scroll the viewport one or more rows down.
    /// @param count The number of rows to scroll.
    void scrollDown(Coordinate count = 1) noexcept;
    /// Scroll the viewport one or more columns left.
    /// @param count The number of columns to scroll.
    void scrollLeft(Coordinate count = 1) noexcept;
    /// Scroll the viewport one or more columns right.
    /// @param count The number of columns to scroll.
    void scrollRight(Coordinate count = 1) noexcept;
    /// Scroll the viewport by one full page up.
    void pageUp() noexcept;
    /// Scroll the viewport by one full page down.
    void pageDown() noexcept;
    /// Scroll the viewport by one full page left.
    void pageLeft() noexcept;
    /// Scroll the viewport by one full page right.
    void pageRight() noexcept;
    /// Scroll to the top-most row.
    void scrollToTop() noexcept;
    /// Scroll to the bottom-most row that still keeps one full viewport visible.
    void scrollToBottom() noexcept;
    /// Scroll to the left-most column.
    void scrollToLeftEdge() noexcept;
    /// Scroll to the right-most column that still keeps one full viewport visible.
    void scrollToRightEdge() noexcept;

public: // implement Surface
    void onLayout(Size newParentSize) noexcept override;
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

private:
    /// Clamp one viewport offset to the visible bounds for the given viewport size.
    /// @param offset The candidate offset.
    /// @param viewSize The viewport size.
    /// @return The clamped offset.
    [[nodiscard]] auto clampedViewOffset(Position offset, Size viewSize) const noexcept -> Position;
    /// Update the cached buffer view rectangle for one target size.
    /// @param viewSize The visible viewport size.
    void updateView(Size viewSize) noexcept;

private:
    ReadableBufferPtr _source;  ///< The source buffer.
    BufferView _view;           ///< The cached source-buffer view.
    Position _viewOffset{0, 0}; ///< The current viewport offset inside the source buffer.
};

}
