// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(Buffer)
class BufferConvenienceTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testFillWritesTheWholeBuffer() {
        auto buffer = Buffer{Size{3, 2}};

        buffer.fill(Char{U'X', fg::Green, bg::Black});

        requireRowsEqual(buffer, {"XXX", "XXX"});
        REQUIRE_EQUAL(buffer.get(Position{1, 1}).color(), Color(fg::Green, bg::Black));
    }

    void testDrawFilledFrameWithTile9StyleUsesTheFillCharacterInside() {
        auto buffer = Buffer{Size{5, 3}};

        buffer.drawFilledFrame(Rectangle{0, 0, 5, 3}, Tile9Style::create("ABCDEFGHI"), Char{U'.'});

        requireRowsEqual(buffer, {"ABBBC", "D...F", "GHHHI"});
    }

    void testDrawTextAtPositionUsesTheExistingBufferColorAsBaseColor() {
        auto buffer = Buffer{Size{4, 2}};
        buffer.fill(Char{U' ', fg::White, bg::Blue});

        buffer.drawText(Position{1, 0}, String{"A\nB"});

        requireRowsEqual(buffer, {" A  ", " B  "});
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::White, bg::Blue));
        REQUIRE_EQUAL(buffer.get(Position{1, 1}).color(), Color(fg::White, bg::Blue));
    }

    void testDrawBufferAtPositionUsesExistingTargetColorAsBaseColor() {
        auto source = Buffer{Size{2, 1}};
        source.set(Position{0, 0}, Char{U'A', fg::Red});
        source.set(Position{1, 0}, Char{U'B', fg::Green});
        auto buffer = Buffer{Size{4, 1}};
        buffer.fill(Char{U' ', fg::White, bg::Blue});

        buffer.drawBuffer(source, Position{1, 0});

        requireRowsEqual(buffer, {" AB "});
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::Red, bg::Blue));
        REQUIRE_EQUAL(buffer.get(Position{2, 0}).color(), Color(fg::Green, bg::Blue));
    }

    void testDrawBufferRectangleUsesAlignmentWhenCroppingTheSource() {
        auto source = createBuffer({"ABCD"});
        auto buffer = Buffer{Size{2, 1}};

        buffer.drawBuffer(source, Rectangle{0, 0, 2, 1}, Alignment::Right);

        requireRowsEqual(buffer, {"CD"});
    }

    void testDrawBufferOptionsCanOverwriteColorsAndCropTheSourceRectangle() {
        auto source = createBuffer({"ABCD"});
        source.set(Position{1, 0}, Char{U'B', fg::Red});
        source.set(Position{2, 0}, Char{U'C', fg::Green});
        auto buffer = Buffer{Size{2, 1}};
        buffer.fill(Char{U' ', fg::White, bg::Blue});
        auto options = BufferDrawOptions{};
        options.setSourceRect(Rectangle{1, 0, 2, 1});
        options.setOverwriteColors(true);

        buffer.drawBuffer(source, options);

        requireRowsEqual(buffer, {"BC"});
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Red, bg::Inherited));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::Green, bg::Inherited));
    }

    void testDrawBufferRejectsDrawingOntoTheSameBuffer() {
        auto buffer = createBuffer({"ABC "});

        REQUIRE_THROWS_AS(std::invalid_argument, buffer.drawBuffer(buffer, Position{1, 0}));
    }
};
