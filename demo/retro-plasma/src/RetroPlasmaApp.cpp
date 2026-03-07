// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "RetroPlasmaApp.hpp"

#include "ScopedTerminalSession.hpp"

#include <algorithm>
#include <string>


namespace demo::retroplasma {

void RetroPlasmaApp::run() noexcept {
    auto session = ScopedTerminalSession{_terminal, Terminal::RefreshMode::Overwrite, Input::Mode::Key};
    _lastFrameTime = std::chrono::steady_clock::now();
    while (!_quitRequested) {
        const auto key = _terminal.input().read(std::chrono::milliseconds{35});
        if (key.valid()) {
            handleKey(key);
        }
        const auto now = std::chrono::steady_clock::now();
        const auto elapsedSeconds = std::chrono::duration<double>{now - _lastFrameTime}.count();
        _lastFrameTime = now;
        if (!_paused) {
            _phase += elapsedSeconds * (_speed * 2.7);
        }
        renderFrame();
    }
}


auto RetroPlasmaApp::canvasSize() const noexcept -> Size {
    return _terminal.size() - Size{1, 1};
}


void RetroPlasmaApp::handleKey(const Key &key) noexcept {
    if (key == Key{Key::Character, 'q'}) {
        _quitRequested = true;
    } else if (key == Key{Key::Character, 'f'}) {
        _speed = std::min(_speed + 0.25, 4.0);
    } else if (key == Key{Key::Character, 's'}) {
        _speed = std::max(_speed - 0.25, 0.25);
    } else if (key == Key{Key::Character, 'p'} || key == Key{Key::Space}) {
        _paused = !_paused;
    } else if (key == Key{Key::Character, 'c'}) {
        ++_paletteIndex;
    }
}


void RetroPlasmaApp::renderFrame() noexcept {
    _terminal.testScreenSize();
    auto buffer = Buffer{canvasSize()};
    buffer.fill(Char{" ", Color{fg::Default, bg::Black}});
    if (buffer.size().width() < 28 || buffer.size().height() < 8) {
        buffer.drawText(
            "Resize the terminal to at least 28x8 cells for the plasma demo.",
            Rectangle{0, 0, buffer.size().width(), std::max(0, buffer.size().height() - 1)},
            Alignment::Center,
            Color{fg::BrightWhite, bg::Black});
    } else {
        const auto contentHeight = std::max(0, buffer.size().height() - 1);
        _renderer.render(buffer, Rectangle{0, 0, buffer.size().width(), contentHeight}, _phase, _paletteIndex);
        drawPrompt(buffer);
    }
    auto settings = UpdateSettings{};
    settings.setMinimumSize(Size{20, 6});
    _terminal.updateScreen(buffer, settings);
    _terminal.flush();
}


void RetroPlasmaApp::drawPrompt(Buffer &buffer) const {
    if (buffer.size().height() <= 0) {
        return;
    }
    const auto promptRow = buffer.size().height() - 1;
    buffer.fill(Rectangle{0, promptRow, buffer.size().width(), 1}, Char{" ", Color{fg::Default, bg::BrightBlack}});
    auto text = Text{buildPrompt(), Rectangle{0, promptRow, buffer.size().width(), 1}, Alignment::CenterLeft};
    buffer.drawText(text);
}


auto RetroPlasmaApp::buildPrompt() const -> String {
    auto result = String{};
    appendText(result, "Retro Plasma  ", Color{fg::BrightWhite, bg::BrightBlack});
    appendText(result, "[Q]", Color{fg::BrightYellow, bg::BrightBlack});
    appendText(result, " quit  ", Color{fg::BrightWhite, bg::BrightBlack});
    appendText(result, "[F]", Color{fg::BrightCyan, bg::BrightBlack});
    appendText(result, " faster  ", Color{fg::BrightWhite, bg::BrightBlack});
    appendText(result, "[S]", Color{fg::BrightCyan, bg::BrightBlack});
    appendText(result, " slower  ", Color{fg::BrightWhite, bg::BrightBlack});
    appendText(result, "[P]", Color{fg::BrightMagenta, bg::BrightBlack});
    appendText(result, _paused ? " resume  " : " pause  ", Color{fg::BrightWhite, bg::BrightBlack});
    appendText(result, "[C]", Color{fg::BrightGreen, bg::BrightBlack});
    appendText(
        result,
        " palette  speed " + std::to_string(static_cast<int>(_speed * 100.0)) + "%",
        Color{fg::BrightWhite, bg::BrightBlack});
    return result;
}


void RetroPlasmaApp::appendText(String &target, const std::string_view text, const Color color) {
    for (const auto &character : String{text}) {
        target.append(character.withColor(color));
    }
}


}
