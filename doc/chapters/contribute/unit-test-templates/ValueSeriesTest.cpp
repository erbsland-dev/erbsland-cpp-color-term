// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "example/Color.hpp"

#include <erbsland/unittest/UnitTest.hpp>


// NOTE: This is a fictive unit test template.
//       It demonstrates how to test a sequence of data.
//       Test suites ending in `...Test` and its tests starting with `test...` are auto-registered when added to CMake.


TESTED_TARGETS(Color)
class ValueSeriesTest final : public el::UnitTest {
public:
    Color color;

    void testFrom() {
        struct TestData {
            std::string input;
            Color expected;
            bool isUndefined = false;
        };
        const auto testData = std::vector<TestData>{
            {"", Color{}, true},
            {"#ff0000", Color(1.0, 0.0, 0.0)},
            {"#00ff00", Color{0.0, 1.0, 0.0}},
            {"#0000ff", Color{0.0, 0.0, 1.0}},
            {"red", Color{1.0, 0.0, 0.0}}
        };
        for (const auto &data : testData) {
            runWithContext(SOURCE_LOCATION(), [&]() {
                color = Color::fromString(data.input);
                REQUIRE_EQUAL(color.undefined(), data.isUndefined);
                REQUIRE_EQUAL(color.r(), data.expected.r());
                REQUIRE_EQUAL(color.g(), data.expected.g());
                REQUIRE_EQUAL(color.b(), data.expected.b());
                REQUIRE(color == data.expected);
            }, [&]() -> std::string {
                return std::format("Failed for input '{}'.", data.input);
            });
        }
    }
};


