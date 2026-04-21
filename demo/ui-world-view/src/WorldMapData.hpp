// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <erbsland/cterm/Bitmap.hpp>
#include <erbsland/cterm/geometry/Position.hpp>
#include <erbsland/cterm/geometry/Size.hpp>
#include <erbsland/cterm/String.hpp>

#include <filesystem>
#include <memory>
#include <string_view>
#include <vector>

namespace demo::ui_world_view {

using namespace erbsland::cterm;

class WorldMapData;
using WorldMapDataPtr = std::shared_ptr<WorldMapData>;

/// Loaded world-map raster and label data for all demo zoom levels.
class WorldMapData final {
public:
    /// One label entry for one zoom level.
    struct Label final {
        Position anchor{};         ///< The label anchor in source pixels/rows.
        String text;               ///< The styled label text.
        Coordinate displayWidth{}; ///< The rendered display width of the label.
    };

    /// One zoom-level payload.
    struct ZoomLevel final {
        Size size{};               ///< The source raster size.
        Bitmap landMask;           ///< The rasterized land mask.
        std::vector<Label> labels; ///< The accepted labels for this zoom.
    };

public:
    WorldMapData() = default;

    // defaults
    ~WorldMapData() = default;
    WorldMapData(const WorldMapData &) = default;
    WorldMapData(WorldMapData &&) noexcept = default;
    auto operator=(const WorldMapData &) -> WorldMapData & = default;
    auto operator=(WorldMapData &&) noexcept -> WorldMapData & = default;

public: // conversion
    /// Load the generated world-map asset from a file.
    /// @param path The file path to read.
    /// @return The parsed world-map data.
    /// @throws std::runtime_error If the file cannot be read or parsed.
    [[nodiscard]] static auto loadFromFile(const std::filesystem::path &path) -> WorldMapData;
    /// Parse world-map data from text.
    /// @param text The asset text.
    /// @return The parsed world-map data.
    /// @throws std::runtime_error If the text does not follow the expected format.
    [[nodiscard]] static auto parse(std::string_view text) -> WorldMapData;

public: // accessors
    /// Get all loaded zoom levels.
    /// @return The zoom levels from lowest to highest zoom.
    [[nodiscard]] auto zoomLevels() const noexcept -> const std::vector<ZoomLevel> &;
    /// Get one zoom level by index.
    /// @param zoomIndex The zero-based zoom index.
    /// @return The requested zoom level.
    /// @throws std::out_of_range If the index is invalid.
    [[nodiscard]] auto zoomLevel(std::size_t zoomIndex) const -> const ZoomLevel &;
    /// Get the number of loaded zoom levels.
    /// @return The zoom count.
    [[nodiscard]] auto zoomLevelCount() const noexcept -> std::size_t;

private:
    /// Parse one positive integer token.
    /// @param text The token text.
    /// @param lineNumber The source line number for diagnostics.
    /// @param name The token name for diagnostics.
    /// @return The parsed integer value.
    [[nodiscard]] static auto parseNumber(std::string_view text, std::size_t lineNumber, std::string_view name)
        -> std::size_t;
    /// Parse one row of alternating sea/land spans.
    /// @param rowText The encoded row text after the `row ` prefix.
    /// @param width The expected target width.
    /// @param lineNumber The source line number for diagnostics.
    /// @return The decoded land mask row.
    [[nodiscard]] static auto parseRow(std::string_view rowText, Coordinate width, std::size_t lineNumber)
        -> std::vector<bool>;
    /// Trim ASCII whitespace from both sides of a view.
    /// @param text The text to trim.
    /// @return The trimmed view.
    [[nodiscard]] static auto trimmed(std::string_view text) noexcept -> std::string_view;
    /// Build the styled runtime label text.
    /// @param text The label text in UTF-8.
    /// @return The styled label string.
    [[nodiscard]] static auto styledLabelText(std::string_view text) -> String;

private:
    std::vector<ZoomLevel> _zoomLevels; ///< The loaded zoom levels.
};

}
