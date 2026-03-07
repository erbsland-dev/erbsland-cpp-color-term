// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Terminal.hpp"

#include <algorithm>
#include <cassert>
#include <format>
#include <iostream>


namespace erbsland::cterm {


Terminal::Terminal(const Size size) : _size{size} {
    _input = createInputForPlatform();
}


void Terminal::setSize(const Size size) noexcept {
    const auto hasChanged = (size != _size);
    _size = size;
    if (hasChanged) {
        _clearScreenAfterResize = true;
    }
}


void Terminal::writeClearSequence() noexcept {
    std::cout << "\x1b[2J\x1b[1;1H";
}


void Terminal::writeResizeClearSequence() noexcept {
    std::cout << "\x1b[2J\x1b[1;1H";
}


void Terminal::writeCursorHomeSequence() noexcept {
    std::cout << "\x1b[H";
}


void Terminal::initializeScreen() noexcept {
    initializePlatform();
    if (colorEnabled()) {
        switch (refreshMode()) {
        case RefreshMode::Clear:
        case RefreshMode::Overwrite:
            writeClearSequence();
            break;
        default:;
            break;
        }
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
    if (colorEnabled()) {
        if (forceFullClear) {
            writeResizeClearSequence();
            return;
        }
        switch (refreshMode()) {
        case RefreshMode::Clear:
            writeClearSequence();
            break;
        case RefreshMode::Overwrite:
            writeCursorHomeSequence();
            break;
        default:;
            break;
        }
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
    if (colorEnabled()) {
        setDefaultColor();
    }
    restorePlatform();
}

void Terminal::write(const Char &character) noexcept {
    setColor(character.color().fg(), character.color().bg());
    write(character.charStr());
}

void Terminal::write(const String &str) noexcept {
    for (const auto &character : str) {
        write(character);
    }
}

void Terminal::write(const std::string_view text) noexcept {
    std::cout << text;
}

void Terminal::lineBreak() noexcept {
    std::cout << '\n';
}

auto Terminal::color() const noexcept -> Color {
    return _color;
}

void Terminal::setForeground(const Foreground color) noexcept {
    if (!colorEnabled()) {
        return;
    }
    if (color != _color.fg()) {
        std::cout << std::format("\x1b[{}m", color.ansiCode());
        _color.setFg(color);
    }
}

void Terminal::setBackground(const Background color) noexcept {
    if (!colorEnabled()) {
        return;
    }
    if (color != _color.bg()) {
        std::cout << std::format("\x1b[{}m", color.ansiCode());
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
    if (color == _color) {
        return;
    }
    if (color.fg() != _color.fg() && color.bg() != _color.bg()) {
        std::cout << std::format("\x1b[{};{}m", color.fg().ansiCode(), color.bg().ansiCode());
    } else if (color.fg() != _color.fg()) {
        std::cout << std::format("\x1b[{}m", color.fg().ansiCode());
    } else if (color.bg() != _color.bg()) {
        std::cout << std::format("\x1b[{}m", color.bg().ansiCode());
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
    std::cout.flush();
}


void Terminal::updateScreen(const Buffer &buffer, const UpdateSettings &settings) noexcept {
    clearScreen();
    setDefaultColor();
    const auto terminalSize = size();
    if (!settings.minimumSize().fitsInto(terminalSize)) {
        write(settings.minimumSizeMark());
        setDefaultColor();
        return;
    }
    const auto visibleWidth = std::min(buffer.size().width(), std::max(terminalSize.width() - 1, 0));
    const auto visibleHeight = std::min(buffer.size().height(), std::max(terminalSize.height() - 1, 0));
    const auto isCroppedOnRight = visibleWidth < buffer.size().width();
    const auto isCroppedAtBottom = visibleHeight < buffer.size().height();
    for (int y = 0; y < visibleHeight; ++y) {
        for (int x = 0; x < visibleWidth; ++x) {
            const auto isLastVisibleColumn = (x == visibleWidth - 1);
            const auto isLastVisibleRow = (y == visibleHeight - 1);
            if (settings.showCropMarks() && isCroppedAtBottom && isLastVisibleRow) {
                write(settings.cropMarkBottom());
            } else if (settings.showCropMarks() && isCroppedOnRight && isLastVisibleColumn) {
                write(settings.cropMarkRight());
            } else {
                write(buffer.get(Position{x, y}));
            }
        }
        setDefaultColor();
        lineBreak();
    }
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
