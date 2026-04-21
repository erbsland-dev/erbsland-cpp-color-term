// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <erbsland/cterm/all.hpp>
#include <erbsland/cterm/ui/all.hpp>

namespace demo::ui_hello_world {

using namespace erbsland::cterm;

/// Minimal UI application that shows the basic framework building blocks.
class UiHelloWorldApp final : public ui::Application {
public:
    /// Create the application and store command line arguments.
    /// @param argc The number of command line arguments.
    /// @param argv The command line arguments.
    UiHelloWorldApp(const int argc, char *argv[]) : Application(argc, argv) {}
    ~UiHelloWorldApp() override = default;

protected: // implement ui::Application
    void setupUi() override;
};

}
