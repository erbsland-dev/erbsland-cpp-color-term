// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "StoryDemo.hpp"

#include <string_view>

namespace demo::terminalchronicle {

auto StoryDemo::beforeRun() -> int {
    printHeader(_terminal);
    printTimeline(_terminal);
    printStory(_terminal);
    printOutro(_terminal);
    _terminal.setDefaultColor();
    _terminal.flush();
    return -1;
}

void StoryDemo::printHeader(Terminal &terminal) noexcept {
    terminal.writeLineBreak();
    terminal.printLine(
        bg::Blue,
        fg::BrightWhite,
        " Terminal Chronicle ",
        Color::reset(),
        " ",
        fg::BrightBlack,
        "Low-level output with readable print calls.");
    terminal.writeLineBreak();
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
    terminal.writeLineBreak();
    terminal.writeLineBreak();
}

void StoryDemo::printStory(Terminal &terminal) noexcept {
    terminal.printLine(
        fg::BrightCyan,
        "1968",
        fg::BrightBlack,
        "  Video terminals began to replace noisy paper with phosphor screens.");
    terminal.printLine("      A cursor could move, lines could be rewritten, and suddenly the console");
    terminal.printLine("      felt alive.");
    terminal.writeLineBreak();
    terminal.printLine(
        fg::BrightGreen,
        "1978",
        fg::BrightBlack,
        "  DEC introduced the ",
        fg::BrightWhite,
        "VT100",
        fg::BrightBlack,
        ", a terminal that had a language of escape ");
    terminal.printLine(
        "      sequences: ",
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
    terminal.writeLineBreak();

    terminal.printLine(
        fg::BrightYellow,
        "1980s",
        fg::BrightBlack,
        " Bulletin boards, editors, and debuggers discovered that text mode did");
    terminal.printLine("      not have to look dull. Progress bars, highlighted menus, and boxed");
    terminal.printLine("      dialogs were all just carefully printed characters.");
    terminal.writeLineBreak();

    terminal.printLine(
        fg::BrightMagenta,
        "Today",
        fg::BrightBlack,
        " The idea is unchanged: emit text, switch styles, keep the code compact,");
    terminal.printLine("      and let the terminal shine.");
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
        " to tell a story");
    terminal.printLine("      without building a custom renderer first.");
    terminal.writeLineBreak();
}

void StoryDemo::printOutro(Terminal &terminal) noexcept {
    terminal.printLine(
        bg::BrightBlack,
        fg::BrightWhite,
        " Try the other demos next: ",
        Color{fg::Default, bg::Default},
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
    terminal.writeLineBreak();
}

}
