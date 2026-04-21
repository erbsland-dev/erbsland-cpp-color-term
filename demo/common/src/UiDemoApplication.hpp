// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <erbsland/cterm/ui/all.hpp>

namespace demo {

using namespace erbsland::cterm;

/// Shared base for UI demos with a prepared contextual help viewer.
class UiDemoApplication : public ui::Application {
    /// The default theme enumeration.
    enum class DefaultTheme : uint8_t { Dark = 0, Light, Monochrome, _count };

public:
    /// Create a UI demo application.
    UiDemoApplication();
    /// Create a UI demo application with command line arguments.
    UiDemoApplication(int argc, char *argv[]);
    /// Create a UI demo application using a custom terminal backend.
    explicit UiDemoApplication(TerminalPtr terminal);
    /// Create a UI demo application with command line arguments and a custom terminal backend.
    UiDemoApplication(int argc, char *argv[], TerminalPtr terminal);
    ~UiDemoApplication() override = default;

    // defaults
    UiDemoApplication(const UiDemoApplication &) = delete;
    UiDemoApplication(UiDemoApplication &&) = delete;
    auto operator=(const UiDemoApplication &) -> UiDemoApplication & = delete;
    auto operator=(UiDemoApplication &&) -> UiDemoApplication & = delete;

protected:
    /// Add all default actions to the given page.
    void addDefaultActions(const ui::PagePtr &page) noexcept;
    /// Access the prepared help viewer.
    [[nodiscard]] auto helpViewer() const noexcept -> const ui::HelpViewerPtr &;
    /// Access the generated action-help section.
    [[nodiscard]] auto actionHelpSection() const noexcept -> const ui::ActionHelpSectionPtr &;
    /// Access the prepared help action.
    [[nodiscard]] auto helpAction() const noexcept -> const ui::ActionPtr &;
    /// Access the prepared quit action.
    [[nodiscard]] auto quitAction() const noexcept -> const ui::ActionPtr &;
    /// Access the theme cycling action.
    [[nodiscard]] auto cycleThemeAction() const noexcept -> const ui::ActionPtr &;

private:
    /// Initialize the help viewer and help action.
    void initializeHelp();
    /// Open the help viewer for the given action context.
    void openHelp(const ui::ActionTriggerContext &context);
    /// Choose the best contextual action source for the given trigger source.
    [[nodiscard]] static auto actionSourceFor(const ui::SurfacePtr &triggerSource) noexcept -> ui::SurfacePtr;
    /// Cycle through available UI themes.
    void cycleTheme();

private:
    DefaultTheme _currentTheme{DefaultTheme::Dark}; ///< The currently displayed UI theme.
    ui::HelpViewerPtr _helpViewer;                  ///< Shared help viewer page.
    ui::ActionHelpSectionPtr _actionHelpSection;    ///< Generated shortcut section.
    ui::ActionPtr _helpAction;                      ///< Shared help action.
    ui::ActionPtr _quitAction;                      ///< The quit action.
    ui::ActionPtr _cycleThemeAction;                ///< The action to cycle through UI themes.
};

}
