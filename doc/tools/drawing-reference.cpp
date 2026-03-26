// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/cterm/all.hpp>


using namespace erbsland::cterm;


namespace erbsland::doctools::drawing_reference {

constexpr auto cExampleWidth = Coordinate{70};
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


void drawLabel(Buffer &buffer, const Rectangle rect, const std::string_view label) {
    buffer.drawText(label, rect, Alignment::TopCenter, cLabelColor);
}


void drawInsetPanel(
    Buffer &buffer, const Rectangle rect, const FrameStyle style, const Color frameColor, const Color fillColor) {
    buffer.drawFilledFrame(rect, style, Char{" ", fillColor}, frameColor);
    buffer.drawText("panel", rect.insetBy(Margins{1}), Alignment::Center, Color{fg::BrightWhite, bg::Inherited});
}


void renderFrameStyleGallery(Terminal &terminal) {
    printSectionTitle(terminal, "FrameStyle Gallery");
    auto buffer = createCanvas(14);

    buffer.drawText(
        "Built-in FrameStyle presets", Rectangle{0, 0, cExampleWidth, 1}, Alignment::TopCenter, cLabelColor);
    buffer.drawText(
        "Choose a built-in style without changing your layout code.",
        Rectangle{0, 1, cExampleWidth, 1},
        Alignment::TopCenter,
        cAccentColor);

    constexpr auto cPanelWidth = Coordinate{20};
    constexpr auto cPanelHeight = Coordinate{4};
    const auto labels =
        std::array<std::string_view, 6>{"Light", "Rounded", "Heavy", "Double", "OuterHalf", "FullBlock"};
    const auto styles = std::array<FrameStyle, 6>{
        FrameStyle::Light,
        FrameStyle::LightWithRoundedCorners,
        FrameStyle::Heavy,
        FrameStyle::Double,
        FrameStyle::OuterHalfBlock,
        FrameStyle::FullBlock,
    };
    const auto frameColors = std::array<Color, 6>{
        Color{fg::BrightCyan, bg::Inherited},
        Color{fg::BrightGreen, bg::Inherited},
        Color{fg::BrightYellow, bg::Inherited},
        Color{fg::BrightMagenta, bg::Inherited},
        Color{fg::BrightBlue, bg::Inherited},
        Color{fg::BrightWhite, bg::Inherited},
    };
    const auto fillColors = std::array<Color, 6>{
        Color{fg::Inherited, bg::Blue},
        Color{fg::Inherited, bg::Green},
        Color{fg::Inherited, bg::Red},
        Color{fg::Inherited, bg::Magenta},
        Color{fg::Inherited, bg::Cyan},
        Color{fg::Inherited, bg::BrightBlack},
    };

    for (auto index = std::size_t{0}; index < labels.size(); ++index) {
        const auto column = static_cast<Coordinate>(index % 3);
        const auto row = static_cast<Coordinate>(index / 3);
        const auto x = 2 + column * 23;
        const auto labelRect = Rectangle{x, 3 + row * 5, cPanelWidth, 1};
        const auto panelRect = Rectangle{x, 4 + row * 5, cPanelWidth, cPanelHeight};
        drawLabel(buffer, labelRect, labels.at(index));
        drawInsetPanel(buffer, panelRect, styles.at(index), frameColors.at(index), fillColors.at(index));
    }
    writeBuffer(terminal, buffer);
}


void drawOverlap(Buffer &buffer, const Coordinate xOffset, const CharCombinationStylePtr &combinationStyle) {
    const auto style = Char16Style::lightFrame();
    buffer.drawFrame(Rectangle{xOffset + 2, 2, 17, 6}, style, combinationStyle, Color{fg::BrightCyan, bg::Inherited});
    buffer.drawFrame(Rectangle{xOffset + 8, 4, 18, 6}, style, combinationStyle, Color{fg::BrightYellow, bg::Inherited});
}


void renderCombinationExamples(Terminal &terminal) {
    printSectionTitle(terminal, "CharCombinationStyle");
    auto buffer = createCanvas(11);

    drawLabel(buffer, Rectangle{0, 0, 35, 1}, "overwrite()");
    drawLabel(buffer, Rectangle{35, 0, 35, 1}, "commonBoxFrame()");
    buffer.drawText(
        "Later frames simply replace earlier glyphs.", Rectangle{0, 1, 35, 1}, Alignment::TopCenter, cAccentColor);
    buffer.drawText(
        "Intersections become matching box-drawing glyphs.",
        Rectangle{35, 1, 35, 1},
        Alignment::TopCenter,
        cAccentColor);

    drawOverlap(buffer, 0, CharCombinationStyle::overwrite());
    drawOverlap(buffer, 35, CharCombinationStyle::commonBoxFrame());

    writeBuffer(terminal, buffer);
}


void renderTile9Examples(Terminal &terminal) {
    printSectionTitle(terminal, "Tile9Style");
    auto buffer = createCanvas(12);

    drawLabel(buffer, Rectangle{0, 0, 35, 1}, "9 tiles repeat edges and center");
    drawLabel(buffer, Rectangle{35, 0, 35, 1}, "16 tiles cover thin edge cases");

    const auto decorativeStyle = Tile9Style::create("╔═╗║·║╚═╝");
    buffer.fill(Rectangle{3, 2, 28, 7}, decorativeStyle, Color{fg::BrightCyan, bg::Inherited});
    buffer.drawText("Repeat", Rectangle{3, 4, 28, 2}, Alignment::Center, Color{fg::BrightWhite, bg::Inherited});

    const auto degenerateStyle = Tile9Style::create("ABCDEFGHIJKLMNOP");
    buffer.fill(Rectangle{39, 2, 10, 5}, degenerateStyle, Color{fg::BrightYellow, bg::Inherited});
    buffer.fill(Rectangle{52, 2, 10, 1}, degenerateStyle, Color{fg::BrightYellow, bg::Inherited});
    buffer.fill(Rectangle{52, 4, 1, 5}, degenerateStyle, Color{fg::BrightYellow, bg::Inherited});
    buffer.fill(Rectangle{57, 4, 1, 1}, degenerateStyle, Color{fg::BrightYellow, bg::Inherited});

    buffer.drawText("normal", Rectangle{39, 8, 10, 1}, Alignment::TopCenter, cAccentColor);
    buffer.drawText("row", Rectangle{52, 1, 10, 1}, Alignment::TopCenter, cAccentColor);
    buffer.drawText("column", Rectangle{48, 9, 10, 1}, Alignment::TopCenter, cAccentColor);
    buffer.drawText("cell", Rectangle{59, 4, 6, 1}, Alignment::TopLeft, cAccentColor);

    writeBuffer(terminal, buffer);
}


void drawOptionsPanel(
    Buffer &buffer,
    const Rectangle rect,
    const std::string_view label,
    const FrameDrawOptions &options,
    const std::size_t animationCycle) {

    drawLabel(buffer, Rectangle{rect.x1(), rect.y1(), rect.width(), 1}, label);
    buffer.drawFrame(rect.insetBy(Margins{1, 0, 0, 0}), options, animationCycle);
}


void renderFrameDrawOptionsExamples(Terminal &terminal) {
    printSectionTitle(terminal, "FrameDrawOptions");
    auto buffer = createCanvas(11);

    auto staticOptions = FrameDrawOptions{};
    staticOptions.setStyle(FrameStyle::LightWithRoundedCorners);
    staticOptions.setFrameColor(Color{fg::BrightCyan, bg::Inherited});
    staticOptions.setFillBlock(Char{" ", Color{fg::Inherited, bg::Blue}});
    staticOptions.setFillColor(Color{fg::Inherited, bg::Blue});

    auto stripeOptions = FrameDrawOptions{};
    stripeOptions.setStyle(FrameStyle::Double);
    stripeOptions.setFillBlock(Char{" ", Color{fg::Inherited, bg::Inherited}});
    stripeOptions.setFrameColorSequence(
        ColorSequence{
            Color{fg::BrightYellow, bg::Inherited},
            Color{fg::BrightMagenta, bg::Inherited},
            Color{fg::BrightCyan, bg::Inherited},
        },
        FrameColorMode::VerticalStripes);
    stripeOptions.setFillColorSequence(
        ColorSequence{
            Color{fg::Inherited, bg::Blue},
            Color{fg::Inherited, bg::Magenta},
        },
        FrameColorMode::HorizontalStripes);

    auto chasingOptions = FrameDrawOptions{};
    chasingOptions.setStyle(FrameStyle::Heavy);
    chasingOptions.setFillBlock(Char{"·", Color{fg::BrightBlack, bg::Inherited}});
    chasingOptions.setFrameColorSequence(
        ColorSequence{
            Color{fg::BrightRed, bg::Inherited},
            Color{fg::BrightYellow, bg::Inherited},
            Color{fg::BrightGreen, bg::Inherited},
            Color{fg::BrightCyan, bg::Inherited},
        },
        FrameColorMode::ChasingBorderCW);
    chasingOptions.setFillColor(Color{fg::Inherited, bg::BrightBlack});

    drawOptionsPanel(buffer, Rectangle{1, 0, 22, 9}, "Static fill", staticOptions, 0);
    drawOptionsPanel(buffer, Rectangle{24, 0, 22, 9}, "Striped sequences", stripeOptions, 0);
    drawOptionsPanel(buffer, Rectangle{47, 0, 22, 9}, "Chasing border", chasingOptions, 3);

    buffer.drawText("fixed colors", Rectangle{1, 9, 22, 1}, Alignment::TopCenter, cAccentColor);
    buffer.drawText("stripe modes", Rectangle{24, 9, 22, 1}, Alignment::TopCenter, cAccentColor);
    buffer.drawText("cycle = 3", Rectangle{47, 9, 22, 1}, Alignment::TopCenter, cAccentColor);

    writeBuffer(terminal, buffer);
}


void renderAllExamples() {
    auto terminal = Terminal{Size{cExampleWidth, 96}};
    terminal.setSizeDetectionEnabled(false);

    renderFrameStyleGallery(terminal);
    renderCombinationExamples(terminal);
    renderTile9Examples(terminal);
    renderFrameDrawOptionsExamples(terminal);

    terminal.setDefaultColor();
    terminal.flush();
}

}


auto main() -> int {
    erbsland::doctools::drawing_reference::renderAllExamples();
    return 0;
}
