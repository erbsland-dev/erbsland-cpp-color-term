// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "FrameColorAnimationsApp.hpp"

#include <exception>
#include <iostream>


auto main() -> int {
    try {
        demo::framecoloranimations::FrameColorAnimationsApp{}.run();
        return 0;
    } catch (const std::exception &exception) {
        std::cerr << "frame-color-animations failed: " << exception.what() << '\n';
    }
    return 1;
}
