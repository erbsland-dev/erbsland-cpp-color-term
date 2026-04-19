// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "CursorBuffer.hpp"

#include "Buffer.hpp"

#include "impl/paragraph/Layout.hpp"
#include "impl/paragraph/Printer.hpp"

namespace erbsland::cterm {

void CursorBuffer::validateFillChar(const Char &fillChar) {
    if (fillChar.displayWidth() != 1) {
        throw std::invalid_argument{"fillChar must be a single-width character."};
    }
}

auto CursorBuffer::color() const noexcept -> Color {
    return _currentColor;
}

auto CursorBuffer::charAttributes() const noexcept -> CharAttributes {
    return _currentCharAttributes;
}

void CursorBuffer::setColor(Color color) noexcept {
    if (color.fg() == Foreground::Inherited) {
        color.setFg(Foreground::Default);
    }
    if (color.bg() == Background::Inherited) {
        color.setBg(Background::Default);
    }
    _currentColor = color;
}

void CursorBuffer::setCharAttributes(const CharAttributes attributes) noexcept {
    _currentCharAttributes = attributes.resolvedWith(CharAttributes::reset());
}

auto CursorBuffer::maximumSize() const noexcept -> Size {
    return _maximumSize;
}

void CursorBuffer::setMaximumSize(Size maximumSize) noexcept {
    _maximumSize = maximumSize;
}

auto CursorBuffer::overflowMode() const noexcept -> OverflowMode {
    return _overflowMode;
}

void CursorBuffer::setOverflowMode(const OverflowMode mode) noexcept {
    _overflowMode = mode;
}

auto CursorBuffer::fillChar() const noexcept -> const Char & {
    return _fillChar;
}

void CursorBuffer::setFillChar(const Char fillChar) {
    validateFillChar(fillChar);
    _fillChar = fillChar;
}

void CursorBuffer::setForeground(Foreground color) noexcept {
    if (color == Foreground::Inherited) {
        color = Foreground::Default;
    }
    _currentColor.setFg(color);
}

void CursorBuffer::setBackground(Background color) noexcept {
    if (color == Background::Inherited) {
        color = Background::Default;
    }
    _currentColor.setBg(color);
}

auto CursorBuffer::supportedCharAttributes() const noexcept -> CharAttributes {
    return CharAttributes::all();
}

void CursorBuffer::moveCursor(const Position posOrDelta, const MoveMode mode) noexcept {
    if (std::abs(posOrDelta.x()) > cMaximumSize.width() || std::abs(posOrDelta.y()) > cMaximumSize.height()) {
        return; // ignore calls with extreme values.
    }
    if (posOrDelta == Position{0, 0}) {
        return;
    }
    _wrapOnNextChar = false;
    if (mode == MoveMode::Absolute) {
        _cursorPosition = rect().clamp(posOrDelta);
    } else {
        _cursorPosition = rect().clamp(_cursorPosition + posOrDelta);
    }
}

void CursorBuffer::setAutoWrap(const bool enabled) noexcept {
    _autoWrap = enabled;
}

void CursorBuffer::clearScreen() noexcept {
    fill(_fillChar);
    _cursorPosition = Position{};
    _wrapOnNextChar = false;
}

void CursorBuffer::writeResolvedCharacter(const Char &character) noexcept {
    const auto displayWidth = character.displayWidth();
    if (displayWidth == 0 || displayWidth > 2) {
        return;
    }
    auto x = _cursorPosition.x();
    auto y = _cursorPosition.y();
    auto width = _size.width();
    if (_wrapOnNextChar || x >= width) {
        if (_autoWrap) {
            writeLineBreak();
        } else {
            _cursorPosition = Position{width - 1, y};
        }
        _wrapOnNextChar = false;
        x = _cursorPosition.x();
        y = _cursorPosition.y();
        width = _size.width();
    }
    const auto lastColumn = width - 1;
    if (x == lastColumn) {
        if (displayWidth == 2) {
            if (!_autoWrap) {
                return;
            }
            writeLineBreak();
            x = _cursorPosition.x();
            y = _cursorPosition.y();
            set(Position{x, y}, character);
            _cursorPosition = Position{x + displayWidth, y};
        } else {
            set(Position{x, y}, character);
            _wrapOnNextChar = _autoWrap;
        }
    } else {
        set(Position{x, y}, character);
        x += displayWidth;
        if (x >= width) {
            x = width - 1;
            _wrapOnNextChar = _autoWrap;
        }
        _cursorPosition = Position{x, y};
    }
}

void CursorBuffer::write(const Char &character) noexcept {
    writeResolvedCharacter(character.withBase(_currentColor, _currentCharAttributes));
}

void CursorBuffer::write(const StringView &str) noexcept {
    const auto color = _currentColor;
    const auto attributes = _currentCharAttributes;
    for (const auto &character : str) {
        writeResolvedCharacter(character.withBase(color, attributes));
    }
}

void CursorBuffer::writeResolved(const Char &character) noexcept {
    writeResolvedCharacter(character);
}

void CursorBuffer::writeResolved(const StringView &str) noexcept {
    for (const auto &character : str) {
        writeResolvedCharacter(character);
    }
}

void CursorBuffer::write(const ReadableBuffer &buffer) noexcept {
    for (Coordinate y = 0; y < buffer.size().height(); ++y) {
        for (Coordinate x = 0; x < buffer.size().width(); ++x) {
            write(buffer.get(Position{x, y}));
        }
        writeLineBreak();
    }
}

void CursorBuffer::writeLineBreak() noexcept {
    _wrapOnNextChar = false;
    if (_cursorPosition.y() < _size.height() - 1) {
        _cursorPosition = Position{0, _cursorPosition.y() + 1};
    } else {
        switch (_overflowMode) {
        case OverflowMode::Wrap:
            _cursorPosition = Position{0, 0};
            break;
        case OverflowMode::Shift:
            shift(Direction::North, _fillChar, 1);
            _cursorPosition = Position{0, _size.height() - 1};
            break;
        case OverflowMode::ExpandThenShift:
            if (_size.height() < _maximumSize.height()) {
                resize(_size + Size{0, 1}, BufferResizeMode::PreserveContent, _fillChar);
            } else {
                shift(Direction::North, _fillChar, 1);
            }
            _cursorPosition = Position{0, _size.height() - 1};
            break;
        case OverflowMode::ExpandThenWrap:
            if (_size.height() < _maximumSize.height()) {
                resize(_size + Size{0, 1}, BufferResizeMode::PreserveContent, _fillChar);
                _cursorPosition = Position{0, _size.height() - 1};
            } else {
                _cursorPosition = Position{0, 0};
            }
        }
    }
}

auto CursorBuffer::printParagraphImpl(const StringView &paragraph, const ParagraphOptions &options) noexcept -> int {
    const auto margins = options.margins();
    const auto x1 = std::max(margins.left(), 0);
    const auto width = std::max(size().width() - std::max(margins.left(), 0) - std::max(margins.right(), 0), 0);
    const auto layout =
        impl::paragraph::Layout{paragraph, width, options, impl::paragraph::LayoutNewlineMode::HardLineBreak}.build();
    if (!layout.valid()) {
        if (options.onError() == ParagraphOnError::Empty) {
            return 0;
        }
        write(paragraph);
        auto lineCount = std::max(paragraph.terminalLines(std::max(width, 1)), 1);
        writeLineBreak();
        if (options.paragraphSpacing() == ParagraphSpacing::DoubleLine) {
            writeLineBreak();
            lineCount += 1;
        }
        return lineCount;
    }
    if (layout.empty()) {
        writeLineBreak();
        auto lineCount = 1;
        if (options.paragraphSpacing() == ParagraphSpacing::DoubleLine) {
            writeLineBreak();
            lineCount += 1;
        }
        return lineCount;
    }
    const auto lineCount =
        impl::paragraph::Printer{
            *this, x1, width, options.alignment(), layout, paragraph, options, options.backgroundMode()}
            .print();
    if (options.paragraphSpacing() == ParagraphSpacing::DoubleLine) {
        writeLineBreak();
        return lineCount + 1;
    }
    return lineCount;
}

}
