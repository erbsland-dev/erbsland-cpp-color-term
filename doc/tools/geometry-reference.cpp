// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/cterm/all.hpp>


using namespace erbsland::cterm;


namespace erbsland::doctools::geometry_reference {

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


void drawNamedRegion(
    Buffer &buffer,
    const Rectangle rect,
    const std::string_view label,
    const Color fillColor,
    const Color frameColor = Color{fg::BrightWhite, bg::Inherited}) {

    buffer.drawFilledFrame(rect, FrameStyle::LightWithRoundedCorners, Char{" ", fillColor}, frameColor);
    buffer.drawText(label, rect, Alignment::Center, Color{fg::BrightWhite, bg::Inherited});
}


[[nodiscard]] auto digit(const int value, const Color color = Color{}) -> Char {
    return Char{static_cast<char32_t>(U'0' + static_cast<char32_t>(value % 10)), color};
}


void renderLayoutExample(Terminal &terminal) {
    printSectionTitle(terminal, "Derived Layout");
    auto buffer = createCanvas(12);

    buffer.drawText(
        "subRectangle() + insetBy() keep layouts explicit",
        Rectangle{0, 0, cExampleWidth, 1},
        Alignment::TopCenter,
        cLabelColor);
    buffer.drawText(
        "One canvas can derive headers, sidebars, content, and footers cleanly.",
        Rectangle{0, 1, cExampleWidth, 1},
        Alignment::TopCenter,
        cAccentColor);

    const auto canvas = Rectangle{2, 3, 68, 8};
    const auto header = canvas.subRectangle(Anchor::TopCenter, Size{0, 2}, Margins{0, 1, 0, 1});
    const auto footer = canvas.subRectangle(Anchor::BottomCenter, Size{0, 1}, Margins{0, 1, 0, 1});
    const auto body = canvas.insetBy(Margins{2, 1, 1, 1});
    const auto sidebar = body.subRectangle(Anchor::Left, Size{18, 0}, Margins{0, 1, 0, 0});
    const auto content = body.subRectangle(Anchor::Right, Size{body.width() - 19, 0}, Margins{0});

    buffer.drawFrame(canvas, FrameStyle::Double, Color{fg::BrightWhite, bg::Inherited});
    buffer.drawText(
        "canvas", Rectangle{canvas.x1() + 1, canvas.y1(), canvas.width() - 2, 1}, Alignment::Center, cAccentColor);
    drawNamedRegion(buffer, header, "header", Color{fg::Inherited, bg::Blue}, Color{fg::BrightCyan, bg::Inherited});
    drawNamedRegion(buffer, sidebar, "sidebar", Color{fg::Inherited, bg::Green}, Color{fg::BrightGreen, bg::Inherited});
    drawNamedRegion(
        buffer, content, "content", Color{fg::Inherited, bg::Magenta}, Color{fg::BrightMagenta, bg::Inherited});
    drawNamedRegion(
        buffer, footer, "footer", Color{fg::Inherited, bg::BrightBlack}, Color{fg::BrightWhite, bg::Inherited});

    writeBuffer(terminal, buffer);
}


void drawOverlapPanel(
    Buffer &buffer,
    const Rectangle rect,
    const std::string_view title,
    const Rectangle a,
    const Rectangle b,
    const bool showUnion) {

    drawCenteredLabel(buffer, Rectangle{rect.x1(), rect.y1(), rect.width(), 1}, title);
    const auto leftRect = rect.insetBy(Margins{1, 1, 0, 1});
    buffer.drawFrame(a, FrameStyle::Light, Color{fg::BrightCyan, bg::Inherited});
    buffer.drawFrame(b, FrameStyle::Light, Color{fg::BrightYellow, bg::Inherited});
    buffer.drawText(
        "A", Rectangle{a.x1(), a.y1() + 1, a.width(), 1}, Alignment::Center, Color{fg::BrightCyan, bg::Inherited});
    buffer.drawText(
        "B", Rectangle{b.x1(), b.y1() + 1, b.width(), 1}, Alignment::Center, Color{fg::BrightYellow, bg::Inherited});
    if (showUnion) {
        buffer.drawFrame(a | b, FrameStyle::Heavy, Color{fg::BrightMagenta, bg::Inherited});
        buffer.drawText(
            "union",
            Rectangle{leftRect.x1(), leftRect.y2() - 1, leftRect.width(), 1},
            Alignment::TopCenter,
            cAccentColor);
    } else {
        buffer.fill(a & b, Char{" ", Color{fg::Inherited, bg::BrightBlack}});
        buffer.drawFrame(a & b, FrameStyle::Double, Color{fg::BrightGreen, bg::Inherited});
        buffer.drawText(
            "intersection",
            Rectangle{leftRect.x1(), leftRect.y2() - 1, leftRect.width(), 1},
            Alignment::TopCenter,
            cAccentColor);
    }
}


void renderCombineExample(Terminal &terminal) {
    printSectionTitle(terminal, "Combining and Clipping");
    auto buffer = createCanvas(11);

    const auto panelLeft = Rectangle{0, 0, 36, 11};
    const auto panelRight = Rectangle{36, 0, 36, 11};
    const auto rectA = Rectangle{4, 2, 13, 5};
    const auto rectB = Rectangle{11, 4, 16, 5};
    const auto rectA2 = Rectangle{40, 2, 13, 5};
    const auto rectB2 = Rectangle{47, 4, 16, 5};

    drawOverlapPanel(buffer, panelLeft, "A | B", rectA, rectB, true);
    drawOverlapPanel(buffer, panelRight, "A & B", rectA2, rectB2, false);

    writeBuffer(terminal, buffer);
}


void drawAlignedBox(Buffer &buffer, const Rectangle target, const Alignment alignment, const std::string_view label) {
    buffer.drawFrame(target, FrameStyle::Light, Color{fg::BrightWhite, bg::Inherited});
    const auto sourceRect = Rectangle{target.alignmentOffset(Size{8, 3}, alignment), Size{8, 3}};
    buffer.drawFilledFrame(
        sourceRect,
        FrameStyle::Double,
        Char{" ", Color{fg::Inherited, bg::Blue}},
        Color{fg::BrightCyan, bg::Inherited});
    buffer.drawText("8x3", sourceRect, Alignment::Center, Color{fg::BrightWhite, bg::Inherited});
    drawCenteredLabel(buffer, Rectangle{target.x1(), target.y2(), target.width(), 1}, label, cAccentColor);
}


void drawCroppedSource(Buffer &buffer, const Rectangle target) {
    buffer.drawFrame(target, FrameStyle::Double, Color{fg::BrightWhite, bg::Inherited});
    const auto aligned = target.insetBy(Margins{1}).alignedSource(Rectangle{0, 0, 18, 5}, Alignment::Center);
    aligned.targetRect.forEach([&](const Position pos) -> void {
        const auto sourceX = aligned.sourceRect.x1() + (pos.x() - aligned.targetRect.x1());
        const auto sourceY = aligned.sourceRect.y1() + (pos.y() - aligned.targetRect.y1());
        const auto code = (sourceY * 2 + sourceX) % 10;
        buffer.set(pos, digit(code, Color{fg::BrightYellow, bg::BrightBlack}));
    });
    buffer.drawText(
        "center crop of 18x5 source",
        Rectangle{target.x1(), target.y2(), target.width(), 1},
        Alignment::TopCenter,
        cAccentColor);
}


void renderAlignmentExample(Terminal &terminal) {
    printSectionTitle(terminal, "Alignment and Cropping");
    auto buffer = createCanvas(14);

    buffer.drawText(
        "alignmentOffset() places smaller content; alignedSource() crops larger content.",
        Rectangle{0, 0, cExampleWidth, 1},
        Alignment::TopCenter,
        cLabelColor);
    drawAlignedBox(buffer, Rectangle{2, 2, 20, 5}, Alignment::TopLeft, "TopLeft");
    drawAlignedBox(buffer, Rectangle{26, 2, 20, 5}, Alignment::Center, "Center");
    drawAlignedBox(buffer, Rectangle{50, 2, 20, 5}, Alignment::BottomRight, "BottomRight");
    drawCroppedSource(buffer, Rectangle{16, 8, 40, 5});

    writeBuffer(terminal, buffer);
}


void renderGridExample(Terminal &terminal) {
    printSectionTitle(terminal, "Grid Cells");
    auto buffer = createCanvas(12);

    buffer.drawText(
        "gridCells() distributes remainder to the top-left cells and keeps row-major order.",
        Rectangle{0, 0, cExampleWidth, 1},
        Alignment::TopCenter,
        cLabelColor);

    const auto grid = Rectangle{2, 2, 68, 8};
    const auto cells = grid.gridCells(2, 3, 2, 1);
    const auto fillColors = std::array<Color, 6>{
        Color{fg::Inherited, bg::Blue},
        Color{fg::Inherited, bg::Cyan},
        Color{fg::Inherited, bg::Magenta},
        Color{fg::Inherited, bg::Green},
        Color{fg::Inherited, bg::Red},
        Color{fg::Inherited, bg::BrightBlack},
    };
    for (std::size_t index = 0; index < cells.size(); ++index) {
        const auto &cell = cells[index];
        buffer.drawFilledFrame(
            cell, FrameStyle::Light, Char{" ", fillColors.at(index)}, Color{fg::BrightWhite, bg::Inherited});
        buffer.drawText(
            std::format("#{}  {}x{}", index, cell.width(), cell.height()),
            cell,
            Alignment::Center,
            Color{fg::BrightWhite, bg::Inherited});
    }

    writeBuffer(terminal, buffer);
}


void renderTraversalExample(Terminal &terminal) {
    printSectionTitle(terminal, "Neighbors and Border Order");
    auto buffer = createCanvas(11);

    drawCenteredLabel(buffer, Rectangle{0, 0, 34, 1}, "Position::ringEight()");
    drawCenteredLabel(buffer, Rectangle{38, 0, 34, 1}, "Rectangle::forEachInFrame()");

    const auto center = Position{12, 5};
    const auto ring = center.ringEight();
    for (std::size_t index = 0; index < ring.size(); ++index) {
        buffer.set(ring.at(index), digit(static_cast<int>(index), Color{fg::BrightCyan, bg::Inherited}));
    }
    for (const auto pos : center.cardinalFour()) {
        buffer.set(pos, Char{U'+', Color{fg::BrightYellow, bg::Inherited}});
    }
    buffer.set(center, Char{U'X', Color{fg::BrightWhite, bg::Inherited}});
    buffer.drawText("0..7 clockwise around X", Rectangle{0, 9, 34, 1}, Alignment::TopCenter, cAccentColor);

    const auto perimeter = Rectangle{46, 2, 18, 7};
    buffer.drawFrame(perimeter, FrameStyle::Double, Color{fg::BrightMagenta, bg::Inherited});
    perimeter.forEachInFrame([&](const Position pos, const int index) -> void {
        buffer.set(pos, digit(index, Color{fg::BrightWhite, bg::Inherited}));
    });
    buffer.drawText("index order around frame", Rectangle{38, 9, 34, 1}, Alignment::TopCenter, cAccentColor);

    writeBuffer(terminal, buffer);
}


void renderBoundsAndClampExample(Terminal &terminal) {
    printSectionTitle(terminal, "Bounds and Clamping");
    auto buffer = createCanvas(11);

    drawCenteredLabel(buffer, Rectangle{0, 0, 34, 1}, "Rectangle::bounds()");
    drawCenteredLabel(buffer, Rectangle{38, 0, 34, 1}, "Rectangle::clamp()");

    const auto points = PositionList{
        Position{5, 5},
        Position{9, 3},
        Position{13, 6},
        Position{17, 4},
        Position{11, 7},
    };
    const auto pointBounds = Rectangle::bounds(points);
    buffer.drawFrame(pointBounds.expandedBy(Margins{1}), FrameStyle::Double, Color{fg::BrightMagenta, bg::Inherited});
    for (const auto pos : points) {
        buffer.set(pos, Char{U'•', Color{fg::BrightYellow, bg::Inherited}});
    }
    buffer.drawText("derive one enclosing rectangle", Rectangle{0, 9, 34, 1}, Alignment::TopCenter, cAccentColor);

    const auto viewport = Rectangle{46, 2, 16, 6};
    const auto rawCursor = Position{66, 8};
    const auto clampedCursor = viewport.clamp(rawCursor);
    buffer.drawFrame(viewport, FrameStyle::Light, Color{fg::BrightCyan, bg::Inherited});
    buffer.drawText(
        "viewport", Rectangle{viewport.x1(), viewport.y1(), viewport.width(), 1}, Alignment::TopCenter, cAccentColor);
    buffer.set(rawCursor, Char{U'X', Color{fg::BrightRed, bg::Inherited}});
    buffer.set(clampedCursor, Char{U'@', Color{fg::BrightGreen, bg::Inherited}});
    buffer.drawText("raw", Rectangle{61, 8, 5, 1}, Alignment::TopCenter, Color{fg::BrightRed, bg::Inherited});
    buffer.drawText("clamped to nearest edge", Rectangle{38, 9, 34, 1}, Alignment::TopCenter, cAccentColor);

    writeBuffer(terminal, buffer);
}


void renderAllExamples() {
    auto terminal = Terminal{Size{cExampleWidth, 96}};
    terminal.setSizeDetectionEnabled(false);

    renderLayoutExample(terminal);
    renderCombineExample(terminal);
    renderAlignmentExample(terminal);
    renderGridExample(terminal);
    renderTraversalExample(terminal);
    renderBoundsAndClampExample(terminal);

    terminal.setDefaultColor();
    terminal.flush();
}

}


auto main() -> int {
    erbsland::doctools::geometry_reference::renderAllExamples();
    return 0;
}
