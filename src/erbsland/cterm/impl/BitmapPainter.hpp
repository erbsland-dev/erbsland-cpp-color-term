// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "WritableBuffer.hpp"


namespace erbsland::cterm::impl {


class BitmapPainter {
public:
    explicit BitmapPainter(WritableBuffer &buffer) : _buffer{buffer} {}

    // delete copy/move
    BitmapPainter(const BitmapPainter &) = delete;
    BitmapPainter(BitmapPainter &&) = delete;
    BitmapPainter &operator=(const BitmapPainter &) = delete;
    BitmapPainter &operator=(BitmapPainter &&) = delete;

public:
    virtual void drawBitmap(
        const Bitmap &bitmap,
        Position pos,
        const BitmapDrawOptions &options = BitmapDrawOptions::defaultOptions(),
        std::size_t animationCycle = 0) noexcept;
    virtual void drawBitmap(
        const Bitmap &bitmap,
        Rectangle rect,
        Alignment alignment = Alignment::TopLeft,
        const BitmapDrawOptions &options = BitmapDrawOptions::defaultOptions(),
        std::size_t animationCycle = 0) noexcept;

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
    [[nodiscard]] static auto bitmapRenderSize(const Bitmap &bitmap, const BitmapDrawOptions &options) noexcept -> Size;
    [[nodiscard]] static auto
    alignedBitmapOffset(int renderedSize, int availableSize, Alignment alignment, Alignment alignmentMask) noexcept
        -> int;
    [[nodiscard]] auto colorForBitmapPosition(
        const BitmapDrawOptions &options, Position bitmapPosition, std::size_t animationCycle) const noexcept -> Color;
    void drawBitmapBlock(Position pos, const Char &block, Color baseColor, const BitmapDrawOptions &options) noexcept;

private:
    WritableBuffer &_buffer;
};


}
