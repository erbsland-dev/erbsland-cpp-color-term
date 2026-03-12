// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "FramePainter.hpp"


namespace erbsland::cterm::impl {

void FramePainter::drawFrame(
    const Rectangle rect, const Char &frameBlock, const CharCombinationStylePtr &combinationStyle) noexcept {

    rect.forEachInFrame([&, this](const Position pos) -> void { set(pos, frameBlock, combinationStyle); });
}

void FramePainter::drawFrame(
    Rectangle rect,
    const Char16StylePtr &frameStyle,
    const CharCombinationStylePtr &combinationStyle,
    const Color frameColor) noexcept {

    rect.forEachInFrame([&, this](const Position pos) -> void {
        set(pos, blockForFrame(rect, pos, frameStyle).withBaseColor(frameColor), combinationStyle);
    });
}

void FramePainter::drawFrame(
    const Rectangle rect,
    const Tile9StylePtr &style,
    const Color frameColor,
    const CharCombinationStylePtr &combinationStyle) noexcept {

    if (style == nullptr) {
        return;
    }
    rect.forEachInFrame([&, this](const Position pos) -> void {
        set(pos, style->block(rect, pos).withBaseColor(frameColor), combinationStyle);
    });
}

void FramePainter::drawFrame(const Rectangle rect, const FrameStyle frameStyle, const Color frameColor) noexcept {
    if (const auto tile9Style = Tile9Style::forStyle(frameStyle); tile9Style != nullptr) {
        drawFrame(rect, tile9Style, frameColor, CharCombinationStyle::commonBoxFrame());
        return;
    }
    if (const auto style = Char16Style::forStyle(frameStyle); style != nullptr) {
        drawFrame(rect, style, CharCombinationStyle::commonBoxFrame(), frameColor);
    }
}

void FramePainter::drawFrame(
    const Rectangle rect, const FrameDrawOptions &options, const std::size_t animationCycle) noexcept {
    if (rect.width() <= 0 || rect.height() <= 0) {
        return;
    }
    const auto tile9Style =
        options.tile9Style() != nullptr ? options.tile9Style() : Tile9Style::forStyle(options.style());
    const auto char16Style = tile9Style == nullptr
        ? (options.char16Style() != nullptr ? options.char16Style() : Char16Style::forStyle(options.style()))
        : Char16StylePtr{};
    if (tile9Style == nullptr && char16Style == nullptr) {
        return;
    }
    rect.forEach([&, this](const Position pos) -> void {
        const auto frameBaseColor = colorForFramePosition(
            options.frameColor(), options.frameColorMode(), rect, pos, animationCycle, options.animationOffset());
        if (rect.isFrame(pos)) {
            if (tile9Style != nullptr) {
                drawFrameBlock(pos, tile9Style->block(rect, pos), frameBaseColor, options.combinationStyle());
            } else {
                drawFrameBlock(pos, blockForFrame(rect, pos, char16Style), frameBaseColor, options.combinationStyle());
            }
            return;
        }
        if (tile9Style != nullptr) {
            const auto fillBaseColor = frameBaseColor.overlayWith(colorForFramePosition(
                options.fillColor(), options.fillColorMode(), rect, pos, animationCycle, options.animationOffset()));
            drawFrameBlock(pos, tile9Style->block(rect, pos), fillBaseColor, options.combinationStyle());
            return;
        }
        if (options.fillBlock().displayWidth() <= 0) {
            return;
        }
        const auto fillBaseColor = frameBaseColor.overlayWith(colorForFramePosition(
            options.fillColor(), options.fillColorMode(), rect, pos, animationCycle, options.animationOffset()));
        drawFrameBlock(pos, options.fillBlock(), fillBaseColor, options.combinationStyle());
    });
}

void FramePainter::drawFilledFrame(
    Rectangle rect,
    const Char &frameBlock,
    const Char &fillBlock,
    const CharCombinationStylePtr &combinationStyle) noexcept {

    rect.forEach([&, this](const Position pos) -> void {
        if (rect.isFrame(pos)) {
            set(pos, frameBlock, combinationStyle);
        } else {
            set(pos, fillBlock, combinationStyle);
        }
    });
}

void FramePainter::drawFilledFrame(
    Rectangle rect,
    const Char16StylePtr &frameStyle,
    const Char &fillBlock,
    const CharCombinationStylePtr &combinationStyle,
    const Color frameColor) noexcept {

    rect.forEach([&, this](const Position pos) -> void {
        if (rect.isFrame(pos)) {
            set(pos, blockForFrame(rect, pos, frameStyle).withBaseColor(frameColor), combinationStyle);
        } else {
            set(pos, fillBlock, combinationStyle);
        }
    });
}

void FramePainter::drawFilledFrame(
    Rectangle rect,
    const Tile9StylePtr &style,
    const Char &fillBlock,
    const CharCombinationStylePtr &combinationStyle,
    const Color frameColor) noexcept {

    if (style == nullptr) {
        fill(rect, fillBlock, combinationStyle);
        return;
    }
    rect.forEach([&, this](const Position pos) -> void {
        if (rect.isFrame(pos)) {
            set(pos, style->block(rect, pos).withBaseColor(frameColor), combinationStyle);
        } else {
            set(pos, fillBlock, combinationStyle);
        }
    });
}

void FramePainter::drawFilledFrame(
    const Rectangle rect, const FrameStyle frameStyle, const Char &fillBlock, const Color frameColor) noexcept {
    if (const auto tile9Style = Tile9Style::forStyle(frameStyle); tile9Style != nullptr) {
        drawFilledFrame(rect, tile9Style, fillBlock, CharCombinationStyle::commonBoxFrame(), frameColor);
        return;
    }
    if (const auto style = Char16Style::forStyle(frameStyle); style != nullptr) {
        drawFilledFrame(rect, style, fillBlock, CharCombinationStyle::commonBoxFrame(), frameColor);
    }
}

auto FramePainter::blockForFrame(const Rectangle rect, const Position pos, const Char16StylePtr &frameStyle) -> Char {

    const auto bitMask =
        pos.cardinalFourBitmask([&](const Position deltaPos) -> bool { return rect.isFrame(deltaPos); });
    return frameStyle->block(bitMask);
}

void FramePainter::drawFrameBlock(
    const Position pos,
    const Char &block,
    const Color baseColor,
    const CharCombinationStylePtr &combinationStyle) noexcept {

    if (!rect().contains(pos)) {
        return;
    }
    const auto finalColor = get(pos).color().overlayWith(baseColor).overlayWith(block.color());
    set(pos, block.withColorReplaced(finalColor), combinationStyle);
}

auto FramePainter::colorForFramePosition(
    const ColorSequence &colorSequence,
    const FrameColorMode colorMode,
    const Rectangle rect,
    const Position pos,
    const std::size_t animationCycle,
    const std::size_t animationOffset) noexcept -> Color {

    if (colorSequence.empty()) {
        return {};
    }
    auto sequenceIndex = static_cast<int64_t>(animationCycle + animationOffset);
    const auto relativePos = pos - rect.topLeft();
    switch (colorMode) {
    case FrameColorMode::VerticalStripes:
        sequenceIndex += relativePos.x();
        break;
    case FrameColorMode::HorizontalStripes:
        sequenceIndex += relativePos.y();
        break;
    case FrameColorMode::ForwardDiagonalStripes:
        sequenceIndex += relativePos.x() + relativePos.y();
        break;
    case FrameColorMode::BackwardDiagonalStripes:
        sequenceIndex += relativePos.y() - relativePos.x();
        break;
    case FrameColorMode::ChasingBorderCW:
        if (!rect.isFrame(pos)) {
            break;
        }
        sequenceIndex = rect.frameIndex(pos) - sequenceIndex;
        break;
    case FrameColorMode::ChasingBorderCCW:
        if (!rect.isFrame(pos)) {
            break;
        }
        sequenceIndex += rect.frameIndex(pos);
        break;
    case FrameColorMode::OneColor:
    default:
        break;
    }
    const auto sequenceLength = static_cast<int64_t>(colorSequence.sequenceLength());
    const auto wrappedSequenceIndex = ((sequenceIndex % sequenceLength) + sequenceLength) % sequenceLength;
    return colorSequence.color(static_cast<std::size_t>(wrappedSequenceIndex));
}


}
