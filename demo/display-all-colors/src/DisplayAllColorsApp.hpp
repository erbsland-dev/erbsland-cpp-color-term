// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <erbsland/cterm/all.hpp>


namespace demo::display_all_colors {


using namespace erbsland::cterm;


/// Display and label all available colors.
class DisplayAllColorsApp final {
public:
    /// Run the demo.
    void run() noexcept;

private:
    void renderTable();
    void renderMatrix();
    void renderRainbow();

private:
    Buffer _buffer{Size{1, 1}};
};


}
