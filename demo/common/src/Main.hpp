// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "DemoError.hpp"

#include <iostream>
#include <type_traits>

namespace demo {

class TerminalApplication;

template <typename tApplication>
    requires std::is_base_of_v<TerminalApplication, tApplication>
class Main {
public:
    auto run(const int argc, char **argv) const noexcept -> int {
        auto app = tApplication{};
        auto finalized = false;
        const auto finalize = [&]() -> void {
            if (finalized) {
                return;
            }
            app.finalize();
            finalized = true;
        };
        try {
            if (auto exitCode = app.initialize(argc, argv); exitCode != 0) {
                finalize();
                if (exitCode == -1) {
                    exitCode = 0;
                }
                return exitCode;
            }
            app.main();
            finalize();
        } catch (const DemoError &error) {
            if (!finalized) {
                try {
                    finalize();
                } catch (...) {
                    // ignored while reporting the original exception
                }
            }
            std::cerr << "Demo failed with this error: " << error.what() << '\n';
            return 1;
        } catch (const std::exception &exception) {
            if (!finalized) {
                try {
                    finalize();
                } catch (...) {
                    // ignored while reporting the original exception
                }
            }
            std::cerr << "Demo failed with this error: " << exception.what() << '\n';
            return 1;
        }
        return 0;
    }
};

}
