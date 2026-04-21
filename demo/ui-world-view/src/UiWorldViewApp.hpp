// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "UiDemoApplication.hpp"
#include "WorldMapData.hpp"
#include "WorldMapView.hpp"

#include <erbsland/cterm/ui/all.hpp>

#include <filesystem>
#include <memory>

namespace demo::ui_world_view {

using namespace erbsland::cterm;

/// Interactive UI demo that displays a zoomable world map with scroll bars.
class UiWorldViewApp final : public demo::UiDemoApplication {
public:
    /// Create the application and store command line arguments.
    /// @param argc The number of command line arguments.
    /// @param argv The command line arguments.
    UiWorldViewApp(const int argc, char *argv[]) : UiDemoApplication(argc, argv) {}

    // defaults
    ~UiWorldViewApp() override = default;
    UiWorldViewApp(const UiWorldViewApp &) = delete;
    UiWorldViewApp(UiWorldViewApp &&) = delete;
    auto operator=(const UiWorldViewApp &) -> UiWorldViewApp & = delete;
    auto operator=(UiWorldViewApp &&) -> UiWorldViewApp & = delete;

protected: // implement ui::Application
    /// Build the UI surface tree.
    void setupUi() override;
    /// Parse command line arguments and attach the map data model to the UI.
    /// @param args The command line arguments.
    /// @return The application exit code, or `cExitCodeContinue` to continue.
    auto processCommandLineArguments(const CommandLineArgs &args) -> ExitCode override;

private:
    /// Synchronize the visible UI state after changing the viewport or labels.
    void updateDynamicUi() noexcept;
    /// Pan the map in rendered content coordinates.
    /// @param delta The rendered content delta.
    void panBy(Position delta) noexcept;
    /// Zoom in if possible.
    void zoomIn() noexcept;
    /// Zoom out if possible.
    void zoomOut() noexcept;
    /// Change the zoom level while preserving the visible center.
    /// @param zoomIn `true` to zoom in, `false` to zoom out.
    void changeZoom(bool zoomIn) noexcept;
    /// Toggle label rendering.
    void toggleLabels() noexcept;
    /// Toggle grid rendering.
    void toggleGrid() noexcept;
    /// Print command line usage.
    void printUsage() const;
    /// Get the default generated data-file path.
    /// @return The compiled-in data-file path.
    [[nodiscard]] static auto defaultDataFilePath() -> std::filesystem::path;

private:
    std::filesystem::path _dataFilePath; ///< The map data file to load.
    WorldMapDataPtr _mapData;            ///< The loaded map data.
    ui::StackPtr _mapHost;               ///< Layout host that lets the map area fill the page body.
    ui::ScrollAreaPtr _mapArea;          ///< The scroll area for the map.
    WorldMapViewPtr _mapView;            ///< The scrollable world-map content.
    ui::HeaderLinePtr _headerStatus;     ///< The top header line.
    ui::FooterLinePtr _footerStatus;     ///< The bottom footer line.
};

}
