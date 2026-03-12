// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <array>
#include <memory>
#include <string>
#include <vector>

TESTED_TARGETS(Buffer)
class BlockBufferTest final : public el::UnitTest {
public:
    void testConstructorAndFill() {
        Buffer buffer(Size(2, 3));
        REQUIRE_EQUAL(buffer.size(), Size(2, 3));
        buffer.size().forEach([&](const Position pos) { REQUIRE_EQUAL(buffer.get(pos), U' '); });
    }

    void testFillConstructor() {
        const Char fill{"X", fg(fg::Red), bg(bg::Blue)};
        Buffer buffer(Size(2, 2));
        buffer.fill(fill);
        buffer.size().forEach([&](const Position pos) {
            const auto &block = buffer.get(pos);
            REQUIRE_EQUAL(block, U'X');
            REQUIRE_EQUAL(block.color(), Color(fg::Red, bg::Blue));
        });
    }

    void testDefaultAndFillConstructorsInitializeCellsAndValidateSize() {
        const auto fill = Char{U'X', fg::Yellow, bg::Blue};

        const auto placeholder = Buffer{};
        REQUIRE_EQUAL(placeholder.size(), Size(1, 1));
        REQUIRE_EQUAL(placeholder.get(Position{0, 0}), U' ');

        const auto filled = Buffer{Size{2, 2}, fill};
        filled.size().forEach([&](const Position pos) -> void {
            REQUIRE_EQUAL(filled.get(pos), U'X');
            REQUIRE_EQUAL(filled.get(pos).color(), Color(fg::Yellow, bg::Blue));
        });

        REQUIRE_THROWS_AS(std::invalid_argument, Buffer(Size{0, 1}));
        REQUIRE_THROWS_AS(std::invalid_argument, Buffer(Size{1, 0}));
        REQUIRE_THROWS_AS(std::invalid_argument, Buffer(Size{10'001, 1}));
    }

    void testResizeWithReorderPreservesContentAndFillsExpandedCells() {
        auto buffer = Buffer{Size{2, 2}};
        buffer.set(Position{0, 0}, Char{U'A'});
        buffer.set(Position{1, 0}, Char{U'B'});
        buffer.set(Position{0, 1}, Char{U'C'});
        buffer.set(Position{1, 1}, Char{U'D'});

        buffer.resize(Size{3, 3}, true, Char{U'.'});

        REQUIRE_EQUAL(buffer.size(), Size(3, 3));
        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"AB.", "CD.", "..."}));
    }

    void testResizeWithReorderCropsToTheNewRectangle() {
        auto buffer = Buffer{Size{3, 2}};
        buffer.set(Position{0, 0}, Char{U'A'});
        buffer.set(Position{1, 0}, Char{U'B'});
        buffer.set(Position{2, 0}, Char{U'C'});
        buffer.set(Position{0, 1}, Char{U'D'});
        buffer.set(Position{1, 1}, Char{U'E'});
        buffer.set(Position{2, 1}, Char{U'F'});

        buffer.resize(Size{2, 1}, true, Char{U'.'});

        REQUIRE_EQUAL(buffer.size(), Size(2, 1));
        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"AB"}));
    }

    void testResizeRejectsInvalidSizes() {
        auto buffer = Buffer{Size{2, 2}};

        REQUIRE_THROWS_AS(std::invalid_argument, buffer.resize(Size{0, 2}));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.resize(Size{2, 0}));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.resize(Size{10'001, 1}));
    }

    void testSetAndGet() {
        Buffer buffer(Size(2, 2));
        Char block{U'A', fg::Green, bg::Black};
        buffer.set(Position(1, 0), block);
        REQUIRE_EQUAL(buffer.get(Position(1, 0)), U'A');
        REQUIRE_EQUAL(buffer.get(Position(1, 0)).color(), Color(fg::Green, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position(0, 0)), U' ');
        buffer.set(Position(5, 5), Char{"B", fg(fg::Blue), bg(bg::Red)});
        REQUIRE_EQUAL(buffer.get(Position(0, 1)), U' ');
    }

    void testSetWideCharacterWithLvalueFillsTheFollowingCell() {
        Buffer buffer(Size(4, 1));
        const auto block = Char{U'界', fg::Green, bg::Blue};

        buffer.set(Position(1, 0), block);

        REQUIRE_EQUAL(buffer.get(Position(1, 0)), U'界');
        REQUIRE_EQUAL(buffer.get(Position(1, 0)).color(), Color(fg::Green, bg::Blue));
        REQUIRE(buffer.get(Position(2, 0)).isEmpty());
        REQUIRE_EQUAL(buffer.get(Position(2, 0)).color(), Color(fg::Green, bg::Blue));
        REQUIRE_EQUAL(buffer.get(Position(3, 0)), U' ');
        REQUIRE_EQUAL(buffer.get(Position(3, 0)).color(), Color{});
    }

    void testSetWideCharacterWithRvalueFillsTheFollowingCell() {
        Buffer buffer(Size(4, 1));

        buffer.set(Position(0, 0), Char{U'界', fg::Yellow, bg::Black});

        REQUIRE_EQUAL(buffer.get(Position(0, 0)), U'界');
        REQUIRE_EQUAL(buffer.get(Position(0, 0)).color(), Color(fg::Yellow, bg::Black));
        REQUIRE(buffer.get(Position(1, 0)).isEmpty());
        REQUIRE_EQUAL(buffer.get(Position(1, 0)).color(), Color(fg::Yellow, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position(2, 0)), U' ');
        REQUIRE_EQUAL(buffer.get(Position(2, 0)).color(), Color{});
    }

    void testSetStringWritesMultipleLinesAndSkipsZeroWidthCharacters() {
        auto buffer = Buffer{Size{4, 2}, Char{U'.'}};
        auto text = String{};
        text.append(Char{U'界', fg::Red, bg::Black});
        text.append(Char{});
        text.append(Char{U'A', fg::Green, bg::Black});
        text.append(Char{U'\n'});
        text.append(Char{U'B', fg::Blue, bg::Black});

        buffer.set(Position{0, 0}, text);

        REQUIRE_EQUAL(buffer.get(Position{0, 0}), U'界');
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Red, bg::Black));
        REQUIRE(buffer.get(Position{1, 0}).isEmpty());
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::Red, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{2, 0}), U'A');
        REQUIRE_EQUAL(buffer.get(Position{2, 0}).color(), Color(fg::Green, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{0, 1}), U'B');
        REQUIRE_EQUAL(buffer.get(Position{0, 1}).color(), Color(fg::Blue, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{3, 1}), U'.');
    }

    void testFromLinesFactoriesBuildBuffersFromPlainAndWideText() {
        const auto lines = StringLines{String{"ab"}, String{}, String{"c"}};
        const auto fromLines = Buffer::fromLines(lines);

        REQUIRE_EQUAL(fromLines.size(), Size(2, 3));
        REQUIRE_EQUAL(renderRows(fromLines), std::vector<std::string>({"ab", "  ", "c "}));

        const auto fromText = Buffer::fromLinesInString(String{"界\nA"});

        REQUIRE_EQUAL(fromText.size(), Size(2, 2));
        REQUIRE_EQUAL(fromText.get(Position{0, 0}), U'界');
        REQUIRE(fromText.get(Position{1, 0}).isEmpty());
        REQUIRE_EQUAL(fromText.get(Position{1, 0}).color(), Color{});
        REQUIRE_EQUAL(renderRows(fromText), std::vector<std::string>({"界 ", "A "}));
    }

    void testFromLinesFactoriesRejectEmptyInput() {
        REQUIRE_THROWS_AS(std::invalid_argument, Buffer::fromLines(StringLines{}));
        REQUIRE_THROWS_AS(std::invalid_argument, Buffer::fromLinesInString(String{}));
    }

    void testToMaskMatchesCharactersFromStringAndCanInvertTheSelection() {
        auto buffer = Buffer{Size{3, 2}};
        buffer.set(Position{0, 0}, Char{U'A', fg::Red, bg::Black});
        buffer.set(Position{1, 0}, Char{U'B', fg::Blue, bg::Black});
        buffer.set(Position{2, 0}, Char{U'C', fg::Green, bg::Black});
        buffer.set(Position{0, 1}, Char{U'B', fg::Yellow, bg::Black});
        buffer.set(Position{1, 1}, Char{U' ', fg::White, bg::Black});
        buffer.set(Position{2, 1}, Char{U'A', fg::Magenta, bg::Black});

        const auto mask = buffer.toMask(U"AB");
        const auto invertedMask = buffer.toMask(U"AB", true);

        REQUIRE(mask.pixel(Position{0, 0}));
        REQUIRE(mask.pixel(Position{1, 0}));
        REQUIRE_FALSE(mask.pixel(Position{2, 0}));
        REQUIRE(mask.pixel(Position{0, 1}));
        REQUIRE_FALSE(mask.pixel(Position{1, 1}));
        REQUIRE(mask.pixel(Position{2, 1}));

        REQUIRE_FALSE(invertedMask.pixel(Position{0, 0}));
        REQUIRE_FALSE(invertedMask.pixel(Position{1, 0}));
        REQUIRE(invertedMask.pixel(Position{2, 0}));
        REQUIRE_FALSE(invertedMask.pixel(Position{0, 1}));
        REQUIRE(invertedMask.pixel(Position{1, 1}));
        REQUIRE_FALSE(invertedMask.pixel(Position{2, 1}));
    }

    void testToMaskMatchesCharactersFromInitializerList() {
        auto buffer = Buffer{Size{2, 2}};
        buffer.set(Position{0, 0}, Char{U'X'});
        buffer.set(Position{1, 0}, Char{U' '});
        buffer.set(Position{0, 1}, Char{U'Y'});
        buffer.set(Position{1, 1}, Char{U'Z'});

        const auto mask = buffer.toMask({U'Y', U'Z'});

        REQUIRE_FALSE(mask.pixel(Position{0, 0}));
        REQUIRE_FALSE(mask.pixel(Position{1, 0}));
        REQUIRE(mask.pixel(Position{0, 1}));
        REQUIRE(mask.pixel(Position{1, 1}));
    }

    void testToMaskWithEmptyCharacterSetsReturnsAClearedBitmap() {
        auto buffer = Buffer{Size{2, 2}};
        buffer.set(Position{0, 0}, Char{U'X'});
        buffer.set(Position{1, 1}, Char{U'Y'});

        const auto stringMask = buffer.toMask(U"");
        const auto listMask = buffer.toMask({});

        REQUIRE_EQUAL(stringMask.size(), Size(2, 2));
        REQUIRE_FALSE(stringMask.pixel(Position{0, 0}));
        REQUIRE_FALSE(stringMask.pixel(Position{1, 0}));
        REQUIRE_FALSE(stringMask.pixel(Position{0, 1}));
        REQUIRE_FALSE(stringMask.pixel(Position{1, 1}));

        REQUIRE_EQUAL(listMask.size(), Size(2, 2));
        REQUIRE_FALSE(listMask.pixel(Position{0, 0}));
        REQUIRE_FALSE(listMask.pixel(Position{1, 0}));
        REQUIRE_FALSE(listMask.pixel(Position{0, 1}));
        REQUIRE_FALSE(listMask.pixel(Position{1, 1}));
    }

    void testFillRectangleAppliesCombinationStyle() {
        Buffer buffer(Size(3, 3));
        buffer.fill(Char{U'A', fg::Green, bg::Blue});

        buffer.fill(Rectangle{1, 1, 1, 1}, Char{U'X', fg::Inherited, bg::Yellow}, CharCombinationStyle::colorOverlay());

        REQUIRE_EQUAL(buffer.get(Position(1, 1)), U'X');
        REQUIRE_EQUAL(buffer.get(Position(1, 1)).color(), Color(fg::Green, bg::Yellow));
        REQUIRE_EQUAL(buffer.get(Position(0, 0)), U'A');
        REQUIRE_EQUAL(buffer.get(Position(0, 0)).color(), Color(fg::Green, bg::Blue));
    }

    void testFillRectangleWithTile9StyleRepeatsEdgesAndCenter() {
        Buffer buffer(Size(4, 3));

        buffer.fill(Rectangle{0, 0, 4, 3}, Tile9Style::create("ABCDEFGHI"));

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"ABBC", "DEEF", "GHHI"}));
    }

    void testFillRectangleWithTile9StyleAppliesBaseColorBeforeTileColor() {
        auto tiles = std::array<Char, 9>{};
        tiles[0] = Char{U'A', fg::Red, bg::Inherited};
        tiles[1] = Char{U'B'};
        tiles[2] = Char{U'C'};
        tiles[3] = Char{U'D'};
        tiles[4] = Char{U'E'};
        tiles[5] = Char{U'F'};
        tiles[6] = Char{U'G'};
        tiles[7] = Char{U'H'};
        tiles[8] = Char{U'I'};
        const auto style = std::make_shared<Tile9Style>(tiles);
        Buffer buffer(Size(2, 2));

        buffer.fill(Rectangle{0, 0, 2, 2}, style, Color{fg::Blue, bg::Black});

        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Red, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::Blue, bg::Black));
    }

    void testDrawFrameWithCharacterMarksTheWholePerimeter() {
        Buffer buffer(Size(3, 3));

        buffer.drawFrame(Rectangle{0, 0, 3, 3}, Char{U'#'});

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"###", "# #", "###"}));
    }

    void testDrawFrameWithStyleRendersExpectedBoxCharacters() {
        Buffer buffer(Size(4, 3));

        buffer.drawFrame(Rectangle{0, 0, 4, 3}, FrameStyle::LightWithRoundedCorners);

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"╭──╮", "│  │", "╰──╯"}));
    }

    void testDrawFrameWithTile9StyleOmitsTheCenterTile() {
        Buffer buffer(Size(4, 3));

        buffer.drawFrame(Rectangle{0, 0, 4, 3}, Tile9Style::create("ABCDEFGHI"));

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"ABBC", "D  F", "GHHI"}));
    }

    void testDrawFrameWithNewPredefinedStylesRendersExpectedShapes() {
        {
            Buffer buffer(Size(4, 3));
            buffer.drawFrame(Rectangle{0, 0, 4, 3}, FrameStyle::LightDoubleDash);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"┌╌╌┐", "╎  ╎", "└╌╌┘"}));
        }
        {
            Buffer buffer(Size(4, 3));
            buffer.drawFrame(Rectangle{0, 0, 4, 3}, FrameStyle::HeavyQuadrupleDash);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"┏┉┉┓", "┋  ┋", "┗┉┉┛"}));
        }
        {
            Buffer buffer(Size(4, 3));
            buffer.drawFrame(Rectangle{0, 0, 4, 3}, FrameStyle::FullBlock);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"████", "█  █", "████"}));
        }
        {
            Buffer buffer(Size(4, 3));
            buffer.drawFrame(Rectangle{0, 0, 4, 3}, FrameStyle::FullBlockWithChamfer);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"◢██◣", "█  █", "◥██◤"}));
        }
        {
            Buffer buffer(Size(4, 3));
            buffer.drawFrame(Rectangle{0, 0, 4, 3}, FrameStyle::OuterHalfBlock);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"▛▀▀▜", "▌  ▐", "▙▄▄▟"}));
        }
        {
            Buffer buffer(Size(4, 3));
            buffer.drawFrame(Rectangle{0, 0, 4, 3}, FrameStyle::InnerHalfBlock);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"▗▄▄▖", "▐  ▌", "▝▀▀▘"}));
        }
    }

    void testDrawFrameWithTile9StylesHandlesDegenerateRectangles() {
        {
            Buffer buffer(Size(1, 1));
            buffer.drawFrame(Rectangle{0, 0, 1, 1}, FrameStyle::OuterHalfBlock);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"█"}));
        }
        {
            Buffer buffer(Size(3, 1));
            buffer.drawFrame(Rectangle{0, 0, 3, 1}, FrameStyle::OuterHalfBlock);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"███"}));
        }
        {
            Buffer buffer(Size(1, 3));
            buffer.drawFrame(Rectangle{0, 0, 1, 3}, FrameStyle::InnerHalfBlock);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"█", "█", "█"}));
        }
    }

    void testDrawFrameWithStyleAppliesBaseFrameColorBeforeTileColor() {
        auto tiles = std::array<Char, 16>{};
        tiles[3] = Char{U'┌', fg::Red, bg::Inherited};
        tiles[5] = Char{U'─'};
        tiles[6] = Char{U'┐'};
        tiles[9] = Char{U'└'};
        tiles[10] = Char{U'│'};
        tiles[12] = Char{U'┘'};
        const auto style = std::make_shared<Char16Style>(tiles);
        Buffer buffer(Size(3, 3));

        buffer.drawFrame(Rectangle{0, 0, 3, 3}, style, CharCombinationStylePtr{}, Color{fg::Blue, bg::Black});

        REQUIRE_EQUAL(buffer.get(Position(0, 0)).color(), Color(fg::Red, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position(1, 0)).color(), Color(fg::Blue, bg::Black));
    }

    void testDrawFilledFrameWithTile9FrameStyleFillsTheInterior() {
        Buffer buffer(Size(4, 3));

        buffer.drawFilledFrame(Rectangle{0, 0, 4, 3}, FrameStyle::OuterHalfBlock, Char{U'.'});

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"▛▀▀▜", "▌..▐", "▙▄▄▟"}));
    }

    void testDrawFilledFrameWithStyleAppliesCombinationStyleToFill() {
        Buffer buffer(Size(3, 3));
        buffer.fill(Char{U'A', fg::Green, bg::Blue});

        buffer.drawFilledFrame(
            Rectangle{0, 0, 3, 3},
            Char16Style::lightFrame(),
            Char{U' ', fg::Inherited, bg::Yellow},
            CharCombinationStyle::colorOverlay());

        REQUIRE_EQUAL(buffer.get(Position(1, 1)), U' ');
        REQUIRE_EQUAL(buffer.get(Position(1, 1)).color(), Color(fg::Green, bg::Yellow));
    }

    void testDrawFrameWithOptionsAppliesFrameAndFillColorOverlayOrder() {
        Buffer buffer(Size(3, 3));
        buffer.fill(Char{U' ', fg::Green, bg::Blue});
        auto options = FrameDrawOptions{};
        options.setFrameColor(Color{fg::Red, bg::Inherited});
        options.setFillColor(Color{fg::Inherited, bg::Yellow});
        options.setFillBlock(Char{U'.', fg::White, bg::Inherited});

        buffer.drawFrame(Rectangle{0, 0, 3, 3}, options);

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"┌─┐", "│.│", "└─┘"}));
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Red, bg::Blue));
        REQUIRE_EQUAL(buffer.get(Position{1, 1}).color(), Color(fg::White, bg::Yellow));
    }

    void testDrawFrameWithOptionsUsesStripeModesAcrossFrameAndFill() {
        Buffer buffer(Size(4, 3));
        buffer.fill(Char{U' ', fg::White, bg::Black});
        auto options = FrameDrawOptions{};
        options.setFrameColorSequence(
            ColorSequence{
                Color{fg::Red, bg::Inherited},
                Color{fg::Blue, bg::Inherited},
            },
            FrameColorMode::VerticalStripes);
        options.setFillColorSequence(
            ColorSequence{
                Color{fg::Inherited, bg::Yellow},
                Color{fg::Inherited, bg::Magenta},
            },
            FrameColorMode::HorizontalStripes);
        options.setFillBlock(Char{U' '});

        buffer.drawFrame(Rectangle{0, 0, 4, 3}, options);

        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Red, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::Blue, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{0, 1}).color(), Color(fg::Red, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{1, 1}).color(), Color(fg::Blue, bg::Magenta));
        REQUIRE_EQUAL(buffer.get(Position{2, 1}).color(), Color(fg::Red, bg::Magenta));
    }

    void testDrawFrameWithOptionsPrefersCustomStylesAndTile9Fill() {
        auto customTiles = std::array<Char, 16>{};
        customTiles.fill(Char{U'X'});
        const auto customStyle = std::make_shared<Char16Style>(customTiles);
        auto options = FrameDrawOptions{};
        options.setStyle(FrameStyle::LightWithRoundedCorners);
        options.setChar16Style(customStyle);

        {
            Buffer buffer(Size(4, 3));
            buffer.drawFrame(Rectangle{0, 0, 4, 3}, options);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"XXXX", "X  X", "XXXX"}));
        }
        {
            Buffer buffer(Size(4, 3));
            options.setTile9Style(Tile9Style::create("ABCDEFGHI"));
            options.setFillBlock(Char{U'.'});
            buffer.drawFrame(Rectangle{0, 0, 4, 3}, options);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"ABBC", "DEEF", "GHHI"}));
        }
    }

    void testDrawFrameWithOptionsChasingBorderModesAnimateClockwiseAndCounterClockwise() {
        const auto colors = ColorSequence{
            Color{fg::Red, bg::Black},
            Color{fg::Green, bg::Black},
            Color{fg::Blue, bg::Black},
        };

        {
            Buffer buffer(Size(4, 4));
            auto options = FrameDrawOptions{};
            options.setFrameColorSequence(colors, FrameColorMode::ChasingBorderCW);

            buffer.drawFrame(Rectangle{0, 0, 4, 4}, options, 1);

            REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Blue, bg::Black));
            REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::Red, bg::Black));
            REQUIRE_EQUAL(buffer.get(Position{2, 0}).color(), Color(fg::Green, bg::Black));
            REQUIRE_EQUAL(buffer.get(Position{3, 1}).color(), Color(fg::Red, bg::Black));
            REQUIRE_EQUAL(buffer.get(Position{0, 1}).color(), Color(fg::Green, bg::Black));
        }
        {
            Buffer buffer(Size(4, 4));
            auto options = FrameDrawOptions{};
            options.setFrameColorSequence(colors, FrameColorMode::ChasingBorderCCW);

            buffer.drawFrame(Rectangle{0, 0, 4, 4}, options, 1);

            REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Green, bg::Black));
            REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::Blue, bg::Black));
            REQUIRE_EQUAL(buffer.get(Position{2, 0}).color(), Color(fg::Red, bg::Black));
            REQUIRE_EQUAL(buffer.get(Position{3, 1}).color(), Color(fg::Blue, bg::Black));
            REQUIRE_EQUAL(buffer.get(Position{0, 1}).color(), Color(fg::Red, bg::Black));
        }
    }

    void testDrawBitmapHalfBlocksUseQuadMaskAndBaseColor() {
        Buffer buffer(Size(1, 1));
        auto bitmap = Bitmap{Size{2, 2}};
        bitmap.setPixel(Position{0, 0}, true);
        bitmap.setPixel(Position{1, 1}, true);
        auto options = BitmapDrawOptions{};
        options.setColor(Color{fg::BrightGreen, bg::Blue});

        buffer.drawBitmap(bitmap, Position{0, 0}, options);

        REQUIRE_EQUAL(buffer.get(Position{0, 0}), U'▚');
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::BrightGreen, bg::Blue));
    }

    void testDrawBitmapFullBlocksOnlyAffectSetPixels() {
        Buffer buffer(Size(3, 2));
        buffer.fill(Char{U' ', fg::White, bg::Black});
        auto bitmap = Bitmap{Size{2, 2}};
        bitmap.setPixel(Position{1, 0}, true);
        auto options = BitmapDrawOptions{};
        options.setScaleMode(BitmapScaleMode::FullBlock);
        options.setColor(Color{fg::BrightYellow, bg::Blue});

        buffer.drawBitmap(bitmap, Position{0, 0}, options);

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({" █ ", "   "}));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::BrightYellow, bg::Blue));
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::White, bg::Black));
    }

    void testDrawBitmapDoubleBlocksUseBothConfiguredCharacters() {
        Buffer buffer(Size(2, 1));
        auto bitmap = Bitmap{Size{1, 1}};
        bitmap.setPixel(Position{0, 0}, true);
        auto options = BitmapDrawOptions{};
        options.setScaleMode(BitmapScaleMode::DoubleBlock);
        options.setDoubleBlocks(String{"[]"});
        options.setColor(Color{fg::BrightCyan, bg::Black});

        buffer.drawBitmap(bitmap, Position{0, 0}, options);

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"[]"}));
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::BrightCyan, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::BrightCyan, bg::Black));
    }

    void testDrawBitmapRectCroppingUsesAlignment() {
        Buffer buffer(Size(2, 1));
        auto bitmap = Bitmap{Size{4, 1}};
        bitmap.setPixel(Position{0, 0}, true);
        bitmap.setPixel(Position{3, 0}, true);
        auto options = BitmapDrawOptions{};
        options.setScaleMode(BitmapScaleMode::FullBlock);

        buffer.drawBitmap(bitmap, Rectangle{0, 0, 2, 1}, Alignment::Right, options);

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({" █"}));
    }

    void testDrawBitmapChar16StyleUsesNeighborConnections() {
        Buffer buffer(Size(3, 1));
        auto bitmap = Bitmap{Size{3, 1}};
        bitmap.setPixel(Position{0, 0}, true);
        bitmap.setPixel(Position{1, 0}, true);
        bitmap.setPixel(Position{2, 0}, true);
        auto options = BitmapDrawOptions{};
        options.setChar16Style(Char16Style::lightFrame());

        buffer.drawBitmap(bitmap, Position{0, 0}, options);

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"╶─╴"}));
    }

    void testDrawBitmapVerticalStripesUseBitmapPixelsForDoubleBlocks() {
        Buffer buffer(Size(4, 1));
        auto bitmap = Bitmap{Size{2, 1}};
        bitmap.setPixel(Position{0, 0}, true);
        bitmap.setPixel(Position{1, 0}, true);
        auto options = BitmapDrawOptions{};
        options.setScaleMode(BitmapScaleMode::DoubleBlock);
        options.setColorSequence(
            ColorSequence{
                Color{fg::Red, bg::Black},
                Color{fg::Blue, bg::Black},
            },
            BitmapColorMode::VerticalStripes);

        buffer.drawBitmap(bitmap, Position{0, 0}, options);

        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Red, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::Red, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{2, 0}).color(), Color(fg::Blue, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{3, 0}).color(), Color(fg::Blue, bg::Black));
    }

    void testDrawBitmapCanInheritExistingBufferColor() {
        Buffer buffer(Size(1, 1));
        buffer.fill(Char{U' ', fg::BrightWhite, bg::Magenta});
        auto bitmap = Bitmap{Size{1, 1}};
        bitmap.setPixel(Position{0, 0}, true);
        auto options = BitmapDrawOptions{};
        options.setScaleMode(BitmapScaleMode::FullBlock);
        options.setColorSequence(ColorSequence{});

        buffer.drawBitmap(bitmap, Position{0, 0}, options);

        REQUIRE_EQUAL(buffer.get(Position{0, 0}), U'█');
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::BrightWhite, bg::Magenta));
    }

    void testDrawBitmapBackwardDiagonalStripesWrapNegativeIndexes() {
        Buffer buffer(Size(2, 1));
        auto bitmap = Bitmap{Size{2, 1}};
        bitmap.setPixel(Position{0, 0}, true);
        bitmap.setPixel(Position{1, 0}, true);
        auto options = BitmapDrawOptions{};
        options.setScaleMode(BitmapScaleMode::FullBlock);
        options.setColorSequence(
            ColorSequence{
                Color{fg::Red, bg::Black},
                Color{fg::Blue, bg::Black},
            },
            BitmapColorMode::BackwardDiagonalStripes);

        buffer.drawBitmap(bitmap, Position{0, 0}, options);

        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Red, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::Blue, bg::Black));
    }

private:
    [[nodiscard]] static auto renderRows(const Buffer &buffer) -> std::vector<std::string> {
        auto rows = std::vector<std::string>{};
        rows.reserve(static_cast<std::size_t>(buffer.size().height()));
        for (auto y = 0; y < buffer.size().height(); ++y) {
            auto row = std::string{};
            for (auto x = 0; x < buffer.size().width(); ++x) {
                const auto &block = buffer.get(Position{x, y});
                row += block.charStr().empty() ? " " : block.charStr();
            }
            rows.push_back(row);
        }
        return rows;
    }
};
