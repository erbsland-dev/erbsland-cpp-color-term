// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/TestHelper.hpp"

#include <format>
#include <vector>

class RectangleTestSupport : public TestHelper {
public:
    void requireRectangleEqual(const Rectangle &actual, const Rectangle &expected) {
        REQUIRE_EQUAL(actual.topLeft(), expected.topLeft());
        REQUIRE_EQUAL(actual.size(), expected.size());
    }

    void requireRectangleSequenceEqual(const std::vector<Rectangle> &actual, const std::vector<Rectangle> &expected) {
        REQUIRE_EQUAL(actual.size(), expected.size());
        for (std::size_t index = 0; index < actual.size(); ++index) {
            runWithContext(
                SOURCE_LOCATION(),
                [&]() { requireRectangleEqual(actual[index], expected[index]); },
                [&]() -> std::string {
                    return std::format(
                        "index = {} / actual = ({}, {}, {}, {}) / expected = ({}, {}, {}, {})",
                        index,
                        actual[index].x1(),
                        actual[index].y1(),
                        actual[index].width(),
                        actual[index].height(),
                        expected[index].x1(),
                        expected[index].y1(),
                        expected[index].width(),
                        expected[index].height());
                });
        }
    }

    void requirePositionSequenceEqual(const PositionList &actual, const PositionList &expected) {
        REQUIRE_EQUAL(actual.size(), expected.size());
        for (std::size_t index = 0; index < expected.size(); ++index) {
            runWithContext(
                SOURCE_LOCATION(),
                [&]() { REQUIRE_EQUAL(actual[index], expected[index]); },
                [&]() -> std::string {
                    return std::format(
                        "index = {} / actual = ({}, {}) / expected = ({}, {})",
                        index,
                        actual[index].x(),
                        actual[index].y(),
                        expected[index].x(),
                        expected[index].y());
                });
        }
    }
};
