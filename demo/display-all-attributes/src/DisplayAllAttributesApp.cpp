// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "DisplayAllAttributesApp.hpp"

namespace demo::display_all_attributes {

auto DisplayAllAttributesApp::attributeSpecs() -> const std::array<AttributeSpec, 8> & {
    static const auto cAttributeSpecs = std::array<AttributeSpec, 8>{{
        {"Bold", CharAttributes::Bold, "1 / 22", "stronger emphasis", "The quick brown fox 0123"},
        {"Dim", CharAttributes::Dim, "2 / 22", "reduced intensity", "The quick brown fox 0123"},
        {"Italic",
         CharAttributes::Italic,
         "3 / 23",
         "often unsupported in basic terminals",
         "The quick brown fox 0123"},
        {"Underline", CharAttributes::Underline, "4 / 24", "classic link-style emphasis", "The quick brown fox 0123"},
        {"Blink", CharAttributes::Blink, "5 / 25", "often disabled by terminal emulators", "The quick brown fox 0123"},
        {"Reverse", CharAttributes::Reverse, "7 / 27", "swaps foreground and background", "The quick brown fox 0123"},
        {"Hidden", CharAttributes::Hidden, "8 / 28", "text should disappear between the markers", "hidden sample"},
        {"Strikethrough", CharAttributes::Strikethrough, "9 / 29", "ANSI crossed-out text", "The quick brown fox 0123"},
    }};
    return cAttributeSpecs;
}

auto DisplayAllAttributesApp::beforeRun() -> int {
    _terminal.input().setMode(Input::Mode::ReadLine);
    printHeader(_terminal);
    printAttributeTable(_terminal);
    printCombinations(_terminal);
    printPausePrompt(_terminal);
    _terminal.flush();
    if (_terminal.isInteractive()) {
        static_cast<void>(_terminal.input().readLine());
    }
    return -1;
}

void DisplayAllAttributesApp::printHeader(Terminal &terminal) noexcept {
    terminal.printLine(fg::BrightWhite, CharAttributes{CharAttributes::Bold}, "Display All Attributes");
    terminal.printLine(
        fg::BrightBlack,
        "Visual differences depend on your terminal emulator and theme. ",
        "If a sample looks unchanged, that attribute is likely ignored.");
    terminal.printLine(
        fg::BrightBlack,
        "The backend support column shows what the library backend can emit, not what your terminal actually renders.");
    terminal.writeLineBreak();
    terminal.printLine(
        fg::BrightWhite,
        "Attribute       Backend   ANSI      Sample                                               Note");
    terminal.printLine(
        fg::BrightBlack,
        "---------------------------------------------------------------------------------------------------");
}

void DisplayAllAttributesApp::printAttributeTable(Terminal &terminal) noexcept {
    const auto supported = terminal.supportedCharAttributes();
    for (const auto &spec : attributeSpecs()) {
        printAttributeRow(terminal, spec, supported.isEnabled(spec.flag));
    }
    terminal.writeLineBreak();
}

void DisplayAllAttributesApp::printAttributeRow(
    Terminal &terminal, const AttributeSpec &spec, const bool supported) noexcept {

    terminal.print(
        fg::BrightCyan,
        padded(spec.name, 15),
        fg::Default,
        padded(supportLabel(supported), 10),
        fg::BrightBlack,
        padded(spec.sgrCodes, 10));
    if (spec.flag.value == CharAttributes::Hidden.value) {
        terminal.print(
            fg::Default,
            " |",
            sampleAttributes(spec.flag),
            spec.sampleText,
            CharAttributes::reset(),
            "|",
            fg::BrightBlack,
            " ",
            spec.note);
    } else {
        terminal.print(
            fg::BrightWhite,
            bg::BrightBlack,
            " ",
            sampleAttributes(spec.flag),
            spec.sampleText,
            CharAttributes::reset(),
            bg::Default,
            " ",
            fg::BrightBlack,
            " ",
            spec.note);
    }
    terminal.writeLineBreak();
}

void DisplayAllAttributesApp::printCombinations(Terminal &terminal) noexcept {
    auto emphatic = CharAttributes{};
    emphatic.setBold(true);
    emphatic.setUnderline(true);
    auto editorial = CharAttributes{};
    editorial.setItalic(true);
    editorial.setStrikethrough(true);
    auto alert = CharAttributes{};
    alert.setBold(true);
    alert.setBlink(true);
    alert.setReverse(true);

    terminal.printLine(fg::BrightWhite, CharAttributes{CharAttributes::Bold}, "Common Combinations");
    terminal.printLine(
        fg::BrightBlack,
        "These examples help spot interactions between attributes, especially reset handling for bold and dim.");
    terminal.printLine(
        fg::BrightCyan,
        padded("Bold + Underline", 28),
        fg::BrightWhite,
        emphatic,
        "Highlighted heading sample",
        CharAttributes::reset());
    terminal.printLine(
        fg::BrightCyan,
        padded("Italic + Strikethrough", 28),
        fg::BrightWhite,
        editorial,
        "Edited-out annotation sample",
        CharAttributes::reset());
    terminal.printLine(
        fg::BrightCyan,
        padded("Bold + Blink + Reverse", 28),
        fg::BrightWhite,
        bg::Red,
        alert,
        "High-attention sample",
        CharAttributes::reset(),
        Color::reset());
    terminal.writeLineBreak();
}

void DisplayAllAttributesApp::printPausePrompt(Terminal &terminal) noexcept {
    if (!terminal.isInteractive()) {
        return;
    }
    terminal.printLine(fg::BrightBlack, "Press Enter to close the demo.");
}

auto DisplayAllAttributesApp::padded(const std::string_view text, const std::size_t width) -> std::string {
    auto result = std::string{text};
    if (result.size() < width) {
        result.append(width - result.size(), ' ');
    }
    return result;
}

auto DisplayAllAttributesApp::supportLabel(const bool supported) -> std::string_view {
    if (supported) {
        return "yes";
    }
    return "no";
}

auto DisplayAllAttributesApp::sampleAttributes(const CharAttributes::Flag flag) -> CharAttributes {
    auto attributes = CharAttributes{};
    if (flag.value == CharAttributes::Bold.value) {
        attributes.setBold(true);
    } else if (flag.value == CharAttributes::Dim.value) {
        attributes.setDim(true);
    } else if (flag.value == CharAttributes::Italic.value) {
        attributes.setItalic(true);
    } else if (flag.value == CharAttributes::Underline.value) {
        attributes.setUnderline(true);
    } else if (flag.value == CharAttributes::Blink.value) {
        attributes.setBlink(true);
    } else if (flag.value == CharAttributes::Reverse.value) {
        attributes.setReverse(true);
    } else if (flag.value == CharAttributes::Hidden.value) {
        attributes.setHidden(true);
    } else if (flag.value == CharAttributes::Strikethrough.value) {
        attributes.setStrikethrough(true);
    }
    return attributes;
}

}
