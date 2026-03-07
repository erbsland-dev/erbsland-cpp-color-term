// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "StoryDemo.hpp"

#include "ScopedTerminalSession.hpp"

#include <string_view>


namespace demo::terminalchronicle {


void StoryDemo::run() noexcept {
    auto terminal = Terminal{Size{100, 30}};
    auto session = demo::ScopedTerminalSession{terminal, Terminal::RefreshMode::Clear, Input::Mode::ReadLine, false};
    printHeader(terminal);
    printTimeline(terminal);
    printStory(terminal);
    printOutro(terminal);
    terminal.setDefaultColor();
    terminal.flush();
}


void StoryDemo::printHeader(Terminal &terminal) noexcept {
    terminal.printLine(
        bg::Blue,
        fg::BrightWhite,
        " Terminal Chronicle ",
        Color{},
        " ",
        fg::BrightBlack,
        "Low-level output with one Terminal instance and readable print calls.");
    terminal.lineBreak();
}


void StoryDemo::printTimeline(Terminal &terminal) noexcept {
    terminal.print(fg::BrightBlack, "Timeline: ");
    terminal.print(
        fg::BrightCyan,
        "teletype",
        fg::BrightBlack,
        " -> ",
        fg::BrightGreen,
        "glass terminal",
        fg::BrightBlack,
        " -> ",
        fg::BrightYellow,
        "VT100",
        fg::BrightBlack,
        " -> ",
        fg::BrightMagenta,
        "modern ANSI consoles");
    terminal.lineBreak();
    terminal.lineBreak();
}


void StoryDemo::printStory(Terminal &terminal) noexcept {
    terminal.printLine(
        fg::BrightCyan,
        "1968",
        fg::BrightBlack,
        "  Video terminals began to replace noisy paper with phosphor screens.");
    terminal.printLine("      A cursor could move, lines could be rewritten, and suddenly the console felt alive.");
    terminal.lineBreak();

    terminal.printLine(
        fg::BrightGreen,
        "1978",
        fg::BrightBlack,
        "  DEC introduced the ",
        fg::BrightWhite,
        "VT100",
        fg::BrightBlack,
        ", a terminal that spoke a compact language of escape sequences.");
    terminal.printLine(
        "      ",
        fg::Yellow,
        "Move the cursor",
        fg::BrightBlack,
        ", ",
        fg::Magenta,
        "change colors",
        fg::BrightBlack,
        ", ",
        fg::Cyan,
        "redraw only what changed",
        fg::BrightBlack,
        ".");
    terminal.lineBreak();

    terminal.printLine(
        fg::BrightYellow,
        "1980s",
        fg::BrightBlack,
        " Bulletin boards, editors, and debuggers discovered that text mode did not have to look dull.");
    terminal.printLine(
        "      Progress bars, highlighted menus, and boxed dialogs were all just carefully printed characters.");
    terminal.lineBreak();

    terminal.printLine(
        fg::BrightMagenta,
        "Today",
        fg::BrightBlack,
        " The idea is unchanged: emit text, switch styles, keep the code compact, and let the terminal shine.");
    terminal.printLine(
        "      This demo uses ",
        fg::BrightWhite,
        "printLine(",
        fg::BrightYellow,
        "\"text\"",
        fg::BrightBlack,
        ", ",
        fg::BrightCyan,
        "fg::...",
        fg::BrightBlack,
        ", ",
        fg::BrightGreen,
        "bg::...",
        fg::BrightWhite,
        ")",
        fg::BrightBlack,
        " to tell a story without building a custom renderer first.");
    terminal.lineBreak();
}


void StoryDemo::printOutro(Terminal &terminal) noexcept {
    terminal.printLine(
        bg::BrightBlack,
        fg::BrightWhite,
        " Try the other demos next: ",
        Color{},
        " ",
        fg::BrightCyan,
        "retro-plasma",
        fg::BrightBlack,
        ", ",
        fg::BrightGreen,
        "frame-weaver",
        fg::BrightBlack,
        ", ",
        fg::BrightMagenta,
        "text-gallery",
        fg::BrightBlack,
        ".");
}


}
