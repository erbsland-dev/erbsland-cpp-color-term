// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "UiDemoApplication.hpp"

#include <erbsland/cterm/all.hpp>
#include <erbsland/cterm/ui/all.hpp>

namespace demo::ui_sections {

using namespace erbsland::cterm;

/// Interactive demo for the `ui::Sections` layout.
class UiSectionsApp final : public demo::UiDemoApplication {
public:
    /// Create the application and store command line arguments.
    /// @param argc The number of command line arguments.
    /// @param argv The command line arguments.
    UiSectionsApp(const int argc, char *argv[]) : UiDemoApplication(argc, argv) {}
    ~UiSectionsApp() override = default;

protected: // implement ui::Application
    void setupUi() override;

private:
    /// Toggle the middle details section.
    void toggleDetails() noexcept;
    /// Toggle the trailing separator row.
    void toggleTrailingSeparator() noexcept;
    /// Cycle the right-side title text.
    void cycleRightText() noexcept;
    /// Refresh text that mirrors the current interactive state.
    void updateStateText();

private:
    ui::SectionsPtr _sections;      ///< The demonstrated sections layout.
    ui::TextBoxPtr _detailsSection; ///< Optional details body.
    ui::TextBoxPtr _stateText;      ///< Current state summary.
    ui::FooterLinePtr _footer;      ///< Footer with key summary.
    bool _detailsVisible{true};     ///< Whether the details section is visible.
    bool _trailingSeparator{true};  ///< Whether the trailing separator is visible.
    std::size_t _rightTextIndex{0}; ///< Current right text variant.
};

}
