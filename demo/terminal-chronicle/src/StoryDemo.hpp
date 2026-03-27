// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "TerminalApplication.hpp"


namespace demo::terminalchronicle {


using namespace erbsland::cterm;


/// Tell a short, colorful story using only `Terminal::print()` and `Terminal::printLine()`.
class StoryDemo final : public TerminalApplication {
public:
    /// Render the story once and exit the demo.
    auto beforeRun() -> int override;

private:
    static void printHeader(Terminal &terminal) noexcept;
    static void printTimeline(Terminal &terminal) noexcept;
    static void printStory(Terminal &terminal) noexcept;
    static void printOutro(Terminal &terminal) noexcept;
};


}
