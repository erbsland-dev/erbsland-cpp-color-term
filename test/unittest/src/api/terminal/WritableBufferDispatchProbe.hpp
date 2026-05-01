// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/TestHelper.hpp"

#include <memory>
#include <optional>

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
        GridLayout,
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
        _lastBaseStyle = {};
        _lastFrameColor = {};
        _lastAnimationCycle = 0;
        _lastAlignment = Alignment::TopLeft;
        _lastGridLayout.reset();
        _lastFrameBorder = {};
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
    CharStyle _lastBaseStyle{};
    Color _lastFrameColor{};
    std::size_t _lastAnimationCycle = 0;
    Alignment _lastAlignment = Alignment::TopLeft;
    std::optional<GridLayout> _lastGridLayout;
    FrameBorder _lastFrameBorder;
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
        const CharStyle baseStyle,
        const CharCombinationStylePtr &) noexcept override {
        _lastCall = Call::FillTile9;
        _lastRect = rect;
        _lastTile9Style = style;
        _lastBaseStyle = baseStyle;
        _lastFrameColor = baseStyle.color();
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

    void drawGridLayoutImpl(const Position pos, const GridLayout &layout, const FrameBorder &border) noexcept override {
        _lastCall = Call::GridLayout;
        _lastPosition = pos;
        _lastGridLayout = layout;
        _lastFrameBorder = border;
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
        CharStyle style,
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
