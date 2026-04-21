// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Surface.hpp"

#include "../../IndexRange.hpp"

namespace erbsland::cterm::ui::surface {

class AbstractScrollBar;
using AbstractScrollBarPtr = std::shared_ptr<AbstractScrollBar>;

/// Shared base class for one-dimensional scroll bar surfaces.
/// The scroll bar is a purely visual indicator for a visible region inside a larger scroll region.
class AbstractScrollBar : public Surface {
protected:
    /// The supported scroll bar orientations.
    enum class Orientation : uint8_t {
        Horizontal, ///< A left-to-right scroll bar.
        Vertical,   ///< A top-to-bottom scroll bar.
    };

public:
    /// Create a scroll bar with explicit layout metrics and orientation.
    /// @param orientation The scroll bar orientation.
    /// @param layoutSize The initial layout metrics.
    explicit AbstractScrollBar(Orientation orientation, const LayoutMetrics &layoutSize, ProtectedTag) noexcept;
    ~AbstractScrollBar() override = default;

public:
    /// Get the total scrollable region.
    /// @return The configured scroll region.
    [[nodiscard]] auto scrollRegion() const noexcept -> IndexRange;
    /// Replace the total scrollable region.
    /// @param scrollRegion The new scroll region.
    void setScrollRegion(IndexRange scrollRegion) noexcept;
    /// Get the currently visible region.
    /// @return The configured visible region.
    [[nodiscard]] auto visibleRegion() const noexcept -> IndexRange;
    /// Replace the currently visible region.
    /// @param visibleRegion The new visible region.
    void setVisibleRegion(IndexRange visibleRegion) noexcept;

public: // implement Surface
    [[nodiscard]] auto isOpaque() const noexcept -> bool override;
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

protected:
    /// Get the configured orientation.
    /// @return The orientation.
    [[nodiscard]] auto orientation() const noexcept -> Orientation;

private:
    struct HandleSpan final {
        Coordinate start{0};  ///< The first track cell occupied by the handle.
        Coordinate length{0}; ///< The number of occupied track cells.

        [[nodiscard]] auto empty() const noexcept -> bool { return length <= 0; }
        [[nodiscard]] auto end() const noexcept -> Coordinate { return start + length; }
    };

private:
    /// Convert one `std::size_t` length into a coordinate with saturation.
    /// @param value The source value.
    /// @return The saturated coordinate value.
    [[nodiscard]] static auto toCoordinate(std::size_t value) noexcept -> Coordinate;
    /// Normalize the configured scroll region into a finite range.
    /// @return The normalized scroll region, or an empty range if projection is not possible.
    [[nodiscard]] auto normalizedScrollRegion() const noexcept -> IndexRange;
    /// Clamp the visible region to the current scroll region.
    /// @return The clamped visible region.
    [[nodiscard]] auto normalizedVisibleRegion() const noexcept -> IndexRange;
    /// Get the painted length along the main axis.
    /// @param rect The target rectangle.
    /// @return The width for horizontal bars or height for vertical bars.
    [[nodiscard]] auto mainAxisLength(const Rectangle &rect) const noexcept -> Coordinate;
    /// Project the visible region into the track.
    /// @param trackLength The available track length.
    /// @return The projected handle span.
    [[nodiscard]] auto projectHandle(Coordinate trackLength) const noexcept -> HandleSpan;
    /// Calculate the minimum rendered handle length for the given track length.
    /// @param trackLength The available track length.
    /// @return The minimum handle length.
    [[nodiscard]] static auto minimumHandleLength(Coordinate trackLength) noexcept -> Coordinate;
    /// Expand one projected handle span to the required minimum length.
    /// @param span The projected span.
    /// @param trackLength The available track length.
    /// @param desiredLength The desired handle length.
    /// @return The expanded handle span.
    [[nodiscard]] static auto
    expandedHandleSpan(HandleSpan span, Coordinate trackLength, Coordinate desiredLength) noexcept -> HandleSpan;
    /// Paint the scroll bar with the active theme.
    /// @param buffer The target buffer.
    /// @param context The paint context.
    void paintThemed(WritableBuffer &buffer, const PaintContext &context) noexcept;
    /// Convert a projected handle span into a rectangle inside the themed track content area.
    /// @param contentRect The usable track content area.
    /// @param handleSpan The projected handle span.
    /// @return The handle rectangle.
    [[nodiscard]] auto handleRectForSpan(Rectangle contentRect, const HandleSpan &handleSpan) const noexcept
        -> Rectangle;

private:
    Orientation _orientation;  ///< The scroll bar orientation.
    IndexRange _scrollRegion;  ///< The total scrollable region.
    IndexRange _visibleRegion; ///< The currently visible region.
};

}
