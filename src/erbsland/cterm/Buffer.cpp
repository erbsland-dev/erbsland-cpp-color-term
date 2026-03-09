// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "Buffer.hpp"


#include "Char16Style.hpp"
#include "Tile9Style.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <string_view>
#include <utility>


namespace erbsland::cterm {


Buffer::Buffer(const Size size) noexcept : _size{size}, _data(static_cast<std::size_t>(size.area()), Char{}) {
}

auto Buffer::size() const noexcept -> Size {
    return _size;
}

auto Buffer::rect() const noexcept -> Rectangle {
    return Rectangle{Position{0, 0}, _size};
}

void Buffer::fill(const Char &fillBlock) noexcept {
    for (auto &dataBlock : _data) {
        dataBlock = fillBlock;
    }
}

void Buffer::fill(
    const Rectangle rect, const Char &fillBlock, const CharCombinationStylePtr &combinationStyle) noexcept {

    rect.forEach([&, this](const Position pos) -> void { set(pos, fillBlock, combinationStyle); });
}

void Buffer::fill(
    const Rectangle rect,
    const Tile9StylePtr &style,
    const Color baseColor,
    const CharCombinationStylePtr &combinationStyle) noexcept {

    if (style == nullptr) {
        return;
    }
    rect.forEach([&, this](const Position pos) -> void {
        set(pos, applyBaseColor(style->block(rect, pos), baseColor), combinationStyle);
    });
}

void Buffer::drawFrame(
    const Rectangle rect, const Char &frameBlock, const CharCombinationStylePtr &combinationStyle) noexcept {

    rect.forEachInFrame([&, this](const Position pos) -> void { set(pos, frameBlock, combinationStyle); });
}

void Buffer::drawFrame(
    Rectangle rect,
    const Char16StylePtr &frameStyle,
    const CharCombinationStylePtr &combinationStyle,
    const Color frameColor) noexcept {

    rect.forEachInFrame([&, this](const Position pos) -> void {
        set(pos, blockForFrame(rect, pos, frameStyle, frameColor), combinationStyle);
    });
}

void Buffer::drawFrame(
    const Rectangle rect,
    const Tile9StylePtr &style,
    const Color frameColor,
    const CharCombinationStylePtr &combinationStyle) noexcept {

    if (style == nullptr) {
        return;
    }
    rect.forEachInFrame([&, this](const Position pos) -> void {
        set(pos, applyBaseColor(style->block(rect, pos), frameColor), combinationStyle);
    });
}

void Buffer::drawFrame(const Rectangle rect, const FrameStyle frameStyle, const Color frameColor) noexcept {
    if (const auto tile9Style = Tile9Style::forStyle(frameStyle); tile9Style != nullptr) {
        drawFrame(rect, tile9Style, frameColor, CharCombinationStyle::commonBoxFrame());
        return;
    }
    if (const auto style = Char16Style::forStyle(frameStyle); style != nullptr) {
        drawFrame(rect, style, CharCombinationStyle::commonBoxFrame(), frameColor);
    }
}

void Buffer::drawFilledFrame(
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
void Buffer::drawFilledFrame(
    Rectangle rect,
    const Char16StylePtr &frameStyle,
    const Char &fillBlock,
    const CharCombinationStylePtr &combinationStyle,
    const Color frameColor) noexcept {

    rect.forEach([&, this](const Position pos) -> void {
        if (rect.isFrame(pos)) {
            set(pos, blockForFrame(rect, pos, frameStyle, frameColor), combinationStyle);
        } else {
            set(pos, fillBlock, combinationStyle);
        }
    });
}

void Buffer::drawFilledFrame(
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
            set(pos, applyBaseColor(style->block(rect, pos), frameColor), combinationStyle);
        } else {
            set(pos, fillBlock, combinationStyle);
        }
    });
}

void Buffer::drawFilledFrame(
    const Rectangle rect, const FrameStyle frameStyle, const Char &fillBlock, const Color frameColor) noexcept {
    if (const auto tile9Style = Tile9Style::forStyle(frameStyle); tile9Style != nullptr) {
        drawFilledFrame(rect, tile9Style, fillBlock, CharCombinationStyle::commonBoxFrame(), frameColor);
        return;
    }
    if (const auto style = Char16Style::forStyle(frameStyle); style != nullptr) {
        drawFilledFrame(rect, style, fillBlock, CharCombinationStyle::commonBoxFrame(), frameColor);
    }
}

void Buffer::set(const Position pos, const Char &block) noexcept {
    if (!_size.contains(pos) || block.displayWidth() == 0 || block.displayWidth() > 2) {
        return;
    }
    if (block.displayWidth() == 1) {
        _data[static_cast<std::size_t>(_size.index(pos))] = block;
    } else {
        const auto secondPosition = pos + Position{1, 0};
        if (!_size.contains(secondPosition)) {
            return;
        }
        _data[static_cast<std::size_t>(_size.index(secondPosition))] = Char{"", block.color()};
        _data[static_cast<std::size_t>(_size.index(pos))] = block;
    }
}


void Buffer::set(const Position pos, Char &&block) noexcept {
    if (!_size.contains(pos) || block.displayWidth() == 0 || block.displayWidth() > 2) {
        return;
    }
    if (block.displayWidth() == 1) {
        _data[static_cast<std::size_t>(_size.index(pos))] = std::move(block);
    } else {
        const auto secondPosition = pos + Position{1, 0};
        if (!_size.contains(secondPosition)) {
            return;
        }
        _data[static_cast<std::size_t>(_size.index(secondPosition))] = Char{"", block.color()};
        _data[static_cast<std::size_t>(_size.index(pos))] = std::move(block);
    }
}

void Buffer::set(const Position pos, const Char &block, const CharCombinationStylePtr &combinationStyle) noexcept {
    if (!_size.contains(pos)) {
        return;
    }
    if (combinationStyle == nullptr) {
        set(pos, block);
        return;
    }
    if (combinationStyle->isSurroundingAware()) {
        std::array<Char *, 9> surroundingBlocks{};
        for (std::size_t i = 0; i < 9; ++i) {
            const auto surroundPosition = pos + Position{static_cast<int>(i) % 3 - 1, static_cast<int>(i) / 3 - 1};
            if (_size.contains(surroundPosition)) {
                surroundingBlocks[i] = &_data[static_cast<std::size_t>(_size.index(surroundPosition))];
            }
        }
        set(pos, combinationStyle->combine(surroundingBlocks, block));
    } else {
        set(pos, combinationStyle->combine(_data[static_cast<std::size_t>(_size.index(pos))], block));
    }
}

auto Buffer::get(const Position pos) const noexcept -> const Char & {
    assert(_size.contains(pos));
    return _data[static_cast<std::size_t>(_size.index(pos))];
}


auto Buffer::applyBaseColor(const Char &block, const Color baseColor) -> Char {
    return block.withBaseColor(baseColor);
}


auto Buffer::blockForFrame(
    const Rectangle rect, const Position pos, const Char16StylePtr &frameStyle, const Color frameColor) -> Char {

    const auto bitMask =
        pos.cardinalFourBitmask([&](const Position deltaPos) -> bool { return rect.isFrame(deltaPos); });
    return applyBaseColor(frameStyle->block(bitMask), frameColor);
}


void Buffer::drawText(const Text &text, const std::size_t animationCycle) {
    applyTextLines(text, buildTextLines(text), animationCycle);
}


void Buffer::drawText(
    const std::string_view text,
    const Alignment alignment,
    const Rectangle rect,
    const Color color,
    const std::size_t animationCycle) {

    auto renderedText = Text{String{text}, rect, alignment};
    renderedText.setColor(color);
    drawText(renderedText, animationCycle);
}


void Buffer::drawText(
    const std::string_view text,
    const Rectangle rect,
    const Alignment alignment,
    const Color color,
    const std::size_t animationCycle) {

    drawText(text, alignment, rect, color, animationCycle);
}


void Buffer::drawBitmap(
    const Bitmap &bitmap,
    const Position pos,
    const BitmapDrawOptions &options,
    const std::size_t animationCycle) noexcept {

    drawBitmap(bitmap, Rectangle{pos, bitmapRenderSize(bitmap, options)}, Alignment::TopLeft, options, animationCycle);
}


void Buffer::drawBitmap(
    const Bitmap &bitmap,
    const Rectangle rect,
    const Alignment alignment,
    const BitmapDrawOptions &options,
    const std::size_t animationCycle) noexcept {

    if (bitmap.size().area() == 0 || rect.width() <= 0 || rect.height() <= 0) {
        return;
    }
    const auto renderedSize = bitmapRenderSize(bitmap, options);
    if (renderedSize.area() == 0) {
        return;
    }
    auto visibleSize = renderedSize.componentMin(rect.size());
    auto sourceOffset = Position{};
    auto targetPos = rect.topLeft();
    if (renderedSize.width() <= rect.width()) {
        targetPos += Position{
            alignedBitmapOffset(renderedSize.width(), rect.width(), alignment, Alignment::HorizontalMask),
            0};
    } else {
        sourceOffset += Position{
            alignedBitmapOffset(renderedSize.width(), rect.width(), alignment, Alignment::HorizontalMask),
            0};
    }
    if (renderedSize.height() <= rect.height()) {
        targetPos += Position{
            0,
            alignedBitmapOffset(renderedSize.height(), rect.height(), alignment, Alignment::VerticalMask)};
    } else {
        sourceOffset += Position{
            0,
            alignedBitmapOffset(renderedSize.height(), rect.height(), alignment, Alignment::VerticalMask)};
    }
    if (options.char16Style() != nullptr) {
        for (auto y = 0; y < visibleSize.height(); ++y) {
            for (auto x = 0; x < visibleSize.width(); ++x) {
                const auto bitmapPos = sourceOffset + Position{x, y};
                if (!bitmap.pixel(bitmapPos)) {
                    continue;
                }
                const auto bitMask = bitmapPos.cardinalFourBitmask(
                    [&](const Position neighborPos) noexcept -> bool { return bitmap.pixel(neighborPos); });
                drawBitmapBlock(
                    targetPos + Position{x, y},
                    options.char16Style()->block(bitMask),
                    colorForBitmapPosition(options, bitmapPos, animationCycle),
                    options);
            }
        }
        return;
    }
    switch (options.scaleMode()) {
    case BitmapScaleMode::HalfBlock:
        for (auto y = 0; y < visibleSize.height(); ++y) {
            for (auto x = 0; x < visibleSize.width(); ++x) {
                const auto bitmapCellPos = sourceOffset + Position{x, y};
                const auto bitMask = bitmap.pixelQuad(bitmapCellPos);
                drawBitmapBlock(
                    targetPos + Position{x, y},
                    options.halfBlocks()[bitMask],
                    colorForBitmapPosition(options, bitmapCellPos, animationCycle),
                    options);
            }
        }
        break;
    case BitmapScaleMode::DoubleBlock:
        for (auto y = 0; y < visibleSize.height(); ++y) {
            for (auto x = 0; x < visibleSize.width(); ++x) {
                const auto renderedBitmapPos = sourceOffset + Position{x, y};
                const auto bitmapPos = Position{renderedBitmapPos.x() / 2, renderedBitmapPos.y()};
                if (!bitmap.pixel(bitmapPos)) {
                    continue;
                }
                drawBitmapBlock(
                    targetPos + Position{x, y},
                    options.doubleBlocks()[static_cast<std::size_t>(renderedBitmapPos.x() % 2)],
                    colorForBitmapPosition(options, bitmapPos, animationCycle),
                    options);
            }
        }
        break;
    case BitmapScaleMode::FullBlock:
    default:
        for (auto y = 0; y < visibleSize.height(); ++y) {
            for (auto x = 0; x < visibleSize.width(); ++x) {
                const auto bitmapPos = sourceOffset + Position{x, y};
                if (!bitmap.pixel(bitmapPos)) {
                    continue;
                }
                drawBitmapBlock(
                    targetPos + Position{x, y},
                    options.fullBlock(),
                    colorForBitmapPosition(options, bitmapPos, animationCycle),
                    options);
            }
        }
        break;
    }
}


auto Buffer::bitmapRenderSize(const Bitmap &bitmap, const BitmapDrawOptions &options) noexcept -> Size {
    if (options.char16Style() != nullptr) {
        return bitmap.size();
    }
    switch (options.scaleMode()) {
    case BitmapScaleMode::HalfBlock:
        return {(bitmap.size().width() + 1) / 2, (bitmap.size().height() + 1) / 2};
    case BitmapScaleMode::DoubleBlock:
        return {bitmap.size().width() * 2, bitmap.size().height()};
    case BitmapScaleMode::FullBlock:
    default:
        return bitmap.size();
    }
}


auto Buffer::alignedBitmapOffset(
    const int renderedSize, const int availableSize, const Alignment alignment, const Alignment alignmentMask) noexcept
    -> int {

    const auto freeSpace = renderedSize <= availableSize ? (availableSize - renderedSize) : (renderedSize - availableSize);
    if (alignmentMask == Alignment::HorizontalMask) {
        switch (alignment & alignmentMask) {
        case Alignment::HCenter:
            return freeSpace / 2;
        case Alignment::Right:
            return freeSpace;
        case Alignment::Left:
        default:
            return 0;
        }
    }
    switch (alignment & alignmentMask) {
    case Alignment::VCenter:
        return freeSpace / 2;
    case Alignment::Bottom:
        return freeSpace;
    case Alignment::Top:
    default:
        return 0;
    }
}


auto Buffer::colorForBitmapPosition(
    const BitmapDrawOptions &options, const Position bitmapPosition, const std::size_t animationCycle) const noexcept
    -> Color {

    const auto &colorSequence = options.color();
    if (colorSequence.empty()) {
        return {};
    }
    auto sequenceIndex = static_cast<int64_t>(animationCycle + options.colorAnimationOffset());
    switch (options.colorMode()) {
    case BitmapColorMode::VerticalStripes:
        sequenceIndex += bitmapPosition.x();
        break;
    case BitmapColorMode::HorizontalStripes:
        sequenceIndex += bitmapPosition.y();
        break;
    case BitmapColorMode::ForwardDiagonalStripes:
        sequenceIndex += bitmapPosition.x() + bitmapPosition.y();
        break;
    case BitmapColorMode::BackwardDiagonalStripes:
        sequenceIndex += bitmapPosition.y() - bitmapPosition.x();
        break;
    case BitmapColorMode::OneColor:
    default:
        break;
    }
    const auto sequenceLength = static_cast<int64_t>(colorSequence.sequenceLength());
    const auto wrappedSequenceIndex = ((sequenceIndex % sequenceLength) + sequenceLength) % sequenceLength;
    return colorSequence.color(static_cast<std::size_t>(wrappedSequenceIndex));
}


void Buffer::drawBitmapBlock(
    const Position pos, const Char &block, const Color baseColor, const BitmapDrawOptions &options) noexcept {

    if (!rect().contains(pos)) {
        return;
    }
    const auto finalColor = get(pos).color().overlayWith(baseColor).overlayWith(block.color());
    set(pos, block.withColorReplaced(finalColor), options.combinationStyle());
}


auto Buffer::buildTextLines(const Text &text) const -> BlockStringLines {
    if (text.font() == nullptr) {
        return text.text().wrapIntoLines(text.rectangle().width());
    }
    return buildFontTextLines(text);
}


auto Buffer::buildFontTextLines(const Text &text) const -> BlockStringLines {
    static constexpr auto pixelMap = std::array<std::string_view, 16>{
        " ", "▘", "▝", "▀", "▖", "▌", "▞", "▛", "▗", "▚", "▐", "▜", "▄", "▙", "▟", "█"};
    if (text.font() == nullptr) {
        return {};
    }
    const auto &font = *text.font();
    auto bitmapWidth = 0;
    auto renderedGlyphs = 0;
    for (const auto &character : text.text()) {
        if (const auto *glyph = font.glyph(character.charStr()); glyph != nullptr) {
            bitmapWidth += glyph->size().width();
            ++renderedGlyphs;
        }
    }
    if (renderedGlyphs == 0) {
        return {};
    }
    bitmapWidth += renderedGlyphs - 1;
    auto bitmap = Bitmap{Size{bitmapWidth, font.height()}};
    auto columnColors = std::vector<Color>(static_cast<std::size_t>((bitmapWidth + 1) / 2 + 1));
    auto insertX = 0;
    auto isFirstGlyph = true;
    for (const auto &character : text.text()) {
        const auto *glyph = font.glyph(character.charStr());
        if (glyph == nullptr) {
            continue;
        }
        if (!isFirstGlyph) {
            ++insertX;
        }
        bitmap.draw(Position{insertX, 0}, *glyph);
        const auto startColumn = insertX / 2;
        const auto columnCount = glyph->size().width() / 2 + 1;
        for (auto columnIndex = 0; columnIndex < columnCount; ++columnIndex) {
            columnColors[static_cast<std::size_t>(startColumn + columnIndex)] = character.color();
        }
        insertX += glyph->size().width();
        isFirstGlyph = false;
    }
    const auto rowCount = std::max(1, (font.height() + 1) / 2);
    const auto columns = std::max(1, (bitmapWidth + 1) / 2);
    auto lines = BlockStringLines(static_cast<std::size_t>(rowCount));
    for (auto y = 0; y < rowCount; ++y) {
        auto &line = lines[static_cast<std::size_t>(y)];
        line.reserve(static_cast<std::size_t>(columns));
        for (auto x = 0; x < columns; ++x) {
            const auto color = columnColors[static_cast<std::size_t>(x)];
            line.append(Char{pixelMap[bitmap.pixelQuad(Position{x, y})], color});
        }
    }
    return lines;
}


void Buffer::applyTextLines(
    const Text &text, const BlockStringLines &lines, const std::size_t animationCycle) noexcept {
    const auto rect = text.rectangle();
    const auto maxLines = std::min(static_cast<int>(lines.size()), rect.height());
    auto yStart = rect.topLeft().y();
    switch (text.alignment() & Alignment::VerticalMask) {
    case Alignment::Top:
        break;
    case Alignment::VCenter:
        yStart += (rect.height() - maxLines) / 2;
        break;
    case Alignment::Bottom:
        yStart = rect.y2() - maxLines;
        break;
    default:
        break;
    }
    for (auto lineIndex = 0; lineIndex < maxLines; ++lineIndex) {
        const auto &line = lines[static_cast<std::size_t>(lineIndex)];
        const auto lineWidth = std::min(line.displayWidth(), rect.width());
        auto xStart = rect.topLeft().x();
        switch (text.alignment() & Alignment::HorizontalMask) {
        case Alignment::Left:
            break;
        case Alignment::Right:
            xStart = rect.x2() - lineWidth;
            break;
        case Alignment::HCenter:
            xStart += (rect.width() - lineWidth) / 2;
            break;
        default:
            break;
        }
        auto pos = Position{xStart, yStart + lineIndex};
        for (const auto &character : line) {
            const auto characterWidth = character.displayWidth();
            if (characterWidth <= 0) {
                continue;
            }
            if (pos.x() + characterWidth > rect.x2()) {
                break;
            }
            // only do this calculation if we actually change the buffer.
            if (this->rect().contains(pos)) {
                auto finalColor = colorForTextPosition(text, character, pos, animationCycle);
                finalColor = get(pos).color().overlayWith(finalColor);
                set(pos, character.withColorOverlay(finalColor));
            }
            pos = pos + Position{characterWidth, 0};
        }
    }
}


auto Buffer::colorForTextPosition(
    const Text &text, const Char &character, const Position position, const std::size_t animationCycle) const noexcept
    -> Color {

    auto color = Color{};
    if (!text.colorSequence().empty()) {
        auto sequenceIndex = std::size_t{0};
        if (text.animation() == TextAnimation::ColorDiagonal) {
            sequenceIndex = animationCycle + static_cast<std::size_t>(std::max(0, position.x() + position.y()));
        }
        color = text.colorSequence().color(sequenceIndex);
    }
    return color.overlayWith(character.color());
}


}
