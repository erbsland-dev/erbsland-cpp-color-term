// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/TestHelper.hpp"

#include <algorithm>
#include <limits>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

class ReferenceBuffer final {
public:
    using Line = std::vector<Char>;

public:
    explicit ReferenceBuffer(const Size size, const Char fillChar = Char::space()) :
        _size{validateSize(size)}, _data(static_cast<std::size_t>(_size.area()), fillChar) {}

    [[nodiscard]] auto size() const noexcept -> Size { return _size; }

    [[nodiscard]] auto get(const Position pos) const -> const Char & { return _data[_size.index(pos)]; }

    void set(const Position pos, const Char &block) {
        if (!_size.contains(pos) || block.displayWidth() == 0 || block.displayWidth() > 2) {
            return;
        }
        if (block.displayWidth() == 1) {
            _data[_size.index(pos)] = block;
            return;
        }
        const auto secondPosition = pos + Position{1, 0};
        if (!_size.contains(secondPosition)) {
            return;
        }
        _data[_size.index(secondPosition)] = Char::emptyBlock(block.style());
        _data[_size.index(pos)] = block;
    }

    void fill(const Char &fillChar) {
        for (auto &block : _data) {
            block = fillChar;
        }
    }

    void resize(const Size newSize, const BufferResizeMode mode, const Char fillChar = Char::space()) {
        const auto validatedSize = validateSize(newSize);
        if (_size == validatedSize) {
            return;
        }
        if (mode == BufferResizeMode::PreserveContent) {
            auto newData = std::vector<Char>(static_cast<std::size_t>(validatedSize.area()), fillChar);
            _size.componentMin(validatedSize).forEach([&](const Position pos) -> void {
                newData[validatedSize.index(pos)] = get(pos);
            });
            _size = validatedSize;
            _data = std::move(newData);
            return;
        }
        const auto oldArea = _data.size();
        _size = validatedSize;
        _data.resize(static_cast<std::size_t>(_size.area()));
        if (!fillChar.isEmpty() && _data.size() > oldArea) {
            for (auto index = oldArea; index < _data.size(); ++index) {
                _data[index] = fillChar;
            }
        }
    }

    void shift(const Direction direction, const Char fillChar, const int count = 1) {
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

    void rotate(const Direction direction, const int count = 1) {
        validateDirectionalCount(direction, count);
        if (count == 0 || direction == Direction::None) {
            return;
        }
        if (direction.contains(Direction::North)) {
            auto rows = toRows();
            rotateLines(rows, count, true);
            fromRows(rows);
        }
        if (direction.contains(Direction::South)) {
            auto rows = toRows();
            rotateLines(rows, count, false);
            fromRows(rows);
        }
        if (direction.contains(Direction::West)) {
            auto columns = toColumns();
            rotateLines(columns, count, true);
            fromColumns(columns);
        }
        if (direction.contains(Direction::East)) {
            auto columns = toColumns();
            rotateLines(columns, count, false);
            fromColumns(columns);
        }
    }

    void eraseRows(const Coordinate startRow, const Char fillChar, const int count = 1) {
        validateSpan(startRow, count, _size.height(), "startRow", "count");
        if (count == 0) {
            return;
        }
        auto rows = toRows();
        rows.erase(rows.begin() + startRow, rows.begin() + startRow + count);
        rows.insert(rows.end(), static_cast<std::size_t>(count), blankRow(fillChar));
        fromRows(rows);
    }

    void eraseColumns(const Coordinate startColumn, const Char fillChar, const int count = 1) {
        validateSpan(startColumn, count, _size.width(), "startColumn", "count");
        if (count == 0) {
            return;
        }
        auto columns = toColumns();
        columns.erase(columns.begin() + startColumn, columns.begin() + startColumn + count);
        columns.insert(columns.end(), static_cast<std::size_t>(count), blankColumn(fillChar));
        fromColumns(columns);
    }

    void insertRows(const Coordinate startRow, const Char fillChar, const int count = 1) {
        validateSpan(startRow, count, _size.height(), "startRow", "count");
        if (count == 0) {
            return;
        }
        auto rows = toRows();
        rows.insert(rows.begin() + startRow, static_cast<std::size_t>(count), blankRow(fillChar));
        rows.erase(rows.end() - count, rows.end());
        fromRows(rows);
    }

    void insertColumns(const Coordinate startColumn, const Char fillChar, const int count = 1) {
        validateSpan(startColumn, count, _size.width(), "startColumn", "count");
        if (count == 0) {
            return;
        }
        auto columns = toColumns();
        columns.insert(columns.begin() + startColumn, static_cast<std::size_t>(count), blankColumn(fillChar));
        columns.erase(columns.end() - count, columns.end());
        fromColumns(columns);
    }

    void moveRows(const Coordinate startRow, const int count, const Coordinate delta, const Char fillChar) {
        validateSpan(startRow, count, _size.height(), "startRow", "count");
        if (count == 0 || delta == 0) {
            return;
        }
        auto rows = toRows();
        rows = moveLines(std::move(rows), startRow, count, delta, blankRow(fillChar));
        fromRows(rows);
    }

    void moveColumns(const Coordinate startColumn, const int count, const Coordinate delta, const Char fillChar) {
        validateSpan(startColumn, count, _size.width(), "startColumn", "count");
        if (count == 0 || delta == 0) {
            return;
        }
        auto columns = toColumns();
        columns = moveLines(std::move(columns), startColumn, count, delta, blankColumn(fillChar));
        fromColumns(columns);
    }

private:
    [[nodiscard]] static auto validateSize(const Size size) -> Size {
        if (size.width() < 1 || size.height() < 1) {
            throw std::invalid_argument("Buffer size must be at least 1x1");
        }
        if (!size.fitsInto(Size{10'000, 10'000})) {
            throw std::invalid_argument("Buffer size must not exceed 10'000x10'000");
        }
        return size;
    }

    static void validateSpan(
        const Coordinate start, const int count, const int limit, const char *startName, const char *countName) {
        if (count < 0 || count > limit) {
            throw std::invalid_argument(std::string{countName} + " is invalid");
        }
        if (count == 0) {
            if (start < 0 || start > limit) {
                throw std::invalid_argument(std::string{startName} + " is invalid");
            }
            return;
        }
        if (start < 0 || start >= limit || start + count > limit) {
            throw std::invalid_argument(std::string{startName} + " is invalid");
        }
    }

    void validateDirectionalCount(const Direction direction, const int count) const {
        if (count < 0) {
            throw std::invalid_argument("count is invalid");
        }
        if ((direction.contains(Direction::North) || direction.contains(Direction::South)) && count > _size.height()) {
            throw std::invalid_argument("count is invalid");
        }
        if ((direction.contains(Direction::West) || direction.contains(Direction::East)) && count > _size.width()) {
            throw std::invalid_argument("count is invalid");
        }
    }

    [[nodiscard]] auto blankRow(const Char &fillChar) const -> Line {
        return Line(static_cast<std::size_t>(_size.width()), fillChar);
    }

    [[nodiscard]] auto blankColumn(const Char &fillChar) const -> Line {
        return Line(static_cast<std::size_t>(_size.height()), fillChar);
    }

    [[nodiscard]] auto toRows() const -> std::vector<Line> {
        auto rows = std::vector<Line>{};
        rows.reserve(static_cast<std::size_t>(_size.height()));
        for (Coordinate y = 0; y < _size.height(); ++y) {
            auto row = Line{};
            row.reserve(static_cast<std::size_t>(_size.width()));
            for (Coordinate x = 0; x < _size.width(); ++x) {
                row.push_back(get(Position{x, y}));
            }
            rows.push_back(std::move(row));
        }
        return rows;
    }

    void fromRows(const std::vector<Line> &rows) {
        for (Coordinate y = 0; y < _size.height(); ++y) {
            for (Coordinate x = 0; x < _size.width(); ++x) {
                _data[_size.index(Position{x, y})] = rows[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
            }
        }
    }

    [[nodiscard]] auto toColumns() const -> std::vector<Line> {
        auto columns = std::vector<Line>{};
        columns.reserve(static_cast<std::size_t>(_size.width()));
        for (Coordinate x = 0; x < _size.width(); ++x) {
            auto column = Line{};
            column.reserve(static_cast<std::size_t>(_size.height()));
            for (Coordinate y = 0; y < _size.height(); ++y) {
                column.push_back(get(Position{x, y}));
            }
            columns.push_back(std::move(column));
        }
        return columns;
    }

    void fromColumns(const std::vector<Line> &columns) {
        for (Coordinate x = 0; x < _size.width(); ++x) {
            for (Coordinate y = 0; y < _size.height(); ++y) {
                _data[_size.index(Position{x, y})] = columns[static_cast<std::size_t>(x)][static_cast<std::size_t>(y)];
            }
        }
    }

    static void rotateLines(std::vector<Line> &lines, const int count, const bool towardFront) {
        if (lines.empty() || count == 0) {
            return;
        }
        const auto normalizedCount = static_cast<std::size_t>(count) % lines.size();
        if (normalizedCount == 0) {
            return;
        }
        if (towardFront) {
            std::ranges::rotate(lines, lines.begin() + static_cast<std::ptrdiff_t>(normalizedCount));
            return;
        }
        std::ranges::rotate(lines, lines.end() - static_cast<std::ptrdiff_t>(normalizedCount));
    }

    [[nodiscard]] static auto
    moveLines(std::vector<Line> lines, Coordinate start, int count, Coordinate delta, const Line &blankLine)
        -> std::vector<Line> {
        auto moved = std::vector<Line>{};
        moved.reserve(static_cast<std::size_t>(count));
        moved.insert(moved.end(), lines.begin() + start, lines.begin() + start + count);

        auto remaining = std::vector<Line>{};
        remaining.reserve(lines.size() - static_cast<std::size_t>(count));
        remaining.insert(remaining.end(), lines.begin(), lines.begin() + start);
        remaining.insert(remaining.end(), lines.begin() + start + count, lines.end());

        const auto targetStart = start + delta;
        const auto droppedBefore = std::clamp(-targetStart, 0, count);
        const auto droppedAfter = std::clamp(targetStart + count - static_cast<Coordinate>(lines.size()), 0, count);
        const auto keptBegin = static_cast<std::size_t>(droppedBefore);
        const auto keptEnd = static_cast<std::size_t>(count - droppedAfter);

        auto kept = std::vector<Line>{};
        kept.reserve(keptEnd - keptBegin);
        kept.insert(kept.end(), moved.begin() + static_cast<std::ptrdiff_t>(keptBegin), moved.begin() + keptEnd);

        auto result = std::vector<Line>{};
        result.reserve(lines.size());
        if (droppedAfter > 0) {
            result.insert(result.end(), static_cast<std::size_t>(droppedAfter), blankLine);
        }
        const auto insertIndex =
            static_cast<std::size_t>(std::clamp(targetStart, 0, static_cast<Coordinate>(remaining.size())));
        result.insert(result.end(), remaining.begin(), remaining.begin() + static_cast<std::ptrdiff_t>(insertIndex));
        result.insert(result.end(), kept.begin(), kept.end());
        result.insert(result.end(), remaining.begin() + static_cast<std::ptrdiff_t>(insertIndex), remaining.end());
        if (droppedBefore > 0) {
            result.insert(result.end(), static_cast<std::size_t>(droppedBefore), blankLine);
        }
        return result;
    }

private:
    Size _size;
    std::vector<Char> _data;
};
