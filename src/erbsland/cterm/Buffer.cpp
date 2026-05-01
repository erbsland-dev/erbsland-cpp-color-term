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
    _size{validatedBufferSize(size)}, _data(static_cast<std::size_t>(_size.area()), fillChar) {
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
    resize(newSize, BufferResizeMode::Fast, Char{}); // fastest possible resize
}

void Buffer::resize(const Size size, const BufferResizeMode mode, const Char fillChar) {
    if (_size == size) {
        return;
    }
    const auto validatedSize = validatedBufferSize(size);
    if (mode != BufferResizeMode::Fast) {
        if (validatedSize.area() > _size.area()) {
            // if the data expands, do resize before reordering.
            _data.resize(static_cast<std::size_t>(validatedSize.area()));
        }
        if (validatedSize.width() < _size.width()) {
            // shrinking horizontally: forward copy should be safe.
            validatedSize.forEach([&](const Position pos) -> void {
                if (_size.contains(pos)) {
                    _data[validatedSize.index(pos)] = _data[_size.index(pos)];
                } else {
                    _data[validatedSize.index(pos)] = fillChar;
                }
            });
        } else {
            // expanding: reverse copy should be safe.
            for (auto y = validatedSize.height(); y > 0; --y) {
                for (auto x = validatedSize.width(); x > 0; --x) {
                    const auto pos = Position{x - 1, y - 1};
                    if (_size.contains(pos)) {
                        _data[validatedSize.index(pos)] = _data[_size.index(pos)];
                    } else {
                        _data[validatedSize.index(pos)] = fillChar;
                    }
                }
            }
        }
        if (validatedSize.area() < _size.area()) {
            // if the data shrinks, do resize after reordering.
            _data.resize(static_cast<std::size_t>(validatedSize.area()));
        }
    } else {
        _data.resize(static_cast<std::size_t>(validatedSize.area()));
        if (!fillChar.isEmpty() && validatedSize.area() > _size.area()) {
            for (auto i = static_cast<std::size_t>(_size.area()); i < _data.size(); ++i) {
                _data[i] = fillChar;
            }
        }
    }
    _size = validatedSize;
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
        // The continuation cell for a wide character must stay logically empty while preserving the style.
        _data[_size.index(secondPosition)] = Char::emptyBlock(block.style());
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
        _size = validatedBufferSize(bufferImpl->_size);
        _data.resize(bufferImpl->_data.size());
        std::ranges::copy(bufferImpl->_data, _data.begin());
        return;
    }
    // fallback to the original safe implementation.
    WritableBuffer::setAndResizeFrom(other);
}

auto Buffer::fromLinesInString(const StringView &text) -> Buffer {
    if (text.empty()) {
        throw std::invalid_argument{"text is empty"};
    }
    auto lines = StringLines{};
    for (const auto &line : text.splitLines()) {
        lines.emplace_back(line);
    }
    return fromLines(lines);
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

auto Buffer::validatedBufferSize(const Size size) -> Size {
    if (size.width() < 1 || size.height() < 1) {
        throw std::invalid_argument("Buffer size must be at least 1x1");
    }
    if (!size.fitsInto(cMaximumSize)) {
        throw std::invalid_argument("Buffer size must not exceed 10'000x10'000");
    }
    return size;
}

void Buffer::drawText(
    const std::string_view text,
    const Alignment alignment,
    const Rectangle rect,
    const Color color,
    const std::size_t animationCycle) {

    // for backward compatibility
    auto renderedText = Text{String{text, EncodingErrors::Replace}, rect, alignment};
    renderedText.setColor(color);
    drawText(renderedText, animationCycle);
}

}
