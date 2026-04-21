// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "WorldMapData.hpp"

#include <erbsland/cterm/CharCombinationStyle.hpp>
#include <erbsland/cterm/ui/Surface.hpp>

#include <memory>

namespace demo::ui_world_view {

using namespace erbsland::cterm;

class WorldMapView;
using WorldMapViewPtr = std::shared_ptr<WorldMapView>;

/// Paint surface for the zoomable world map.
class WorldMapView final : public ui::Surface {
public:
    /// Create the world-map view.
    /// @param mapData The parsed world-map data.
    explicit WorldMapView(WorldMapDataPtr mapData, ProtectedTag) noexcept;
    ~WorldMapView() override = default;

public:
    /// Create the world-map view.
    /// @param mapData The parsed world-map data.
    /// @return The new map view.
    [[nodiscard]] static auto create(WorldMapDataPtr mapData) noexcept -> WorldMapViewPtr;

public: // accessors
    /// Get the current zoom index.
    /// @return The zero-based zoom index.
    [[nodiscard]] auto zoomIndex() const noexcept -> std::size_t;
    /// Get the number of available zoom levels.
    /// @return The zoom-level count.
    [[nodiscard]] auto zoomLevelCount() const noexcept -> std::size_t;
    /// Get the current source map size.
    /// @return The size for the active zoom level.
    [[nodiscard]] auto mapSize() const noexcept -> Size;
    /// Get the current rendered content size.
    /// @return The rendered content size in terminal cells.
    [[nodiscard]] auto contentSize() const noexcept -> Size;
    /// Convert a rendered content rectangle to the covered source-pixel rectangle.
    /// @param contentRect The rendered content rectangle.
    /// @return The covered source-pixel rectangle.
    [[nodiscard]] auto visibleSourceRect(Rectangle contentRect) const noexcept -> Rectangle;
    /// Test whether labels are rendered.
    /// @return `true` if labels are shown.
    [[nodiscard]] auto labelsShown() const noexcept -> bool;
    /// Enable or disable label rendering.
    /// @param labelsShown `true` to render labels.
    void setLabelsShown(bool labelsShown) noexcept;
    /// Test whether the coordinate grid is rendered.
    /// @return `true` if the grid is shown.
    [[nodiscard]] auto gridShown() const noexcept -> bool;
    /// Enable or disable the coordinate grid.
    /// @param gridShown `true` to render the grid.
    void setGridShown(bool gridShown) noexcept;
    /// Zoom in by one level.
    /// @return `true` if the zoom level changed.
    [[nodiscard]] auto zoomIn() noexcept -> bool;
    /// Zoom out by one level.
    /// @return `true` if the zoom level changed.
    [[nodiscard]] auto zoomOut() noexcept -> bool;
    /// Get the major grid spacing for one zoom level.
    /// @param zoomIndex The zero-based zoom index.
    /// @return The grid spacing in degrees.
    [[nodiscard]] static auto gridStepForZoomIndex(std::size_t zoomIndex) noexcept -> Coordinate;
    /// Project one longitude onto a source x position.
    /// @param longitude The longitude in degrees.
    /// @param width The source map width.
    /// @return The projected source x position.
    [[nodiscard]] static auto projectLongitude(Coordinate longitude, Coordinate width) noexcept -> Coordinate;
    /// Project one latitude onto a source y position.
    /// @param latitude The latitude in degrees.
    /// @param height The source map height.
    /// @return The projected source y position.
    [[nodiscard]] static auto projectLatitude(Coordinate latitude, Coordinate height) noexcept -> Coordinate;

public: // implement Surface
    [[nodiscard]] auto isOpaque() const noexcept -> bool override;
    void onLayout(ui::LayoutScope &scope) noexcept override;
    void onPaint(WritableBuffer &buffer, const ui::PaintContext &context) noexcept override;

private:
    /// Change to one zoom level.
    /// @param zoomIndex The requested zoom index.
    /// @return `true` if the zoom level changed.
    [[nodiscard]] auto setZoomIndex(std::size_t zoomIndex) noexcept -> bool;
    /// Synchronize this surface layout size with the active zoom level.
    void updateLayoutSize() noexcept;
    /// Paint the visible terrain for the current viewport.
    /// @param buffer The target buffer.
    /// @param targetRect The rendered content rectangle.
    /// @param visibleRect The visible source rectangle.
    void drawTerrain(WritableBuffer &buffer, Rectangle targetRect, Rectangle visibleRect) const noexcept;
    /// Draw the visible latitude and longitude grid.
    /// @param buffer The target buffer.
    /// @param targetRect The rendered content rectangle.
    /// @param visibleRect The visible source rectangle.
    void drawGrid(WritableBuffer &buffer, Rectangle targetRect, Rectangle visibleRect) const noexcept;
    /// Draw the pinned coordinate labels.
    /// @param buffer The target buffer.
    /// @param targetRect The rendered content rectangle.
    /// @param visibleRect The visible source rectangle.
    void drawCoordinateLabels(WritableBuffer &buffer, Rectangle targetRect, Rectangle visibleRect) const;
    /// Draw the visible label overlays for the current viewport.
    /// @param buffer The target buffer.
    /// @param targetRect The rendered content rectangle.
    /// @param visibleRect The visible source rectangle.
    void drawLabels(WritableBuffer &buffer, Rectangle targetRect, Rectangle visibleRect) const;
    /// Format one longitude for the pinned coordinate overlay.
    /// @param longitude The longitude in degrees.
    /// @return The formatted longitude text.
    [[nodiscard]] static auto formatLongitude(Coordinate longitude) -> String;
    /// Format one latitude for the pinned coordinate overlay.
    /// @param latitude The latitude in degrees.
    /// @return The formatted latitude text.
    [[nodiscard]] static auto formatLatitude(Coordinate latitude) -> String;
    /// Test whether a country label overlaps the reserved coordinate strips.
    /// @param targetRect The rendered target rectangle.
    /// @param startX The rendered label start position.
    /// @param endX The rendered label end position.
    /// @param y The rendered label row.
    /// @return `true` if the label must be skipped.
    [[nodiscard]] static auto
    intersectsPinnedCoordinateArea(Rectangle targetRect, Coordinate startX, Coordinate endX, Coordinate y) noexcept
        -> bool;

private:
    static constexpr auto cPinnedTopStripHeight = Coordinate{1};
    static constexpr auto cPinnedLeftStripWidth = Coordinate{4};

    WorldMapDataPtr _mapData;  ///< The parsed map data.
    std::size_t _zoomIndex{0}; ///< The active zoom level.
    bool _labelsShown{true};   ///< Toggle for label rendering.
    bool _gridShown{true};     ///< Toggle for grid rendering.
};

}
