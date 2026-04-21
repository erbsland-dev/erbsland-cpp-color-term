// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "RemappedBuffer.hpp"

#include <algorithm>
#include <cassert>
#include <format>
#include <ranges>

namespace erbsland::cterm {

RemappedBuffer::RemappedBuffer() : RemappedBuffer{Size{1, 1}, Orientation::Vertical, Char{U' '}} {
}

RemappedBuffer::RemappedBuffer(const Size size, Orientation orientation, const Char fillChar) :
    _size{validatedBufferSize(size)},
    _orientation{orientation},
    _buffer(static_cast<std::size_t>(_size.area()), fillChar),
    _rowRemap(linearIndex(static_cast<std::size_t>(_size.height()))),
    _columnRemap(linearIndex(static_cast<std::size_t>(_size.width()))) {
}

auto RemappedBuffer::size() const noexcept -> Size {
    return _size;
}

auto RemappedBuffer::rect() const noexcept -> Rectangle {
    return Rectangle{Position{0, 0}, _size};
}

auto RemappedBuffer::get(const Position pos) const noexcept -> const Char & {
    assert(_size.contains(pos));
    if (!_size.contains(pos)) {
        return Char::space();
    }
    return _buffer[bufferIndex(pos.x(), pos.y())];
}

auto RemappedBuffer::clone() const -> WritableBufferPtr {
    return std::make_shared<RemappedBuffer>(*this);
}

void RemappedBuffer::resize(const Size newSize) {
    resize(newSize, BufferResizeMode::Fast, Char{});
}

void RemappedBuffer::resize(const Size newSize, const BufferResizeMode mode, const Char fillChar) {
    const auto validatedSize = validatedBufferSize(newSize);
    if (_size == validatedSize) {
        return;
    }
    if (mode == BufferResizeMode::PreserveContent) {
        if (isPrimaryAxisOnlyResize(validatedSize)) {
            primaryAxisResize(validatedSize, fillChar);
        } else {
            reorderedResize(validatedSize, fillChar);
        }
        return;
    }
    fastResize(validatedSize, fillChar);
}

void RemappedBuffer::set(const Position pos, const Char &block) noexcept {
    const auto displayWidth = block.displayWidth();
    if (!_size.contains(pos) || displayWidth == 0 || displayWidth > 2) {
        return;
    }
    if (displayWidth == 1) {
        _buffer[bufferIndex(pos.x(), pos.y())] = block;
        return;
    }
    const auto secondPosition = pos + Position{1, 0};
    if (!_size.contains(secondPosition)) {
        return;
    }
    // The continuation cell for a wide character must stay logically empty while preserving the style.
    _buffer[bufferIndex(secondPosition.x(), secondPosition.y())] = Char::emptyBlock(block.style());
    _buffer[bufferIndex(pos.x(), pos.y())] = block;
}

void RemappedBuffer::reserve(Size size) noexcept {
    _buffer.reserve(static_cast<std::size_t>(size.area()));
    _rowRemap.reserve(static_cast<std::size_t>(size.height()));
    _columnRemap.reserve(static_cast<std::size_t>(size.width()));
}

void RemappedBuffer::shift(const Direction direction, const Char fillChar, const int count) {
    validateDirectionalCount(direction, count);
    if (count == 0 || direction == Direction::None) {
        return;
    }
    if (direction.contains(Direction::North)) {
        eraseRows(0, fillChar, count);
    }
    if (direction.contains(Direction::South)) {
        insertRows(0, fillChar, count);
    }
    if (direction.contains(Direction::West)) {
        eraseColumns(0, fillChar, count);
    }
    if (direction.contains(Direction::East)) {
        insertColumns(0, fillChar, count);
    }
}

void RemappedBuffer::rotate(const Direction direction, const int count) {
    validateDirectionalCount(direction, count);
    if (count == 0 || direction == Direction::None) {
        return;
    }
    if (direction.contains(Direction::North)) {
        rotateMap(_rowRemap, count, true);
    }
    if (direction.contains(Direction::South)) {
        rotateMap(_rowRemap, count, false);
    }
    if (direction.contains(Direction::West)) {
        rotateMap(_columnRemap, count, true);
    }
    if (direction.contains(Direction::East)) {
        rotateMap(_columnRemap, count, false);
    }
}

void RemappedBuffer::eraseRows(const Coordinate startRow, const Char fillChar, const int count) {
    validateExistingSpan(startRow, count, _size.height(), "startRow", "count");
    if (count == 0) {
        return;
    }
    fillStoredRows(eraseFromMap(_rowRemap, startRow, count), fillChar);
}

void RemappedBuffer::eraseColumns(const Coordinate startColumn, const Char fillChar, const int count) {
    validateExistingSpan(startColumn, count, _size.width(), "startColumn", "count");
    if (count == 0) {
        return;
    }
    fillStoredColumns(eraseFromMap(_columnRemap, startColumn, count), fillChar);
}

void RemappedBuffer::insertRows(const Coordinate startRow, const Char fillChar, const int count) {
    validateInsertArguments(startRow, count, _size.height(), "startRow", "count");
    if (count == 0) {
        return;
    }
    fillStoredRows(insertIntoMap(_rowRemap, startRow, count), fillChar);
}

void RemappedBuffer::insertColumns(const Coordinate startColumn, const Char fillChar, const int count) {
    validateInsertArguments(startColumn, count, _size.width(), "startColumn", "count");
    if (count == 0) {
        return;
    }
    fillStoredColumns(insertIntoMap(_columnRemap, startColumn, count), fillChar);
}

void RemappedBuffer::moveRows(const Coordinate startRow, const int count, const Coordinate delta, const Char fillChar) {
    validateExistingSpan(startRow, count, _size.height(), "startRow", "count");
    if (count == 0 || delta == 0) {
        return;
    }
    fillStoredRows(moveInMap(_rowRemap, startRow, count, delta), fillChar);
}

void RemappedBuffer::moveColumns(
    const Coordinate startColumn, const int count, const Coordinate delta, const Char fillChar) {
    validateExistingSpan(startColumn, count, _size.width(), "startColumn", "count");
    if (count == 0 || delta == 0) {
        return;
    }
    fillStoredColumns(moveInMap(_columnRemap, startColumn, count, delta), fillChar);
}

void RemappedBuffer::fill(const Char &fillBlock) noexcept {
    for (auto &character : _buffer) {
        character = fillBlock;
    }
    for (std::size_t i = 0; i < _rowRemap.size(); ++i) {
        _rowRemap[i] = static_cast<Coordinate>(i);
    }
    for (std::size_t i = 0; i < _columnRemap.size(); ++i) {
        _columnRemap[i] = static_cast<Coordinate>(i);
    }
}

auto RemappedBuffer::validatedBufferSize(const Size size) -> Size {
    if (size.width() < 1 || size.height() < 1) {
        throw std::invalid_argument("Buffer size must be at least 1x1");
    }
    if (!size.fitsInto(cMaximumSize)) {
        throw std::invalid_argument("Buffer size must not exceed 10'000x10'000");
    }
    return size;
}

auto RemappedBuffer::linearIndex(std::size_t size) -> CoordinateMap {
    CoordinateMap result;
    result.resize(size);
    for (std::size_t index = 0; index < size; ++index) {
        result[index] = static_cast<Coordinate>(index);
    }
    return result;
}

void RemappedBuffer::validateCount(const int count, const int maximum, const std::string_view parameterName) {
    if (count < 0) {
        throw std::invalid_argument(std::format("{} must not be negative", parameterName));
    }
    if (count > maximum) {
        throw std::invalid_argument(std::format("{} must not exceed the buffer dimension", parameterName));
    }
}

void RemappedBuffer::validateExistingSpan(
    const Coordinate start,
    const int count,
    const int limit,
    const std::string_view startName,
    const std::string_view countName) {
    validateCount(count, limit, countName);
    if (count == 0) {
        if (start < 0 || start > limit) {
            throw std::invalid_argument(std::format("{} is out of bounds", startName));
        }
        return;
    }
    if (start < 0 || start >= limit) {
        throw std::invalid_argument(std::format("{} is out of bounds", startName));
    }
    if (start + count > limit) {
        throw std::invalid_argument(std::format("{} exceeds the remaining buffer dimension", countName));
    }
}

void RemappedBuffer::validateInsertArguments(
    const Coordinate start,
    const int count,
    const int limit,
    const std::string_view startName,
    const std::string_view countName) {
    validateCount(count, limit, countName);
    if (count == 0) {
        if (start < 0 || start > limit) {
            throw std::invalid_argument(std::format("{} is out of bounds", startName));
        }
        return;
    }
    if (start < 0 || start >= limit) {
        throw std::invalid_argument(std::format("{} is out of bounds", startName));
    }
    if (start + count > limit) {
        throw std::invalid_argument(std::format("{} exceeds the remaining buffer dimension", countName));
    }
}

void RemappedBuffer::validateDirectionalCount(const Direction direction, const int count) const {
    if (count < 0) {
        throw std::invalid_argument("count must not be negative");
    }
    if (direction.contains(Direction::North) || direction.contains(Direction::South)) {
        validateCount(count, _size.height(), "count");
    }
    if (direction.contains(Direction::West) || direction.contains(Direction::East)) {
        validateCount(count, _size.width(), "count");
    }
}

auto RemappedBuffer::bufferIndex(const Position pos, const Size size, const Orientation orientation) noexcept
    -> std::size_t {
    const auto crossAxis = orientation.crossed();
    return static_cast<std::size_t>(pos.coordinate(orientation)) *
        static_cast<std::size_t>(size.coordinate(crossAxis)) +
        static_cast<std::size_t>(pos.coordinate(crossAxis));
}

void RemappedBuffer::rotateMap(CoordinateMap &map, const int count, const bool towardFront) noexcept {
    if (map.empty() || count == 0) {
        return;
    }
    const auto normalizedCount = static_cast<std::size_t>(count) % map.size();
    if (normalizedCount == 0) {
        return;
    }
    if (towardFront) {
        std::ranges::rotate(map, map.begin() + static_cast<std::ptrdiff_t>(normalizedCount));
        return;
    }
    std::ranges::rotate(map, map.end() - static_cast<std::ptrdiff_t>(normalizedCount));
}

auto RemappedBuffer::eraseFromMap(CoordinateMap &map, const Coordinate start, const int count)
    -> std::span<const Coordinate> {
    const auto recycledSize = static_cast<std::size_t>(count);
    const auto first = map.begin() + start;
    const auto last = first + count;
    std::ranges::rotate(first, last, map.end());
    return {map.data() + (map.size() - recycledSize), recycledSize};
}

auto RemappedBuffer::insertIntoMap(CoordinateMap &map, const Coordinate start, const int count)
    -> std::span<const Coordinate> {
    const auto recycledSize = static_cast<std::size_t>(count);
    std::ranges::rotate(map.begin() + start, map.end() - count, map.end());
    return {map.data() + static_cast<std::size_t>(start), recycledSize};
}

auto RemappedBuffer::moveInMap(CoordinateMap &map, const Coordinate start, const int count, const Coordinate delta)
    -> std::span<const Coordinate> {
    const auto targetStart = start + delta;
    const auto droppedBefore = std::clamp(-targetStart, 0, count);
    const auto droppedAfter = std::clamp(targetStart + count - static_cast<Coordinate>(map.size()), 0, count);

    if (droppedBefore > 0) {
        const auto recycledSize = static_cast<std::size_t>(droppedBefore);
        const auto keptSize = static_cast<std::ptrdiff_t>(count - droppedBefore);
        std::ranges::rotate(map.begin() + start, map.begin() + start + droppedBefore, map.end());
        std::ranges::rotate(map.begin(), map.begin() + start, map.begin() + start + keptSize);
        return {map.data() + (map.size() - recycledSize), recycledSize};
    }

    if (droppedAfter > 0) {
        const auto recycledSize = static_cast<std::size_t>(droppedAfter);
        const auto keptSize = static_cast<std::ptrdiff_t>(count - droppedAfter);
        std::ranges::rotate(map.begin(), map.begin() + start + keptSize, map.begin() + start + count);
        std::ranges::rotate(
            map.begin() + static_cast<std::ptrdiff_t>(recycledSize) + start,
            map.begin() + static_cast<std::ptrdiff_t>(recycledSize) + start + keptSize,
            map.end());
        return {map.data(), recycledSize};
    }

    if (delta > 0) {
        std::ranges::rotate(map.begin() + start, map.begin() + start + count, map.begin() + start + count + delta);
        return {};
    }

    std::ranges::rotate(map.begin() + targetStart, map.begin() + start, map.begin() + start + count);
    return {};
}

void RemappedBuffer::fillStoredRows(const std::span<const Coordinate> rows, const Char &fillChar) noexcept {
    for (const auto storedRow : rows) {
        for (Coordinate x = 0; x < _size.width(); ++x) {
            _buffer[storedBufferIndex(x, storedRow)] = fillChar;
        }
    }
}

void RemappedBuffer::fillStoredColumns(const std::span<const Coordinate> columns, const Char &fillChar) noexcept {
    for (const auto storedColumn : columns) {
        for (Coordinate y = 0; y < _size.height(); ++y) {
            _buffer[storedBufferIndex(storedColumn, y)] = fillChar;
        }
    }
}

void RemappedBuffer::fastResize(const Size newSize, const Char &fillChar) {
    const auto oldArea = _buffer.size();
    _size = newSize;
    _buffer.resize(static_cast<std::size_t>(_size.area()));
    if (!fillChar.isEmpty() && _buffer.size() > oldArea) {
        for (auto index = oldArea; index < _buffer.size(); ++index) {
            _buffer[index] = fillChar;
        }
    }
    _rowRemap = linearIndex(static_cast<std::size_t>(_size.height()));
    _columnRemap = linearIndex(static_cast<std::size_t>(_size.width()));
}

void RemappedBuffer::primaryAxisResize(const Size newSize, const Char &fillChar) {
    const auto oldArea = _buffer.size();
    const auto oldPrimarySize = _size.coordinate(_orientation);
    const auto newPrimarySize = newSize.coordinate(_orientation);
    auto &map = primaryMap();

    if (newPrimarySize > oldPrimarySize) {
        _size = newSize;
        _buffer.resize(static_cast<std::size_t>(_size.area()));
        if (!fillChar.isEmpty() && _buffer.size() > oldArea) {
            for (auto index = oldArea; index < _buffer.size(); ++index) {
                _buffer[index] = fillChar;
            }
        }
        map.resize(static_cast<std::size_t>(newPrimarySize));
        for (auto index = oldPrimarySize; index < newPrimarySize; ++index) {
            map[static_cast<std::size_t>(index)] = index;
        }
        return;
    }

    auto availableDestinations = CoordinateMap{};
    availableDestinations.reserve(static_cast<std::size_t>(oldPrimarySize - newPrimarySize));
    auto destinationUsed = std::vector<bool>(static_cast<std::size_t>(newPrimarySize), false);
    for (Coordinate index = 0; index < newPrimarySize; ++index) {
        const auto storedCoordinate = map[static_cast<std::size_t>(index)];
        if (storedCoordinate < newPrimarySize) {
            destinationUsed[static_cast<std::size_t>(storedCoordinate)] = true;
        }
    }
    for (Coordinate index = 0; index < newPrimarySize; ++index) {
        if (!destinationUsed[static_cast<std::size_t>(index)]) {
            availableDestinations.push_back(index);
        }
    }

    auto destinationIndex = std::size_t{0};
    for (Coordinate index = 0; index < newPrimarySize; ++index) {
        auto &storedCoordinate = map[static_cast<std::size_t>(index)];
        if (storedCoordinate < newPrimarySize) {
            continue;
        }
        const auto destination = availableDestinations[destinationIndex++];
        copyStoredPrimaryLine(storedCoordinate, destination);
        storedCoordinate = destination;
    }

    _size = newSize;
    _buffer.resize(static_cast<std::size_t>(_size.area()));
    map.resize(static_cast<std::size_t>(newPrimarySize));
}

void RemappedBuffer::reorderedResize(const Size newSize, const Char &fillChar) {
    auto newBuffer = std::vector<Char>(static_cast<std::size_t>(newSize.area()), fillChar);
    const auto copySize = _size.componentMin(newSize);
    copySize.forEach(
        [&](const Position pos) -> void { newBuffer[bufferIndex(pos, newSize, _orientation)] = get(pos); });
    _size = newSize;
    _buffer = std::move(newBuffer);
    _rowRemap = linearIndex(static_cast<std::size_t>(_size.height()));
    _columnRemap = linearIndex(static_cast<std::size_t>(_size.width()));
}

auto RemappedBuffer::isPrimaryAxisOnlyResize(const Size newSize) const noexcept -> bool {
    return newSize.coordinate(_orientation.crossed()) == _size.coordinate(_orientation.crossed());
}

auto RemappedBuffer::primaryMap() noexcept -> CoordinateMap & {
    return _orientation == Orientation::Vertical ? _rowRemap : _columnRemap;
}

auto RemappedBuffer::primaryMap() const noexcept -> const CoordinateMap & {
    return _orientation == Orientation::Vertical ? _rowRemap : _columnRemap;
}

void RemappedBuffer::copyStoredPrimaryLine(const Coordinate source, const Coordinate destination) noexcept {
    if (source == destination) {
        return;
    }
    if (_orientation == Orientation::Vertical) {
        for (Coordinate x = 0; x < _size.width(); ++x) {
            _buffer[storedBufferIndex(x, destination)] = _buffer[storedBufferIndex(x, source)];
        }
        return;
    }
    for (Coordinate y = 0; y < _size.height(); ++y) {
        _buffer[storedBufferIndex(destination, y)] = _buffer[storedBufferIndex(source, y)];
    }
}

}
