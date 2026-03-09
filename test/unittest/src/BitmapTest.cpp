// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/unittest/UnitTest.hpp>

#include "TestHelper.hpp"

#include <vector>


TESTED_TARGETS(Bitmap)
class BitmapTest final : public el::UnitTest {
public:
    void testPixelsCanBeSetAndRead() {
        auto bitmap = Bitmap{Size{4, 3}};
        bitmap.setPixel(Position{1, 2}, true);

        REQUIRE(bitmap.pixel(Position{1, 2}));
        REQUIRE_FALSE(bitmap.pixel(Position{0, 0}));
        REQUIRE_FALSE(bitmap.pixel(Position{99, 99}));
    }

    void testFlipHorizontalMirrorsContent() {
        auto bitmap = Bitmap{Size{4, 1}};
        bitmap.setPixel(Position{0, 0}, true);
        bitmap.setPixel(Position{1, 0}, true);

        bitmap.flipHorizontal();

        REQUIRE_FALSE(bitmap.pixel(Position{0, 0}));
        REQUIRE_FALSE(bitmap.pixel(Position{1, 0}));
        REQUIRE(bitmap.pixel(Position{2, 0}));
        REQUIRE(bitmap.pixel(Position{3, 0}));
    }

    void testPixelQuadBuildsExpectedMask() {
        auto bitmap = Bitmap{Size{2, 2}};
        bitmap.setPixel(Position{0, 0}, true);
        bitmap.setPixel(Position{1, 1}, true);

        REQUIRE_EQUAL(bitmap.pixelQuad(Position{0, 0}), static_cast<std::uint8_t>(0b1001U));
    }

    void testDrawCopiesOtherBitmap() {
        auto source = Bitmap{Size{2, 2}};
        source.setPixel(Position{0, 1}, true);
        auto destination = Bitmap{Size{4, 4}};

        destination.draw(Position{1, 1}, source);

        REQUIRE(destination.pixel(Position{1, 2}));
        REQUIRE_FALSE(destination.pixel(Position{1, 1}));
    }

    void testFromPatternParsesFilledAndEmptyPixels() {
        const auto bitmap = Bitmap::fromPattern({
            "#.X",
            " o ",
        });

        REQUIRE_EQUAL(bitmap.size(), Size(3, 2));
        REQUIRE(bitmap.pixel(Position{0, 0}));
        REQUIRE_FALSE(bitmap.pixel(Position{1, 0}));
        REQUIRE(bitmap.pixel(Position{2, 0}));
        REQUIRE_FALSE(bitmap.pixel(Position{0, 1}));
        REQUIRE(bitmap.pixel(Position{1, 1}));
        REQUIRE_FALSE(bitmap.pixel(Position{2, 1}));
    }

    void testFromPatternPadsShortRowsWithClearedPixels() {
        const auto bitmap = Bitmap::fromPattern({
            "##",
            "#",
            "",
        });

        REQUIRE_EQUAL(bitmap.size(), Size(2, 3));
        REQUIRE(bitmap.pixel(Position{0, 0}));
        REQUIRE(bitmap.pixel(Position{1, 0}));
        REQUIRE(bitmap.pixel(Position{0, 1}));
        REQUIRE_FALSE(bitmap.pixel(Position{1, 1}));
        REQUIRE_FALSE(bitmap.pixel(Position{0, 2}));
        REQUIRE_FALSE(bitmap.pixel(Position{1, 2}));
    }
};
