// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "FrameWeaverApp.hpp"

#include "ScopedTerminalSession.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <format>
#include <string>


namespace demo::frameweaver {


void FrameWeaverApp::run() {
    _updateSettings.setMinimumSize(Size{32, 10});
    _updateSettings.setMinimumSizeBackground(Char{" ", bg::Black});
    _updateSettings.setMinimumSizeMessage(
        String{"Resize the terminal to at least 32x10 cells for the frame demo.", Color{fg::BrightWhite, bg::Black}});
    auto session = ScopedTerminalSession{_terminal, Terminal::RefreshMode::Overwrite, Input::Mode::Key};
    _lastTick = std::chrono::steady_clock::now();
    while (!_quitRequested) {
        const auto key = _terminal.input().read(std::chrono::milliseconds{100});
        if (key.valid()) {
            handleKey(key);
        }
        const auto now = std::chrono::steady_clock::now();
        updateAnimation(std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastTick));
        _lastTick = now;
        renderFrame();
    }
}


auto FrameWeaverApp::canvasSize() const noexcept -> Size {
    return _terminal.size();
}


void FrameWeaverApp::handleKey(const Key &key) noexcept {
    if (key == Key{Key::Character, 'q'}) {
        _quitRequested = true;
    } else if (key == Key{Key::Character, 'c'}) {
        _frames.clear();
        _accumulator = std::chrono::milliseconds{};
    } else if (key == Key{Key::Character, 'f'}) {
        _frameDelay = std::max(std::chrono::milliseconds{250}, _frameDelay - std::chrono::milliseconds{150});
    } else if (key == Key{Key::Character, 's'}) {
        _frameDelay = std::min(std::chrono::milliseconds{2000}, _frameDelay + std::chrono::milliseconds{150});
    } else if (key == Key{Key::Character, '1'}) {
        _styleMode = StyleMode::Light;
    } else if (key == Key{Key::Character, '2'}) {
        _styleMode = StyleMode::Double;
    } else if (key == Key{Key::Character, '3'}) {
        _styleMode = StyleMode::Heavy;
    } else if (key == Key{Key::Character, '4'}) {
        _styleMode = StyleMode::Mixed;
    } else if (key == Key{Key::Character, '5'}) {
        _styleMode = StyleMode::Block;
    } else if (key == Key{Key::Character, '6'}) {
        _styleMode = StyleMode::Custom;
    } else if (key == Key{Key::Character, '7'}) {
        _styleMode = StyleMode::All;
    }
}


void FrameWeaverApp::updateAnimation(const std::chrono::milliseconds elapsed) noexcept {
    _accumulator += elapsed;
    while (_accumulator >= _frameDelay) {
        _accumulator -= _frameDelay;
        addFrame();
    }
}


void FrameWeaverApp::addFrame() noexcept {
    static constexpr auto cMaxFrames = std::size_t{20};

    if (_frames.size() >= cMaxFrames) {
        _frames.clear();
    }
    _frames.push_back(createRandomFrame());
}


void FrameWeaverApp::renderFrame() {
    _terminal.testScreenSize();
    _buffer.resize(canvasSize().componentMax(_updateSettings.minimumSize()));
    _buffer.fill(Char{" ", bg::Black});
    const auto titleRect = Rectangle{0, 0, _buffer.size().width(), 1};
    const auto contentRect = Rectangle{0, 1, _buffer.size().width(), _buffer.size().height() - 2};
    const auto footerRect = Rectangle{0, _buffer.size().height() - 1, _buffer.size().width(), 1};
    _buffer.fill(titleRect, Char{" ", bg::Blue});
    _buffer.fill(footerRect, Char{" ", bg::BrightBlack});
    _buffer.drawText(
        std::format(
            "Frame Weaver  |  frames {:02d}/20  |  interval {:.2f}s  |  mode {}",
            static_cast<int>(_frames.size()),
            static_cast<double>(_frameDelay.count()) / 1000.0,
            modeName()),
        titleRect,
        Alignment::CenterLeft,
        Color{fg::BrightWhite, bg::Blue});
    renderFrames(contentRect);
    auto prompt = Text{buildPrompt(), footerRect, Alignment::CenterLeft};
    _buffer.drawText(prompt);
    _terminal.updateScreen(_buffer, _updateSettings);
}


void FrameWeaverApp::renderFrames(const Rectangle contentRect) {
    for (const auto &frame : _frames) {
        if (frame.customStyle != nullptr) {
            _buffer.drawFrame(
                frameRectangle(frame, contentRect),
                frame.customStyle,
                CharCombinationStyle::commonBoxFrame(),
                frame.color);
        } else {
            _buffer.drawFrame(frameRectangle(frame, contentRect), frame.style, frame.color);
        }
    }
}


auto FrameWeaverApp::createRandomFrame() -> FrameSpec {
    auto factorDistribution = std::uniform_real_distribution<double>{0.0, 1.0};
    auto widthDistribution = std::uniform_real_distribution<double>{0.20, 0.78};
    auto heightDistribution = std::uniform_real_distribution<double>{0.20, 0.72};
    auto styles = availableStyles();
    auto styleDistribution = std::uniform_int_distribution<std::size_t>{0, styles.size() - 1};
    const auto &colorPalette = colors();
    auto colorDistribution = std::uniform_int_distribution<std::size_t>{0, colorPalette.sequenceLength() - 1};
    auto frame = FrameSpec{
        .x = factorDistribution(_rng),
        .y = factorDistribution(_rng),
        .width = widthDistribution(_rng),
        .height = heightDistribution(_rng),
        .color = colorPalette.color(colorDistribution(_rng)),
    };
    const auto &style = styles[styleDistribution(_rng)];
    frame.style = style.style;
    frame.customStyle = style.customStyle;
    return frame;
}


auto FrameWeaverApp::frameRectangle(const FrameSpec frame, const Rectangle contentRect) -> Rectangle {
    const auto width = std::clamp(
        static_cast<int>(std::lround(frame.width * static_cast<double>(contentRect.width()))),
        4,
        std::max(4, contentRect.width()));
    const auto height = std::clamp(
        static_cast<int>(std::lround(frame.height * static_cast<double>(contentRect.height()))),
        3,
        std::max(3, contentRect.height()));
    const auto maxX = std::max(0, contentRect.width() - width);
    const auto maxY = std::max(0, contentRect.height() - height);
    const auto x = contentRect.x1() + static_cast<int>(std::lround(frame.x * static_cast<double>(maxX)));
    const auto y = contentRect.y1() + static_cast<int>(std::lround(frame.y * static_cast<double>(maxY)));
    return {x, y, width, height};
}


auto FrameWeaverApp::prismFrameStyle() -> const Char16StylePtr & {
    static const auto style = std::make_shared<Char16Style>(std::array<Char, 16>{
        Char{U'∙'},
        Char{U'╶'},
        Char{U'╷'},
        Char{U'←', fg::BrightRed, bg::Red},
        Char{U'╴'},
        Char{U'─'},
        Char{U'→', fg::BrightBlue, bg::Blue},
        Char{U'┬'},
        Char{U'╵'},
        Char{U'↓', fg::BrightRed, bg::Red},
        Char{U'│'},
        Char{U'├'},
        Char{U'↓', fg::BrightBlue, bg::Blue},
        Char{U'┴'},
        Char{U'┤'},
        Char{U'┼'},
    });
    return style;
}


auto FrameWeaverApp::colors() -> const ColorSequence & {
    static const auto cColors = ColorSequence{
        {fg::BrightCyan, bg::Black},
        {fg::BrightMagenta, bg::Black},
        {fg::BrightYellow, bg::Black},
        {fg::BrightGreen, bg::Black},
        {fg::BrightBlue, bg::Black},
    };
    return cColors;
}


auto FrameWeaverApp::availableStyles() const -> std::vector<FrameSpec> {
    switch (_styleMode) {
    case StyleMode::Light:
        return {FrameSpec{.style = FrameStyle::Light}};
    case StyleMode::Double:
        return {FrameSpec{.style = FrameStyle::Double}};
    case StyleMode::Heavy:
        return {FrameSpec{.style = FrameStyle::Heavy}};
    case StyleMode::Mixed:
        return {
            FrameSpec{.style = FrameStyle::Light},
            FrameSpec{.style = FrameStyle::Double},
            FrameSpec{.style = FrameStyle::Heavy},
        };
    case StyleMode::Block:
        return {
            FrameSpec{.style = FrameStyle::FullBlock},
            FrameSpec{.style = FrameStyle::FullBlockWithChamfer},
            FrameSpec{.style = FrameStyle::OuterHalfBlock},
            FrameSpec{.style = FrameStyle::InnerHalfBlock},
        };
    case StyleMode::Custom:
        return {FrameSpec{.customStyle = prismFrameStyle()}};
    case StyleMode::All:
    default:
        return {
            FrameSpec{.style = FrameStyle::Light},
            FrameSpec{.style = FrameStyle::LightWithRoundedCorners},
            FrameSpec{.style = FrameStyle::LightDoubleDash},
            FrameSpec{.style = FrameStyle::LightTripleDash},
            FrameSpec{.style = FrameStyle::LightQuadrupleDash},
            FrameSpec{.style = FrameStyle::Heavy},
            FrameSpec{.style = FrameStyle::HeavyDoubleDash},
            FrameSpec{.style = FrameStyle::HeavyTripleDash},
            FrameSpec{.style = FrameStyle::HeavyQuadrupleDash},
            FrameSpec{.style = FrameStyle::Double},
            FrameSpec{.style = FrameStyle::FullBlock},
            FrameSpec{.style = FrameStyle::FullBlockWithChamfer},
            FrameSpec{.style = FrameStyle::OuterHalfBlock},
            FrameSpec{.style = FrameStyle::InnerHalfBlock},
            FrameSpec{.customStyle = prismFrameStyle()},
        };
    }
}


auto FrameWeaverApp::modeName() const -> std::string_view {
    switch (_styleMode) {
    case StyleMode::Light:
        return "light";
    case StyleMode::Double:
        return "double";
    case StyleMode::Heavy:
        return "heavy";
    case StyleMode::Mixed:
        return "mixed";
    case StyleMode::Block:
        return "block";
    case StyleMode::Custom:
        return "custom";
    case StyleMode::All:
    default:
        return "all";
    }
}


auto FrameWeaverApp::buildPrompt() const -> String {
    auto result = String{};
    result.append(
        fg::BrightYellow,
        bg::BrightBlack,
        "[Q]",
        fg::BrightWhite,
        " quit  ",
        fg::BrightCyan,
        "[F]/[S]",
        fg::BrightWhite,
        " speed  ",
        fg::BrightMagenta,
        "[C]",
        fg::BrightWhite,
        " clear  ",
        fg::BrightGreen,
        "[1]",
        fg::BrightWhite,
        " light  ",
        fg::BrightGreen,
        "[2]",
        fg::BrightWhite,
        " double  ",
        fg::BrightGreen,
        "[3]",
        fg::BrightWhite,
        " heavy  ",
        fg::BrightGreen,
        "[4]",
        fg::BrightWhite,
        " mixed  ",
        fg::BrightGreen,
        "[5]",
        fg::BrightWhite,
        " block  ",
        fg::BrightGreen,
        "[6]",
        fg::BrightWhite,
        " custom  ",
        fg::BrightGreen,
        "[7]",
        fg::BrightWhite,
        " all");
    return result;
}


}
