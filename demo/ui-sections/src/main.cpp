// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "UiSectionsApp.hpp"

#include <exception>
#include <iostream>

auto main(const int argc, char *argv[]) -> int {
    try {
        return demo::ui_sections::UiSectionsApp{argc, argv}.run();
    } catch (const std::exception &exception) {
        std::cerr << "ui-sections failed: " << exception.what() << '\n';
    }
    return 1;
}
