// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Terminal.hpp"

#include "Buffer.hpp"
#include "BufferView.hpp"

#include <algorithm>
#include <cassert>
#include <format>


namespace erbsland::cterm {


Terminal::Terminal() : Terminal(Size{80, 25}, TerminalFlags{}) {
}

Terminal::Terminal(const TerminalFlags flags) : Terminal(Size{80, 25}, flags) {
}

Terminal::Terminal(const Size size, const TerminalFlags flags) :
    _flags{flags}, _size{size.componentMin(cMaximumSize).componentMax(cMinimumSize)} {
    _backend = Backend::createPlatformDefault(flags);
    _input.setBackend(_backend);
    _lineBuffer.setBackend(_backend);
}

Terminal::Terminal(BackendPtr backend, const Size size) :
    _backend{std::move(backend)}, _size{size.componentMin(cMaximumSize).componentMax(cMinimumSize)} {
    if (_backend == nullptr) {
        _backend = Backend::createPlatformDefault(TerminalFlags{});
    }
    _input.setBackend(_backend);
    _lineBuffer.setBackend(_backend);
}

void Terminal::setSize(const Size size) noexcept {
    if (_size != size) {
        _size = size.componentMin(cMaximumSize).componentMax(cMinimumSize);
        _afterResize = true;
    }
}

void Terminal::setOutputMode(const OutputMode outputMode) noexcept {
    if (_outputMode != outputMode) {
        _outputMode = outputMode;
        if (_outputMode == OutputMode::Text) {
            setSizeDetectionEnabled(false);
            setRefreshMode(RefreshMode::Keep);
            setBackBufferEnabled(false);
        }
    }
}

auto Terminal::sizeDetectionEnabled() const noexcept -> bool {
    return _sizeDetectionEnabled;
}

void Terminal::setSizeDetectionEnabled(const bool enabled) noexcept {
    if (_outputMode == OutputMode::Text && enabled) {
        return;
    }
    _sizeDetectionEnabled = enabled;
}

auto Terminal::lineBufferEnabled() const noexcept -> bool {
    return _lineBuffer.cachingEnabled();
}

void Terminal::setLineBufferEnabled(const bool enabled) noexcept {
    if ((_outputMode == OutputMode::Text || !_backend->supportsColorCodes() || !_backend->supportsCursorCodes()) &&
        enabled) {
        return;
    }
    _lineBuffer.setCachingEnabled(enabled);
}

auto Terminal::safeMarginEnabled() const noexcept -> bool {
    return _safeMarginEnabled;
}

void Terminal::setSafeMarginEnabled(const bool enabled) noexcept {
    _safeMarginEnabled = enabled;
    if (_terminalSize == Size{}) {
        return; // ignore if we have no detected terminal size.
    }
    setSize(applySafeMargin(_terminalSize));
}

auto Terminal::backBufferEnabled() const noexcept -> bool {
    return _backBufferEnabled;
}

void Terminal::setBackBufferEnabled(const bool enabled) noexcept {
    if (_outputMode == OutputMode::Text && enabled) {
        return;
    }
    if (_backBufferEnabled != enabled) {
        _backBufferEnabled = enabled;
        if (enabled) {
            // enforce a full rebuild on the next update.
            _backBuffer = {};
            _afterResize = true;
        } else {
            _backBuffer.reset();
        }
    }
}

void Terminal::setBackend(BackendPtr backend) noexcept {
    if (_backend != backend) {
        if (backend == nullptr) {
            _backend = Backend::createPlatformDefault(_flags);
        } else {
            _backend = std::move(backend);
        }
        _input.setBackend(_backend);
        _lineBuffer.setBackend(_backend);
        if (!_backend->supportsColorCodes() || !_backend->supportsCursorCodes()) {
            _lineBuffer.setCachingEnabled(false);
        }
    }
}

auto Terminal::input() noexcept -> Input & {
    return _input;
}

void Terminal::clearScreen() noexcept {
    if (_outputMode == OutputMode::Text) {
        return;
    }
    if (!_backend->supportsCursorCodes()) {
        _backend->clearScreen();
        return;
    }
    _lineBuffer.write("\x1b[2J\x1b[1;1H");
    _lineBuffer.handleEmit();
}
auto Terminal::isAlternateScreenActive() const noexcept -> bool {
    return _isAlternateScreenActive;
}

void Terminal::setAlternateScreen(const bool enabled) noexcept {
    if (_isAlternateScreenActive == enabled) {
        return;
    }
    if (_backend->supportsAlternateScreenBufferCodes()) {
        if (enabled) {
            _lineBuffer.write("\x1b[?1049h");
        } else {
            _lineBuffer.write("\x1b[?1049l");
        }
        _lineBuffer.handleEmit();
        flush(); // make sure this is done before any rendering is started.
    }
    // always notify the backend about the alternate screen buffer switch.
    _backend->setAlternateScreenBuffer(enabled);
    _isAlternateScreenActive = enabled;
}

void Terminal::updateScreen(const ReadableBuffer &buffer, const UpdateSettings &settings) noexcept {
    if (_outputMode == OutputMode::Text) {
        write(buffer);
        flush();
        return;
    }
    if (settings.switchToAlternateBuffer() && !isAlternateScreenActive()) {
        setAlternateScreen(true);
    }
    {
        impl::LineBuffer::EmitLockGuard guard(_lineBuffer);
        updateSizeTooSmallBuffer(settings);
        refreshScreen();
        setAutoWrap(false); // disable auto wrapping.
        if (_sizeTooSmallBuffer) {
            if (backBufferEnabled()) {
                updateScreenWithBackBuffer(*_sizeTooSmallBuffer, UpdateSettings::defaultSettings());
            } else {
                updateScreenWithoutBackBuffer(*_sizeTooSmallBuffer, UpdateSettings::defaultSettings());
            }
        } else {
            if (backBufferEnabled()) {
                updateScreenWithBackBuffer(buffer, settings);
            } else {
                updateScreenWithoutBackBuffer(buffer, settings);
            }
        }
        moveCursor(Position{0, _size.height() - 1}, MoveMode::Absolute);
        setAutoWrap(true); // activate auto wrapping again.
    }
    flush();
}

void Terminal::moveHome() noexcept {
    if (_outputMode == OutputMode::Text) {
        return;
    }
    if (!_backend->supportsCursorCodes()) {
        _backend->moveCursor(Position{0, 0}, MoveMode::Absolute);
        return;
    }
    _lineBuffer.write("\x1b[H");
    _lineBuffer.handleEmit();
}

void Terminal::moveCursor(const Position posOrDelta, const MoveMode mode) noexcept {
    if (!_backend->supportsCursorCodes()) {
        _backend->moveCursor(posOrDelta, mode);
        return;
    }
    if (mode == MoveMode::Absolute) {
        moveTo(posOrDelta);
    } else {
        if (posOrDelta.x() < 0) {
            moveLeft(-posOrDelta.x());
        } else if (posOrDelta.x() > 0) {
            moveRight(posOrDelta.x());
        }
        if (posOrDelta.y() < 0) {
            moveUp(-posOrDelta.y());
        } else if (posOrDelta.y() > 0) {
            moveDown(posOrDelta.y());
        }
    }
}

void Terminal::setAutoWrap(const bool enabled) noexcept {
    if (_outputMode == OutputMode::Text || !_backend->supportsCursorCodes()) {
        return;
    }
    if (enabled) {
        _lineBuffer.write("\x1b[?7h");
    } else {
        _lineBuffer.write("\x1b[?7l");
    }
    _lineBuffer.handleEmit();
}

void Terminal::setCursorVisible(const bool visible) noexcept {
    if (_outputMode == OutputMode::Text) {
        return;
    }
    if (_backend->supportsCursorVisibilityCodes()) {
        _lineBuffer.write(visible ? "\x1b[?25h" : "\x1b[?25l");
    } else {
        _backend->setCursorVisible(visible);
    }
}

void Terminal::setColorEnabled(const bool enabled) noexcept {
    setOutputMode(enabled ? OutputMode::FullControl : OutputMode::Text);
}

void Terminal::refreshScreen() noexcept {
    if (_afterResize) {
        _afterResize = false;
        clearScreen();
        return;
    }
    switch (refreshMode()) {
    case RefreshMode::Clear:
        clearScreen();
        break;
    case RefreshMode::Overwrite:
        moveHome();
        break;
    default:
        break;
    }
}

auto Terminal::updateScreenWithBackBuffer(const ReadableBuffer &buffer, const UpdateSettings &settings) -> void {
    setAutoWrap(false); // disable auto wrapping.
    if (_backBuffer == nullptr) {
        updateScreenAndCreateNewBackBuffer(buffer, settings);
    } else if (_backBuffer->size() != _size) {
        updateScreenAndResizeBackBuffer(buffer, settings);
    } else {
        BufferConstRefView view(buffer, _size);
        settings.applyTo(view);
        const auto differences = _backBuffer->countDifferencesTo(view);
        const auto percent = differences * 100U / static_cast<std::size_t>(buffer.size().area());
        if (percent <= 20) {
            updateScreenPartialWithBackBuffer(view);
        } else {
            writeImpl(view, true);
            _backBuffer->setAndResizeFrom(view);
        }
    }
}

auto Terminal::updateScreenAndCreateNewBackBuffer(const ReadableBuffer &buffer, const UpdateSettings &settings)
    -> void {
    BufferConstRefView view(buffer, _size);
    settings.applyTo(view);
    writeImpl(view, true);
    _backBuffer = view.clone();
}

auto Terminal::updateScreenAndResizeBackBuffer(const ReadableBuffer &buffer, const UpdateSettings &settings) -> void {
    if (_backBuffer == nullptr) {
        throw std::runtime_error("Back buffer is not initialized.");
    }
    BufferConstRefView view(buffer, _size);
    settings.applyTo(view);
    writeImpl(view, true);
    _backBuffer->setAndResizeFrom(view);
}

auto Terminal::updateScreenPartialWithBackBuffer(const ReadableBuffer &view) -> void {
    if (_backBuffer == nullptr) {
        throw std::runtime_error("Back buffer is not initialized.");
    }
    impl::LineBuffer::EmitLockGuard guard(_lineBuffer);
    auto lastWriteCursor = Position{0, 0};
    view.size().forEach([&](const Position pos) -> void {
        const auto &newCharacter = view.get(pos);
        const auto &oldCharacter = _backBuffer->get(pos);
        if (newCharacter == oldCharacter) {
            return;
        }
        if (lastWriteCursor != pos) {
            moveTo(pos);
            lastWriteCursor = pos;
        }
        write(newCharacter);
        _backBuffer->set(pos, newCharacter);
        lastWriteCursor += Position{newCharacter.displayWidth(), 0};
        if (newCharacter.displayWidth() == 2) {
            // if we have a 2-width character, copy the second position as well (it should be empty).
            const auto secondPosition = pos + Position{1, 0};
            _backBuffer->set(secondPosition, view.get(secondPosition));
        }
    });
}

void Terminal::moveTo(const Position pos) noexcept {
    if (_outputMode == OutputMode::Text) {
        return;
    }
    if (!_backend->supportsCursorCodes()) {
        _backend->moveCursor(pos, MoveMode::Absolute);
        return;
    }
    _lineBuffer.write(std::format("\x1b[{};{}H", pos.y() + 1, pos.x() + 1));
    _lineBuffer.handleEmit();
}

auto Terminal::updateScreenWithoutBackBuffer(const ReadableBuffer &buffer, const UpdateSettings &settings) -> void {
    BufferConstRefView view(buffer, _size);
    settings.applyTo(view);
    writeImpl(view, true); // Output the view, line by line.
}

void Terminal::updateSizeTooSmallBuffer(const UpdateSettings &settings) noexcept {
    if (!settings.minimumSize().fitsInto(_size)) {
        if (_sizeTooSmallBuffer == nullptr) {
            _sizeTooSmallBuffer = std::make_unique<Buffer>(_size);
        } else {
            _sizeTooSmallBuffer->resize(_size);
        }
        _sizeTooSmallBuffer->fill(settings.minimumSizeBackground());
        if (!settings.minimumSizeMessage().empty()) {
            _sizeTooSmallBuffer->drawText(
                settings.minimumSizeMessage(), _sizeTooSmallBuffer->rect(), Alignment::Center);
        }
    } else {
        _sizeTooSmallBuffer = {};
    }
}

void Terminal::writeImpl(const ReadableBuffer &buffer, bool withRowMove) noexcept {
    for (int y = 0; y < buffer.size().height(); ++y) {
        if (withRowMove && y != 0) {
            moveTo(Position{0, y});
        }
        for (int x = 0; x < buffer.size().width(); ++x) {
            auto &character = buffer.get(Position{x, y});
            setColor(character.color().fg(), character.color().bg());
            _lineBuffer.write(character);
        }
        if (!withRowMove) {
            setDefaultColor();
            _lineBuffer.write("\n");
        }
    }
}

auto Terminal::applySafeMargin(const Size terminalSize) const noexcept -> Size {
    if (!_safeMarginEnabled) {
        return terminalSize;
    }
    return terminalSize - Size{1, 1};
}

void Terminal::initializeScreen() noexcept {
    _backend->initializePlatform();
    _color = Color::reset();
    testScreenSize();
    setCursorVisible(false);
    flush();
}

void Terminal::testScreenSize() noexcept {
    if (!_sizeDetectionEnabled) {
        return;
    }
    if (const auto detectedSize = _backend->detectScreenSize(); detectedSize.has_value()) {
        _terminalSize = *detectedSize;
        setSize(applySafeMargin(_terminalSize));
    }
}

void Terminal::restoreScreen() noexcept {
    setDefaultColor();
    setCursorVisible(true);
    flush();
    _lineBuffer.shutdown();
    _backBuffer = {}; // free the back buffer resources
    _backend->restorePlatform();
}

void Terminal::write(const Char &character) noexcept {
    const auto resolvedCharacter = character.withBaseColor(_color);
    setColor(resolvedCharacter.color().fg(), resolvedCharacter.color().bg());
    _lineBuffer.write(resolvedCharacter);
    _lineBuffer.handleEmit();
}

void Terminal::write(const String &str) noexcept {
    for (const auto &character : str) {
        const auto resolvedCharacter = character.withBaseColor(_color);
        setColor(resolvedCharacter.color().fg(), resolvedCharacter.color().bg());
        _lineBuffer.write(resolvedCharacter);
    }
    _lineBuffer.handleEmit();
}

void Terminal::write(const std::string_view text) noexcept {
    _lineBuffer.write(text);
    _lineBuffer.handleEmit();
}

void Terminal::write(const ReadableBuffer &buffer) noexcept {
    {
        impl::LineBuffer::EmitLockGuard emitLock{_lineBuffer};
        writeImpl(buffer, false);
    }
    _lineBuffer.handleEmit();
}

void Terminal::writeLineBreak() noexcept {
    _lineBuffer.write("\n");
    _lineBuffer.handleEmit();
}

auto Terminal::color() const noexcept -> Color {
    return _color;
}

void Terminal::setForeground(Foreground color) noexcept {
    if (_outputMode == OutputMode::Text) {
        return;
    }
    if (color == Foreground::Inherited) {
        color = Foreground::Default;
    }
    if (color != _color.fg()) {
        _color.setFg(color);
        if (_backend->supportsColorCodes()) {
            _lineBuffer.write(std::format("\x1b[{}m", color.ansiCode()));
            _lineBuffer.handleEmit();
        } else {
            _backend->emitColor(_color);
        }
    }
}

void Terminal::setBackground(Background color) noexcept {
    if (_outputMode == OutputMode::Text) {
        return;
    }
    if (color == Background::Inherited) {
        color = Background::Default;
    }
    if (color != _color.bg()) {
        _color.setBg(color);
        if (_backend->supportsColorCodes()) {
            _lineBuffer.write(std::format("\x1b[{}m", color.ansiCode()));
            _lineBuffer.handleEmit();
        } else {
            _backend->emitColor(_color);
        }
    }
}

void Terminal::setColor(const Foreground foregroundColor, const Background backgroundColor) noexcept {
    setColor(Color(foregroundColor, backgroundColor));
}

void Terminal::setColor(Color color) noexcept {
    if (_outputMode == OutputMode::Text) {
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
    if (_backend->supportsColorCodes()) {
        if (color.fg() != _color.fg() && color.bg() != _color.bg()) {
            _lineBuffer.write(std::format("\x1b[{};{}m", color.fg().ansiCode(), color.bg().ansiCode()));
        } else if (color.fg() != _color.fg()) {
            _lineBuffer.write(std::format("\x1b[{}m", color.fg().ansiCode()));
        } else if (color.bg() != _color.bg()) {
            _lineBuffer.write(std::format("\x1b[{}m", color.bg().ansiCode()));
        }
        _lineBuffer.handleEmit();
    } else {
        _backend->emitColor(color);
    }
    _color = color;
}

void Terminal::setDefaultColor() noexcept {
    setColor(Color::reset());
}

void Terminal::moveLeft(int count) {
    if (_outputMode == OutputMode::Text) {
        return;
    }
    if (!_backend->supportsCursorCodes()) {
        _backend->moveCursor(Position{-count, 0}, MoveMode::Relative);
        return;
    }
    _lineBuffer.write(std::format("\x1b[{}D", count));
    _lineBuffer.handleEmit();
}

void Terminal::moveRight(int count) {
    if (_outputMode == OutputMode::Text) {
        return;
    }
    if (!_backend->supportsCursorCodes()) {
        _backend->moveCursor(Position{count, 0}, MoveMode::Relative);
        return;
    }
    _lineBuffer.write(std::format("\x1b[{}C", count));
    _lineBuffer.handleEmit();
}

void Terminal::moveUp(int count) {
    if (_outputMode == OutputMode::Text) {
        return;
    }
    if (!_backend->supportsCursorCodes()) {
        _backend->moveCursor(Position{0, -count}, MoveMode::Relative);
        return;
    }
    _lineBuffer.write(std::format("\x1b[{}A", count));
    _lineBuffer.handleEmit();
}

void Terminal::moveDown(int count) {
    if (_outputMode == OutputMode::Text) {
        return;
    }
    if (!_backend->supportsCursorCodes()) {
        _backend->moveCursor(Position{0, count}, MoveMode::Relative);
        return;
    }
    _lineBuffer.write(std::format("\x1b[{}B", count));
    _lineBuffer.handleEmit();
}

void Terminal::flush() noexcept {
    _lineBuffer.handleEmit(true); // force-emit the line buffer.
    _backend->emitFlush();        // flush the output to the terminal.
}

}
