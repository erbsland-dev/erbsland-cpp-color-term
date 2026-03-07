// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "RetroPlasmaApp.hpp"

#include <exception>
#include <iostream>


auto main() -> int {
    try {
        demo::retroplasma::RetroPlasmaApp{}.run();
        return 0;
    } catch (const std::exception &exception) {
        std::cerr << "retro-plasma failed: " << exception.what() << '\n';
    }
    return 1;
}
