// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "WritableBuffer.hpp"


#include "impl/BitmapPainter.hpp"
#include "impl/FramePainter.hpp"
#include "impl/TextPainter.hpp"


namespace erbsland::cterm {

void WritableBuffer::resize(const Size newSize, const BufferResizeMode mode, const Char fillChar) {
    if (size() == newSize) {
        return;
    }
    if (mode == BufferResizeMode::Fast) {
        resize(newSize);
        return;
    }
    const auto previous = clone();
    resize(newSize);
    setFrom(*previous, fillChar);
}

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
            const auto surroundPosition =
                pos + Position{static_cast<Coordinate>(i) % 3 - 1, static_cast<Coordinate>(i) / 3 - 1};
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
    setFromImpl(other, fillChar);
}

void WritableBuffer::setFromImpl(const ReadableBuffer &other, const Char fillChar) {
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
    fillImpl(rect, fillBlock, combinationStyle);
}

void WritableBuffer::fillImpl(
    const Rectangle rect, const Char &fillBlock, const CharCombinationStylePtr &combinationStyle) noexcept {
    rect.forEach([&, this](const Position pos) -> void { set(pos, fillBlock, combinationStyle); });
}

void WritableBuffer::fill(
    const Rectangle rect,
    const Tile9StylePtr &style,
    const Color baseColor,
    const CharCombinationStylePtr &combinationStyle) noexcept {
    fillImpl(rect, style, baseColor, combinationStyle);
}

void WritableBuffer::fillImpl(
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
    const Rectangle rect, const Char &frameBlock, const CharCombinationStylePtr &combinationStyle) noexcept {
    drawFrameImpl(rect, frameBlock, std::nullopt, combinationStyle);
}

void WritableBuffer::drawFrameImpl(
    const Rectangle rect,
    const Char &frameBlock,
    const std::optional<Char> fillBlock,
    const CharCombinationStylePtr &combinationStyle) noexcept {
    if (fillBlock.has_value()) {
        impl::FramePainter{*this}.drawFilledFrame(rect, frameBlock, *fillBlock, combinationStyle);
        return;
    }
    impl::FramePainter{*this}.drawFrame(rect, frameBlock, combinationStyle);
}

void WritableBuffer::drawFrame(
    const Rectangle rect,
    const Char16StylePtr &frameStyle,
    const CharCombinationStylePtr &combinationStyle,
    const Color frameColor) noexcept {
    drawFrameImpl(rect, frameStyle, std::nullopt, combinationStyle, frameColor);
}

void WritableBuffer::drawFrameImpl(
    const Rectangle rect,
    const Char16StylePtr &frameStyle,
    const std::optional<Char> fillBlock,
    const CharCombinationStylePtr &combinationStyle,
    const Color frameColor) noexcept {
    if (fillBlock.has_value()) {
        impl::FramePainter{*this}.drawFilledFrame(rect, frameStyle, *fillBlock, combinationStyle, frameColor);
        return;
    }
    impl::FramePainter{*this}.drawFrame(rect, frameStyle, combinationStyle, frameColor);
}

void WritableBuffer::drawFrame(
    const Rectangle rect,
    const Tile9StylePtr &style,
    const Color frameColor,
    const CharCombinationStylePtr &combinationStyle) noexcept {
    drawFrameImpl(rect, style, std::nullopt, combinationStyle, frameColor);
}

void WritableBuffer::drawFrameImpl(
    const Rectangle rect,
    const Tile9StylePtr &style,
    const std::optional<Char> fillBlock,
    const CharCombinationStylePtr &combinationStyle,
    const Color frameColor) noexcept {
    if (fillBlock.has_value()) {
        impl::FramePainter{*this}.drawFilledFrame(rect, style, *fillBlock, combinationStyle, frameColor);
        return;
    }
    impl::FramePainter{*this}.drawFrame(rect, style, frameColor, combinationStyle);
}

void WritableBuffer::drawFrame(const Rectangle rect, const FrameStyle frameStyle, const Color frameColor) noexcept {
    if (const auto tile9Style = Tile9Style::forStyle(frameStyle); tile9Style != nullptr) {
        drawFrameImpl(rect, tile9Style, std::nullopt, CharCombinationStyle::commonBoxFrame(), frameColor);
        return;
    }
    if (const auto char16Style = Char16Style::forStyle(frameStyle); char16Style != nullptr) {
        drawFrameImpl(rect, char16Style, std::nullopt, CharCombinationStyle::commonBoxFrame(), frameColor);
    }
}

void WritableBuffer::drawFrame(Rectangle rect, const FrameDrawOptions &options, std::size_t animationCycle) noexcept {
    drawFrameImpl(rect, options, animationCycle);
}

void WritableBuffer::drawFrameImpl(
    const Rectangle rect, const FrameDrawOptions &options, const std::size_t animationCycle) noexcept {
    impl::FramePainter{*this}.drawFrame(rect, options, animationCycle);
}

void WritableBuffer::drawFilledFrame(
    const Rectangle rect,
    const Char &frameBlock,
    const Char &fillBlock,
    const CharCombinationStylePtr &combinationStyle) noexcept {
    drawFrameImpl(rect, frameBlock, fillBlock, combinationStyle);
}

void WritableBuffer::drawFilledFrame(
    const Rectangle rect,
    const Char16StylePtr &frameStyle,
    const Char &fillBlock,
    const CharCombinationStylePtr &combinationStyle,
    const Color frameColor) noexcept {
    drawFrameImpl(rect, frameStyle, fillBlock, combinationStyle, frameColor);
}

void WritableBuffer::drawFilledFrame(
    const Rectangle rect,
    const Tile9StylePtr &style,
    const Char &fillBlock,
    const CharCombinationStylePtr &combinationStyle,
    const Color frameColor) noexcept {
    drawFrameImpl(rect, style, fillBlock, combinationStyle, frameColor);
}

void WritableBuffer::drawFilledFrame(
    const Rectangle rect, const FrameStyle frameStyle, const Char &fillBlock, const Color frameColor) noexcept {
    if (const auto tile9Style = Tile9Style::forStyle(frameStyle); tile9Style != nullptr) {
        drawFrameImpl(rect, tile9Style, fillBlock, CharCombinationStyle::commonBoxFrame(), frameColor);
        return;
    }
    if (const auto char16Style = Char16Style::forStyle(frameStyle); char16Style != nullptr) {
        drawFrameImpl(rect, char16Style, fillBlock, CharCombinationStyle::commonBoxFrame(), frameColor);
    }
}

void WritableBuffer::drawText(Position pos, const String &str) {
    impl::TextPainter{*this}.drawText(pos, str);
}

void WritableBuffer::drawText(const Text &text, std::size_t animationCycle) {
    drawTextImpl(text, animationCycle);
}

void WritableBuffer::drawTextImpl(const Text &text, const std::size_t animationCycle) {
    impl::TextPainter{*this}.drawText(text, animationCycle);
}

void WritableBuffer::drawText(
    const std::string_view text,
    const Rectangle rect,
    const Alignment alignment,
    const Color color,
    const std::size_t animationCycle) {
    drawText(String{text, EncodingErrors::Replace}, rect, alignment, color, animationCycle);
}

void WritableBuffer::drawText(
    const String &text,
    const Rectangle rect,
    const Alignment alignment,
    const Color color,
    const std::size_t animationCycle) {
    drawTextImpl(text, rect, alignment, color, animationCycle);
}

void WritableBuffer::drawText(
    const String &text, const Rectangle rect, const TextOptions &options, const std::size_t animationCycle) {
    drawTextImpl(text, rect, options, animationCycle);
}

void WritableBuffer::drawTextImpl(
    const String &text,
    const Rectangle rect,
    const Alignment alignment,
    const Color color,
    const std::size_t animationCycle) {
    impl::TextPainter{*this}.drawText(text, rect, alignment, color, animationCycle);
}

void WritableBuffer::drawTextImpl(
    const String &text, const Rectangle rect, const TextOptions &options, const std::size_t animationCycle) {
    impl::TextPainter{*this}.drawText(text, rect, options, animationCycle);
}

void WritableBuffer::drawBitmap(
    const Bitmap &bitmap,
    const Position pos,
    const BitmapDrawOptions &options,
    const std::size_t animationCycle) noexcept {
    drawBitmapImpl(bitmap, pos, options, animationCycle);
}

void WritableBuffer::drawBitmapImpl(
    const Bitmap &bitmap,
    const Position pos,
    const BitmapDrawOptions &options,
    const std::size_t animationCycle) noexcept {
    impl::BitmapPainter{*this}.drawBitmap(bitmap, pos, options, animationCycle);
}

void WritableBuffer::drawBitmap(
    const Bitmap &bitmap,
    const Rectangle rect,
    const Alignment alignment,
    const BitmapDrawOptions &options,
    const std::size_t animationCycle) noexcept {
    drawBitmapImpl(bitmap, rect, alignment, options, animationCycle);
}

void WritableBuffer::drawBuffer(const ReadableBuffer &buffer, const Position targetPos) {
    drawBuffer(buffer, BufferDrawOptions{targetPos});
}

void WritableBuffer::drawBuffer(const ReadableBuffer &buffer, const Rectangle targetRect, const Alignment alignment) {
    if (buffer.size().area() == 0 || targetRect.width() <= 0 || targetRect.height() <= 0) {
        return;
    }
    const auto alignedSource = targetRect.alignedSource(buffer.rect(), alignment);
    drawBuffer(buffer, BufferDrawOptions{alignedSource.targetRect, alignedSource.sourceRect});
}

void WritableBuffer::drawBuffer(const ReadableBuffer &buffer, const BufferDrawOptions &options) {
    if (static_cast<const ReadableBuffer *>(this) == &buffer) {
        throw std::invalid_argument{"WritableBuffer::drawBuffer() does not support drawing a buffer onto itself."};
    }
    const auto *source = &buffer;
    auto sourceRect = options.useFullSource() ? source->rect() : options.sourceRect();
    sourceRect &= source->rect();
    if (sourceRect.width() <= 0 || sourceRect.height() <= 0) {
        return;
    }
    auto targetRect = options.isTargetPosition() ? Rectangle{options.targetRect().topLeft(), sourceRect.size()}
                                                 : options.targetRect();
    targetRect.setSize(targetRect.size().componentMin(sourceRect.size()));
    if (targetRect.width() <= 0 || targetRect.height() <= 0) {
        return;
    }
    const auto visibleTargetRect = targetRect & rect();
    if (visibleTargetRect.width() <= 0 || visibleTargetRect.height() <= 0) {
        return;
    }
    const auto clippedOffset = visibleTargetRect.topLeft() - targetRect.topLeft();
    const auto sourceStartPos = sourceRect.topLeft() + clippedOffset;
    const auto &combinationStyle = options.combinationStyle();
    visibleTargetRect.forEach([&](const Position targetPos) -> void {
        auto sourceBlock = source->get(sourceStartPos + targetPos - visibleTargetRect.topLeft());
        if (combinationStyle != nullptr) {
            set(targetPos, sourceBlock, combinationStyle);
            return;
        }
        if (!options.overwriteColors()) {
            sourceBlock = sourceBlock.withBaseColor(get(targetPos).color());
        }
        set(targetPos, sourceBlock);
    });
}

void WritableBuffer::drawBitmapImpl(
    const Bitmap &bitmap,
    const Rectangle rect,
    const Alignment alignment,
    const BitmapDrawOptions &options,
    const std::size_t animationCycle) noexcept {
    impl::BitmapPainter{*this}.drawBitmap(bitmap, rect, alignment, options, animationCycle);
}

}
