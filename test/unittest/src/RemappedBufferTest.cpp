// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/RemappedBuffer.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <algorithm>
#include <array>
#include <format>
#include <random>
#include <string>
#include <vector>


TESTED_TARGETS(RemappedBuffer)
class RemappedBufferTest final : public el::UnitTest {
public:
    using Line = std::vector<Char>;

    class ReferenceBuffer final {
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
            _data[_size.index(secondPosition)] = Char{"", block.color()};
            _data[_size.index(pos)] = block;
        }

        void fill(const Char &fillChar) {
            for (auto &block : _data) {
                block = fillChar;
            }
        }

        void resize(const Size newSize, const bool reorder, const Char fillChar = Char::space()) {
            const auto validatedSize = validateSize(newSize);
            if (_size == validatedSize) {
                return;
            }
            if (reorder) {
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
        static auto validateSize(const Size size) -> Size {
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
            if ((direction.contains(Direction::North) || direction.contains(Direction::South)) &&
                count > _size.height()) {
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
                    _data[_size.index(Position{x, y})] =
                        columns[static_cast<std::size_t>(x)][static_cast<std::size_t>(y)];
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
            result.insert(
                result.end(), remaining.begin(), remaining.begin() + static_cast<std::ptrdiff_t>(insertIndex));
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

public:
    std::string trace;
    std::vector<std::string> expectedRows;
    std::vector<std::string> actualRows;

    void testConstructorCloneAndWideCharacters() {
        forEachOrientation([&](const Orientation orientation) -> void {
            const auto fillChar = Char{U'X', fg::Yellow, bg::Blue};
            auto buffer = RemappedBuffer{Size{3, 2}, orientation, fillChar};

            REQUIRE_EQUAL(buffer.size(), Size(3, 2));
            buffer.size().forEach([&](const Position pos) -> void {
                REQUIRE_EQUAL(buffer.get(pos), U'X');
                REQUIRE_EQUAL(buffer.get(pos).color(), Color(fg::Yellow, bg::Blue));
            });

            buffer.set(Position{0, 1}, Char{U'界', fg::Green, bg::Black});
            REQUIRE_EQUAL(buffer.get(Position{0, 1}), U'界');
            REQUIRE(buffer.get(Position{1, 1}).isEmpty());
            REQUIRE_EQUAL(buffer.get(Position{1, 1}).color(), Color(fg::Green, bg::Black));

            const auto clone = buffer.clone();
            REQUIRE(clone != nullptr);
            clone->set(Position{2, 0}, Char{U'Z', fg::Red, bg::Black});

            REQUIRE_EQUAL(buffer.get(Position{2, 0}), U'X');
            REQUIRE_EQUAL(clone->get(Position{2, 0}), U'Z');
            REQUIRE_EQUAL(clone->get(Position{2, 0}).color(), Color(fg::Red, bg::Black));
        });

        REQUIRE_THROWS_AS(std::invalid_argument, RemappedBuffer(Size{0, 1}));
        REQUIRE_THROWS_AS(std::invalid_argument, RemappedBuffer(Size{1, 0}));
        REQUIRE_THROWS_AS(std::invalid_argument, RemappedBuffer(Size{10'001, 1}));
    }

    void testResizeWithReorderPreservesVisibleContent() {
        forEachOrientation([&](const Orientation orientation) -> void {
            auto buffer = createPatternBuffer(Size{3, 2}, orientation);
            auto model = createPatternModel(Size{3, 2});

            scramble(buffer, model);
            buffer.resize(Size{4, 3}, true, Char{U'.'});
            model.resize(Size{4, 3}, true, Char{U'.'});
            requireMatches(buffer, model, std::format("resize reorder expand {}", orientationName(orientation)));

            buffer.resize(Size{2, 2}, true, Char{U'.'});
            model.resize(Size{2, 2}, true, Char{U'.'});
            requireMatches(buffer, model, std::format("resize reorder shrink {}", orientationName(orientation)));
        });
    }

    void testFastResizeKeepsBufferUsableAfterScrambledMaps() {
        forEachOrientation([&](const Orientation orientation) -> void {
            auto buffer = createPatternBuffer(Size{4, 3}, orientation);
            auto model = createPatternModel(Size{5, 4});

            scramble(buffer);
            buffer.resize(Size{5, 4});
            REQUIRE_EQUAL(buffer.size(), Size(5, 4));

            buffer.fill(Char{U'.', fg::White, bg::Black});
            fillPattern(buffer);
            requireMatches(buffer, model, std::format("fast resize refill {}", orientationName(orientation)));
        });
    }

    void testShiftRotateInsertEraseAndMoveMatchReferenceModel() {
        forEachOrientation([&](const Orientation orientation) -> void {
            auto buffer = createPatternBuffer(Size{5, 4}, orientation);
            auto model = createPatternModel(Size{5, 4});

            buffer.shift(Direction::NorthEast, Char{U'.'}, 1);
            model.shift(Direction::NorthEast, Char{U'.'}, 1);
            requireMatches(buffer, model, std::format("shift northeast {}", orientationName(orientation)));

            buffer.rotate(Direction::SouthWest, 2);
            model.rotate(Direction::SouthWest, 2);
            requireMatches(buffer, model, std::format("rotate southwest {}", orientationName(orientation)));

            buffer.insertRows(1, Char{U'+'}, 2);
            model.insertRows(1, Char{U'+'}, 2);
            requireMatches(buffer, model, std::format("insert rows {}", orientationName(orientation)));

            buffer.eraseColumns(2, Char{U'-'}, 2);
            model.eraseColumns(2, Char{U'-'}, 2);
            requireMatches(buffer, model, std::format("erase columns {}", orientationName(orientation)));

            buffer.moveRows(1, 2, -1, Char{U'#'});
            model.moveRows(1, 2, -1, Char{U'#'});
            requireMatches(buffer, model, std::format("move rows {}", orientationName(orientation)));

            buffer.moveColumns(1, 2, 2, Char{U'!'});
            model.moveColumns(1, 2, 2, Char{U'!'});
            requireMatches(buffer, model, std::format("move columns {}", orientationName(orientation)));
        });
    }

    void testMoveOperationsHandleOverflow() {
        forEachOrientation([&](const Orientation orientation) -> void {
            auto buffer = createPatternBuffer(Size{5, 4}, orientation);
            auto model = createPatternModel(Size{5, 4});

            buffer.moveRows(1, 3, -2, Char{U'^'});
            model.moveRows(1, 3, -2, Char{U'^'});
            requireMatches(buffer, model, std::format("move rows overflow up {}", orientationName(orientation)));

            buffer.moveColumns(0, 3, 3, Char{U'v'});
            model.moveColumns(0, 3, 3, Char{U'v'});
            requireMatches(buffer, model, std::format("move columns overflow right {}", orientationName(orientation)));
        });
    }

    void testInvalidArgumentsAreRejected() {
        auto buffer = RemappedBuffer{Size{4, 3}, Orientation::Vertical};

        REQUIRE_THROWS_AS(std::invalid_argument, buffer.resize(Size{0, 3}));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.shift(Direction::North, Char::space(), -1));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.rotate(Direction::East, 5));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.eraseRows(2, Char::space(), 2));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.eraseColumns(-1, Char::space(), 1));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.insertRows(2, Char::space(), 2));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.insertColumns(3, Char::space(), 2));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.moveRows(1, 3, 1, Char::space()));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.moveColumns(3, 2, -1, Char::space()));
    }

    void testStressOperationsAgainstReferenceModel() {
        forEachOrientation([&](const Orientation orientation) -> void {
            auto buffer = createPatternBuffer(Size{6, 5}, orientation);
            auto model = createPatternModel(Size{6, 5});
            auto rng = std::mt19937{0xC0DE1234U + static_cast<uint32_t>(orientation == Orientation::Horizontal)};

            for (int step = 0; step < 250; ++step) {
                const auto operation = randomInt(rng, 0, 8);
                switch (operation) {
                case 0: {
                    const auto direction = randomCardinalOrDiagonal(rng);
                    const auto count = maxDirectionalCount(model.size(), direction) == 0
                        ? 0
                        : randomInt(rng, 0, maxDirectionalCount(model.size(), direction));
                    const auto fillChar = Char{static_cast<char32_t>(U'a' + (step % 26))};
                    buffer.shift(direction, fillChar, count);
                    model.shift(direction, fillChar, count);
                    break;
                }
                case 1: {
                    const auto direction = randomCardinalOrDiagonal(rng);
                    const auto count = maxDirectionalCount(model.size(), direction) == 0
                        ? 0
                        : randomInt(rng, 0, maxDirectionalCount(model.size(), direction));
                    buffer.rotate(direction, count);
                    model.rotate(direction, count);
                    break;
                }
                case 2: {
                    const auto count = randomInt(rng, 0, model.size().height());
                    const auto start =
                        count == 0 ? model.size().height() : randomInt(rng, 0, model.size().height() - count);
                    const auto fillChar = Char{static_cast<char32_t>(U'A' + (step % 26))};
                    buffer.eraseRows(start, fillChar, count);
                    model.eraseRows(start, fillChar, count);
                    break;
                }
                case 3: {
                    const auto count = randomInt(rng, 0, model.size().width());
                    const auto start =
                        count == 0 ? model.size().width() : randomInt(rng, 0, model.size().width() - count);
                    const auto fillChar = Char{static_cast<char32_t>(U'0' + (step % 10))};
                    buffer.insertColumns(start, fillChar, count);
                    model.insertColumns(start, fillChar, count);
                    break;
                }
                case 4: {
                    const auto count = randomInt(rng, 0, model.size().height());
                    const auto start =
                        count == 0 ? model.size().height() : randomInt(rng, 0, model.size().height() - count);
                    const auto delta = randomInt(rng, -model.size().height(), model.size().height());
                    const auto fillChar = Char{static_cast<char32_t>(U'k' + (step % 10))};
                    buffer.moveRows(start, count, delta, fillChar);
                    model.moveRows(start, count, delta, fillChar);
                    break;
                }
                case 5: {
                    const auto count = randomInt(rng, 0, model.size().width());
                    const auto start =
                        count == 0 ? model.size().width() : randomInt(rng, 0, model.size().width() - count);
                    const auto delta = randomInt(rng, -model.size().width(), model.size().width());
                    const auto fillChar = Char{static_cast<char32_t>(U'p' + (step % 10))};
                    buffer.moveColumns(start, count, delta, fillChar);
                    model.moveColumns(start, count, delta, fillChar);
                    break;
                }
                case 6: {
                    const auto newSize = Size{
                        randomInt(rng, 1, std::max(1, model.size().width() + 1)),
                        randomInt(rng, 1, std::max(1, model.size().height() + 1))};
                    const auto fillChar = Char{static_cast<char32_t>(U'Z' - (step % 20))};
                    buffer.resize(newSize, true, fillChar);
                    model.resize(newSize, true, fillChar);
                    break;
                }
                case 7: {
                    const auto pos = Position{
                        randomInt(rng, 0, model.size().width() - 1), randomInt(rng, 0, model.size().height() - 1)};
                    const auto value = Char{static_cast<char32_t>(U'!' + (step % 60))};
                    buffer.set(pos, value);
                    model.set(pos, value);
                    break;
                }
                case 8: {
                    const auto fillChar = Char{static_cast<char32_t>(U'.' + (step % 20))};
                    buffer.fill(fillChar);
                    model.fill(fillChar);
                    break;
                }
                default:
                    REQUIRE(false);
                }
                requireMatches(buffer, model, std::format("stress step {} {}", step, orientationName(orientation)));
            }
        });
    }

    auto additionalErrorMessages() -> std::string override {
        auto message = std::string{};
        if (!trace.empty()) {
            message += std::format("trace: {}\n", trace);
        }
        if (!expectedRows.empty()) {
            message += "expected rows:\n";
            for (const auto &row : expectedRows) {
                message += std::format("  {}\n", row);
            }
        }
        if (!actualRows.empty()) {
            message += "actual rows:\n";
            for (const auto &row : actualRows) {
                message += std::format("  {}\n", row);
            }
        }
        return message;
    }

private:
    template <typename Fn>
    void forEachOrientation(Fn fn) {
        for (const auto orientation : cOrientations) {
            runWithContext(
                SOURCE_LOCATION(),
                [&]() -> void { fn(orientation); },
                [&]() -> std::string { return std::format("orientation = {}", orientationName(orientation)); });
        }
    }

    static constexpr auto cOrientations = std::array{Orientation::Vertical, Orientation::Horizontal};

    [[nodiscard]] static auto orientationName(const Orientation orientation) -> std::string {
        return orientation == Orientation::Vertical ? "vertical" : "horizontal";
    }

    [[nodiscard]] static auto createPatternBuffer(const Size size, const Orientation orientation) -> RemappedBuffer {
        auto buffer = RemappedBuffer{size, orientation};
        fillPattern(buffer);
        return buffer;
    }

    [[nodiscard]] static auto createPatternModel(const Size size) -> ReferenceBuffer {
        auto buffer = ReferenceBuffer{size};
        fillPattern(buffer);
        return buffer;
    }

    static void fillPattern(auto &buffer) {
        auto index = 0;
        buffer.size().forEach([&](const Position pos) -> void {
            const auto codePoint = static_cast<char32_t>(U'A' + (index % 26));
            buffer.set(pos, Char{codePoint, (index % 2 == 0) ? fg::Green : fg::Cyan, bg::Black});
            index += 1;
        });
    }

    void scramble(RemappedBuffer &buffer, ReferenceBuffer &model) {
        buffer.rotate(Direction::South, 1);
        model.rotate(Direction::South, 1);
        buffer.insertColumns(1, Char{U'+'}, 1);
        model.insertColumns(1, Char{U'+'}, 1);
        buffer.eraseRows(0, Char{U'-'}, 1);
        model.eraseRows(0, Char{U'-'}, 1);
        buffer.moveColumns(0, std::min(2, buffer.size().width()), 1, Char{U'*'});
        model.moveColumns(0, std::min(2, model.size().width()), 1, Char{U'*'});
        requireMatches(buffer, model, "scramble");
    }

    void scramble(RemappedBuffer &buffer) {
        buffer.rotate(Direction::East, 1);
        buffer.insertRows(1, Char{U'+'}, 1);
        buffer.eraseColumns(0, Char{U'-'}, 1);
        buffer.moveRows(0, std::min(2, buffer.size().height()), 1, Char{U'*'});
    }

    void requireMatches(const ReadableBuffer &buffer, const ReferenceBuffer &model, const std::string &newTrace) {
        trace = newTrace;
        expectedRows = renderRows(model);
        actualRows = renderRows(buffer);

        REQUIRE_EQUAL(buffer.size(), model.size());
        model.size().forEach([&](const Position pos) -> void {
            runWithContext(
                SOURCE_LOCATION(),
                [&]() -> void { REQUIRE_EQUAL(buffer.get(pos), model.get(pos)); },
                [&]() -> std::string { return std::format("{} at {}", trace, pos); });
        });
    }

    [[nodiscard]] static auto renderRows(const auto &buffer) -> std::vector<std::string> {
        auto rows = std::vector<std::string>{};
        rows.reserve(static_cast<std::size_t>(buffer.size().height()));
        for (Coordinate y = 0; y < buffer.size().height(); ++y) {
            auto row = std::string{};
            for (Coordinate x = 0; x < buffer.size().width(); ++x) {
                buffer.get(Position{x, y}).appendTo(row);
            }
            rows.push_back(std::move(row));
        }
        return rows;
    }

    [[nodiscard]] static auto randomInt(std::mt19937 &rng, const int minValue, const int maxValue) -> int {
        auto distribution = std::uniform_int_distribution<int>{minValue, maxValue};
        return distribution(rng);
    }

    [[nodiscard]] static auto randomCardinalOrDiagonal(std::mt19937 &rng) -> Direction {
        constexpr auto directions = std::array{
            Direction::North,
            Direction::NorthEast,
            Direction::East,
            Direction::SouthEast,
            Direction::South,
            Direction::SouthWest,
            Direction::West,
            Direction::NorthWest};
        return directions[static_cast<std::size_t>(randomInt(rng, 0, static_cast<int>(directions.size()) - 1))];
    }

    [[nodiscard]] static auto maxDirectionalCount(const Size size, const Direction direction) -> int {
        auto maximum = std::numeric_limits<int>::max();
        if (direction.contains(Direction::North) || direction.contains(Direction::South)) {
            maximum = std::min(maximum, size.height());
        }
        if (direction.contains(Direction::West) || direction.contains(Direction::East)) {
            maximum = std::min(maximum, size.width());
        }
        return maximum == std::numeric_limits<int>::max() ? 0 : maximum;
    }
};
