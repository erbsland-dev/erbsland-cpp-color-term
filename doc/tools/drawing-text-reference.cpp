// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/cterm/all.hpp>


using namespace erbsland::cterm;


namespace erbsland::doctools::drawing_text_reference {

constexpr auto cExampleWidth = Coordinate{72};
constexpr auto cCanvasColor = Color{fg::BrightWhite, bg::Black};
constexpr auto cLabelColor = Color{fg::BrightWhite, bg::Black};
constexpr auto cAccentColor = Color{fg::BrightBlack, bg::Black};


[[nodiscard]] auto createCanvas(const Coordinate height) -> Buffer {
    return Buffer{Size{cExampleWidth, height}, Char{" ", cCanvasColor}};
}


void printSectionTitle(Terminal &terminal, const std::string_view title) {
    terminal.printLine(fg::BrightWhite, title);
    terminal.setDefaultColor();
}


void writeBuffer(Terminal &terminal, const Buffer &buffer) {
    terminal.write(buffer);
    terminal.writeLineBreak();
    terminal.writeLineBreak();
}


void drawCenteredLabel(
    Buffer &buffer, const Rectangle rect, const std::string_view label, const Color color = cLabelColor) {
    buffer.drawText(label, rect, Alignment::TopCenter, color);
}


[[nodiscard]] auto buildMixedStatus() -> String {
    auto result = String{};
    result.append(
        bg::BrightBlack,
        fg::BrightYellow,
        "[Q]",
        fg::BrightWhite,
        " quit  ",
        fg::BrightCyan,
        "[R]",
        fg::BrightWhite,
        " refresh");
    return result;
}


void renderDirectStringExample(Terminal &terminal) {
    printSectionTitle(terminal, "Direct Strings and Block Text");
    auto buffer = createCanvas(10);

    drawCenteredLabel(buffer, Rectangle{0, 0, 34, 1}, "drawText(Position, String)");
    drawCenteredLabel(buffer, Rectangle{38, 0, 34, 1}, "drawText(String, Rectangle, ...)");

    auto ruler = String{};
    for (int index = 0; index < 30; ++index) {
        ruler.append(Char{U'0' + static_cast<char32_t>(index % 10), fg::BrightBlack});
    }
    buffer.drawText(Position{2, 2}, ruler);
    buffer.drawText(Position{4, 4}, buildMixedStatus());
    buffer.drawText("exact position, mixed colors", Rectangle{0, 8, 34, 1}, Alignment::TopCenter, cAccentColor);

    const auto panel = Rectangle{42, 2, 24, 5};
    buffer.drawFilledFrame(
        panel,
        FrameStyle::LightWithRoundedCorners,
        Char{" ", Color{fg::Inherited, bg::Blue}},
        Color{fg::BrightCyan, bg::Inherited});
    buffer.drawText("Overview", panel, Alignment::Center, Color{fg::BrightWhite, bg::Inherited});
    buffer.drawText("same API, aligned in a rectangle", Rectangle{38, 8, 34, 1}, Alignment::TopCenter, cAccentColor);

    writeBuffer(terminal, buffer);
}


void drawTextCard(Buffer &buffer, const Rectangle rect, const Alignment alignment, const std::string_view label) {
    buffer.drawFilledFrame(
        rect,
        FrameStyle::Light,
        Char{" ", Color{fg::Inherited, bg::BrightBlack}},
        Color{fg::BrightWhite, bg::Inherited});
    auto text = Text{String{"Short note"}, rect.insetBy(Margins{1}), alignment};
    text.setColor(Color{fg::BrightYellow, bg::Inherited});
    buffer.drawText(text);
    drawCenteredLabel(buffer, Rectangle{rect.x1(), rect.y2(), rect.width(), 1}, label, cAccentColor);
}


void renderAlignmentExample(Terminal &terminal) {
    printSectionTitle(terminal, "Text Alignment Inside a Rectangle");
    auto buffer = createCanvas(9);

    buffer.drawText(
        "Text uses the same alignment model as other geometry-aware drawing helpers.",
        Rectangle{0, 0, cExampleWidth, 1},
        Alignment::TopCenter,
        cLabelColor);

    drawTextCard(buffer, Rectangle{2, 2, 20, 4}, Alignment::TopLeft, "TopLeft");
    drawTextCard(buffer, Rectangle{26, 2, 20, 4}, Alignment::Center, "Center");
    drawTextCard(buffer, Rectangle{50, 2, 20, 4}, Alignment::BottomRight, "BottomRight");

    writeBuffer(terminal, buffer);
}


void renderParagraphExample(Terminal &terminal) {
    printSectionTitle(terminal, "Wrapped Paragraphs");
    auto buffer = createCanvas(13);

    buffer.drawFilledFrame(
        Rectangle{6, 1, 60, 11},
        FrameStyle::Double,
        Char{" ", Color{fg::Inherited, bg::Blue}},
        Color{fg::BrightWhite, bg::Inherited});
    buffer.drawText("Help Panel", Rectangle{8, 1, 56, 1}, Alignment::Center, Color{fg::BrightWhite, bg::Blue});

    auto help = Text{
        String{"Navigation: Use arrow keys to move.\n\nActions: Press Enter to open."},
        Rectangle{10, 3, 52, 7},
        Alignment::TopLeft};
    help.setParagraphSpacing(ParagraphSpacing::DoubleLine);
    help.setWrappedLineIndent(2);
    help.setColor(Color{fg::BrightWhite, bg::Inherited});
    buffer.drawText(help);

    buffer.drawText(
        "Text automatically wraps and keeps explicit paragraph breaks.",
        Rectangle{0, 12, cExampleWidth, 1},
        Alignment::TopCenter,
        cAccentColor);
    writeBuffer(terminal, buffer);
}


void renderOptionsAnimationExample(Terminal &terminal) {
    printSectionTitle(terminal, "TextOptions and Animation");
    auto buffer = createCanvas(10);

    auto options = TextOptions{Alignment::Center};
    options.setColorSequence(
        ColorSequence{
            Color{fg::BrightYellow, bg::Inherited},
            Color{fg::BrightRed, bg::Inherited},
            Color{fg::BrightMagenta, bg::Inherited},
            Color{fg::BrightCyan, bg::Inherited},
        });
    options.setAnimation(TextAnimation::ColorDiagonal);

    buffer.drawFilledFrame(Rectangle{2, 1, 32, 7}, FrameStyle::LightWithRoundedCorners, Char{" ", bg::BrightBlack});
    buffer.drawFilledFrame(Rectangle{38, 1, 32, 7}, FrameStyle::LightWithRoundedCorners, Char{" ", bg::BrightBlack});
    buffer.drawText(
        "Reuse one TextOptions preset", Rectangle{0, 0, cExampleWidth, 1}, Alignment::TopCenter, cLabelColor);

    auto left = Text{String{"ALERT PANEL"}, Rectangle{4, 3, 28, 2}, Alignment::Center};
    left.setTextOptions(options);
    auto right = Text{String{"ALERT PANEL"}, Rectangle{40, 3, 28, 2}, Alignment::Center};
    right.setTextOptions(options);

    buffer.drawText(left, 1);
    buffer.drawText(right, 3);
    buffer.drawText("cycle = 1", Rectangle{2, 8, 32, 1}, Alignment::TopCenter, cAccentColor);
    buffer.drawText("cycle = 3", Rectangle{38, 8, 32, 1}, Alignment::TopCenter, cAccentColor);

    writeBuffer(terminal, buffer);
}


void renderAllExamples() {
    auto terminal = Terminal{Size{cExampleWidth, 96}};
    terminal.setSizeDetectionEnabled(false);

    renderDirectStringExample(terminal);
    renderAlignmentExample(terminal);
    renderParagraphExample(terminal);
    renderOptionsAnimationExample(terminal);

    terminal.setDefaultColor();
    terminal.flush();
}

}


auto main() -> int {
    erbsland::doctools::drawing_text_reference::renderAllExamples();
    return 0;
}
