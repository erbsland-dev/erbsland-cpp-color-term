// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <chrono>
#include <optional>

class InputDispatchProbe final : public Input {
public:
    [[nodiscard]] auto mode() const noexcept -> Mode override { return _mode; }
    void setMode(const Mode mode) override { _mode = mode; }
    [[nodiscard]] auto readLine() -> std::string override { return _readLineResult; }

public:
    Mode _mode = Mode::ReadLine;
    mutable std::chrono::milliseconds _lastTimeout{};
    mutable bool _waitForKeyWasCalled = false;
    Key _readResult{Key::Escape};
    Key _waitForKeyResult{Key::Enter};
    std::string _readLineResult = "line";

protected:
    [[nodiscard]] auto readKeyImpl(const std::chrono::milliseconds timeout) const -> Key override {
        _lastTimeout = timeout;
        return _readResult;
    }
    [[nodiscard]] auto waitForKeyImpl() const -> Key override {
        _waitForKeyWasCalled = true;
        return _waitForKeyResult;
    }
};

class ReadableBufferDispatchProbe final : public ReadableBuffer {
public:
    [[nodiscard]] auto size() const noexcept -> Size override { return _buffer.size(); }
    [[nodiscard]] auto rect() const noexcept -> Rectangle override { return _buffer.rect(); }
    [[nodiscard]] auto get(const Position pos) const noexcept -> const Char & override { return _buffer.get(pos); }
    [[nodiscard]] auto clone() const -> WritableBufferPtr override { return std::make_shared<Buffer>(_buffer); }

public:
    mutable std::u32string _characters;
    mutable bool _invert = false;

protected:
    [[nodiscard]] auto toMaskImpl(const std::u32string &characters, const bool invert) -> Bitmap override {
        _characters = characters;
        _invert = invert;
        return Bitmap{_buffer.size()};
    }

private:
    Buffer _buffer{Size{2, 1}};
};

class WritableBufferDispatchProbe final : public WritableBuffer {
public:
    enum class Call {
        None,
        Resize,
        SetFrom,
        FillBlock,
        FillTile9,
        FrameBlock,
        FrameChar16,
        FrameTile9,
        FrameOptions,
        TextObject,
        TextRect,
        BitmapPosition,
        BitmapRect,
    };

public:
    using WritableBuffer::resize;

    [[nodiscard]] auto size() const noexcept -> Size override { return _buffer.size(); }
    [[nodiscard]] auto rect() const noexcept -> Rectangle override { return _buffer.rect(); }
    [[nodiscard]] auto get(const Position pos) const noexcept -> const Char & override { return _buffer.get(pos); }
    [[nodiscard]] auto clone() const -> WritableBufferPtr override { return std::make_shared<Buffer>(_buffer); }
    void resize(const Size newSize) override {
        _lastCall = Call::Resize;
        _lastResizeSize = newSize;
        ++_resizeCallCount;
        _buffer.resize(newSize);
    }
    void set(const Position pos, const Char &block) noexcept override { _buffer.set(pos, block); }

    void clearRecording() {
        _lastCall = Call::None;
        _lastRect = {};
        _lastFillChar = {};
        _lastFrameBlock = {};
        _lastOptionalFillBlock.reset();
        _lastFrameColor = {};
        _lastAnimationCycle = 0;
        _lastAlignment = Alignment::TopLeft;
        _lastTile9Style.reset();
        _lastChar16Style.reset();
        _lastText = {};
        _lastBitmapSize = {};
        _lastPosition = {};
        _lastResizeSize = {};
        _resizeCallCount = 0;
    }

public:
    Call _lastCall = Call::None;
    Rectangle _lastRect{};
    Char _lastFillChar{};
    Char _lastFrameBlock{};
    std::optional<Char> _lastOptionalFillBlock;
    Color _lastFrameColor{};
    std::size_t _lastAnimationCycle = 0;
    Alignment _lastAlignment = Alignment::TopLeft;
    Tile9StylePtr _lastTile9Style;
    Char16StylePtr _lastChar16Style;
    String _lastText;
    Size _lastBitmapSize{};
    Position _lastPosition{};
    Size _lastResizeSize{};
    int _resizeCallCount = 0;

protected:
    void setFromImpl(const ReadableBuffer &, const Char fillChar) override {
        _lastCall = Call::SetFrom;
        _lastFillChar = fillChar;
    }

    void fillImpl(const Rectangle rect, const Char &fillBlock, const CharCombinationStylePtr &) noexcept override {
        _lastCall = Call::FillBlock;
        _lastRect = rect;
        _lastFillChar = fillBlock;
    }

    void fillImpl(
        const Rectangle rect,
        const Tile9StylePtr &style,
        const Color baseColor,
        const CharCombinationStylePtr &) noexcept override {
        _lastCall = Call::FillTile9;
        _lastRect = rect;
        _lastTile9Style = style;
        _lastFrameColor = baseColor;
    }

    void drawFrameImpl(
        const Rectangle rect,
        const Char &frameBlock,
        std::optional<Char> fillBlock,
        const CharCombinationStylePtr &) noexcept override {
        _lastCall = Call::FrameBlock;
        _lastRect = rect;
        _lastFrameBlock = frameBlock;
        _lastOptionalFillBlock = std::move(fillBlock);
    }

    void drawFrameImpl(
        const Rectangle rect,
        const Char16StylePtr &frameStyle,
        std::optional<Char> fillBlock,
        const CharCombinationStylePtr &,
        const Color frameColor) noexcept override {
        _lastCall = Call::FrameChar16;
        _lastRect = rect;
        _lastChar16Style = frameStyle;
        _lastOptionalFillBlock = std::move(fillBlock);
        _lastFrameColor = frameColor;
    }

    void drawFrameImpl(
        const Rectangle rect,
        const Tile9StylePtr &style,
        std::optional<Char> fillBlock,
        const CharCombinationStylePtr &,
        const Color frameColor) noexcept override {
        _lastCall = Call::FrameTile9;
        _lastRect = rect;
        _lastTile9Style = style;
        _lastOptionalFillBlock = std::move(fillBlock);
        _lastFrameColor = frameColor;
    }

    void
    drawFrameImpl(const Rectangle rect, const FrameDrawOptions &, const std::size_t animationCycle) noexcept override {
        _lastCall = Call::FrameOptions;
        _lastRect = rect;
        _lastAnimationCycle = animationCycle;
    }

    void drawTextImpl(const Text &text, const std::size_t animationCycle) override {
        _lastCall = Call::TextObject;
        _lastRect = text.rectangle();
        _lastText = text.text();
        _lastAnimationCycle = animationCycle;
    }

    void drawTextImpl(
        const StringView &text,
        const Rectangle rect,
        const Alignment alignment,
        const Color,
        const std::size_t animationCycle) override {
        _lastCall = Call::TextRect;
        _lastRect = rect;
        _lastText = String{text};
        _lastAlignment = alignment;
        _lastAnimationCycle = animationCycle;
    }

    void drawBitmapImpl(
        const Bitmap &bitmap,
        const Position pos,
        const BitmapDrawOptions &,
        const std::size_t animationCycle) noexcept override {
        _lastCall = Call::BitmapPosition;
        _lastBitmapSize = bitmap.size();
        _lastPosition = pos;
        _lastAnimationCycle = animationCycle;
    }

    void drawBitmapImpl(
        const Bitmap &bitmap,
        const Rectangle rect,
        const Alignment alignment,
        const BitmapDrawOptions &,
        const std::size_t animationCycle) noexcept override {
        _lastCall = Call::BitmapRect;
        _lastBitmapSize = bitmap.size();
        _lastRect = rect;
        _lastAlignment = alignment;
        _lastAnimationCycle = animationCycle;
    }

private:
    Buffer _buffer{Size{4, 4}};
};

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
