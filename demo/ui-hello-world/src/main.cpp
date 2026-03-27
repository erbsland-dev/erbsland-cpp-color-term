// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "UiHelloWorldApp.hpp"

#include <exception>
#include <iostream>


auto main() -> int {
    try {
        demo::ui_hello_world::UiHelloWorldApp{}.run();
        return 0;
    } catch (const std::exception &exception) {
        std::cerr << "ui-hello-world failed: " << exception.what() << '\n';
    }
    return 1;
}
