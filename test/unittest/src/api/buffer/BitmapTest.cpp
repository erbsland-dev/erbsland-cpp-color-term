// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "BitmapTestSupport.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(Bitmap)
class BitmapTest final : public UNITTEST_SUBCLASS(BitmapTestSupport) {
public:
    void testPixelsCanBeSetAndRead() {
        auto bitmap = Bitmap{Size{4, 3}};
        bitmap.setPixel(Position{1, 2}, true);

        REQUIRE(bitmap.pixel(Position{1, 2}));
        REQUIRE_FALSE(bitmap.pixel(Position{0, 0}));
        REQUIRE_FALSE(bitmap.pixel(Position{99, 99}));
    }

    void testSetPixelIgnoresOutsideCoordinatesWithoutChangingTheBitmap() {
        auto bitmap = Bitmap::fromPattern({
            "#.",
            ".#",
        });

        bitmap.setPixel(Position{-1, 0}, true);
        bitmap.setPixel(Position{2, 1}, true);
        bitmap.setPixel(Position{0, 2}, false);

        requireRowsEqual(
            bitmap,
            {
                "#.",
                ".#",
            });
    }

    void testRectMatchesBitmapBounds() {
        const auto bitmap = Bitmap{Size{4, 3}};

        REQUIRE_EQUAL(bitmap.rect().topLeft(), Position(0, 0));
        REQUIRE_EQUAL(bitmap.rect().size(), Size(4, 3));
    }

    void testDataAccessorsExposeTheUnderlyingPixelStorage() {
        auto bitmap = Bitmap{Size{2, 2}};

        bitmap.data()[1] = true;
        bitmap.data()[2] = true;

        const auto &constData = static_cast<const Bitmap &>(bitmap).data();
        REQUIRE_EQUAL(constData.size(), static_cast<std::size_t>(4));
        REQUIRE(constData[1]);
        REQUIRE(constData[2]);
        REQUIRE(bitmap.pixel(Position{1, 0}));
        REQUIRE(bitmap.pixel(Position{0, 1}));
    }

    void testProtectedPixelRefAllowsMutationAndThrowsForOutOfBoundsAccess() {
        auto bitmap = BitmapAccessor{Size{2, 1}};

        bitmap.writePixelRef(Position{1, 0}, true);

        REQUIRE(bitmap.pixel(Position{1, 0}));
        REQUIRE(bitmap.readPixelRef(Position{1, 0}));
        REQUIRE_THROWS_AS(std::out_of_range, bitmap.readPixelRef(Position{2, 0}));
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

    void testInvertTogglesAllPixelsInPlace() {
        auto bitmap = Bitmap::fromPattern({
            "#.",
            " .",
        });

        bitmap.invert();

        REQUIRE_FALSE(bitmap.pixel(Position{0, 0}));
        REQUIRE(bitmap.pixel(Position{1, 0}));
        REQUIRE(bitmap.pixel(Position{0, 1}));
        REQUIRE(bitmap.pixel(Position{1, 1}));

        bitmap.invert();

        REQUIRE(bitmap.pixel(Position{0, 0}));
        REQUIRE_FALSE(bitmap.pixel(Position{1, 0}));
        REQUIRE_FALSE(bitmap.pixel(Position{0, 1}));
        REQUIRE_FALSE(bitmap.pixel(Position{1, 1}));
    }

    void testPixelCountCountsSetAndClearedPixels() {
        const auto bitmap = Bitmap::fromPattern({
            "#.#",
            ".#.",
        });

        REQUIRE_EQUAL(bitmap.pixelCount(), static_cast<std::size_t>(3));
        REQUIRE_EQUAL(bitmap.pixelCount(false), static_cast<std::size_t>(3));
    }

    void testInvertedReturnsAnInvertedCopyWithoutChangingTheOriginal() {
        const auto bitmap = Bitmap::fromPattern({
            "#..",
            ".##",
        });

        const auto inverted = bitmap.inverted();

        requireRowsEqual(
            bitmap,
            {
                "#..",
                ".##",
            });
        requireRowsEqual(
            inverted,
            {
                ".##",
                "#..",
            });
    }

    void testPixelQuadBuildsExpectedMask() {
        auto bitmap = Bitmap{Size{2, 2}};
        bitmap.setPixel(Position{0, 0}, true);
        bitmap.setPixel(Position{1, 1}, true);

        REQUIRE_EQUAL(bitmap.pixelQuad(Position{0, 0}), static_cast<std::uint8_t>(0b1001U));
    }

    void testPixelCardinalBuildsExpectedMaskInClockwiseOrder() {
        const auto bitmap = Bitmap::fromPattern({
            ".#.",
            "..#",
            ".#.",
        });

        REQUIRE_EQUAL(bitmap.pixelCardinal(Position{1, 1}), static_cast<std::uint8_t>(0b1011U));
    }

    void testPixelCardinalIgnoresTheCenterPixelAndOutOfBoundsNeighbors() {
        auto bitmap = Bitmap{Size{2, 1}};
        bitmap.setPixel(Position{0, 0}, true);
        bitmap.setPixel(Position{1, 0}, true);

        REQUIRE_EQUAL(bitmap.pixelCardinal(Position{0, 0}), static_cast<std::uint8_t>(0b0001U));
        REQUIRE_EQUAL(bitmap.pixelCardinal(Position{-1, 0}), static_cast<std::uint8_t>(0b0001U));
    }

    void testPixelRingBuildsExpectedMaskInClockwiseOrder() {
        const auto bitmap = Bitmap::fromPattern({
            "###",
            "#.#",
            ".##",
        });

        REQUIRE_EQUAL(bitmap.pixelRing(Position{1, 1}), static_cast<std::uint8_t>(0b11110111U));
    }

    void testPixelRingIgnoresTheCenterPixelAndOutOfBoundsNeighbors() {
        auto bitmap = Bitmap{Size{2, 2}};
        bitmap.setPixel(Position{0, 0}, true);
        bitmap.setPixel(Position{1, 1}, true);

        REQUIRE_EQUAL(bitmap.pixelRing(Position{0, 0}), static_cast<std::uint8_t>(0b00000010U));
        REQUIRE_EQUAL(bitmap.pixelRing(Position{-1, -1}), static_cast<std::uint8_t>(0b00000010U));
    }

    void testBoundingRectReturnsTheCoveredAreaForSetPixels() {
        const auto bitmap = Bitmap::fromPattern({
            "......",
            ".#..#.",
            "..##..",
            "......",
        });

        requireRectangleEqual(bitmap.boundingRect(), Rectangle(1, 1, 4, 2));
    }

    void testBoundingRectSupportsSinglePixelAndSingleRowSpans() {
        auto singlePixel = Bitmap{Size{5, 4}};
        singlePixel.setPixel(Position{3, 2}, true);
        requireRectangleEqual(singlePixel.boundingRect(), Rectangle(3, 2, 1, 1));

        auto singleRow = Bitmap{Size{6, 4}};
        singleRow.fillRect(Rectangle{1, 3, 4, 1}, true);
        requireRectangleEqual(singleRow.boundingRect(), Rectangle(1, 3, 4, 1));
    }

    void testBoundingRectReturnsEmptyRectangleWhenTheRequestedValueDoesNotExist() {
        requireRectangleEqual(Bitmap{Size{3, 2}}.boundingRect(), Rectangle{});

        auto filled = Bitmap{Size{3, 2}};
        filled.fillRect(filled.rect(), true);
        requireRectangleEqual(filled.boundingRect(false), Rectangle{});
    }

    void testBoundingRectCanLocateClearedPixelsInsideAFilledBitmap() {
        auto bitmap = Bitmap{Size{5, 4}};
        bitmap.fillRect(bitmap.rect(), true);
        bitmap.fillRect(Rectangle{1, 1, 3, 2}, false);

        requireRectangleEqual(bitmap.boundingRect(false), Rectangle(1, 1, 3, 2));
    }

    void testFromFunctionCreatesPixelsFromTheGenerator() {
        const auto bitmap =
            Bitmap::fromFunction(Size{4, 3}, [](const Position pos) -> bool { return (pos.x() + pos.y()) % 2 == 0; });

        requireRowsEqual(
            bitmap,
            {
                "#.#.",
                ".#.#",
                "#.#.",
            });
    }

    void testFromFunctionDoesNotInvokeTheGeneratorForAnEmptyBitmap() {
        auto calls = 0;

        const auto bitmap = Bitmap::fromFunction(Size{0, 0}, [&](const Position) -> bool {
            ++calls;
            return true;
        });

        REQUIRE_EQUAL(bitmap.size(), Size(0, 0));
        REQUIRE_EQUAL(calls, 0);
    }

    void testOutlinedCreatesAnEightConnectedBorderAroundFilledPixels() {
        const auto bitmap = Bitmap::fromPattern({
            ".....",
            ".....",
            "..#..",
            ".....",
            ".....",
        });

        const auto outlined = bitmap.outlined();

        requireRowsEqual(
            outlined,
            {
                ".....",
                ".###.",
                ".#.#.",
                ".###.",
                ".....",
            });
    }

    void testOutlinedReturnsEmptyForAnEmptyBitmapAndMarksInteriorHoles() {
        requireRowsEqual(
            Bitmap{Size{3, 3}}.outlined(),
            {
                "...",
                "...",
                "...",
            });

        const auto bitmapWithHole = Bitmap::fromPattern({
            "###",
            "#.#",
            "###",
        });

        requireRowsEqual(
            bitmapWithHole.outlined(),
            {
                "...",
                ".#.",
                "...",
            });
    }

    void testOutlinedMarksAllEmptyNeighborsOfAComplexShape() {
        const auto bitmap = Bitmap::fromPattern({
            ".....",
            ".##..",
            "..#..",
            ".....",
        });

        requireRowsEqual(
            bitmap.outlined(),
            {
                "####.",
                "#..#.",
                "##.#.",
                ".###.",
            });
    }

    void testExpandedAddsClearedMarginsAroundTheBitmap() {
        const auto bitmap = Bitmap::fromPattern({
            "#.",
            ".#",
        });

        const auto expanded = bitmap.expanded(Margins{1, 2, 1, 3}, false);

        requireRowsEqual(
            expanded,
            {
                ".......",
                "...#...",
                "....#..",
                ".......",
            });
        requireRowsEqual(
            bitmap,
            {
                "#.",
                ".#",
            });
    }

    void testExpandedCanFillTheAddedMarginArea() {
        const auto bitmap = Bitmap::fromPattern({
            "#.",
            "##",
        });

        const auto expanded = bitmap.expanded(Margins{1, 1, 0, 2}, true);

        requireRowsEqual(
            expanded,
            {
                "#####",
                "###.#",
                "#####",
            });
    }

    void testExpandedWithNegativeMarginsCropsTheBitmap() {
        const auto bitmap = Bitmap::fromPattern({
            ".....",
            ".##..",
            "..#..",
            ".##..",
        });

        const auto expanded = bitmap.expanded(Margins{-1, -2, 0, -1}, false);

        requireRowsEqual(
            expanded,
            {
                "##",
                ".#",
                "##",
            });
    }

    void testExpandedReturnsAnEmptyBitmapIfMarginsRemoveAWholeDimension() {
        const auto bitmap = Bitmap::fromPattern({
            "##",
            "##",
        });

        const auto emptyWidth = bitmap.expanded(Margins{0, -2, 0, 0}, false);
        const auto emptyHeight = bitmap.expanded(Margins{-2, 0, 0, 0}, false);

        REQUIRE_EQUAL(emptyWidth.size(), Size(0, 0));
        REQUIRE_EQUAL(emptyHeight.size(), Size(0, 0));
    }

    void testDrawCopiesOtherBitmap() {
        auto source = Bitmap{Size{2, 2}};
        source.setPixel(Position{0, 1}, true);
        auto destination = Bitmap{Size{4, 4}};

        destination.draw(Position{1, 1}, source);

        REQUIRE(destination.pixel(Position{1, 2}));
        REQUIRE_FALSE(destination.pixel(Position{1, 1}));
    }

    void testDrawFromUnsignedBitRowsUsesLeastSignificantBitsFromLeftToRight() {
        auto bitmap = Bitmap{Size{4, 2}};

        bitmap.draw(Position{0, 0}, std::vector<std::uint8_t>{0b0101U, 0b1010U});

        requireRowsEqual(
            bitmap,
            {
                "#.#.",
                ".#.#",
            });
    }

    void testFillRectFillsTheWholeInteriorAndCanClearPixels() {
        auto bitmap = Bitmap{Size{5, 5}};

        bitmap.fillRect(Rectangle{1, 1, 3, 3}, true);
        requireRowsEqual(
            bitmap,
            {
                ".....",
                ".###.",
                ".###.",
                ".###.",
                ".....",
            });

        bitmap.fillRect(Rectangle{2, 2, 1, 1}, false);
        requireRowsEqual(
            bitmap,
            {
                ".....",
                ".###.",
                ".#.#.",
                ".###.",
                ".....",
            });
    }

    void testFillRectClipsToTheBitmapBounds() {
        auto bitmap = Bitmap{Size{4, 3}};

        bitmap.fillRect(Rectangle{-1, 1, 3, 3}, true);

        requireRowsEqual(
            bitmap,
            {
                "....",
                "##..",
                "##..",
            });
    }

    void testFloodFillOnlyChangesTheConnectedRegionWithTheOriginalValue() {
        auto bitmap = Bitmap::fromPattern({
            "##..",
            "##..",
            "..##",
            "..##",
        });

        bitmap.floodFill(Position{0, 0}, false);

        requireRowsEqual(
            bitmap,
            {
                "....",
                "....",
                "..##",
                "..##",
            });
    }

    void testFloodFillIgnoresOutsideStartPositionsAndMatchingValues() {
        auto bitmap = Bitmap::fromPattern({
            "#.",
            ".#",
        });

        bitmap.floodFill(Position{-1, 0}, true);
        bitmap.floodFill(Position{0, 0}, true);

        requireRowsEqual(
            bitmap,
            {
                "#.",
                ".#",
            });
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

    void testToPatternReturnsHashDotRowsWithTrailingNewlines() {
        const auto bitmap = Bitmap::fromPattern({
            "#.",
            " #",
            "..",
        });

        REQUIRE_EQUAL(bitmap.toPattern(), std::string("#.\n.#\n..\n"));
    }
};
