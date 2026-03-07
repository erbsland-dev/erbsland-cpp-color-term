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
            term::Char{"X", term::Color{term::fg::Default, term::bg::Yellow}},
            term::CharCombinationStyle::colorOverlay());

        REQUIRE_EQUAL(buffer.get(Position(1, 1)).charStr(), std::string{"X"});
        REQUIRE_EQUAL(buffer.get(Position(1, 1)).color(), term::Color(term::fg::Green, term::bg::Yellow));
        REQUIRE_EQUAL(buffer.get(Position(0, 0)).charStr(), std::string{"A"});
        REQUIRE_EQUAL(buffer.get(Position(0, 0)).color(), term::Color(term::fg::Green, term::bg::Blue));
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

    void testDrawFrameWithStyleAppliesBaseFrameColorBeforeTileColor() {
        auto tiles = std::array<term::Char, 16>{};
        tiles[3] = term::Char{"┌", term::Color{term::fg::Red, term::bg::Default}};
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

    void testDrawFilledFrameWithStyleAppliesCombinationStyleToFill() {
        term::Buffer buffer(Size(3, 3));
        buffer.fill(term::Char{"A", term::Color{term::fg::Green, term::bg::Blue}});

        buffer.drawFilledFrame(
            Rectangle{0, 0, 3, 3},
            term::Char16Style::lightFrame(),
            term::Char{" ", term::Color{term::fg::Default, term::bg::Yellow}},
            term::CharCombinationStyle::colorOverlay());

        REQUIRE_EQUAL(buffer.get(Position(1, 1)).charStr(), std::string{" "});
        REQUIRE_EQUAL(buffer.get(Position(1, 1)).color(), term::Color(term::fg::Green, term::bg::Yellow));
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
