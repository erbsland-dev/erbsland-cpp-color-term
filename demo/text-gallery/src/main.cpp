// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TextGalleryApp.hpp"

#include <exception>
#include <iostream>


auto main() -> int {
    try {
        demo::textgallery::TextGalleryApp{}.run();
        return 0;
    } catch (const std::exception &exception) {
        std::cerr << "text-gallery failed: " << exception.what() << '\n';
    }
    return 1;
}
