// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "WritableBuffer.hpp"


namespace erbsland::cterm::impl {

class FramePainter final {
public:
    explicit FramePainter(WritableBuffer &buffer) : _buffer(buffer) {}

    // delete move/copy
    ~FramePainter() = default;
    FramePainter(const FramePainter &) = delete;
    FramePainter(FramePainter &&) = delete;
    auto operator=(const FramePainter &) -> FramePainter & = delete;
    auto operator=(FramePainter &&) -> FramePainter & = delete;

public:
    void
    drawFrame(Rectangle rect, const Char &frameBlock, const CharCombinationStylePtr &combinationStyle = {}) noexcept;
    void drawFrame(
        Rectangle rect,
        const Char16StylePtr &frameStyle,
        const CharCombinationStylePtr &combinationStyle = {},
        Color frameColor = {}) noexcept;
    void drawFrame(
        Rectangle rect,
        const Tile9StylePtr &style,
        Color frameColor = {},
        const CharCombinationStylePtr &combinationStyle = {}) noexcept;
    void drawFrame(Rectangle rect, FrameStyle frameStyle, Color frameColor = {}) noexcept;
    void drawFrame(
        Rectangle rect,
        const FrameDrawOptions &options = FrameDrawOptions::defaultOptions(),
        std::size_t animationCycle = 0) noexcept;
    void drawFilledFrame(
        Rectangle rect,
        const Char &frameBlock,
        const Char &fillBlock,
        const CharCombinationStylePtr &combinationStyle = {}) noexcept;
    void drawFilledFrame(
        Rectangle rect,
        const Char16StylePtr &frameStyle,
        const Char &fillBlock,
        const CharCombinationStylePtr &combinationStyle = {},
        Color frameColor = {}) noexcept;
    void drawFilledFrame(
        Rectangle rect,
        const Tile9StylePtr &style,
        const Char &fillBlock,
        const CharCombinationStylePtr &combinationStyle = {},
        Color frameColor = {}) noexcept;
    void drawFilledFrame(Rectangle rect, FrameStyle frameStyle, const Char &fillBlock, Color frameColor = {}) noexcept;

private: // wrapper (to keep code simple)
    [[nodiscard]] auto rect() const noexcept -> Rectangle { return _buffer.rect(); }
    [[nodiscard]] auto get(Position pos) const noexcept -> const Char & { return _buffer.get(pos); }
    void set(Position pos, const Char &block, const CharCombinationStylePtr &combinationStyle) noexcept {
        _buffer.set(pos, block, combinationStyle);
    }
    void fill(Rectangle rect, const Char &fillBlock, const CharCombinationStylePtr &combinationStyle = {}) noexcept {
        _buffer.fill(rect, fillBlock, combinationStyle);
    }

private: // helper
    [[nodiscard]] static auto blockForFrame(Rectangle rect, Position pos, const Char16StylePtr &frameStyle) -> Char;
    void drawFrameBlock(
        Position pos, const Char &block, Color baseColor, const CharCombinationStylePtr &combinationStyle) noexcept;
    [[nodiscard]] static auto colorForFramePosition(
        const ColorSequence &colorSequence,
        FrameColorMode colorMode,
        Rectangle rect,
        Position pos,
        std::size_t animationCycle,
        std::size_t animationOffset) noexcept -> Color;

private:
    WritableBuffer &_buffer;
};

}
