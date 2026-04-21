// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "UiDemoApplication.hpp"

#include <erbsland/cterm/all.hpp>
#include <erbsland/cterm/ui/all.hpp>

namespace demo::ui_choice {

using namespace erbsland::cterm;

/// Interactive demo for the composed `ui::Choice` page.
class UiChoiceApp final : public demo::UiDemoApplication {
public:
    /// Create the application and store command line arguments.
    /// @param argc The number of command line arguments.
    /// @param argv The command line arguments.
    UiChoiceApp(const int argc, char *argv[]) : UiDemoApplication(argc, argv) {}
    ~UiChoiceApp() override = default;

protected: // implement ui::Application
    void setupUi() override;

private:
    /// Open the choice dialog.
    void openChoice();

private:
    ui::TextBoxPtr _status; ///< Displays the last selected choice.
};

}
