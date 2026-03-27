// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <erbsland/cterm/all.hpp>
#include <erbsland/cterm/ui/all.hpp>

namespace demo::ui_hello_world {

using namespace erbsland::cterm;

class UiHelloWorldApp {
public:
    void run();

private:
    void setupUi();
    [[nodiscard]] static auto currentDateTimeText() -> String;

private:
    ui::Application _app;
};


}
