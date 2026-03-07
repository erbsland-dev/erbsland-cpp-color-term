// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "example/Color.hpp"

#include <erbsland/unittest/UnitTest.hpp>


// NOTE: This is a fictive minimal unit test template.
//       It demonstrates a very small test.
//       Test suites ending in `...Test` and its tests starting with `test...` are auto-registered when added to CMake.


TESTED_TARGETS(Color)
class MinimalTest final : public el::UnitTest {
public:
    Color color;

    void testEmpty() {
        color = {};
        REQUIRE(color.undefined());
    }

    void testConstruction() {
        color = Color{Color::Red};
        REQUIRE_EQUAL(color.r(), 1.0);
        REQUIRE_EQUAL(color.g(), 0.0);
        REQUIRE_EQUAL(color.b(), 0.0);
    }

    void testConversion() {
        color = Color{Color::Red};
        REQUIRE_EQUAL(color.toString(), "#ff0000");
    }
};


