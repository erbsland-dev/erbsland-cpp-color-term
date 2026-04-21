// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "ReferenceBuffer.hpp"

#include "support/BufferTestHelper.hpp"

#include <algorithm>
#include <array>
#include <format>
#include <limits>
#include <random>
#include <string>
#include <vector>

class RemappedBufferTestSupport : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    std::string trace;
    std::vector<std::string> expectedRows;
    std::vector<std::string> actualRows;

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

protected:
    template <typename Fn>
    void forEachOrientation(Fn fn) {
        for (const auto orientation : cOrientations) {
            runWithContext(
                SOURCE_LOCATION(),
                [&]() -> void { fn(orientation); },
                [&]() -> std::string { return std::format("orientation = {}", orientationName(orientation)); });
        }
    }

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

    template <typename T>
    static void fillPattern(T &buffer) {
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

private:
    static constexpr auto cOrientations = std::array{Orientation::Vertical, Orientation::Horizontal};
};
