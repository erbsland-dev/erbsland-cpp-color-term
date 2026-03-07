// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "FrameWeaverApp.hpp"

#include <exception>
#include <iostream>


auto main() -> int {
    try {
        demo::frameweaver::FrameWeaverApp{}.run();
        return 0;
    } catch (const std::exception &exception) {
        std::cerr << "frame-weaver failed: " << exception.what() << '\n';
    }
    return 1;
}
