// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/cterm/all.hpp>


using namespace erbsland::cterm;


namespace erbsland::doctools::paragraph_options_reference {

constexpr auto cExampleWidth = 78;


[[nodiscard]] auto buildRuler() -> String {
    auto result = String{};
    for (int index = 0; index < cExampleWidth; ++index) {
        result.append(Char{U'0' + static_cast<char32_t>(index % 10), fg::BrightBlack});
    }
    return result;
}


[[nodiscard]] auto buildLeadAndFillerText(const std::string_view lead, const std::string_view filler) -> String {
    auto result = String{};
    result.append(fg::BrightWhite, lead, fg::White, filler, fg::Default);
    return result;
}


[[nodiscard]] auto buildAlignmentText() -> String {
    return buildLeadAndFillerText(
        "A paragraph can announce its theme at once: ",
        "the winter lantern swung above the harbor road while clerks, musicians, and late readers hurried "
        "homeward beneath the same patient rain, each keeping a different pace and yet belonging to the same line.");
}


[[nodiscard]] auto buildIndentText() -> String {
    return buildLeadAndFillerText(
        "Indented paragraphs are excellent for guidance text: ",
        "they let a short heading stand close to the margin while the calmer explanatory part settles slightly "
        "deeper, which keeps option descriptions, notes, and examples easy to scan in a crowded terminal.");
}


[[nodiscard]] auto buildWrappedIndentText() -> String {
    auto result = String{};
    result.append(
        fg::BrightWhite,
        "First line: ",
        fg::White,
        "A wrapped continuation should move to the deeper continuation indent so the reader can tell that the "
        "sentence is still flowing forward.",
        fg::Default);
    result.append("\n");
    result.append(
        fg::BrightWhite,
        "After newline: ",
        fg::White,
        "A hard line break starts again at the normal line indent before any later wraps move back to the deeper "
        "wrapped-line indent.",
        fg::Default);
    return result;
}


[[nodiscard]] auto buildWrapMarkerText() -> String {
    return buildLeadAndFillerText(
        "Visible wrap markers turn layout into something the reader can trust: ",
        "they show exactly where the sentence continues, which is especially helpful in previews, manuals, and "
        "teaching material where the shape of the paragraph matters as much as the words.");
}


[[nodiscard]] auto buildSpacingText() -> String {
    auto result = String{};
    result.append(
        fg::BrightWhite,
        "Synopsis: ",
        fg::White,
        "The watchman trimmed the lamp, checked the gate, and listened for the returning carriage.",
        fg::Default);
    result.append("\n");
    result.append(
        fg::BrightWhite,
        "Examples: ",
        fg::White,
        "Use double spacing when neighboring paragraphs should read like separate steps instead of one continuous "
        "argument.",
        fg::Default);
    return result;
}


[[nodiscard]] auto buildWordSeparatorText() -> String {
    auto result = String{};
    result.append(
        fg::BrightWhite,
        "docs/reference/paragraph-options/with/illustrated/examples/for/layout/choices/",
        fg::White,
        "and/friendly/terminal/output/that/readers/can/skim/without/guesswork",
        fg::Default);
    return result;
}


[[nodiscard]] auto buildWordBreakText() -> String {
    auto result = String{};
    result.append(
        fg::BrightWhite,
        "ParagraphLayoutDemonstrationIdentifierForReadersWhoPrefer",
        fg::White,
        "VeryLongNamesThatStillNeedPredictableWrappingInReferenceManualsAndTerminalPreviews",
        fg::Default);
    return result;
}


[[nodiscard]] auto buildWrapLimitText() -> String {
    return buildLeadAndFillerText(
        "Sometimes a paragraph should stop politely instead of taking over the screen: ",
        "for release notes, narrow side panels, or compact popovers, a short ellipsis can admit that more text "
        "exists without forcing the entire chapter into a space meant for a summary.");
}


[[nodiscard]] auto buildTabStopText() -> String {
    auto result = String{};
    result.append(
        fg::BrightWhite,
        "build",
        fg::White,
        "\tCompile the project tree and refresh the generated headers",
        fg::Default);
    result.append("\n");
    result.append(
        fg::BrightWhite,
        "test",
        fg::White,
        "\tRun the unit tests and inspect the failing cases while the logs are still fresh",
        fg::Default);
    result.append("\n");
    result.append(
        fg::BrightWhite,
        "publish",
        fg::White,
        "\tCreate the release archive and attach the changelog for the maintainers",
        fg::Default);
    return result;
}


[[nodiscard]] auto buildTabAddSpaceText() -> String {
    auto result = String{};
    result.append(fg::BrightWhite, "Name", fg::White, "\tStart\tMiddle\tFinish\tNotes\tOwner\tState", fg::Default);
    result.append("\n");
    result.append(fg::BrightWhite, "River", fg::White, "\tStone\tCandlelight\tMap\tInk\tRope\tReady", fg::Default);
    result.append("\n");
    result.append(
        fg::BrightWhite, "Harbor", fg::White, "\tLantern\tWeatherproof\tClock\tSeal\tLedger\tWaiting", fg::Default);
    result.append("\n");
    result.append(
        fg::BrightWhite, "Garden", fg::White, "\tGate\tSilverthread\tBell\tTwine\tPacket\tQueued", fg::Default);
    return result;
}


[[nodiscard]] auto buildTabLineBreakText() -> String {
    auto result = String{};
    result.append(
        fg::BrightWhite,
        "--color",
        fg::White,
        "\tChoose the accent colors for the preview panels and the footer hints",
        fg::Default);
    result.append("\n");
    result.append(
        fg::BrightWhite,
        "--maximum-description-column",
        fg::White,
        "\tCap the description tab stop so narrow terminals still wrap cleanly",
        fg::Default);
    result.append("\n");
    result.append(
        fg::BrightWhite,
        "--paragraph-ellipsis-mark",
        fg::White,
        "\tShow a compact marker when the preview summary had to be clipped",
        fg::Default);
    return result;
}


[[nodiscard]] auto buildBackgroundText() -> String {
    return buildLeadAndFillerText(
        "Background fill is easier to judge with a sentence that keeps moving: ",
        "the blue paper lantern glowed at the window while the final line faded into the dim grey hall beyond it.");
}


[[nodiscard]] auto buildInvalidLayoutText() -> String {
    auto result = String{};
    result.append(fg::BrightWhite, "AA", fg::White, " BB", fg::Default);
    return result;
}


void printSectionTitle(Terminal &terminal, const std::string_view title) {
    terminal.printLine(fg::BrightWhite, title);
    terminal.printLine(buildRuler());
    terminal.setDefaultColor();
}


void printLabel(Terminal &terminal, const std::string_view label) {
    terminal.printLine(fg::BrightYellow, label);
    terminal.setDefaultColor();
}


void writeBuffer(Terminal &terminal, const Buffer &buffer) {
    terminal.write(buffer);
    terminal.writeLineBreak();
    terminal.writeLineBreak();
}


void renderAlignmentExamples(Terminal &terminal) {
    printSectionTitle(terminal, "Alignment");
    auto options = ParagraphOptions{};
    const auto text = buildAlignmentText();

    printLabel(terminal, "Left");
    options.setAlignment(Alignment::Left);
    terminal.printParagraph(text, options);
    terminal.writeLineBreak();

    printLabel(terminal, "Center");
    options.setAlignment(Alignment::Center);
    terminal.printParagraph(text, options);
    terminal.writeLineBreak();

    printLabel(terminal, "Right");
    options.setAlignment(Alignment::Right);
    terminal.printParagraph(text, options);
    terminal.writeLineBreak();
}


void renderIndentExamples(Terminal &terminal) {
    printSectionTitle(terminal, "Indentation");
    const auto text = buildIndentText();

    {
        auto options = ParagraphOptions{};
        options.setLineIndent(8);
        printLabel(terminal, "lineIndent = 8");
        terminal.printParagraph(text, options);
        terminal.writeLineBreak();
    }
    {
        auto options = ParagraphOptions{};
        options.setLineIndent(8);
        options.setFirstLineIndent(0);
        printLabel(terminal, "firstLineIndent = 0");
        terminal.printParagraph(text, options);
        terminal.writeLineBreak();
    }
    {
        auto options = ParagraphOptions{};
        options.setLineIndent(8);
        options.setWrappedLineIndent(14);
        printLabel(terminal, "wrappedLineIndent = 14");
        terminal.printParagraph(buildWrappedIndentText(), options);
        terminal.writeLineBreak();
    }
}


void renderWrapMarkerExamples(Terminal &terminal) {
    printSectionTitle(terminal, "Wrap Markers");
    const auto text = buildWrapMarkerText();

    {
        auto options = ParagraphOptions{};
        options.setLineBreakStartMark(String{U"⤥"});
        printLabel(terminal, "lineBreakStartMark = \"⤥\"");
        terminal.printParagraph(text, options);
        terminal.writeLineBreak();
    }
    {
        auto options = ParagraphOptions{};
        options.setLineBreakEndMark(String{U"⤦"});
        printLabel(terminal, "lineBreakEndMark = \"⤦\"");
        terminal.printParagraph(text, options);
        terminal.writeLineBreak();
    }
    {
        auto options = ParagraphOptions{};
        options.setWrappedLineIndent(8);
        options.setLineBreakStartMark(String{U"⤥"});
        options.setLineBreakEndMark(String{U"⤦"});
        printLabel(terminal, "both marks with wrappedLineIndent = 8");
        terminal.printParagraph(text, options);
        terminal.writeLineBreak();
    }
}


void renderParagraphSpacingExamples(Terminal &terminal) {
    printSectionTitle(terminal, "Paragraph Spacing");

    {
        auto buffer = Buffer{Size{cExampleWidth, 4}};
        auto text = Text{buildSpacingText(), buffer.rect(), Alignment::TopLeft};
        text.setParagraphSpacing(ParagraphSpacing::SingleLine);
        buffer.drawText(text);
        printLabel(terminal, "ParagraphSpacing::SingleLine");
        writeBuffer(terminal, buffer);
    }
    {
        auto buffer = Buffer{Size{cExampleWidth, 5}};
        auto text = Text{buildSpacingText(), buffer.rect(), Alignment::TopLeft};
        text.setParagraphSpacing(ParagraphSpacing::DoubleLine);
        buffer.drawText(text);
        printLabel(terminal, "ParagraphSpacing::DoubleLine");
        writeBuffer(terminal, buffer);
    }
}


void renderWordSeparatorExamples(Terminal &terminal) {
    printSectionTitle(terminal, "Word Separators");

    {
        auto buffer = Buffer{Size{cExampleWidth, 3}};
        auto text = Text{buildWordSeparatorText(), buffer.rect(), Alignment::TopLeft};
        printLabel(terminal, "Default separators");
        buffer.drawText(text);
        writeBuffer(terminal, buffer);
    }
    {
        auto buffer = Buffer{Size{cExampleWidth, 4}};
        auto text = Text{buildWordSeparatorText(), buffer.rect(), Alignment::TopLeft};
        text.setWordSeparators(U" /");
        printLabel(terminal, "wordSeparators = U\" /\"");
        buffer.drawText(text);
        writeBuffer(terminal, buffer);
    }
}


void renderWordBreakExamples(Terminal &terminal) {
    printSectionTitle(terminal, "Word Break Mark");

    {
        auto buffer = Buffer{Size{cExampleWidth, 3}};
        auto text = Text{buildWordBreakText(), buffer.rect(), Alignment::TopLeft};
        printLabel(terminal, "Default '-'");
        buffer.drawText(text);
        writeBuffer(terminal, buffer);
    }
    {
        auto buffer = Buffer{Size{cExampleWidth, 3}};
        auto text = Text{buildWordBreakText(), buffer.rect(), Alignment::TopLeft};
        text.setWordBreakMark(Char{U'~'});
        printLabel(terminal, "wordBreakMark = '~'");
        buffer.drawText(text);
        writeBuffer(terminal, buffer);
    }
}


void renderWrapLimitExamples(Terminal &terminal) {
    printSectionTitle(terminal, "Wrap Limit and Ellipsis");

    {
        auto buffer = Buffer{Size{cExampleWidth, 4}};
        auto text = Text{buildWrapLimitText(), buffer.rect(), Alignment::TopLeft};
        printLabel(terminal, "Unlimited wraps");
        buffer.drawText(text);
        writeBuffer(terminal, buffer);
    }
    {
        auto buffer = Buffer{Size{cExampleWidth, 2}};
        auto text = Text{buildWrapLimitText(), buffer.rect(), Alignment::TopLeft};
        text.setMaximumLineWraps(1);
        text.setParagraphEllipsisMark(String{" (more)"});
        printLabel(terminal, "maximumLineWraps = 1");
        buffer.drawText(text);
        writeBuffer(terminal, buffer);
    }
}


void renderTabExamples(Terminal &terminal) {
    printSectionTitle(terminal, "Tabs and Overflow");

    {
        auto buffer = Buffer{Size{cExampleWidth, 4}};
        auto text = Text{buildTabStopText(), buffer.rect(), Alignment::TopLeft};
        text.setTabStops({14});
        printLabel(terminal, "tabStops = {14}");
        buffer.drawText(text);
        writeBuffer(terminal, buffer);
    }
    {
        auto buffer = Buffer{Size{cExampleWidth, 4}};
        auto text = Text{buildTabAddSpaceText(), buffer.rect(), Alignment::TopLeft};
        text.setTabStops({10, 20, 30, 40, 50, 60, 70});
        text.setTabOverflowBehavior(TabOverflowBehavior::AddSpace);
        printLabel(terminal, "TabOverflowBehavior::AddSpace");
        buffer.drawText(text);
        writeBuffer(terminal, buffer);
    }
    {
        auto buffer = Buffer{Size{cExampleWidth, 8}};
        auto text = Text{buildTabLineBreakText(), buffer.rect(), Alignment::TopLeft};
        text.setWrappedLineIndent(15);
        text.setTabStops({15});
        text.setTabOverflowBehavior(TabOverflowBehavior::LineBreak);
        printLabel(terminal, "TabOverflowBehavior::LineBreak");
        buffer.drawText(text);
        writeBuffer(terminal, buffer);
    }
}


void renderBackgroundMode(Terminal &terminal, const std::string_view label, const ParagraphBackgroundMode mode) {
    auto buffer = Buffer{Size{cExampleWidth, 3}, Char{" ", fg::BrightWhite, bg::BrightBlack}};
    auto text = Text{buildBackgroundText(), buffer.rect(), Alignment::TopLeft};
    text.setColor(Color{fg::BrightWhite, bg::Blue});
    text.setWrappedLineIndent(10);
    text.setBackgroundMode(mode);
    printLabel(terminal, label);
    buffer.drawText(text);
    writeBuffer(terminal, buffer);
}


void renderBackgroundExamples(Terminal &terminal) {
    printSectionTitle(terminal, "Background Mode");
    renderBackgroundMode(terminal, "ParagraphBackgroundMode::Default", ParagraphBackgroundMode::Default);
    renderBackgroundMode(terminal, "ParagraphBackgroundMode::WrappedLeft", ParagraphBackgroundMode::WrappedLeft);
    renderBackgroundMode(terminal, "ParagraphBackgroundMode::WrappedRight", ParagraphBackgroundMode::WrappedRight);
    renderBackgroundMode(terminal, "ParagraphBackgroundMode::WrappedBoth", ParagraphBackgroundMode::WrappedBoth);
    renderBackgroundMode(terminal, "ParagraphBackgroundMode::FullRight", ParagraphBackgroundMode::FullRight);
    renderBackgroundMode(terminal, "ParagraphBackgroundMode::FullBoth", ParagraphBackgroundMode::FullBoth);
}


void renderOnErrorExamples(Terminal &terminal) {
    printSectionTitle(terminal, "On Error");

    {
        auto buffer = Buffer{Size{cExampleWidth, 2}};
        auto text = Text{buildInvalidLayoutText(), Rectangle{0, 0, 2, 2}, Alignment::TopLeft};
        text.setLineBreakEndMark(String{U"⤥⤥"});
        text.setOnError(ParagraphOnError::PlainOutput);
        printLabel(terminal, "ParagraphOnError::PlainOutput");
        buffer.drawText(text);
        writeBuffer(terminal, buffer);
    }
    {
        auto buffer = Buffer{Size{cExampleWidth, 2}, Char{U'·', fg::BrightBlack, bg::Default}};
        auto text = Text{buildInvalidLayoutText(), Rectangle{0, 0, 2, 2}, Alignment::TopLeft};
        text.setLineBreakEndMark(String{U"⤥⤥"});
        text.setOnError(ParagraphOnError::Empty);
        printLabel(terminal, "ParagraphOnError::Empty");
        buffer.drawText(text);
        writeBuffer(terminal, buffer);
    }
}


void renderAllExamples() {
    auto terminal = Terminal{Size{cExampleWidth, 160}};
    terminal.setSizeDetectionEnabled(false);

    renderAlignmentExamples(terminal);
    renderIndentExamples(terminal);
    renderWrapMarkerExamples(terminal);
    renderParagraphSpacingExamples(terminal);
    renderWordSeparatorExamples(terminal);
    renderWordBreakExamples(terminal);
    renderWrapLimitExamples(terminal);
    renderTabExamples(terminal);
    renderBackgroundExamples(terminal);
    renderOnErrorExamples(terminal);

    terminal.setDefaultColor();
    terminal.flush();
}

}


auto main() -> int {
    erbsland::doctools::paragraph_options_reference::renderAllExamples();
    return 0;
}
