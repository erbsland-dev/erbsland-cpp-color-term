// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "UpdateScreenModesApp.hpp"

#include <exception>
#include <iostream>


auto main() -> int {
    try {
        demo::updatescreenmodes::UpdateScreenModesApp{}.run();
        return 0;
    } catch (const std::exception &exception) {
        std::cerr << "update-screen-modes failed: " << exception.what() << '\n';
    }
    return 1;
}
