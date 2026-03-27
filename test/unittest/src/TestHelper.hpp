// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <erbsland/cterm/all.hpp>
#include <erbsland/cterm/impl/UnicodeWidth.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <cstdint>
#include <format>
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

// import everything into the global namespace
using namespace erbsland::cterm;

// create an alias to resolve name conflicts.
namespace term = erbsland::cterm;


class TestHelper : public el::UnitTest {
public:
    [[nodiscard]] static auto bytes(const std::initializer_list<uint8_t> values) -> std::string {
        auto result = std::string{};
        result.reserve(values.size());
        for (const auto value : values) {
            result.push_back(static_cast<char>(value));
        }
        return result;
    }
};


class BufferTestHelper : public TestHelper {
public:
    auto createBuffer(const std::initializer_list<std::string_view> rows) -> Buffer {
        REQUIRE_FALSE(rows.size() == 0);
        auto width = Coordinate{0};
        auto rowIndex = 0;
        for (const auto row : rows) {
            runWithContext(
                SOURCE_LOCATION(),
                [&]() {
                    REQUIRE_FALSE(row.empty());
                    if (width == 0) {
                        width = static_cast<Coordinate>(row.size());
                    } else {
                        REQUIRE_EQUAL(static_cast<Coordinate>(row.size()), width);
                    }
                },
                [&]() -> std::string {
                    return std::format(
                        "rowIndex = {} / row = \"{}\" / rowSize = {} / expectedWidth = {}",
                        rowIndex,
                        row,
                        row.size(),
                        width);
                });
            rowIndex += 1;
        }
        auto buffer = Buffer{Size{width, static_cast<Coordinate>(rows.size())}};
        fillBufferFromRows(buffer, rows);
        return buffer;
    }

    auto createSharedBuffer(const std::initializer_list<std::string_view> rows) -> std::shared_ptr<Buffer> {
        return std::make_shared<Buffer>(createBuffer(rows));
    }

    void fillBufferFromRows(WritableBuffer &buffer, const std::initializer_list<std::string_view> rows) {
        REQUIRE(buffer.size().height() >= static_cast<Coordinate>(rows.size()));
        auto y = Coordinate{0};
        auto rowIndex = 0;
        for (const auto row : rows) {
            runWithContext(
                SOURCE_LOCATION(),
                [&]() { REQUIRE(buffer.size().width() >= static_cast<Coordinate>(row.size())); },
                [&]() -> std::string {
                    return std::format(
                        "rowIndex = {} / row = \"{}\" / rowSize = {} / bufferWidth = {} / bufferHeight = {}",
                        rowIndex,
                        row,
                        row.size(),
                        buffer.size().width(),
                        buffer.size().height());
                });
            for (auto x = Coordinate{0}; x < static_cast<Coordinate>(row.size()); ++x) {
                buffer.set(Position{x, y}, Char{static_cast<char32_t>(static_cast<unsigned char>(row[x]))});
            }
            y += 1;
            rowIndex += 1;
        }
    }

    [[nodiscard]] static auto renderRows(const auto &buffer) -> std::vector<std::string> {
        auto rows = std::vector<std::string>{};
        rows.reserve(static_cast<std::size_t>(buffer.size().height()));
        for (Coordinate y = 0; y < buffer.size().height(); ++y) {
            auto row = std::string{};
            for (Coordinate x = 0; x < buffer.size().width(); ++x) {
                const auto &block = buffer.get(Position{x, y});
                row += block.charStr().empty() ? " " : block.charStr();
            }
            rows.push_back(std::move(row));
        }
        return rows;
    }

    void requireRowsEqual(const auto &buffer, const std::initializer_list<std::string_view> expectedRows) {
        const auto actualRows = renderRows(buffer);
        REQUIRE_EQUAL(actualRows.size(), expectedRows.size());
        auto rowIndex = std::size_t{0};
        for (const auto expectedRow : expectedRows) {
            runWithContext(
                SOURCE_LOCATION(),
                [&]() { REQUIRE_EQUAL(actualRows[rowIndex], std::string{expectedRow}); },
                [&]() -> std::string {
                    return std::format(
                        "rowIndex = {} / actual = \"{}\" / expected = \"{}\"",
                        rowIndex,
                        actualRows[rowIndex],
                        expectedRow);
                });
            rowIndex += 1;
        }
    }
};
