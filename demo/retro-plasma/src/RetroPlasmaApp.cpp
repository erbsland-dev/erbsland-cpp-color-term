// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "RetroPlasmaApp.hpp"

#include <algorithm>
#include <string>

namespace demo::retroplasma {

void RetroPlasmaApp::beforeInitialize() {
    _updateSettings.setMinimumSize(Size{28, 8});
    _updateSettings.setMinimumSizeBackground(Char{" ", bg::Black});
    _updateSettings.setMinimumSizeMessage(
        String{"Resize the terminal to at least 28x8 cells for the plasma demo.", Color{fg::BrightWhite, bg::Black}});
}

auto RetroPlasmaApp::beforeRun() -> int {
    _lastFrameTime = std::chrono::steady_clock::now();
    return 0;
}

void RetroPlasmaApp::onKey(const Key &key) {
    if (key == U'f') {
        _speed = std::min(_speed + 0.25, 4.0);
    } else if (key == U's') {
        _speed = std::max(_speed - 0.25, 0.25);
    } else if (key == U'p' || key == Key::Space) {
        _paused = !_paused;
    } else if (key == U'c') {
        ++_paletteIndex;
    } else {
        TerminalApplication::onKey(key);
    }
}

void RetroPlasmaApp::onRenderToBuffer() {
    const auto now = std::chrono::steady_clock::now();
    const auto elapsedSeconds = std::chrono::duration<double>{now - _lastFrameTime}.count();
    _lastFrameTime = now;
    if (!_paused) {
        _phase += elapsedSeconds * (_speed * 2.7);
    }
    _buffer.fill(Char{U' ', fg::Default, bg::Black});
    const auto contentHeight = std::max(0, _buffer.size().height() - 1);
    _renderer.render(_buffer, Rectangle{0, 0, _buffer.size().width(), contentHeight}, _phase, _paletteIndex);
    drawPrompt();
}

void RetroPlasmaApp::drawPrompt() noexcept {
    if (_buffer.size().height() <= 0) {
        return;
    }
    const auto promptRow = _buffer.size().height() - 1;
    _buffer.fill(Rectangle{0, promptRow, _buffer.size().width(), 1}, Char{" ", bg::BrightBlack});
    auto text = Text{buildPrompt(), Rectangle{0, promptRow, _buffer.size().width(), 1}, Alignment::CenterLeft};
    _buffer.drawText(text);
}

auto RetroPlasmaApp::buildPrompt() const -> String {
    auto result = String{};
    result.append(
        bg::BrightBlack,
        fg::BrightWhite,
        "Retro Plasma  ",
        fg::BrightYellow,
        "[Q]",
        fg::BrightWhite,
        " quit  ",
        fg::BrightCyan,
        "[F]",
        fg::BrightWhite,
        " faster  ",
        fg::BrightCyan,
        "[S]",
        fg::BrightWhite,
        " slower  ",
        fg::BrightMagenta,
        "[P]",
        fg::BrightWhite,
        (_paused ? " resume  " : " pause  "),
        fg::BrightGreen,
        "[C]",
        fg::BrightWhite,
        std::format(" palette  speed {}%", (_speed * 100.0)));
    return result;
}

}
