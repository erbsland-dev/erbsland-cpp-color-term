// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/unittest/UnitTest.hpp>

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "TestHelper.hpp"

class BlockBufferTest : public el::UnitTest {
public:
    void testConstructorAndFill() {
        term::Buffer buffer(Size(2, 3));
        REQUIRE_EQUAL(buffer.size(), Size(2, 3));
        buffer.size().forEach([&](const Position pos) { REQUIRE_EQUAL(buffer.get(pos).charStr(), std::string{}); });
    }

    void testFillConstructor() {
        const term::Char fill{"X", term::fg(term::fg::Red), term::bg(term::bg::Blue)};
        term::Buffer buffer(Size(2, 2));
        buffer.fill(fill);
        buffer.size().forEach([&](const Position pos) {
            const auto &block = buffer.get(pos);
            REQUIRE_EQUAL(block.charStr(), std::string{"X"});
            REQUIRE_EQUAL(block.color(), term::Color(term::fg::Red, term::bg::Blue));
        });
    }

    void testSetAndGet() {
        term::Buffer buffer(Size(2, 2));
        term::Char block{"A", term::fg(term::fg::Green), term::bg(term::bg::Black)};
        buffer.set(Position(1, 0), block);
        REQUIRE_EQUAL(buffer.get(Position(1, 0)).charStr(), std::string{"A"});
        REQUIRE_EQUAL(buffer.get(Position(1, 0)).color(), term::Color(term::fg::Green, term::bg::Black));
        REQUIRE_EQUAL(buffer.get(Position(0, 0)).charStr(), std::string{});
        buffer.set(Position(5, 5), term::Char{"B", term::fg(term::fg::Blue), term::bg(term::bg::Red)});
        REQUIRE_EQUAL(buffer.get(Position(0, 1)).charStr(), std::string{});
    }

    void testSetWideCharacterWithLvalueFillsTheFollowingCell() {
        term::Buffer buffer(Size(4, 1));
        const auto block = term::Char{"界", term::Color{term::fg::Green, term::bg::Blue}};

        buffer.set(Position(1, 0), block);

        REQUIRE_EQUAL(buffer.get(Position(1, 0)).charStr(), std::string{"界"});
        REQUIRE_EQUAL(buffer.get(Position(1, 0)).color(), term::Color(term::fg::Green, term::bg::Blue));
        REQUIRE_EQUAL(buffer.get(Position(2, 0)).charStr(), std::string{});
        REQUIRE_EQUAL(buffer.get(Position(2, 0)).color(), term::Color(term::fg::Green, term::bg::Blue));
        REQUIRE_EQUAL(buffer.get(Position(3, 0)).charStr(), std::string{});
        REQUIRE_EQUAL(buffer.get(Position(3, 0)).color(), term::Color{});
    }

    void testSetWideCharacterWithRvalueFillsTheFollowingCell() {
        term::Buffer buffer(Size(4, 1));

        buffer.set(Position(0, 0), term::Char{"界", term::Color{term::fg::Yellow, term::bg::Black}});

        REQUIRE_EQUAL(buffer.get(Position(0, 0)).charStr(), std::string{"界"});
        REQUIRE_EQUAL(buffer.get(Position(0, 0)).color(), term::Color(term::fg::Yellow, term::bg::Black));
        REQUIRE_EQUAL(buffer.get(Position(1, 0)).charStr(), std::string{});
        REQUIRE_EQUAL(buffer.get(Position(1, 0)).color(), term::Color(term::fg::Yellow, term::bg::Black));
        REQUIRE_EQUAL(buffer.get(Position(2, 0)).charStr(), std::string{});
        REQUIRE_EQUAL(buffer.get(Position(2, 0)).color(), term::Color{});
    }

    void testFillRectangleAppliesCombinationStyle() {
        term::Buffer buffer(Size(3, 3));
        buffer.fill(term::Char{"A", term::Color{term::fg::Green, term::bg::Blue}});

        buffer.fill(
            Rectangle{1, 1, 1, 1},
            term::Char{"X", term::Color{term::fg::Inherited, term::bg::Yellow}},
            term::CharCombinationStyle::colorOverlay());

        REQUIRE_EQUAL(buffer.get(Position(1, 1)).charStr(), std::string{"X"});
        REQUIRE_EQUAL(buffer.get(Position(1, 1)).color(), term::Color(term::fg::Green, term::bg::Yellow));
        REQUIRE_EQUAL(buffer.get(Position(0, 0)).charStr(), std::string{"A"});
        REQUIRE_EQUAL(buffer.get(Position(0, 0)).color(), term::Color(term::fg::Green, term::bg::Blue));
    }

    void testFillRectangleWithTile9StyleRepeatsEdgesAndCenter() {
        term::Buffer buffer(Size(4, 3));

        buffer.fill(Rectangle{0, 0, 4, 3}, term::Tile9Style::create("ABCDEFGHI"));

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"ABBC", "DEEF", "GHHI"}));
    }

    void testFillRectangleWithTile9StyleAppliesBaseColorBeforeTileColor() {
        auto tiles = std::array<term::Char, 9>{};
        tiles[0] = term::Char{"A", term::Color{term::fg::Red, term::bg::Inherited}};
        tiles[1] = term::Char{"B"};
        tiles[2] = term::Char{"C"};
        tiles[3] = term::Char{"D"};
        tiles[4] = term::Char{"E"};
        tiles[5] = term::Char{"F"};
        tiles[6] = term::Char{"G"};
        tiles[7] = term::Char{"H"};
        tiles[8] = term::Char{"I"};
        const auto style = std::make_shared<term::Tile9Style>(tiles);
        term::Buffer buffer(Size(2, 2));

        buffer.fill(Rectangle{0, 0, 2, 2}, style, term::Color{term::fg::Blue, term::bg::Black});

        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), term::Color(term::fg::Red, term::bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), term::Color(term::fg::Blue, term::bg::Black));
    }

    void testDrawFrameWithCharacterMarksTheWholePerimeter() {
        term::Buffer buffer(Size(3, 3));

        buffer.drawFrame(Rectangle{0, 0, 3, 3}, term::Char{"#"});

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"###", "# #", "###"}));
    }

    void testDrawFrameWithStyleRendersExpectedBoxCharacters() {
        term::Buffer buffer(Size(4, 3));

        buffer.drawFrame(Rectangle{0, 0, 4, 3}, term::FrameStyle::LightWithRoundedCorners);

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"╭──╮", "│  │", "╰──╯"}));
    }

    void testDrawFrameWithTile9StyleOmitsTheCenterTile() {
        term::Buffer buffer(Size(4, 3));

        buffer.drawFrame(Rectangle{0, 0, 4, 3}, term::Tile9Style::create("ABCDEFGHI"));

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"ABBC", "D  F", "GHHI"}));
    }

    void testDrawFrameWithNewPredefinedStylesRendersExpectedShapes() {
        {
            term::Buffer buffer(Size(4, 3));
            buffer.drawFrame(Rectangle{0, 0, 4, 3}, term::FrameStyle::LightDoubleDash);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"┌╌╌┐", "╎  ╎", "└╌╌┘"}));
        }
        {
            term::Buffer buffer(Size(4, 3));
            buffer.drawFrame(Rectangle{0, 0, 4, 3}, term::FrameStyle::HeavyQuadrupleDash);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"┏┉┉┓", "┋  ┋", "┗┉┉┛"}));
        }
        {
            term::Buffer buffer(Size(4, 3));
            buffer.drawFrame(Rectangle{0, 0, 4, 3}, term::FrameStyle::FullBlock);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"████", "█  █", "████"}));
        }
        {
            term::Buffer buffer(Size(4, 3));
            buffer.drawFrame(Rectangle{0, 0, 4, 3}, term::FrameStyle::FullBlockWithChamfer);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"◢██◣", "█  █", "◥██◤"}));
        }
        {
            term::Buffer buffer(Size(4, 3));
            buffer.drawFrame(Rectangle{0, 0, 4, 3}, term::FrameStyle::OuterHalfBlock);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"▛▀▀▜", "▌  ▐", "▙▄▄▟"}));
        }
        {
            term::Buffer buffer(Size(4, 3));
            buffer.drawFrame(Rectangle{0, 0, 4, 3}, term::FrameStyle::InnerHalfBlock);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"▗▄▄▖", "▐  ▌", "▝▀▀▘"}));
        }
    }

    void testDrawFrameWithTile9StylesHandlesDegenerateRectangles() {
        {
            term::Buffer buffer(Size(1, 1));
            buffer.drawFrame(Rectangle{0, 0, 1, 1}, term::FrameStyle::OuterHalfBlock);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"█"}));
        }
        {
            term::Buffer buffer(Size(3, 1));
            buffer.drawFrame(Rectangle{0, 0, 3, 1}, term::FrameStyle::OuterHalfBlock);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"███"}));
        }
        {
            term::Buffer buffer(Size(1, 3));
            buffer.drawFrame(Rectangle{0, 0, 1, 3}, term::FrameStyle::InnerHalfBlock);
            REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"█", "█", "█"}));
        }
    }

    void testDrawFrameWithStyleAppliesBaseFrameColorBeforeTileColor() {
        auto tiles = std::array<term::Char, 16>{};
        tiles[3] = term::Char{"┌", term::Color{term::fg::Red, term::bg::Inherited}};
        tiles[5] = term::Char{"─"};
        tiles[6] = term::Char{"┐"};
        tiles[9] = term::Char{"└"};
        tiles[10] = term::Char{"│"};
        tiles[12] = term::Char{"┘"};
        const auto style = std::make_shared<term::Char16Style>(tiles);
        term::Buffer buffer(Size(3, 3));

        buffer.drawFrame(
            Rectangle{0, 0, 3, 3},
            style,
            term::CharCombinationStylePtr{},
            term::Color{term::fg::Blue, term::bg::Black});

        REQUIRE_EQUAL(buffer.get(Position(0, 0)).color(), term::Color(term::fg::Red, term::bg::Black));
        REQUIRE_EQUAL(buffer.get(Position(1, 0)).color(), term::Color(term::fg::Blue, term::bg::Black));
    }

    void testDrawFilledFrameWithTile9FrameStyleFillsTheInterior() {
        term::Buffer buffer(Size(4, 3));

        buffer.drawFilledFrame(Rectangle{0, 0, 4, 3}, term::FrameStyle::OuterHalfBlock, term::Char{"."});

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"▛▀▀▜", "▌..▐", "▙▄▄▟"}));
    }

    void testDrawFilledFrameWithStyleAppliesCombinationStyleToFill() {
        term::Buffer buffer(Size(3, 3));
        buffer.fill(term::Char{"A", term::Color{term::fg::Green, term::bg::Blue}});

        buffer.drawFilledFrame(
            Rectangle{0, 0, 3, 3},
            term::Char16Style::lightFrame(),
            term::Char{" ", term::Color{term::fg::Inherited, term::bg::Yellow}},
            term::CharCombinationStyle::colorOverlay());

        REQUIRE_EQUAL(buffer.get(Position(1, 1)).charStr(), std::string{" "});
        REQUIRE_EQUAL(buffer.get(Position(1, 1)).color(), term::Color(term::fg::Green, term::bg::Yellow));
    }

    void testDrawBitmapHalfBlocksUseQuadMaskAndBaseColor() {
        term::Buffer buffer(Size(1, 1));
        auto bitmap = Bitmap{Size{2, 2}};
        bitmap.setPixel(Position{0, 0}, true);
        bitmap.setPixel(Position{1, 1}, true);
        auto options = term::BitmapDrawOptions{};
        options.setColor(term::Color{term::fg::BrightGreen, term::bg::Blue});

        buffer.drawBitmap(bitmap, Position{0, 0}, options);

        REQUIRE_EQUAL(buffer.get(Position{0, 0}).charStr(), std::string{"▚"});
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), term::Color(term::fg::BrightGreen, term::bg::Blue));
    }

    void testDrawBitmapFullBlocksOnlyAffectSetPixels() {
        term::Buffer buffer(Size(3, 2));
        buffer.fill(term::Char{" ", term::Color{term::fg::White, term::bg::Black}});
        auto bitmap = Bitmap{Size{2, 2}};
        bitmap.setPixel(Position{1, 0}, true);
        auto options = term::BitmapDrawOptions{};
        options.setScaleMode(term::BitmapScaleMode::FullBlock);
        options.setColor(term::Color{term::fg::BrightYellow, term::bg::Blue});

        buffer.drawBitmap(bitmap, Position{0, 0}, options);

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({" █ ", "   "}));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), term::Color(term::fg::BrightYellow, term::bg::Blue));
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), term::Color(term::fg::White, term::bg::Black));
    }

    void testDrawBitmapDoubleBlocksUseBothConfiguredCharacters() {
        term::Buffer buffer(Size(2, 1));
        auto bitmap = Bitmap{Size{1, 1}};
        bitmap.setPixel(Position{0, 0}, true);
        auto options = term::BitmapDrawOptions{};
        options.setScaleMode(term::BitmapScaleMode::DoubleBlock);
        options.setDoubleBlocks(term::String{"[]"});
        options.setColor(term::Color{term::fg::BrightCyan, term::bg::Black});

        buffer.drawBitmap(bitmap, Position{0, 0}, options);

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"[]"}));
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), term::Color(term::fg::BrightCyan, term::bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), term::Color(term::fg::BrightCyan, term::bg::Black));
    }

    void testDrawBitmapRectCroppingUsesAlignment() {
        term::Buffer buffer(Size(2, 1));
        auto bitmap = Bitmap{Size{4, 1}};
        bitmap.setPixel(Position{0, 0}, true);
        bitmap.setPixel(Position{3, 0}, true);
        auto options = term::BitmapDrawOptions{};
        options.setScaleMode(term::BitmapScaleMode::FullBlock);

        buffer.drawBitmap(bitmap, Rectangle{0, 0, 2, 1}, Alignment::Right, options);

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({" █"}));
    }

    void testDrawBitmapChar16StyleUsesNeighborConnections() {
        term::Buffer buffer(Size(3, 1));
        auto bitmap = Bitmap{Size{3, 1}};
        bitmap.setPixel(Position{0, 0}, true);
        bitmap.setPixel(Position{1, 0}, true);
        bitmap.setPixel(Position{2, 0}, true);
        auto options = term::BitmapDrawOptions{};
        options.setChar16Style(term::Char16Style::lightFrame());

        buffer.drawBitmap(bitmap, Position{0, 0}, options);

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"╶─╴"}));
    }

    void testDrawBitmapVerticalStripesUseBitmapPixelsForDoubleBlocks() {
        term::Buffer buffer(Size(4, 1));
        auto bitmap = Bitmap{Size{2, 1}};
        bitmap.setPixel(Position{0, 0}, true);
        bitmap.setPixel(Position{1, 0}, true);
        auto options = term::BitmapDrawOptions{};
        options.setScaleMode(term::BitmapScaleMode::DoubleBlock);
        options.setColorSequence(
            term::ColorSequence{
                term::Color{term::fg::Red, term::bg::Black},
                term::Color{term::fg::Blue, term::bg::Black},
            },
            term::BitmapColorMode::VerticalStripes);

        buffer.drawBitmap(bitmap, Position{0, 0}, options);

        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), term::Color(term::fg::Red, term::bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), term::Color(term::fg::Red, term::bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{2, 0}).color(), term::Color(term::fg::Blue, term::bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{3, 0}).color(), term::Color(term::fg::Blue, term::bg::Black));
    }

    void testDrawBitmapCanInheritExistingBufferColor() {
        term::Buffer buffer(Size(1, 1));
        buffer.fill(term::Char{" ", term::Color{term::fg::BrightWhite, term::bg::Magenta}});
        auto bitmap = Bitmap{Size{1, 1}};
        bitmap.setPixel(Position{0, 0}, true);
        auto options = term::BitmapDrawOptions{};
        options.setScaleMode(term::BitmapScaleMode::FullBlock);
        options.setColorSequence(term::ColorSequence{});

        buffer.drawBitmap(bitmap, Position{0, 0}, options);

        REQUIRE_EQUAL(buffer.get(Position{0, 0}).charStr(), std::string{"█"});
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), term::Color(term::fg::BrightWhite, term::bg::Magenta));
    }

    void testDrawBitmapBackwardDiagonalStripesWrapNegativeIndexes() {
        term::Buffer buffer(Size(2, 1));
        auto bitmap = Bitmap{Size{2, 1}};
        bitmap.setPixel(Position{0, 0}, true);
        bitmap.setPixel(Position{1, 0}, true);
        auto options = term::BitmapDrawOptions{};
        options.setScaleMode(term::BitmapScaleMode::FullBlock);
        options.setColorSequence(
            term::ColorSequence{
                term::Color{term::fg::Red, term::bg::Black},
                term::Color{term::fg::Blue, term::bg::Black},
            },
            term::BitmapColorMode::BackwardDiagonalStripes);

        buffer.drawBitmap(bitmap, Position{0, 0}, options);

        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), term::Color(term::fg::Red, term::bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), term::Color(term::fg::Blue, term::bg::Black));
    }

private:
    [[nodiscard]] static auto renderRows(const term::Buffer &buffer) -> std::vector<std::string> {
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
