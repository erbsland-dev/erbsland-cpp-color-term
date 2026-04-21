// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "WritableBuffer.hpp"

#include <functional>
#include <optional>
#include <vector>

namespace erbsland::cterm::impl {

class FramePainter final {
    using FrameBorderReference = std::reference_wrapper<const FrameBorder::Border>;
    using OptionalFrameBorderReference = std::optional<FrameBorderReference>;

    struct FrameLine final {
        Coordinate coordinate;
        FrameBorderReference border;
    };

    using FrameLineList = std::vector<FrameLine>;

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
    void drawGridLayout(Position pos, const GridLayout &layout, const FrameBorder &border) noexcept;
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
    [[nodiscard]] static auto blockForGridLine(const FrameBorder::Border &border, uint32_t bitMask) noexcept -> Char;
    [[nodiscard]] static auto lineSize(const FrameBorder::Border &border) noexcept -> Coordinate;
    [[nodiscard]] static auto lineAt(const FrameLineList &lines, Coordinate coordinate, std::size_t &index) noexcept
        -> OptionalFrameBorderReference;
    void addGridLine(FrameLineList &lines, Coordinate coordinate, const FrameBorder::Border &border) noexcept;
    void drawHorizontalGridLine(
        const FrameLine &horizontalLine,
        const FrameLineList &verticalLines,
        Coordinate x1,
        Coordinate x2,
        Coordinate y1,
        Coordinate y2) noexcept;
    void drawVerticalGridLine(
        const FrameLine &verticalLine, const FrameLineList &horizontalLines, Coordinate y1, Coordinate y2) noexcept;
    void drawGridBlock(
        Position pos,
        OptionalFrameBorderReference east,
        OptionalFrameBorderReference south,
        OptionalFrameBorderReference west,
        OptionalFrameBorderReference north) noexcept;
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
