// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Terminal.hpp"


#include "impl/InputFactory.hpp"

#include <algorithm>
#include <cassert>
#include <format>
#include <iostream>


namespace erbsland::cterm {


Terminal::Terminal(const Size size) : _size{size} {
    _input = impl::createInputForPlatform();
}

void Terminal::setSize(const Size size) noexcept {
    const auto hasChanged = (size != _size);
    _size = size;
    if (hasChanged) {
        _clearScreenAfterResize = true;
    }
}

void Terminal::setLineBufferEnabled(const bool enabled) noexcept {
    if (_lineBufferEnabled == enabled) {
        return;
    }
    if (!enabled) {
        flushLineBuffer();
    }
    _lineBufferEnabled = enabled;
}

void Terminal::setBackBufferEnabled(const bool enabled) noexcept {
    if (enabled) {
        if (!_backBuffer.has_value()) {
            _backBuffer = Buffer{Size{}};
        }
        return;
    }
    _backBuffer.reset();
}

void Terminal::appendClearSequence(std::string &output) noexcept {
    output += "\x1b[2J\x1b[1;1H";
}

void Terminal::appendResizeClearSequence(std::string &output) noexcept {
    output += "\x1b[2J\x1b[1;1H";
}

void Terminal::appendCursorHomeSequence(std::string &output) noexcept {
    output += "\x1b[H";
}

void Terminal::appendCursorMoveSequence(std::string &output, const Position pos) noexcept {
    output += std::format("\x1b[{};{}H", pos.y() + 1, pos.x() + 1);
}

void Terminal::appendDefaultColorSequence(std::string &output, Color &currentColor) const noexcept {
    if (!colorEnabled()) {
        return;
    }
    const auto defaultColor = Color::reset();
    if (currentColor == defaultColor) {
        return;
    }
    output += std::format("\x1b[{};{}m", defaultColor.fg().ansiCode(), defaultColor.bg().ansiCode());
    currentColor = defaultColor;
}

void Terminal::appendCharacterOutput(std::string &output, const Char &character, Color &currentColor) const noexcept {
    if (colorEnabled()) {
        auto targetColor = character.color();
        if (targetColor.fg() == Foreground::Inherited) {
            targetColor.setFg(Foreground::Default);
        }
        if (targetColor.bg() == Background::Inherited) {
            targetColor.setBg(Background::Default);
        }
        if (targetColor != currentColor) {
            if (targetColor.fg() != currentColor.fg() && targetColor.bg() != currentColor.bg()) {
                output += std::format("\x1b[{};{}m", targetColor.fg().ansiCode(), targetColor.bg().ansiCode());
            } else if (targetColor.fg() != currentColor.fg()) {
                output += std::format("\x1b[{}m", targetColor.fg().ansiCode());
            } else if (targetColor.bg() != currentColor.bg()) {
                output += std::format("\x1b[{}m", targetColor.bg().ansiCode());
            }
            currentColor = targetColor;
        }
    }
    character.appendTo(output);
}

void Terminal::appendFullFrameOutput(
    std::string &output, const Buffer &buffer, const Size terminalSize, const UpdateSettings &settings) const noexcept {

    auto currentColor = _color;
    appendDefaultColorSequence(output, currentColor);
    if (isMinimumSizeOnly(terminalSize, settings)) {
        appendCharacterOutput(output, getCropped(buffer, terminalSize, settings, Position{0, 0}), currentColor);
        appendDefaultColorSequence(output, currentColor);
        return;
    }
    const auto frameSize = renderedFrameSize(buffer, terminalSize, settings);
    for (int y = 0; y < frameSize.height(); ++y) {
        for (int x = 0; x < frameSize.width(); ++x) {
            appendCharacterOutput(output, getCropped(buffer, terminalSize, settings, Position{x, y}), currentColor);
        }
        appendDefaultColorSequence(output, currentColor);
        output.push_back('\n');
    }
}

void Terminal::appendDiffFrameOutput(
    std::string &output,
    const Buffer &previousFrame,
    const Buffer &buffer,
    const Size terminalSize,
    const UpdateSettings &settings) const noexcept {

    auto currentColor = _color;
    for (int y = 0; y < previousFrame.size().height(); ++y) {
        auto x = 0;
        while (x < previousFrame.size().width()) {
            const auto startPos = Position{x, y};
            if (previousFrame.get(startPos).renderedEquals(
                    getCropped(buffer, terminalSize, settings, startPos), colorEnabled())) {
                ++x;
                continue;
            }
            appendCursorMoveSequence(output, startPos);
            appendDefaultColorSequence(output, currentColor);
            while (x < previousFrame.size().width()) {
                const auto pos = Position{x, y};
                const auto &nextCharacter = getCropped(buffer, terminalSize, settings, pos);
                if (previousFrame.get(pos).renderedEquals(nextCharacter, colorEnabled())) {
                    break;
                }
                appendCharacterOutput(output, nextCharacter, currentColor);
                ++x;
            }
        }
    }
    appendDefaultColorSequence(output, currentColor);
    appendCursorMoveSequence(output, Position{0, previousFrame.size().height()});
}

void Terminal::appendRefreshSequence(std::string &output, const bool forceFullClear) const noexcept {
    if (!colorEnabled()) {
        return;
    }
    if (forceFullClear) {
        appendResizeClearSequence(output);
        return;
    }
    switch (refreshMode()) {
    case RefreshMode::Clear:
        appendClearSequence(output);
        break;
    case RefreshMode::Overwrite:
        appendCursorHomeSequence(output);
        break;
    case RefreshMode::Keep:
    default:
        break;
    }
}

void Terminal::emitBufferedOutput(const std::string_view text) noexcept {
    if (!_lineBufferEnabled) {
        std::cout << text;
        return;
    }
    _lineBuffer.append(text);
    while (true) {
        const auto newlinePos = _lineBuffer.find('\n');
        if (newlinePos == std::string::npos) {
            break;
        }
        std::cout << _lineBuffer.substr(0, newlinePos + 1);
        _lineBuffer.erase(0, newlinePos + 1);
    }
}

void Terminal::emitDirectOutput(const std::string_view text, const bool flushConsole) noexcept {
    flushLineBuffer();
    std::cout << text;
    if (flushConsole) {
        std::cout.flush();
    }
}

void Terminal::flushLineBuffer() noexcept {
    if (_lineBuffer.empty()) {
        return;
    }
    std::cout << _lineBuffer;
    _lineBuffer.clear();
}

void Terminal::invalidateBackBuffer() noexcept {
    if (_backBuffer.has_value()) {
        _backBuffer = Buffer{Size{}};
    }
}

auto Terminal::isMinimumSizeOnly(const Size terminalSize, const UpdateSettings &settings) noexcept -> bool {
    return !settings.minimumSize().fitsInto(terminalSize);
}


auto Terminal::renderedFrameSize(const Buffer &buffer, const Size terminalSize, const UpdateSettings &settings) noexcept
    -> Size {

    if (isMinimumSizeOnly(terminalSize, settings)) {
        return {1, 1};
    }
    const auto visibleWidth = std::min(buffer.size().width(), std::max(terminalSize.width() - 1, 0));
    const auto visibleHeight = std::min(buffer.size().height(), std::max(terminalSize.height() - 1, 0));
    return {visibleWidth, visibleHeight};
}


auto Terminal::getCropped(
    const Buffer &buffer, const Size terminalSize, const UpdateSettings &settings, const Position pos) -> const Char & {

    if (isMinimumSizeOnly(terminalSize, settings)) {
        return settings.minimumSizeMark();
    }
    const auto frameSize = renderedFrameSize(buffer, terminalSize, settings);
    const auto isCroppedOnRight = frameSize.width() < buffer.size().width();
    const auto isCroppedAtBottom = frameSize.height() < buffer.size().height();
    const auto isLastVisibleColumn = (pos.x() == frameSize.width() - 1);
    const auto isLastVisibleRow = (pos.y() == frameSize.height() - 1);
    if (settings.showCropMarks() && isCroppedAtBottom && isLastVisibleRow) {
        return settings.cropMarkBottom();
    }
    if (settings.showCropMarks() && isCroppedOnRight && isLastVisibleColumn) {
        return settings.cropMarkRight();
    }
    return buffer.get(pos);
}


void Terminal::updateBackBuffer(
    const Buffer &buffer, const Size terminalSize, const UpdateSettings &settings) noexcept {
    if (!_backBuffer.has_value()) {
        return;
    }
    const auto frameSize = renderedFrameSize(buffer, terminalSize, settings);
    if (_backBuffer->size() != frameSize) {
        *_backBuffer = Buffer{frameSize};
    }
    frameSize.forEach(
        [&](const Position pos) { _backBuffer->set(pos, getCropped(buffer, terminalSize, settings, pos)); });
}


auto Terminal::changedCharacterCount(
    const Buffer &previousFrame,
    const Buffer &buffer,
    const Size terminalSize,
    const UpdateSettings &settings) const noexcept -> int {

    const auto frameSize = renderedFrameSize(buffer, terminalSize, settings);
    if (previousFrame.size() != frameSize) {
        return frameSize.area();
    }
    auto result = 0;
    frameSize.forEach([&](const Position pos) {
        if (!previousFrame.get(pos).renderedEquals(getCropped(buffer, terminalSize, settings, pos), colorEnabled())) {
            ++result;
        }
    });
    return result;
}

void Terminal::initializeScreen() noexcept {
    initializePlatform();
    invalidateBackBuffer();
    auto output = std::string{};
    if (colorEnabled()) {
        switch (refreshMode()) {
        case RefreshMode::Clear:
        case RefreshMode::Overwrite:
            appendClearSequence(output);
            break;
        case RefreshMode::Keep:
        default:
            break;
        }
    }
    if (!output.empty()) {
        emitDirectOutput(output, true);
        _color = Color::reset();
    }
    auto initialSize = _size;
    if (_sizeDetectionEnabled) {
        if (const auto detectedSize = detectScreenSize(); detectedSize.has_value()) {
            initialSize = *detectedSize;
        }
    }
    submitScreenSize(initialSize, false);
}

void Terminal::clearScreen() noexcept {
    const auto forceFullClear = _clearScreenAfterResize;
    _clearScreenAfterResize = false;
    auto output = std::string{};
    appendRefreshSequence(output, forceFullClear);
    if (!output.empty()) {
        emitDirectOutput(output, true);
        _color = Color::reset();
        invalidateBackBuffer();
    }
}

void Terminal::testScreenSize() noexcept {
    if (!_sizeDetectionEnabled) {
        return;
    }
    const auto detectedSize = detectScreenSize();
    if (!detectedSize.has_value() || *detectedSize == _size) {
        return;
    }
    submitScreenSize(*detectedSize, true);
}

void Terminal::restoreScreen() noexcept {
    auto output = std::string{};
    if (colorEnabled()) {
        auto currentColor = _color;
        appendDefaultColorSequence(output, currentColor);
        _color = Color::reset();
    }
    if (!output.empty()) {
        emitDirectOutput(output, true);
    }
    restorePlatform();
    invalidateBackBuffer();
}

void Terminal::write(const Char &character) noexcept {
    setColor(character.color().fg(), character.color().bg());
    auto buffer = std::string{};
    buffer.reserve(character.byteCount());
    character.appendTo(buffer);
    write(buffer);
}

void Terminal::write(const String &str) noexcept {
    for (const auto &character : str) {
        write(character);
    }
}

void Terminal::write(const std::string_view text) noexcept {
    emitBufferedOutput(text);
}

void Terminal::write(const Buffer &buffer) noexcept {
    for (int y = 0; y < buffer.size().height(); ++y) {
        for (int x = 0; x < buffer.size().width(); ++x) {
            write(buffer.get(Position{x, y}));
        }
        setDefaultColor();
        lineBreak();
    }
}

void Terminal::lineBreak() noexcept {
    emitBufferedOutput("\n");
}

auto Terminal::color() const noexcept -> Color {
    return _color;
}

void Terminal::setForeground(Foreground color) noexcept {
    if (!colorEnabled()) {
        return;
    }
    if (color == Foreground::Inherited) {
        color = Foreground::Default;
    }
    if (color != _color.fg()) {
        emitBufferedOutput(std::format("\x1b[{}m", color.ansiCode()));
        _color.setFg(color);
    }
}

void Terminal::setBackground(Background color) noexcept {
    if (!colorEnabled()) {
        return;
    }
    if (color == Background::Inherited) {
        color = Background::Default;
    }
    if (color != _color.bg()) {
        emitBufferedOutput(std::format("\x1b[{}m", color.ansiCode()));
        _color.setBg(color);
    }
}

void Terminal::setColor(const Foreground foregroundColor, const Background backgroundColor) noexcept {
    setColor(Color(foregroundColor, backgroundColor));
}

void Terminal::setColor(Color color) noexcept {
    if (!colorEnabled()) {
        return;
    }
    if (color.fg() == Foreground::Inherited) {
        color.setFg(Foreground::Default);
    }
    if (color.bg() == Background::Inherited) {
        color.setBg(Background::Default);
    }
    if (color == _color) {
        return;
    }
    if (color.fg() != _color.fg() && color.bg() != _color.bg()) {
        emitBufferedOutput(std::format("\x1b[{};{}m", color.fg().ansiCode(), color.bg().ansiCode()));
    } else if (color.fg() != _color.fg()) {
        emitBufferedOutput(std::format("\x1b[{}m", color.fg().ansiCode()));
    } else if (color.bg() != _color.bg()) {
        emitBufferedOutput(std::format("\x1b[{}m", color.bg().ansiCode()));
    }
    _color = color;
}

void Terminal::setDefaultColor() noexcept {
    setColor(Color{});
}

void Terminal::moveLeft(int count) {
    if (!colorEnabled()) {
        return;
    }
    write(std::format("\x1b[{}D", count));
}

void Terminal::flush() noexcept {
    flushLineBuffer();
    std::cout.flush();
}

void Terminal::updateScreen(const Buffer &buffer, const UpdateSettings &settings) noexcept {
    const auto forceFullClear = _clearScreenAfterResize;
    _clearScreenAfterResize = false;
    const auto terminalSize = size();
    const auto frameSize = renderedFrameSize(buffer, terminalSize, settings);
    auto output = std::string{};
    const auto useSmartBackBuffer = colorEnabled() && refreshMode() == RefreshMode::Overwrite && backBufferEnabled();
    if (useSmartBackBuffer) {
        const auto &previousFrame = *_backBuffer;
        if (forceFullClear || previousFrame.size() != frameSize) {
            if (forceFullClear) {
                appendResizeClearSequence(output);
            } else {
                appendClearSequence(output);
            }
            appendFullFrameOutput(output, buffer, terminalSize, settings);
        } else {
            const auto changedCount = changedCharacterCount(previousFrame, buffer, terminalSize, settings);
            const auto totalCharacters = frameSize.area();
            if (changedCount > 0) {
                if (changedCount * 5 >= totalCharacters) {
                    appendCursorHomeSequence(output);
                    appendFullFrameOutput(output, buffer, terminalSize, settings);
                } else {
                    appendCursorHomeSequence(output);
                    appendDiffFrameOutput(output, previousFrame, buffer, terminalSize, settings);
                }
            }
        }
    } else {
        appendRefreshSequence(output, forceFullClear);
        appendFullFrameOutput(output, buffer, terminalSize, settings);
    }
    if (!output.empty()) {
        emitDirectOutput(output);
        if (colorEnabled()) {
            _color = Color::reset();
        }
    }
    updateBackBuffer(buffer, terminalSize, settings);
}

void Terminal::submitScreenSize(const Size size, const bool fromTestScreenSize) noexcept {
    const auto hasChanged = (size != _size);
    _size = size;
    if (fromTestScreenSize && hasChanged) {
        _clearScreenAfterResize = true;
    }
    if (_screenSizeChangedCallback) {
        _screenSizeChangedCallback(size);
    }
}


}
