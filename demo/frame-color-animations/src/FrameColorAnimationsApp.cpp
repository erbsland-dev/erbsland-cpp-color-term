// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "FrameColorAnimationsApp.hpp"

#include "ScopedTerminalSession.hpp"

#include <array>
#include <chrono>
#include <string_view>


namespace demo::framecoloranimations {


void FrameColorAnimationsApp::run() {
    auto session = ScopedTerminalSession{_terminal, Terminal::RefreshMode::Overwrite, Input::Mode::Key};
    while (!_quitRequested) {
        const auto key = _terminal.input().read(std::chrono::milliseconds{90});
        if (key.valid()) {
            handleKey(key);
        }
        renderFrame();
        ++_animationCycle;
    }
}


auto FrameColorAnimationsApp::canvasSize() const noexcept -> Size {
    return _terminal.size() - Size{1, 1};
}


void FrameColorAnimationsApp::handleKey(const Key &key) noexcept {
    if (key == Key{Key::Character, 'q'}) {
        _quitRequested = true;
    }
}


void FrameColorAnimationsApp::renderFrame() {
    _terminal.testScreenSize();
    auto buffer = Buffer{canvasSize()};
    buffer.fill(Char{" ", bg::Black});
    if (buffer.size().width() < 78 || buffer.size().height() < 22) {
        buffer.drawText(
            "Resize the terminal to at least 78x22 cells for the frame color animation demo.",
            Rectangle{0, 0, buffer.size().width(), buffer.size().height()},
            Alignment::Center,
            Color{fg::BrightWhite, bg::Black});
        _terminal.updateScreen(buffer);
        _terminal.flush();
        return;
    }

    const auto outerRect = Rectangle{0, 0, buffer.size().width(), buffer.size().height()};
    const auto headerRect = Rectangle{2, 1, buffer.size().width() - 4, 3};
    const auto contentRect = Rectangle{2, 5, buffer.size().width() - 4, buffer.size().height() - 9};
    const auto footerRect = Rectangle{2, buffer.size().height() - 3, buffer.size().width() - 4, 1};

    auto outerOptions = FrameDrawOptions{};
    outerOptions.setStyle(FrameStyle::LightWithRoundedCorners);
    outerOptions.setFrameColorSequence(outerFrameColors(), FrameColorMode::ChasingBorderCW);
    outerOptions.setFillBlock(Char{" "});
    outerOptions.setFillColorSequence(fillColors(), FrameColorMode::ForwardDiagonalStripes);
    buffer.drawFrame(outerRect, outerOptions, _animationCycle);

    drawHeader(buffer, headerRect);

    const auto cells = contentRect.gridCells(2, 4);
    const auto &panels = panelSpecs();
    for (std::size_t index = 0; index < panels.size(); ++index) {
        drawPanel(buffer, cells[index].insetBy(Margins{1, 0}), panels[index]);
    }
    drawFooter(buffer, footerRect);
    _terminal.updateScreen(buffer);
    _terminal.flush();
}


void FrameColorAnimationsApp::drawPanel(Buffer &buffer, const Rectangle rect, const PanelSpec &panel) const {
    auto options = FrameDrawOptions{};
    options.setStyle(panel.style);
    options.setFrameColorSequence(colorSequence(panel.sequenceIndex), panel.mode);
    options.setFillBlock(Char{" "});
    options.setFillColorSequence(ColorSequence{Color{fg::Inherited, bg::Black}, 1}, FrameColorMode::OneColor);

    if (panel.mode == FrameColorMode::ForwardDiagonalStripes || panel.mode == FrameColorMode::BackwardDiagonalStripes) {
        buffer.drawFrame(rect, options, _animationCycle * 6);
    } else {
        buffer.drawFrame(rect, options, _animationCycle);
    }
    buffer.drawText(
        panel.title,
        Rectangle{rect.x1() + 2, rect.y1() + 1, rect.width() - 4, 1},
        Alignment::Center,
        Color{fg::BrightWhite, bg::Inherited});
    buffer.drawText(
        "animated frame",
        Rectangle{rect.x1() + 2, rect.y1() + 3, rect.width() - 4, 1},
        Alignment::Center,
        Color{fg::BrightBlack, bg::Inherited});
}


void FrameColorAnimationsApp::drawHeader(Buffer &buffer, const Rectangle rect) const {
    buffer.drawText(
        "Frame Color Animations",
        Rectangle{rect.x1(), rect.y1(), rect.width(), 1},
        Alignment::Center,
        Color{fg::BrightWhite, bg::Black});
    buffer.drawText(
        "This is a demo of various frame animation modes.",
        Rectangle{rect.x1(), rect.y1() + 2, rect.width(), 1},
        Alignment::Center,
        Color{fg::BrightYellow, bg::Black});
}


void FrameColorAnimationsApp::drawFooter(Buffer &buffer, const Rectangle rect) const {
    buffer.drawText("Press q to quit.", rect, Alignment::CenterRight, Color{fg::BrightBlack, bg::Inherited});
}


auto FrameColorAnimationsApp::panelSpecs() -> const std::array<PanelSpec, 7> & {
    static const auto cPanels = std::array<PanelSpec, 7>{
        PanelSpec{"OneColor", FrameStyle::LightWithRoundedCorners, FrameColorMode::OneColor, 6},
        PanelSpec{"Vertical Stripes", FrameStyle::Light, FrameColorMode::VerticalStripes, 1},
        PanelSpec{"Horizontal Stripes", FrameStyle::Heavy, FrameColorMode::HorizontalStripes, 3},
        PanelSpec{"Forward Diagonal", FrameStyle::Double, FrameColorMode::ForwardDiagonalStripes, 0},
        PanelSpec{"Backward Diagonal", FrameStyle::OuterHalfBlock, FrameColorMode::BackwardDiagonalStripes, 4},
        PanelSpec{"Chasing Border CW", FrameStyle::Heavy, FrameColorMode::ChasingBorderCW, 5},
        PanelSpec{"Chasing Border CCW", FrameStyle::LightWithRoundedCorners, FrameColorMode::ChasingBorderCCW, 2},
    };
    return cPanels;
}


auto FrameColorAnimationsApp::colorSequence(const std::size_t index) -> const ColorSequence & {
    static const auto cSequences = std::array<ColorSequence, 7>{
        ColorSequence{
            {Color{fg::BrightWhite, bg::Blue}, 2},
            {Color{fg::BrightWhite, bg::Black}, 4},
            {Color{fg::BrightCyan, bg::Black}, 6},
            {Color{fg::BrightBlue, bg::Black}, 80},
        },
        ColorSequence{
            {Color{fg::BrightCyan, bg::Black}, 1},
            {Color{fg::BrightWhite, bg::Black}, 2},
            {Color{fg::BrightCyan, bg::Black}, 1},
            {Color{fg::Cyan, bg::Black}, 4},
        },
        ColorSequence{
            {Color{fg::BrightGreen, bg::Black}, 1},
            {Color{fg::BrightWhite, bg::Black}, 2},
            {Color{fg::BrightGreen, bg::Black}, 1},
            {Color{fg::Green, bg::Black}, 4},
        },
        ColorSequence{
            {Color{fg::BrightYellow, bg::Black}, 2},
            {Color{fg::BrightWhite, bg::Black}, 3},
            {Color{fg::BrightGreen, bg::Black}, 10},
            {Color{fg::Green, bg::Black}, 84},
        },
        ColorSequence{
            {Color{fg::Yellow, bg::Black}, 2},
            {Color{fg::BrightWhite, bg::Black}, 3},
            {Color{fg::Yellow, bg::Black}, 10},
            {Color{fg::Red, bg::Black}, 84},
        },
        ColorSequence{
            {Color{fg::BrightWhite, bg::Black}, 2},
            {Color{fg::BrightYellow, bg::Black}, 4},
            {Color{fg::Yellow, bg::Black}, 10},
            {Color{fg::BrightYellow, bg::Black}, 84},
        },
        ColorSequence{
            {Color{fg::BrightMagenta, bg::Black}, 1},
            {Color{fg::BrightWhite, bg::Black}, 2},
            {Color{fg::BrightMagenta, bg::Black}, 1},
            {Color{fg::Magenta, bg::Black}, 4},
        },
    };
    return cSequences[index % cSequences.size()];
}


auto FrameColorAnimationsApp::outerFrameColors() -> const ColorSequence & {
    return colorSequence(2);
}


auto FrameColorAnimationsApp::fillColors() -> const ColorSequence & {
    static const auto cFillColors = ColorSequence{
        {Color{fg::Inherited, bg::Black}, 15},
        {Color{fg::Inherited, bg::BrightBlack}, 1},
    };
    return cFillColors;
}


}
