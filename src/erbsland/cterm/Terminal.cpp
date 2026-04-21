// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Terminal.hpp"

#include "Buffer.hpp"
#include "BufferView.hpp"

#include "impl/paragraph/Layout.hpp"
#include "impl/paragraph/Printer.hpp"
#include "text/Style.hpp"

#include <algorithm>
#include <cassert>
#include <format>
#include <vector>

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
    if ((_outputMode == OutputMode::Text || !canUseLineBuffer()) && enabled) {
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
        if (!canUseLineBuffer()) {
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

void Terminal::writeImpl(const ReadableBuffer &buffer, const bool withRowMove) noexcept {
    for (Coordinate y = 0; y < buffer.size().height(); ++y) {
        if (withRowMove && y != 0) {
            moveTo(Position{0, y});
        }
        for (Coordinate x = 0; x < buffer.size().width(); ++x) {
            auto &character = buffer.get(Position{x, y});
            setStyle(character.style());
            _lineBuffer.write(character);
        }
        if (!withRowMove) {
            setDefaultColor();
            setCharAttributes(CharAttributes::reset());
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
    _style = CharStyle::reset();
    testScreenSize();
    setCursorVisible(false);
    flush();
}

auto Terminal::isInteractive() const noexcept -> bool {
    return _backend->isInteractive();
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
    setCharAttributes(CharAttributes::reset());
    setCursorVisible(true);
    if (isAlternateScreenActive()) {
        setAlternateScreen(false);
    }
    flush();
    _lineBuffer.shutdown();
    _backBuffer = {}; // free the back buffer resources
    _backend->restorePlatform();
}

void Terminal::write(const Char &character) noexcept {
    const auto resolvedCharacter = character.withBase(_style);
    setStyle(resolvedCharacter.style());
    _lineBuffer.write(resolvedCharacter);
    _lineBuffer.handleEmit();
}

void Terminal::write(const StringView &str) noexcept {
    for (const auto &character : str) {
        const auto resolvedCharacter = character.withBase(_style);
        setStyle(resolvedCharacter.style());
        _lineBuffer.write(resolvedCharacter);
    }
    _lineBuffer.handleEmit();
}

void Terminal::writeResolved(const Char &character) noexcept {
    setStyle(character.style());
    _lineBuffer.write(character);
    _lineBuffer.handleEmit();
}

void Terminal::writeResolved(const StringView &str) noexcept {
    for (const auto &character : str) {
        setStyle(character.style());
        _lineBuffer.write(character);
    }
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

auto Terminal::printParagraphImpl(const StringView &paragraph, const ParagraphOptions &options) noexcept -> int {
    const auto margins = options.margins();
    const auto x1 = std::max(margins.left(), 0);
    const auto width = std::max(size().width() - std::max(margins.left(), 0) - std::max(margins.right(), 0), 0);
    const auto layout =
        impl::paragraph::Layout{paragraph, width, options, impl::paragraph::LayoutNewlineMode::HardLineBreak}.build();
    if (!layout.valid()) {
        return printParagraphPlainOutput(paragraph, options);
    }
    if (layout.empty()) {
        return finishParagraphWithExplicitLineBreaks(0, options.paragraphSpacing());
    }
    const auto lineCount = [&]() -> int {
        impl::LineBuffer::EmitLockGuard emitLock{_lineBuffer};
        return impl::paragraph::Printer{
            *this, x1, width, options.alignment(), layout, paragraph, options, options.backgroundMode()}
            .print();
    }();
    _lineBuffer.handleEmit();
    if (options.paragraphSpacing() == ParagraphSpacing::DoubleLine) {
        writeLineBreak();
        return lineCount + 1;
    }
    return lineCount;
}

auto Terminal::printParagraphPlainOutput(const StringView &paragraph, const ParagraphOptions &options) noexcept -> int {
    if (options.onError() == ParagraphOnError::Empty) {
        return 0;
    }
    write(paragraph);
    const auto margins = options.margins();
    const auto width = std::max(size().width() - std::max(margins.left(), 0) - std::max(margins.right(), 0), 1);
    return finishParagraphWithExplicitLineBreaks(paragraph.terminalLines(width), options.paragraphSpacing());
}

auto Terminal::finishParagraphWithExplicitLineBreaks(
    const int renderedLines, const ParagraphSpacing paragraphSpacing) noexcept -> int {
    writeLineBreak();
    auto totalLines = std::max(renderedLines, 1);
    if (paragraphSpacing == ParagraphSpacing::DoubleLine) {
        writeLineBreak();
        totalLines += 1;
    }
    return totalLines;
}

auto Terminal::color() const noexcept -> Color {
    return _style.color();
}

auto Terminal::charAttributes() const noexcept -> CharAttributes {
    return _style.attributes();
}

auto Terminal::supportedCharAttributes() const noexcept -> CharAttributes {
    return CharAttributes::fromMask(_backend->supportedCharAttributes().mask());
}

void Terminal::setForeground(Foreground color) noexcept {
    if (_outputMode == OutputMode::Text) {
        return;
    }
    if (color == Foreground::Inherited) {
        color = Foreground::Default;
    }
    if (color != _style.fg()) {
        _style.setFg(color);
        if (_backend->supportsColorCodes()) {
            _lineBuffer.write(std::format("\x1b[{}m", color.ansiCode()));
            _lineBuffer.handleEmit();
        } else {
            _backend->emitColor(_style.color());
        }
    }
}

void Terminal::setCharAttributes(CharAttributes attributes) noexcept {
    if (_outputMode == OutputMode::Text) {
        return;
    }
    attributes = normalizedSupportedAttributes(attributes);
    if (attributes == _style.attributes()) {
        return;
    }
    const auto supportedCodeMask = static_cast<uint8_t>(
        _backend->supportedCharAttributes().mask() & _backend->supportedCharAttributeCodes().mask());
    const auto previousCodeAttributes =
        CharAttributes::fromMasks(_style.attributes().enabledMask() & supportedCodeMask, supportedCodeMask);
    const auto newCodeAttributes =
        CharAttributes::fromMasks(attributes.enabledMask() & supportedCodeMask, supportedCodeMask);
    if (previousCodeAttributes != newCodeAttributes) {
        emitCharAttributeCodes(previousCodeAttributes, newCodeAttributes);
    }
    const auto callbackMask = static_cast<uint8_t>(_backend->supportedCharAttributes().mask() & ~supportedCodeMask);
    if (((_style.attributes().enabledMask() ^ attributes.enabledMask()) & callbackMask) != 0) {
        flush();
        _backend->emitCharAttributes(CharAttributes::fromMasks(attributes.enabledMask() & callbackMask, callbackMask));
    }
    _style.setAttributes(attributes);
}

void Terminal::setBackground(Background color) noexcept {
    if (_outputMode == OutputMode::Text) {
        return;
    }
    if (color == Background::Inherited) {
        color = Background::Default;
    }
    if (color != _style.bg()) {
        _style.setBg(color);
        if (_backend->supportsColorCodes()) {
            _lineBuffer.write(std::format("\x1b[{}m", color.ansiCode()));
            _lineBuffer.handleEmit();
        } else {
            _backend->emitColor(_style.color());
        }
    }
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
    if (color == _style.color()) {
        return;
    }
    if (_backend->supportsColorCodes()) {
        if (color.fg() != _style.fg() && color.bg() != _style.bg()) {
            _lineBuffer.write(std::format("\x1b[{};{}m", color.fg().ansiCode(), color.bg().ansiCode()));
        } else if (color.fg() != _style.fg()) {
            _lineBuffer.write(std::format("\x1b[{}m", color.fg().ansiCode()));
        } else if (color.bg() != _style.bg()) {
            _lineBuffer.write(std::format("\x1b[{}m", color.bg().ansiCode()));
        }
        _lineBuffer.handleEmit();
    } else {
        _backend->emitColor(color);
    }
    _style.setColor(color);
}

auto Terminal::canUseLineBuffer() const noexcept -> bool {
    return _backend->supportsColorCodes() && _backend->supportsCursorCodes() &&
        _backend->supportedCharAttributes().mask() == _backend->supportedCharAttributeCodes().mask();
}

auto Terminal::normalizedSupportedAttributes(CharAttributes attributes) const noexcept -> CharAttributes {
    const auto supportedMask = _backend->supportedCharAttributes().mask();
    attributes = attributes.withBase(CharAttributes::reset());
    return CharAttributes::fromMasks(attributes.enabledMask() & supportedMask, supportedMask)
        .withBase(CharAttributes::reset());
}

void Terminal::emitCharAttributeCodes(
    const CharAttributes previousAttributes, const CharAttributes newAttributes) noexcept {
    auto codes = std::vector<int>{};
    if (previousAttributes.isBold() != newAttributes.isBold() || previousAttributes.isDim() != newAttributes.isDim()) {
        if (!newAttributes.isBold() && !newAttributes.isDim()) {
            codes.push_back(22);
        } else {
            if (previousAttributes.isBold() || previousAttributes.isDim()) {
                codes.push_back(22);
            }
            if (newAttributes.isBold()) {
                codes.push_back(1);
            }
            if (newAttributes.isDim()) {
                codes.push_back(2);
            }
        }
    }
    if (previousAttributes.isItalic() != newAttributes.isItalic()) {
        codes.push_back(newAttributes.isItalic() ? 3 : 23);
    }
    if (previousAttributes.isUnderline() != newAttributes.isUnderline()) {
        codes.push_back(newAttributes.isUnderline() ? 4 : 24);
    }
    if (previousAttributes.isBlink() != newAttributes.isBlink()) {
        codes.push_back(newAttributes.isBlink() ? 5 : 25);
    }
    if (previousAttributes.isReverse() != newAttributes.isReverse()) {
        codes.push_back(newAttributes.isReverse() ? 7 : 27);
    }
    if (previousAttributes.isHidden() != newAttributes.isHidden()) {
        codes.push_back(newAttributes.isHidden() ? 8 : 28);
    }
    if (previousAttributes.isStrikethrough() != newAttributes.isStrikethrough()) {
        codes.push_back(newAttributes.isStrikethrough() ? 9 : 29);
    }
    if (codes.empty()) {
        return;
    }
    auto sequence = std::string{"\x1b["};
    for (auto index = std::size_t{0}; index < codes.size(); ++index) {
        if (index != 0) {
            sequence += ';';
        }
        sequence += std::to_string(codes[index]);
    }
    sequence += 'm';
    _lineBuffer.write(sequence);
    _lineBuffer.handleEmit();
}

void Terminal::moveLeft(const Coordinate count) noexcept {
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

void Terminal::moveRight(const Coordinate count) noexcept {
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

void Terminal::moveUp(const Coordinate count) noexcept {
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

void Terminal::moveDown(const Coordinate count) noexcept {
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
