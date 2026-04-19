// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "UiHtmlViewerApp.hpp"

#include <exception>
#include <iostream>

auto main(const int argc, char **argv) -> int {
    try {
        return demo::ui_html_viewer::UiHtmlViewerApp{}.run(argc, argv);
    } catch (const std::exception &exception) {
        std::cerr << "ui-html-viewer failed: " << exception.what() << '\n';
    }
    return 1;
}
