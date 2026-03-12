// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "WritableBuffer.hpp"


#include "impl/BitmapPainter.hpp"
#include "impl/FramePainter.hpp"
#include "impl/TextPainter.hpp"


namespace erbsland::cterm {

void WritableBuffer::set(
    const Position pos, const Char &block, const CharCombinationStylePtr &combinationStyle) noexcept {
    if (!size().contains(pos)) {
        return;
    }
    if (combinationStyle == nullptr) {
        set(pos, block);
        return;
    }
    if (combinationStyle->isSurroundingAware()) {
        std::array<const Char *, 9> surroundingBlocks{};
        for (std::size_t i = 0; i < 9; ++i) {
            const auto surroundPosition = pos + Position{static_cast<int>(i) % 3 - 1, static_cast<int>(i) / 3 - 1};
            if (size().contains(surroundPosition)) {
                surroundingBlocks[i] = &get(surroundPosition);
            }
        }
        set(pos, combinationStyle->combine(surroundingBlocks, block));
    } else {
        set(pos, combinationStyle->combine(get(pos), block));
    }
}

void WritableBuffer::set(Position pos, const String &str) noexcept {
    if (str.empty() || !size().contains(pos)) {
        return;
    }
    const auto x = pos.x();
    for (const auto character : str) {
        if (character == U'\n') {
            pos.setX(x);
            pos += Position{0, 1};
            continue;
        }
        if (character.displayWidth() == 0) {
            continue;
        }
        set(pos, character);
        pos += Position{character.displayWidth(), 0};
    }
}

void WritableBuffer::setFrom(const ReadableBuffer &other, const Char fillChar) {
    size().forEach([&](const Position pos) -> void {
        if (other.size().contains(pos)) {
            set(pos, other.get(pos));
        } else {
            set(pos, fillChar);
        }
    });
}

void WritableBuffer::setAndResizeFrom(const ReadableBuffer &other) {
    resize(other.size());
    setFrom(other);
}

void WritableBuffer::fill(const Char &fillBlock) noexcept {
    rect().forEach([&, this](const Position pos) -> void { set(pos, fillBlock); });
}

void WritableBuffer::fill(
    const Rectangle rect, const Char &fillBlock, const CharCombinationStylePtr &combinationStyle) noexcept {

    rect.forEach([&, this](const Position pos) -> void { set(pos, fillBlock, combinationStyle); });
}

void WritableBuffer::fill(
    const Rectangle rect,
    const Tile9StylePtr &style,
    const Color baseColor,
    const CharCombinationStylePtr &combinationStyle) noexcept {

    if (style == nullptr) {
        return;
    }
    rect.forEach([&, this](const Position pos) -> void {
        set(pos, style->block(rect, pos).withBaseColor(baseColor), combinationStyle);
    });
}

void WritableBuffer::drawFrame(
    Rectangle rect, const Char &frameBlock, const CharCombinationStylePtr &combinationStyle) noexcept {
    impl::FramePainter{*this}.drawFrame(rect, frameBlock, combinationStyle);
}

void WritableBuffer::drawFrame(
    Rectangle rect,
    const Char16StylePtr &frameStyle,
    const CharCombinationStylePtr &combinationStyle,
    Color frameColor) noexcept {
    impl::FramePainter{*this}.drawFrame(rect, frameStyle, combinationStyle, frameColor);
}

void WritableBuffer::drawFrame(
    Rectangle rect,
    const Tile9StylePtr &style,
    Color frameColor,
    const CharCombinationStylePtr &combinationStyle) noexcept {
    impl::FramePainter{*this}.drawFrame(rect, style, frameColor, combinationStyle);
}

void WritableBuffer::drawFrame(Rectangle rect, FrameStyle frameStyle, Color frameColor) noexcept {
    impl::FramePainter{*this}.drawFrame(rect, frameStyle, frameColor);
}

void WritableBuffer::drawFrame(Rectangle rect, const FrameDrawOptions &options, std::size_t animationCycle) noexcept {
    impl::FramePainter{*this}.drawFrame(rect, options, animationCycle);
}

void WritableBuffer::drawFilledFrame(
    Rectangle rect,
    const Char &frameBlock,
    const Char &fillBlock,
    const CharCombinationStylePtr &combinationStyle) noexcept {
    impl::FramePainter{*this}.drawFilledFrame(rect, frameBlock, fillBlock, combinationStyle);
}

void WritableBuffer::drawFilledFrame(
    Rectangle rect,
    const Char16StylePtr &frameStyle,
    const Char &fillBlock,
    const CharCombinationStylePtr &combinationStyle,
    Color frameColor) noexcept {
    impl::FramePainter{*this}.drawFilledFrame(rect, frameStyle, fillBlock, combinationStyle, frameColor);
}

void WritableBuffer::drawFilledFrame(
    Rectangle rect,
    const Tile9StylePtr &style,
    const Char &fillBlock,
    const CharCombinationStylePtr &combinationStyle,
    Color frameColor) noexcept {
    impl::FramePainter{*this}.drawFilledFrame(rect, style, fillBlock, combinationStyle, frameColor);
}

void WritableBuffer::drawFilledFrame(
    Rectangle rect, FrameStyle frameStyle, const Char &fillBlock, Color frameColor) noexcept {
    impl::FramePainter{*this}.drawFilledFrame(rect, frameStyle, fillBlock, frameColor);
}

void WritableBuffer::drawText(Position pos, const String &str) {
    impl::TextPainter{*this}.drawText(pos, str);
}

void WritableBuffer::drawText(const Text &text, std::size_t animationCycle) {
    impl::TextPainter{*this}.drawText(text, animationCycle);
}

void WritableBuffer::drawText(
    std::string_view text, Rectangle rect, Alignment alignment, Color color, std::size_t animationCycle) {
    drawText(String{text}, rect, alignment, color, animationCycle);
}

void WritableBuffer::drawText(
    String text, Rectangle rect, Alignment alignment, Color color, std::size_t animationCycle) {
    impl::TextPainter{*this}.drawText(text, rect, alignment, color, animationCycle);
}

void WritableBuffer::drawBitmap(
    const Bitmap &bitmap, Position pos, const BitmapDrawOptions &options, std::size_t animationCycle) noexcept {
    impl::BitmapPainter{*this}.drawBitmap(bitmap, pos, options, animationCycle);
}

void WritableBuffer::drawBitmap(
    const Bitmap &bitmap,
    Rectangle rect,
    Alignment alignment,
    const BitmapDrawOptions &options,
    std::size_t animationCycle) noexcept {
    impl::BitmapPainter{*this}.drawBitmap(bitmap, rect, alignment, options, animationCycle);
}

}
