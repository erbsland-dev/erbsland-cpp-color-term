// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "BitmapShowcaseApp.hpp"

#include <exception>
#include <iostream>


auto main() -> int {
    try {
        demo::bitmapshowcase::BitmapShowcaseApp{}.run();
        return 0;
    } catch (const std::exception &exception) {
        std::cerr << "bitmap-showcase failed: " << exception.what() << '\n';
    }
    return 1;
}
