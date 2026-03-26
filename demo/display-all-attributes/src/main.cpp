// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "DisplayAllAttributesApp.hpp"

#include <exception>
#include <iostream>


auto main() -> int {
    try {
        demo::display_all_attributes::DisplayAllAttributesApp{}.run();
        return 0;
    } catch (const std::exception &exception) {
        std::cerr << "display-all-attributes failed: " << exception.what() << '\n';
    }
    return 1;
}
