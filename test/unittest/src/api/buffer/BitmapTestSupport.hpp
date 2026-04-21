// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "BitmapAccessor.hpp"

#include "support/TestHelper.hpp"

#include <vector>

class BitmapTestSupport : public TestHelper {
public:
    [[nodiscard]] auto renderRows(const Bitmap &bitmap) -> std::vector<std::string> {
        auto rows = std::vector<std::string>{};
        rows.reserve(static_cast<std::size_t>(bitmap.size().height()));
        for (Coordinate y = 0; y < bitmap.size().height(); ++y) {
            auto row = std::string{};
            row.reserve(static_cast<std::size_t>(bitmap.size().width()));
            for (Coordinate x = 0; x < bitmap.size().width(); ++x) {
                row += bitmap.pixel(Position{x, y}) ? '#' : '.';
            }
            rows.push_back(std::move(row));
        }
        return rows;
    }

    void requireRowsEqual(const Bitmap &bitmap, const std::vector<std::string> &expectedRows) {
        REQUIRE_EQUAL_LINES(renderRows(bitmap), expectedRows);
    }

    void requireRectangleEqual(const Rectangle &actual, const Rectangle &expected) {
        REQUIRE_EQUAL(actual.topLeft(), expected.topLeft());
        REQUIRE_EQUAL(actual.size(), expected.size());
    }
};
