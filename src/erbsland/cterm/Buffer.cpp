// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Buffer.hpp"

#include "Char16Style.hpp"
#include "Tile9Style.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <string_view>
#include <utility>


namespace erbsland::cterm {


Buffer::Buffer() : _size{1, 1}, _data(1U, Char{U' '}) {
}

Buffer::Buffer(const Size size, const Char fillChar) :
    _size{size.componentMin(cMaximumSize)}, _data(static_cast<std::size_t>(std::max(1, _size.area())), fillChar) {
    validateBufferSize(size);
}

auto Buffer::size() const noexcept -> Size {
    return _size;
}

auto Buffer::rect() const noexcept -> Rectangle {
    return Rectangle{Position{0, 0}, _size};
}

auto Buffer::get(const Position pos) const noexcept -> const Char & {
    assert(_size.contains(pos));
    if (!_size.contains(pos)) {
        return Char::space();
    }
    return _data[_size.index(pos)];
}

void Buffer::resize(const Size newSize) {
    resize(newSize, false, Char{}); // fastest possible resize
}

void Buffer::resize(const Size size, const bool reorder, const Char fillChar) {
    if (_size == size) {
        return;
    }
    validateBufferSize(size);
    if (reorder) {
        if (size.area() > _size.area()) {
            // if the data expands, do resize before reordering.
            _data.resize(static_cast<std::size_t>(size.area()));
        }
        if (size.width() < _size.width()) {
            // shrinking horizontally: forward copy should be safe.
            size.forEach([&](const Position pos) -> void {
                if (_size.contains(pos)) {
                    _data[size.index(pos)] = _data[_size.index(pos)];
                } else {
                    _data[size.index(pos)] = fillChar;
                }
            });
        } else {
            // expanding: reverse copy should be safe.
            for (auto y = size.height(); y > 0; --y) {
                for (auto x = size.width(); x > 0; --x) {
                    const auto pos = Position{x - 1, y - 1};
                    if (_size.contains(pos)) {
                        _data[size.index(pos)] = _data[_size.index(pos)];
                    } else {
                        _data[size.index(pos)] = fillChar;
                    }
                }
            }
        }
        if (size.area() < _size.area()) {
            // if the data shrinks, do resize after reordering.
            _data.resize(static_cast<std::size_t>(size.area()));
        }
    } else {
        _data.resize(static_cast<std::size_t>(size.area()));
        if (!fillChar.isEmpty() && size.area() > _size.area()) {
            for (auto i = static_cast<std::size_t>(_size.area()); i < _data.size(); ++i) {
                _data[i] = fillChar;
            }
        }
    }
    _size = size;
}

void Buffer::set(const Position pos, const Char &block) noexcept {
    // faster
    if (!_size.contains(pos) || block.displayWidth() == 0 || block.displayWidth() > 2) {
        return;
    }
    if (block.displayWidth() == 1) {
        _data[_size.index(pos)] = block;
    } else {
        const auto secondPosition = pos + Position{1, 0};
        if (!_size.contains(secondPosition)) {
            return;
        }
        _data[_size.index(secondPosition)] = Char{"", block.style()};
        _data[_size.index(pos)] = block;
    }
}

void Buffer::fill(const Char &fillBlock) noexcept {
    // faster
    for (auto &dataBlock : _data) {
        dataBlock = fillBlock;
    }
}

auto Buffer::clone() const -> WritableBufferPtr {
    return std::make_shared<Buffer>(*this);
}

void Buffer::setAndResizeFrom(const ReadableBuffer &other) {
    // if we do a copy `Buffer -> Buffer`, use a fast path.
    if (const auto bufferImpl = dynamic_cast<Buffer const *>(&other); bufferImpl != nullptr) {
        _size = bufferImpl->_size;
        _data.resize(bufferImpl->_data.size());
        std::ranges::copy(bufferImpl->_data, _data.begin());
        return;
    }
    // fallback to the original safe implementation.
    WritableBuffer::setAndResizeFrom(other);
}

auto Buffer::fromLinesInString(const String &text) -> Buffer {
    if (text.empty()) {
        throw std::invalid_argument{"text is empty"};
    }
    return fromLines(text.splitLines());
}

auto Buffer::fromLines(const StringLines &lines) -> Buffer {
    if (lines.empty()) {
        throw std::invalid_argument{"lines is empty"};
    }
    Size size{1, static_cast<Coordinate>(lines.size())};
    for (const auto &line : lines) {
        if (size.width() < line.displayWidth()) {
            size.setWidth(line.displayWidth());
        }
    }
    auto buffer = Buffer{size};
    Position pos{0, 0};
    for (const auto &line : lines) {
        buffer.set(pos, line);
        pos += Position{0, 1};
    }
    return buffer;
}

void Buffer::validateBufferSize(const Size size) {
    if (size.width() < 1 || size.height() < 1) {
        throw std::invalid_argument("Buffer size must be at least 1x1");
    }
    if (!size.fitsInto(cMaximumSize)) {
        throw std::invalid_argument("Buffer size must not exceed 10'000x10'000");
    }
}

void Buffer::drawText(
    const std::string_view text,
    const Alignment alignment,
    const Rectangle rect,
    const Color color,
    const std::size_t animationCycle) {

    // for backward compatibility
    auto renderedText = Text{String{text}, rect, alignment};
    renderedText.setColor(color);
    drawText(renderedText, animationCycle);
}


}
