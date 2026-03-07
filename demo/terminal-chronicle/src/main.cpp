// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "StoryDemo.hpp"

#include <exception>
#include <iostream>


auto main() -> int {
    try {
        demo::terminalchronicle::StoryDemo{}.run();
        return 0;
    } catch (const std::exception &exception) {
        std::cerr << "terminal-chronicle failed: " << exception.what() << '\n';
    }
    return 1;
}
