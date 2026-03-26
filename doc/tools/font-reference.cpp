// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/cterm/all.hpp>


using namespace erbsland::cterm;


namespace erbsland::doctools::font_reference {

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


void drawCenteredLabel(Buffer &buffer, const Rectangle rect, const std::string_view label) {
    buffer.drawText(label, rect, Alignment::TopCenter, cLabelColor);
}


[[nodiscard]] auto makeGlyph(std::initializer_list<std::string_view> rows) -> FontGlyph {
    auto bitmap = Bitmap::fromPattern(rows);
    auto glyph = FontGlyph{bitmap.size()};
    glyph.draw(Position{0, 0}, bitmap);
    return glyph;
}


[[nodiscard]] auto makeRowMaskFont() -> FontPtr {
    auto font = std::make_shared<Font>(5);
    font->addGlyph("A", FontGlyph{std::vector<uint64_t>{0b01110U, 0b10001U, 0b11111U, 0b10001U, 0b10001U}});
    font->addGlyph("M", FontGlyph{std::vector<uint64_t>{0b10001U, 0b11011U, 0b10101U, 0b10001U, 0b10001U}});
    font->addGlyph("O", FontGlyph{std::vector<uint64_t>{0b01110U, 0b10001U, 0b10001U, 0b10001U, 0b01110U}});
    font->addGlyph("T", FontGlyph{std::vector<uint64_t>{0b11111U, 0b00100U, 0b00100U, 0b00100U, 0b00100U}});
    return font;
}


[[nodiscard]] auto makeEmbeddedClockFont() -> FontPtr {
    auto glyphs = Font::GlyphMap{};
    glyphs.emplace(
        "0",
        makeGlyph({
            ".###.",
            "##.##",
            "##.##",
            "##.##",
            "##.##",
            "##.##",
            ".###.",
        }));
    glyphs.emplace(
        "1",
        makeGlyph({
            "..#..",
            ".##..",
            "..#..",
            "..#..",
            "..#..",
            "..#..",
            ".###.",
        }));
    glyphs.emplace(
        "2",
        makeGlyph({
            ".###.",
            "##.##",
            "...##",
            "..##.",
            ".##..",
            "##...",
            "#####",
        }));
    glyphs.emplace(
        "3",
        makeGlyph({
            "####.",
            "...##",
            "...##",
            ".###.",
            "...##",
            "...##",
            "####.",
        }));
    glyphs.emplace(
        ":",
        makeGlyph({
            ".",
            "#",
            ".",
            ".",
            "#",
            ".",
            ".",
        }));
    return std::make_shared<Font>(7, std::move(glyphs));
}


void renderDefaultAsciiExample(Terminal &terminal) {
    printSectionTitle(terminal, "Built-in Font");
    auto buffer = createCanvas(9);

    buffer.drawText(
        "defaultAscii() for large titles", Rectangle{0, 0, cExampleWidth, 1}, Alignment::TopCenter, cLabelColor);
    buffer.drawText(
        "Bitmap fonts still use Text alignment, color sequences, and animation.",
        Rectangle{0, 1, cExampleWidth, 1},
        Alignment::TopCenter,
        cAccentColor);

    auto title = Text{String{"SIGNAL"}, Rectangle{0, 3, cExampleWidth, 4}, Alignment::Center};
    title.setFont(Font::defaultAscii());
    title.setColorSequence(
        ColorSequence{
            Color{fg::BrightBlue, bg::Inherited},
            Color{fg::BrightCyan, bg::Inherited},
            Color{fg::BrightMagenta, bg::Inherited},
            Color{fg::BrightYellow, bg::Inherited},
        });
    title.setAnimation(TextAnimation::ColorDiagonal);
    buffer.drawText(title, 3);

    buffer.drawText(
        "animated headline in a regular Text rectangle",
        Rectangle{0, 7, cExampleWidth, 1},
        Alignment::TopCenter,
        cAccentColor);
    writeBuffer(terminal, buffer);
}


void renderRowMaskFontExample(Terminal &terminal) {
    printSectionTitle(terminal, "Row Mask Font");
    auto buffer = createCanvas(8);

    buffer.drawText("Custom font from row masks", Rectangle{0, 0, cExampleWidth, 1}, Alignment::TopCenter, cLabelColor);
    buffer.drawText(
        "Add just the glyphs you need for logos, badges, or tiny alphabets.",
        Rectangle{0, 1, cExampleWidth, 1},
        Alignment::TopCenter,
        cAccentColor);

    auto text = Text{String{"ATOM"}, Rectangle{0, 3, cExampleWidth, 3}, Alignment::Center};
    text.setFont(makeRowMaskFont());
    text.setColor(Color{fg::BrightGreen, bg::Inherited});
    buffer.drawText(text);

    buffer.drawText(
        "5 bitmap rows become 3 terminal rows", Rectangle{0, 6, cExampleWidth, 1}, Alignment::TopCenter, cAccentColor);
    writeBuffer(terminal, buffer);
}


void renderEmbeddedFontExample(Terminal &terminal) {
    printSectionTitle(terminal, "Embedded Font");
    auto buffer = createCanvas(10);

    buffer.drawFilledFrame(
        Rectangle{10, 1, 52, 8},
        FrameStyle::Double,
        Char{" ", Color{fg::Inherited, bg::Blue}},
        Color{fg::BrightWhite, bg::Inherited});
    buffer.drawText(
        "Embedded font preset", Rectangle{12, 1, 48, 1}, Alignment::Center, Color{fg::BrightWhite, bg::Blue});

    auto clock = Text{String{"12:30"}, Rectangle{12, 3, 48, 4}, Alignment::Center};
    clock.setFont(makeEmbeddedClockFont());
    clock.setColor(Color{fg::BrightYellow, bg::Inherited});
    buffer.drawText(clock);

    buffer.drawText(
        "Build one GlyphMap and reuse the FontPtr.",
        Rectangle{12, 7, 48, 1},
        Alignment::Center,
        Color{fg::BrightWhite, bg::Blue});
    writeBuffer(terminal, buffer);
}


void renderTextOptionsReuseExample(Terminal &terminal) {
    printSectionTitle(terminal, "TextOptions Reuse");
    auto buffer = createCanvas(9);

    auto options = TextOptions{Alignment::Center};
    options.setFont(makeRowMaskFont());
    options.setColorSequence(
        ColorSequence{
            Color{fg::BrightYellow, bg::Inherited},
            Color{fg::BrightRed, bg::Inherited},
            Color{fg::BrightMagenta, bg::Inherited},
        });
    options.setAnimation(TextAnimation::ColorDiagonal);

    buffer.drawFilledFrame(Rectangle{2, 1, 32, 7}, FrameStyle::LightWithRoundedCorners, Char{" ", bg::BrightBlack});
    buffer.drawFilledFrame(Rectangle{38, 1, 32, 7}, FrameStyle::LightWithRoundedCorners, Char{" ", bg::BrightBlack});
    buffer.drawText(
        "Reuse one TextOptions preset", Rectangle{0, 0, cExampleWidth, 1}, Alignment::TopCenter, cLabelColor);

    auto left = Text{String{"ATOM"}, Rectangle{4, 3, 28, 3}, Alignment::Center};
    left.setTextOptions(options);
    auto right = Text{String{"TOMATO"}, Rectangle{40, 3, 28, 3}, Alignment::Center};
    right.setTextOptions(options);

    buffer.drawText(left, 1);
    buffer.drawText(right, 6);
    buffer.drawText(
        "one font preset, multiple text blocks", Rectangle{0, 8, cExampleWidth, 1}, Alignment::TopCenter, cAccentColor);
    writeBuffer(terminal, buffer);
}


void renderAllExamples() {
    auto terminal = Terminal{Size{cExampleWidth, 80}};
    terminal.setSizeDetectionEnabled(false);

    renderDefaultAsciiExample(terminal);
    renderRowMaskFontExample(terminal);
    renderEmbeddedFontExample(terminal);
    renderTextOptionsReuseExample(terminal);

    terminal.setDefaultColor();
    terminal.flush();
}

}


auto main() -> int {
    erbsland::doctools::font_reference::renderAllExamples();
    return 0;
}
