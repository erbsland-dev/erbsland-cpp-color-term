// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <string>
#include <vector>


TESTED_TARGETS(Buffer)
class BufferConvenienceTest final : public el::UnitTest {
public:
    void testFillWritesTheWholeBuffer() {
        auto buffer = Buffer{Size{3, 2}};

        buffer.fill(Char{U'X', fg::Green, bg::Black});

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"XXX", "XXX"}));
        REQUIRE_EQUAL(buffer.get(Position{1, 1}).color(), Color(fg::Green, bg::Black));
    }

    void testDrawFilledFrameWithTile9StyleUsesTheFillCharacterInside() {
        auto buffer = Buffer{Size{5, 3}};

        buffer.drawFilledFrame(Rectangle{0, 0, 5, 3}, Tile9Style::create("ABCDEFGHI"), Char{U'.'});

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({"ABBBC", "D...F", "GHHHI"}));
    }

    void testDrawTextAtPositionUsesTheExistingBufferColorAsBaseColor() {
        auto buffer = Buffer{Size{4, 2}};
        buffer.fill(Char{U' ', fg::White, bg::Blue});

        buffer.drawText(Position{1, 0}, String{"A\nB"});

        REQUIRE_EQUAL(renderRows(buffer), std::vector<std::string>({" A  ", " B  "}));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::White, bg::Blue));
        REQUIRE_EQUAL(buffer.get(Position{1, 1}).color(), Color(fg::White, bg::Blue));
    }

private:
    [[nodiscard]] static auto renderRows(const Buffer &buffer) -> std::vector<std::string> {
        auto rows = std::vector<std::string>{};
        rows.reserve(static_cast<std::size_t>(buffer.size().height()));
        for (int y = 0; y < buffer.size().height(); ++y) {
            auto row = std::string{};
            row.reserve(static_cast<std::size_t>(buffer.size().width()));
            for (int x = 0; x < buffer.size().width(); ++x) {
                buffer.get(Position{x, y}).appendTo(row);
            }
            rows.push_back(row);
        }
        return rows;
    }
};
