// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "UiChoiceApp.hpp"

#include <exception>
#include <iostream>

auto main(const int argc, char *argv[]) -> int {
    try {
        return demo::ui_choice::UiChoiceApp{argc, argv}.run();
    } catch (const std::exception &exception) {
        std::cerr << "ui-choice failed: " << exception.what() << '\n';
    }
    return 1;
}
