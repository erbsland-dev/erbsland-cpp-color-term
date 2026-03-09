// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "UpdateScreenModesApp.hpp"

#include "ScopedTerminalSession.hpp"

#include <algorithm>
#include <cmath>
#include <format>


namespace demo::updatescreenmodes {


void UpdateScreenModesApp::run() {
    auto session = ScopedTerminalSession{_terminal, Terminal::RefreshMode::Overwrite, Input::Mode::Key};
    applyMode();
    _lastTick = std::chrono::steady_clock::now();
    while (!_quitRequested) {
        const auto key = _terminal.input().read(std::chrono::milliseconds{70});
        if (key.valid()) {
            handleKey(key);
        }
        const auto now = std::chrono::steady_clock::now();
        updateAnimation(std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastTick));
        _lastTick = now;
        renderFrame();
    }
}


auto UpdateScreenModesApp::canvasSize() const noexcept -> Size {
    return _terminal.size() - Size{1, 1};
}


void UpdateScreenModesApp::handleKey(const Key &key) noexcept {
    if (key == Key{Key::Character, 'q'}) {
        _quitRequested = true;
    } else if (key == Key{Key::Left}) {
        advanceMode(-1);
    } else if (key == Key{Key::Right} || key == Key{Key::Space} || key == Key{Key::Character, 'n'}) {
        advanceMode(1);
    }
}


void UpdateScreenModesApp::updateAnimation(const std::chrono::milliseconds elapsed) noexcept {
    static constexpr auto cModeCycle = std::chrono::seconds{6};

    _modeCycleAccumulator += elapsed;
    while (_modeCycleAccumulator >= cModeCycle) {
        _modeCycleAccumulator -= cModeCycle;
        advanceMode(1);
    }
    ++_animationStep;
}


void UpdateScreenModesApp::advanceMode(const int delta) noexcept {
    const auto modeCount = 3;
    auto index = static_cast<int>(_mode);
    index = (index + delta) % modeCount;
    if (index < 0) {
        index += modeCount;
    }
    _mode = static_cast<Mode>(index);
    _modeCycleAccumulator = std::chrono::milliseconds{};
    applyMode();
}


void UpdateScreenModesApp::applyMode() noexcept {
    switch (_mode) {
    case Mode::Clear:
        _terminal.setRefreshMode(Terminal::RefreshMode::Clear);
        _terminal.setBackBufferEnabled(false);
        break;
    case Mode::Overwrite:
        _terminal.setRefreshMode(Terminal::RefreshMode::Overwrite);
        _terminal.setBackBufferEnabled(false);
        break;
    case Mode::OverwriteWithBackBuffer:
        _terminal.setRefreshMode(Terminal::RefreshMode::Overwrite);
        _terminal.setBackBufferEnabled(true);
        break;
    }
}


void UpdateScreenModesApp::renderFrame() {
    _terminal.testScreenSize();
    auto buffer = Buffer{canvasSize()};
    buffer.fill(Char{" ", bg::Black});
    if (buffer.size().width() < 70 || buffer.size().height() < 18) {
        buffer.drawText(
            "Resize the terminal to at least 70x18 cells for the update-screen demo.",
            Rectangle{0, 0, buffer.size().width(), buffer.size().height()},
            Alignment::Center,
            Color{fg::BrightWhite, bg::Black});
    } else {
        const auto outerRect = Rectangle{0, 0, buffer.size().width(), buffer.size().height()};
        const auto titleRect = Rectangle{2, 1, buffer.size().width() - 4, 1};
        const auto contentRect = Rectangle{2, 3, buffer.size().width() - 4, buffer.size().height() - 7};
        const auto footerRect = Rectangle{2, buffer.size().height() - 3, buffer.size().width() - 4, 1};
        buffer.drawFrame(outerRect, FrameStyle::LightWithRoundedCorners);
        buffer.drawText(
            std::format("updateScreen Demo  |  mode: {}", modeTitle()),
            titleRect,
            Alignment::Center,
            Color{fg::BrightWhite, bg::Black});
        drawScene(buffer, contentRect);
        drawFooter(buffer, footerRect);
    }
    _terminal.updateScreen(buffer);
    _terminal.flush();
}


void UpdateScreenModesApp::drawScene(Buffer &buffer, const Rectangle contentRect) const noexcept {
    const auto infoWidth = std::clamp(contentRect.width() / 3, 24, 30);
    const auto infoRect = Rectangle{contentRect.x1(), contentRect.y1(), infoWidth, contentRect.height()};
    const auto arenaRect =
        Rectangle{infoRect.x2() + 2, contentRect.y1(), contentRect.x2() - infoRect.x2() - 2, contentRect.height()};
    buffer.drawFilledFrame(infoRect, FrameStyle::Light, Char{" ", bg::BrightBlack});
    buffer.drawFilledFrame(arenaRect, FrameStyle::Light, Char{" ", bg::Black});

    buffer.drawText(
        "Refresh Mode",
        Rectangle{infoRect.x1() + 1, infoRect.y1() + 1, infoRect.width() - 2, 1},
        Alignment::Center);
    buffer.drawText(
        modeTitle(),
        Rectangle{infoRect.x1() + 1, infoRect.y1() + 3, infoRect.width() - 2, 2},
        Alignment::Center,
        Color{fg::BrightYellow, bg::BrightBlack});
    buffer.drawText(
        modeDescription(),
        Rectangle{infoRect.x1() + 1, infoRect.y1() + 6, infoRect.width() - 2, infoRect.height() - 10},
        Alignment::TopLeft,
        Color{fg::BrightWhite, bg::BrightBlack});
    buffer.drawText(
        "The scene on the right stays mostly static.\nOnly the spark and the short trail move.\nThat makes the back "
        "buffer path useful because it can patch just a handful of cells.",
        Rectangle{infoRect.x1() + 1, infoRect.y2() - 4, infoRect.width() - 2, 3},
        Alignment::TopLeft,
        Color{fg::White, bg::BrightBlack});

    const auto labelRect = Rectangle{arenaRect.x1() + 2, arenaRect.y1() + 1, arenaRect.width() - 4, 1};
    buffer.drawText(
        std::format(
            "Sparse animation field  |  frame {:05d}  |  mode switches every 6s",
            static_cast<int>(_animationStep)),
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
                buffer.set(Position{x, y}, Char{".", fg::BrightBlack, bg::Black});
            }
        }
    }
    for (int x = waveRect.x1(); x < waveRect.x2(); ++x) {
        buffer.set(Position{x, centerY}, Char{"-", fg::BrightBlack, bg::Black});
    }
    for (auto trail = 0; trail < 6; ++trail) {
        if (_animationStep < static_cast<std::size_t>(trail * 2)) {
            continue;
        }
        const auto waveWidth = std::max(1, waveRect.width());
        const auto localX = static_cast<int>(
            (_animationStep - static_cast<std::size_t>(trail * 2)) % static_cast<std::size_t>(waveWidth));
        const auto angle = static_cast<double>(localX + static_cast<int>(_animationStep / 2U)) / 5.0;
        const auto offsetY = static_cast<int>(std::lround(std::sin(angle) * static_cast<double>(amplitude)));
        const auto pos = Position{waveRect.x1() + localX, centerY + offsetY};
        const auto color = (trail == 0) ? Color{fg::BrightYellow, bg::Black} : Color{fg::BrightBlue, bg::Black};
        const auto glyph = (trail == 0) ? "@" : "*";
        if (waveRect.size().contains(pos - waveRect.topLeft())) {
            buffer.set(pos, Char{glyph, color});
        }
    }
}


void UpdateScreenModesApp::drawFooter(Buffer &buffer, const Rectangle footerRect) const {
    buffer.fill(footerRect, Char{" ", bg::BrightBlack});
    auto footer = String{};
    footer.append(
        bg::BrightBlack,
        fg::BrightYellow,
        "[Left][Right]",
        fg::BrightWhite,
        " switch mode  ",
        fg::BrightGreen,
        "[Space]",
        fg::BrightWhite,
        " next mode  ",
        fg::BrightMagenta,
        "[Q]",
        fg::BrightWhite,
        " quit");
    buffer.drawText(Text{footer, footerRect, Alignment::CenterLeft});
}


auto UpdateScreenModesApp::modeTitle() const noexcept -> std::string_view {
    switch (_mode) {
    case Mode::Clear:
        return "Clear";
    case Mode::Overwrite:
        return "Overwrite";
    case Mode::OverwriteWithBackBuffer:
    default:
        return "Overwrite + Back Buffer";
    }
}


auto UpdateScreenModesApp::modeDescription() const noexcept -> std::string_view {
    switch (_mode) {
    case Mode::Clear:
        return "Every frame clears the full screen and redraws everything. This is the simplest refresh strategy and "
               "works well when the entire scene changes.";
    case Mode::Overwrite:
        return "The cursor jumps back to the top-left corner and the whole frame is redrawn. This avoids the explicit "
               "clear, but it still rewrites every visible cell.";
    case Mode::OverwriteWithBackBuffer:
    default:
        return "Overwrite mode keeps a rendered back buffer. If only a small fraction of the cells changed, "
               "updateScreen() emits a compact patch instead of repainting the whole frame.";
    }
}


}
