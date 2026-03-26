// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "KeyInputDemoApp.hpp"

#include "ScopedTerminalSession.hpp"

#include <algorithm>
#include <format>


namespace demo::keyinputdemo {


void KeyInputDemoApp::run() {
    _updateSettings.setMinimumSize(Size{56, 10});
    _updateSettings.setMinimumSizeBackground(backgroundChar());
    _updateSettings.setMinimumSizeMessage(
        String{
            "Resize the terminal to at least 56x10 cells for the key input demo.", Color{fg::BrightWhite, bg::Black}});
    initializeScrollBuffer();
    auto session = ScopedTerminalSession{_terminal, Terminal::RefreshMode::Overwrite, Input::Mode::Key};
    renderFrame();
    while (!_quitRequested) {
        _terminal.testScreenSize();
        if (const auto key = _terminal.input().read(cScrollDelay); key.valid()) {
            handleKey(key);
        }
        if (_quitRequested) {
            break;
        }
        advanceScroll();
        renderFrame();
    }
}


auto KeyInputDemoApp::canvasSize() const noexcept -> Size {
    return _terminal.size();
}


auto KeyInputDemoApp::fieldRectForCanvas(const Size canvasSize) const noexcept -> Rectangle {
    const auto availableHeight = std::max(1, canvasSize.height() - 4);
    const auto fieldSize = Size{
        std::min(canvasSize.width(), cScrollBufferSize.width()),
        std::min(availableHeight, cScrollBufferSize.height()),
    };
    const auto x = std::max(0, (canvasSize.width() - fieldSize.width()) / 2);
    return {x, 2, fieldSize.width(), fieldSize.height()};
}


auto KeyInputDemoApp::visibleFieldSize() const noexcept -> Size {
    return fieldRectForCanvas(canvasSize()).size();
}


void KeyInputDemoApp::initializeScrollBuffer() noexcept {
    _scrollBuffer.fill(backgroundChar());
    for (auto column = 0; column < cScrollBufferSize.width(); column += 20) {
        _scrollBuffer.fill(Rectangle{column, 0, 1, cScrollBufferSize.height()}, guideColumnChar());
    }
    _insertedColumnCount = cScrollBufferSize.width();
}


void KeyInputDemoApp::advanceScroll() noexcept {
    const auto fillChar = (_insertedColumnCount % 20 == 0) ? guideColumnChar() : backgroundChar();
    _scrollBuffer.shift(Direction::East, fillChar, 1);
    _insertedColumnCount += 1;
}


void KeyInputDemoApp::handleKey(const Key &key) noexcept {
    if (key == Key{Key::Escape}) {
        _quitRequested = true;
        return;
    }
    stampKeyBlock(key);
}


void KeyInputDemoApp::stampKeyBlock(const Key &key) noexcept {
    const auto fieldSize = visibleFieldSize();
    if (fieldSize.width() <= 0 || fieldSize.height() <= 0) {
        return;
    }
    const auto colorIndex = static_cast<std::size_t>(
        std::uniform_int_distribution<int>{0, static_cast<int>(stampColors().size()) - 1}(_random));
    auto block = String{key.toDisplayText(), stampColors()[colorIndex]};
    const auto maximumX = std::min(20, std::max(0, fieldSize.width() - block.displayWidth()));
    const auto x = std::uniform_int_distribution<int>{0, maximumX}(_random);
    const auto y = std::uniform_int_distribution<int>{0, fieldSize.height() - 1}(_random);
    _scrollBuffer.drawText(Position{x, y}, block);
}


void KeyInputDemoApp::renderFrame() {
    const auto visibleCanvas = canvasSize();
    _buffer.resize(visibleCanvas.componentMax(_updateSettings.minimumSize()));
    _buffer.fill(backgroundChar());

    const auto headerRect = Rectangle{0, 0, _buffer.size().width(), 1};
    const auto fieldRect = fieldRectForCanvas(visibleCanvas.componentMax(_updateSettings.minimumSize()));
    const auto footerRect = Rectangle{0, _buffer.size().height() - 1, _buffer.size().width(), 1};

    drawHeader(headerRect);
    drawField(fieldRect);
    drawFooter(footerRect);
    _terminal.updateScreen(_buffer, _updateSettings);
}


void KeyInputDemoApp::drawHeader(const Rectangle rect) {
    _buffer.fill(rect, Char{" ", Color{fg::BrightWhite, bg::BrightBlack}});
    _buffer.drawText(
        "Key Input Demo", rect.insetBy(Margins{2, 0}), Alignment::CenterLeft, Color{fg::BrightWhite, bg::BrightBlack});
    _buffer.drawText(
        std::format("horizontal RemappedBuffer {}x{}", cScrollBufferSize.width(), cScrollBufferSize.height()),
        rect.insetBy(Margins{2, 0}),
        Alignment::CenterRight,
        Color{fg::BrightCyan, bg::BrightBlack});
}


void KeyInputDemoApp::drawField(const Rectangle rect) {
    _buffer.fill(rect, backgroundChar());
    auto view = BufferConstRefView{_scrollBuffer, rect.size()};
    _buffer.drawBuffer(view, rect);
}


void KeyInputDemoApp::drawFooter(const Rectangle rect) {
    _buffer.fill(rect, Char{" ", Color{fg::BrightWhite, bg::BrightBlack}});
    _buffer.drawText(Text{footerText(), rect.insetBy(Margins{1, 0}), Alignment::CenterLeft});
}


auto KeyInputDemoApp::footerText() const -> String {
    auto result = String{};
    result.append(
        fg::BrightYellow,
        Key{Key::Escape}.toDisplayText(),
        fg::BrightWhite,
        " quit  ",
        fg::BrightCyan,
        "Press any other key to stamp a colored key block into the scrolling field.");
    return result;
}


auto KeyInputDemoApp::backgroundChar() noexcept -> Char {
    return Char{" ", Color{fg::Default, bg::Black}};
}


auto KeyInputDemoApp::guideColumnChar() noexcept -> Char {
    return Char{U'∙', Color{fg::BrightBlack, bg::Black}};
}


auto KeyInputDemoApp::stampColors() noexcept -> const std::array<Color, 10> & {
    static const auto cStampColors = std::array<Color, 10>{
        Color{fg::Black, bg::BrightYellow},
        Color{fg::Black, bg::BrightRed},
        Color{fg::Black, bg::BrightCyan},
        Color{fg::Black, bg::BrightGreen},
        Color{fg::Black, bg::BrightBlue},
        Color{fg::BrightWhite, bg::Green},
        Color{fg::BrightWhite, bg::Blue},
        Color{fg::BrightWhite, bg::Cyan},
        Color{fg::BrightWhite, bg::Red},
        Color{fg::BrightWhite, bg::Yellow},
    };
    return cStampColors;
}


}
