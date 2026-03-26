// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "KeyInputDemoApp.hpp"

#include <exception>
#include <iostream>


auto main() -> int {
    try {
        demo::keyinputdemo::KeyInputDemoApp{}.run();
        return 0;
    } catch (const std::exception &exception) {
        std::cerr << "key-input-demo failed: " << exception.what() << '\n';
    }
    return 1;
}
