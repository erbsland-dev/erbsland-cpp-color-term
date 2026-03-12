// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/cterm/all.hpp>

using namespace erbsland::cterm;

const auto cThisCode = String::fromLines(
    {
        R"(auto main() -> int {)",
        R"(    Terminal term{Size{80, 24}};)",
        R"(    term.initializeScreen();)",
        R"(    Buffer buffer{term.size()};)",
        R"(    buffer.fill(Char{' ', fg::BrightBlue, bg::Black});)",
        R"(    buffer.drawText(Position{3, 5}, cThisCode);)",
        R"(    buffer.drawFrame(buffer.rect(), FrameStyle::Light);)",
        R"(    auto titleRect = buffer.rect().subRectangle(Anchor::Top, {0, 3}, {3, 2});)",
        R"(    buffer.drawText("Demo with Only 14 Lines of Code",)",
        R"(        titleRect, Alignment::Center, Color{fg::BrightYellow});)",
        R"(    term.updateScreen(buffer);)",
        R"(    term.restoreScreen();)",
        R"(    return 0;)",
        R"(})",
    },
    Color{fg::BrightCyan, bg::Inherited});

auto main() -> int {
    Terminal term{Size{80, 24}};
    term.initializeScreen();
    Buffer buffer{term.size()};
    buffer.fill(Char{' ', fg::BrightBlue, bg::Black});
    buffer.drawText(Position{3, 5}, cThisCode);
    buffer.drawFrame(buffer.rect(), FrameStyle::Light);
    auto titleRect = buffer.rect().subRectangle(Anchor::Top, {0, 3}, {3, 2});
    buffer.drawText("Demo with Only 14 Lines of Code", titleRect, Alignment::Center, Color{fg::BrightYellow});
    term.updateScreen(buffer);
    term.restoreScreen();
    return 0;
}
