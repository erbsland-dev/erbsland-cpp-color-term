// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "CommandLineHelpDemo.hpp"

#include <exception>
#include <iostream>


auto main(const int argc, char **argv) -> int {
    try {
        demo::command_line_help::CommandLineHelpDemo{}.run(argc, argv);
        return 0;
    } catch (const std::exception &exception) {
        std::cerr << "command-line-help failed: " << exception.what() << '\n';
    }
    return 1;
}
