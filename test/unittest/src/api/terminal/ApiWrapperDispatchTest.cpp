// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "InputDispatchProbe.hpp"
#include "ReadableBufferDispatchProbe.hpp"
#include "WritableBufferDispatchProbe.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(Input ReadableBuffer WritableBuffer)
class ApiWrapperDispatchTest final : public el::UnitTest {
public:
    void testInputReadKeyAndWaitForKeyDelegateToTheImplementation() {
        auto input = InputDispatchProbe{};

        REQUIRE_EQUAL(input.readKey(std::chrono::milliseconds{-5}), Key{Key::Escape});
        REQUIRE_EQUAL(input._lastTimeout, std::chrono::milliseconds{0});

        REQUIRE_EQUAL(input.readKey(std::chrono::milliseconds{125}), Key{Key::Escape});
        REQUIRE_EQUAL(input._lastTimeout, std::chrono::milliseconds{125});

        REQUIRE_EQUAL(input.waitForKey(), Key{Key::Enter});
        REQUIRE(input._waitForKeyWasCalled);
    }

    void testReadableBufferToMaskOverloadsDelegateToToMaskImpl() {
        auto buffer = ReadableBufferDispatchProbe{};

        REQUIRE_EQUAL(buffer.toMask(U"AB").size(), (Size{2, 1}));
        REQUIRE_EQUAL(buffer._characters, std::u32string{U"AB"});
        REQUIRE_FALSE(buffer._invert);

        REQUIRE_EQUAL(buffer.toMask({U'X', U'Y'}, true).size(), (Size{2, 1}));
        REQUIRE_EQUAL(buffer._characters, std::u32string{U"XY"});
        REQUIRE(buffer._invert);
    }

    void testWritableBufferSetFromAndFillDelegateToImplMethods() {
        auto buffer = WritableBufferDispatchProbe{};
        auto source = Buffer{Size{2, 2}};

        buffer.setFrom(source);
        REQUIRE_EQUAL(buffer._lastCall, WritableBufferDispatchProbe::Call::SetFrom);
        REQUIRE_EQUAL(buffer._lastFillChar, Char::space());

        buffer.clearRecording();
        buffer.fill(Rectangle{0, 0, 2, 2}, Char{U'.'});
        REQUIRE_EQUAL(buffer._lastCall, WritableBufferDispatchProbe::Call::FillBlock);
        REQUIRE_EQUAL(buffer._lastRect, (Rectangle{0, 0, 2, 2}));
        REQUIRE_EQUAL(buffer._lastFillChar, Char{U'.'});

        buffer.clearRecording();
        buffer.fill(Rectangle{0, 0, 3, 3}, Tile9Style::create("ABCDEFGHI"), Color{fg::Blue, bg::Black});
        REQUIRE_EQUAL(buffer._lastCall, WritableBufferDispatchProbe::Call::FillTile9);
        REQUIRE(buffer._lastTile9Style != nullptr);
        REQUIRE_EQUAL(buffer._lastFrameColor, Color(fg::Blue, bg::Black));
    }

    void testWritableBufferResizeWithModeUsesTheDefaultImplementation() {
        auto buffer = WritableBufferDispatchProbe{};

        buffer.resize(Size{5, 3}, BufferResizeMode::Fast, Char{U'.'});
        REQUIRE_EQUAL(buffer._lastCall, WritableBufferDispatchProbe::Call::Resize);
        REQUIRE_EQUAL(buffer._lastResizeSize, (Size{5, 3}));
        REQUIRE_EQUAL(buffer._resizeCallCount, 1);

        buffer.clearRecording();
        buffer.resize(Size{6, 4}, BufferResizeMode::PreserveContent, Char{U'.'});
        REQUIRE_EQUAL(buffer._lastCall, WritableBufferDispatchProbe::Call::SetFrom);
        REQUIRE_EQUAL(buffer._lastFillChar, Char{U'.'});
        REQUIRE_EQUAL(buffer._resizeCallCount, 1);
        REQUIRE_EQUAL(buffer._lastResizeSize, (Size{6, 4}));
    }

    void testWritableBufferFrameWrappersDelegateToGroupedImplMethods() {
        auto buffer = WritableBufferDispatchProbe{};

        buffer.drawFrame(Rectangle{0, 0, 2, 2}, Char{U'#'});
        REQUIRE_EQUAL(buffer._lastCall, WritableBufferDispatchProbe::Call::FrameBlock);
        REQUIRE_FALSE(buffer._lastOptionalFillBlock.has_value());
        REQUIRE_EQUAL(buffer._lastFrameBlock, Char{U'#'});

        buffer.clearRecording();
        buffer.drawFilledFrame(Rectangle{0, 0, 2, 2}, Char{U'#'}, Char{U'.'});
        REQUIRE_EQUAL(buffer._lastCall, WritableBufferDispatchProbe::Call::FrameBlock);
        REQUIRE(buffer._lastOptionalFillBlock.has_value());
        REQUIRE_EQUAL(*buffer._lastOptionalFillBlock, Char{U'.'});

        buffer.clearRecording();
        buffer.drawFrame(Rectangle{0, 0, 3, 3}, FrameStyle::Light, Color{fg::Green, bg::Black});
        REQUIRE_EQUAL(buffer._lastCall, WritableBufferDispatchProbe::Call::FrameChar16);
        REQUIRE(buffer._lastChar16Style != nullptr);
        REQUIRE_FALSE(buffer._lastOptionalFillBlock.has_value());
        REQUIRE_EQUAL(buffer._lastFrameColor, Color(fg::Green, bg::Black));

        buffer.clearRecording();
        buffer.drawFilledFrame(Rectangle{0, 0, 3, 3}, FrameStyle::OuterHalfBlock, Char{U'.'});
        REQUIRE_EQUAL(buffer._lastCall, WritableBufferDispatchProbe::Call::FrameTile9);
        REQUIRE(buffer._lastTile9Style != nullptr);
        REQUIRE(buffer._lastOptionalFillBlock.has_value());
        REQUIRE_EQUAL(*buffer._lastOptionalFillBlock, Char{U'.'});

        buffer.clearRecording();
        buffer.drawFrame(Rectangle{0, 0, 3, 3}, FrameDrawOptions::defaultOptions(), 7);
        REQUIRE_EQUAL(buffer._lastCall, WritableBufferDispatchProbe::Call::FrameOptions);
        REQUIRE_EQUAL(buffer._lastAnimationCycle, std::size_t{7});

        buffer.clearRecording();
        auto border = FrameBorder{FrameStyle::Light, Color{fg::Green, bg::Black}};
        buffer.drawGridLayout(Position{1, 2}, GridLayout{{2, 3}, {1, 2}}, border);
        REQUIRE_EQUAL(buffer._lastCall, WritableBufferDispatchProbe::Call::GridLayout);
        REQUIRE_EQUAL(buffer._lastPosition, (Position{1, 2}));
        REQUIRE(buffer._lastGridLayout.has_value());
        REQUIRE_EQUAL(buffer._lastGridLayout->size(border), (Size{8, 6}));
        REQUIRE_EQUAL(buffer._lastFrameBorder, border);
    }

    void testWritableBufferTextWrappersDelegateToImplMethods() {
        auto buffer = WritableBufferDispatchProbe{};

        buffer.drawText(Text{String{"Hello"}, Rectangle{1, 2, 3, 4}});
        REQUIRE_EQUAL(buffer._lastCall, WritableBufferDispatchProbe::Call::TextObject);
        REQUIRE_EQUAL(buffer._lastRect, (Rectangle{1, 2, 3, 4}));
        REQUIRE_EQUAL(buffer._lastText.size(), std::size_t{5});
        REQUIRE_EQUAL(buffer._lastText[0], Char{U'H'});
        REQUIRE_EQUAL(buffer._lastText[4], Char{U'o'});
        REQUIRE_EQUAL(buffer._lastAnimationCycle, std::size_t{0});

        buffer.clearRecording();
        buffer.drawText("Hi", Rectangle{0, 0, 5, 1}, Alignment::Center, Color{fg::Yellow, bg::Blue}, 9);
        REQUIRE_EQUAL(buffer._lastCall, WritableBufferDispatchProbe::Call::TextRect);
        REQUIRE_EQUAL(buffer._lastRect, (Rectangle{0, 0, 5, 1}));
        REQUIRE_EQUAL(buffer._lastText.size(), std::size_t{2});
        REQUIRE_EQUAL(buffer._lastText[0], Char{U'H'});
        REQUIRE_EQUAL(buffer._lastText[1], Char{U'i'});
        REQUIRE_EQUAL(buffer._lastAlignment, Alignment::Center);
        REQUIRE_EQUAL(buffer._lastAnimationCycle, std::size_t{9});
    }

    void testWritableBufferBitmapWrappersDelegateToImplMethods() {
        auto buffer = WritableBufferDispatchProbe{};
        auto bitmap = Bitmap{Size{2, 3}};

        buffer.drawBitmap(bitmap, Position{1, 2});
        REQUIRE_EQUAL(buffer._lastCall, WritableBufferDispatchProbe::Call::BitmapPosition);
        REQUIRE_EQUAL(buffer._lastBitmapSize, (Size{2, 3}));
        REQUIRE_EQUAL(buffer._lastPosition, (Position{1, 2}));
        REQUIRE_EQUAL(buffer._lastAnimationCycle, std::size_t{0});

        buffer.clearRecording();
        buffer.drawBitmap(
            bitmap, Rectangle{0, 0, 4, 5}, Alignment::BottomRight, BitmapDrawOptions::defaultOptions(), 3);
        REQUIRE_EQUAL(buffer._lastCall, WritableBufferDispatchProbe::Call::BitmapRect);
        REQUIRE_EQUAL(buffer._lastBitmapSize, (Size{2, 3}));
        REQUIRE_EQUAL(buffer._lastRect, (Rectangle{0, 0, 4, 5}));
        REQUIRE_EQUAL(buffer._lastAlignment, Alignment::BottomRight);
        REQUIRE_EQUAL(buffer._lastAnimationCycle, std::size_t{3});
    }
};
