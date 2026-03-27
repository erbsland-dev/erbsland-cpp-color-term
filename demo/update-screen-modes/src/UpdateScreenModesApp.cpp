// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "UpdateScreenModesApp.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <string>


namespace demo::updatescreenmodes {


void UpdateScreenModesApp::onKey(const Key &key) {
    if (key == U'q') {
        _quitRequested = true;
    } else if (key == U'o') {
        _state.toggleOverwriteMode();
    } else if (key == U'l') {
        _state.toggleLineBuffer();
    } else if (key == U'b') {
        _state.toggleBackBuffer();
    } else if (key == U's') {
        _state.toggleSafeMargin();
    }
}


void UpdateScreenModesApp::applyTerminalSettings() noexcept {
    if (_state.overwriteModeEnabled()) {
        _terminal.setRefreshMode(Terminal::RefreshMode::Overwrite);
    } else {
        _terminal.setRefreshMode(Terminal::RefreshMode::Clear);
    }
    _terminal.setLineBufferEnabled(_state.lineBufferEnabled());
    _terminal.setBackBufferEnabled(_state.backBufferActive());
    _terminal.setSafeMarginEnabled(_state.safeMarginEnabled());
}


void UpdateScreenModesApp::onRenderToBuffer() {
    applyTerminalSettings();
    _buffer.fill(Char{" ", bg::Black});
    const auto outerRect = Rectangle{0, 0, _buffer.size().width(), _buffer.size().height()};
    const auto titleRect = Rectangle{2, 1, _buffer.size().width() - 4, 1};
    const auto contentRect = Rectangle{2, 3, _buffer.size().width() - 4, _buffer.size().height() - 7};
    const auto footerRect = Rectangle{2, _buffer.size().height() - 3, _buffer.size().width() - 4, 1};
    _buffer.drawFrame(outerRect, FrameStyle::LightWithRoundedCorners);
    _buffer.drawText(
        std::format("Screen Update Demo  |  refresh mode: {}", _state.modeTitle()),
        titleRect,
        Alignment::Center,
        Color{fg::BrightWhite, bg::Black});
    drawScene(contentRect);
    drawFooter(footerRect);
}


void UpdateScreenModesApp::onAfterUpdateScreen(const std::chrono::nanoseconds duration) {
    _flushSpeedTracker.addSample(duration);
}


void UpdateScreenModesApp::drawScene(const Rectangle contentRect) noexcept {
    const auto infoWidth = std::clamp(contentRect.width() / 3, 24, 30);
    const auto infoRect = Rectangle{contentRect.x1(), contentRect.y1(), infoWidth, contentRect.height()};
    const auto arenaRect =
        Rectangle{infoRect.x2() + 2, contentRect.y1(), contentRect.x2() - infoRect.x2() - 2, contentRect.height()};
    _buffer.drawFilledFrame(infoRect, FrameStyle::Light, Char{" ", bg::BrightBlack});
    _buffer.drawFilledFrame(arenaRect, FrameStyle::Light, Char{" ", bg::Black});

    _buffer.drawText(
        "Refresh Mode", Rectangle{infoRect.x1() + 1, infoRect.y1() + 1, infoRect.width() - 2, 1}, Alignment::Center);
    _buffer.drawText(
        _state.modeTitle(),
        Rectangle{infoRect.x1() + 1, infoRect.y1() + 3, infoRect.width() - 2, 1},
        Alignment::Center,
        Color{fg::BrightYellow, bg::BrightBlack});

    auto statusText = String{};
    appendTimingLine(statusText, "Last Update", _flushSpeedTracker.lastMilliseconds());
    appendTimingLine(statusText, "Average Update", _flushSpeedTracker.averageMilliseconds());
    appendStateLine(statusText, "Overwrite Mode", _state.overwriteModeEnabled());
    appendStateLine(statusText, "Line Buffer", _state.lineBufferEnabled());
    appendStateLine(statusText, "Back Buffer", _state.backBufferEnabled());
    appendStateLine(statusText, "Safe Margin", _state.safeMarginEnabled());
    _buffer.drawText(
        Text{
            statusText,
            Rectangle{infoRect.x1() + 1, infoRect.y1() + 4, infoRect.width() - 2, infoRect.height() - 5},
            Alignment::TopLeft});

    const auto labelRect = Rectangle{arenaRect.x1() + 2, arenaRect.y1() + 1, arenaRect.width() - 4, 1};
    _buffer.drawText(
        std::format(
            "Sparse animation field  |  frame {:05d}  |  line buffer: {}  |  back buffer: {}",
            static_cast<int>(_animationCycle),
            _state.lineBufferEnabled() ? "on" : "off",
            _state.backBufferActive() ? "active" : "inactive"),
        labelRect,
        Alignment::Center,
        Color{fg::BrightCyan, bg::Black});

    const auto waveRect =
        Rectangle{arenaRect.x1() + 2, arenaRect.y1() + 3, arenaRect.width() - 4, arenaRect.height() - 6};
    const auto centerY = waveRect.y1() + waveRect.height() / 2;
    const auto amplitude = std::max(1, waveRect.height() / 2 - 2);
    for (int y = waveRect.y1(); y < waveRect.y2(); ++y) {
        for (int x = waveRect.x1(); x < waveRect.x2(); ++x) {
            if ((x + y) % 4 == 0) {
                _buffer.set(Position{x, y}, Char{U'.', fg::BrightBlack, bg::Black});
            }
        }
    }
    for (int x = waveRect.x1(); x < waveRect.x2(); ++x) {
        _buffer.set(Position{x, centerY}, Char{U'-', fg::BrightBlack, bg::Black});
    }
    for (auto trail = 0; trail < 6; ++trail) {
        if (_animationCycle < static_cast<std::size_t>(trail * 2)) {
            continue;
        }
        const auto waveWidth = std::max(1, waveRect.width());
        const auto localX = static_cast<int>(
            (_animationCycle - static_cast<std::size_t>(trail * 2)) % static_cast<std::size_t>(waveWidth));
        const auto angle = static_cast<double>(localX + static_cast<int>(_animationCycle / 2U)) / 5.0;
        const auto offsetY = static_cast<int>(std::lround(std::sin(angle) * static_cast<double>(amplitude)));
        const auto pos = Position{waveRect.x1() + localX, centerY + offsetY};
        const auto color = (trail == 0) ? Color{fg::BrightYellow, bg::Black} : Color{fg::BrightBlue, bg::Black};
        const auto glyph = (trail == 0) ? "@" : "*";
        if (waveRect.size().contains(pos - waveRect.topLeft())) {
            _buffer.set(pos, Char{glyph, color});
        }
    }
}


void UpdateScreenModesApp::drawFooter(const Rectangle footerRect) {
    _buffer.fill(footerRect, Char{" ", bg::BrightBlack});

    auto line = String{};
    line.append(
        bg::BrightBlack,
        fg::BrightCyan,
        "[O]",
        fg::BrightWhite,
        " overwrite  ",
        fg::BrightYellow,
        "[L]",
        fg::BrightWhite,
        " line buf  ",
        fg::BrightMagenta,
        "[B]",
        fg::BrightWhite,
        " back buf  ",
        fg::BrightGreen,
        "[S]",
        fg::BrightWhite,
        " safe margin  ",
        fg::BrightGreen,
        "[Q]",
        fg::BrightWhite,
        " quit");
    _buffer.drawText(
        Text{line, Rectangle{footerRect.x1(), footerRect.y1(), footerRect.width(), 1}, Alignment::CenterLeft});
}


void UpdateScreenModesApp::appendStateLine(String &text, const std::string_view label, const bool enabled) noexcept {
    constexpr auto cLabelWidth = std::size_t{18};

    text.append(bg::BrightBlack, fg::BrightWhite, label);
    if (label.size() < cLabelWidth) {
        text.append(bg::BrightBlack, fg::White, std::string(cLabelWidth - label.size(), '.'));
    }
    text.append(bg::BrightBlack, fg::White, "[");
    if (enabled) {
        text.append(bg::BrightBlack, fg::BrightGreen, "✓");
    } else {
        text.append(bg::BrightBlack, fg::BrightRed, "✕");
    }
    text.append(bg::BrightBlack, fg::White, "]\n");
}


void UpdateScreenModesApp::appendTimingLine(
    String &text, const std::string_view label, const double milliseconds) noexcept {

    constexpr auto cLabelWidth = std::size_t{18};

    text.append(bg::BrightBlack, fg::BrightWhite, label);
    if (label.size() < cLabelWidth) {
        text.append(bg::BrightBlack, fg::White, std::string(cLabelWidth - label.size(), '.'));
    }
    text.append(bg::BrightBlack, fg::BrightWhite, std::format("{:>6.3f} ms\n", milliseconds));
}


}
