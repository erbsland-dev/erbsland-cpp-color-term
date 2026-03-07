// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <erbsland/cterm/all.hpp>


namespace demo::terminalchronicle {


using namespace erbsland::cterm;


/// Tell a short, colorful story using only `Terminal::print()` and `Terminal::printLine()`.
class StoryDemo final {
public:
    /// Run the demo.
    void run() noexcept;

private:
    static void printHeader(Terminal &terminal) noexcept;
    static void printTimeline(Terminal &terminal) noexcept;
    static void printStory(Terminal &terminal) noexcept;
    static void printOutro(Terminal &terminal) noexcept;
};


}
