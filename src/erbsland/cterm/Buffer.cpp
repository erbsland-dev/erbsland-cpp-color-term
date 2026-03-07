// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "Buffer.hpp"

#include "Char16Style.hpp"

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

void Buffer::fill(const Char &fillBlock) noexcept {
    for (auto &dataBlock : _data) {
        dataBlock = fillBlock;
    }
}

void Buffer::fill(
    const Rectangle rect, const Char &fillBlock, const CharCombinationStylePtr &combinationStyle) noexcept {

    rect.forEach([&, this](const Position pos) -> void { set(pos, fillBlock, combinationStyle); });
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

    rect.forEachInFrame(
        [&, this](const Position pos) -> void { set(pos, blockForFrame(rect, pos, frameStyle, frameColor), combinationStyle); });
}

void Buffer::drawFrame(const Rectangle rect, const FrameStyle frameStyle, const Color frameColor) noexcept {
    const auto style = Char16Style::forStyle(frameStyle);
    drawFrame(rect, style, CharCombinationStyle::commonBoxFrame(), frameColor);
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
    const Rectangle rect, const FrameStyle frameStyle, const Char &fillBlock, const Color frameColor) noexcept {
    const auto style = Char16Style::forStyle(frameStyle);
    drawFilledFrame(rect, style, fillBlock, CharCombinationStyle::commonBoxFrame(), frameColor);
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


auto Buffer::applyFrameColor(const Char &frameBlock, const Color frameColor) -> Char {
    return Char{frameBlock.charStr(), frameColor.overlayWith(frameBlock.color())};
}


auto Buffer::blockForFrame(
    const Rectangle rect, const Position pos, const Char16StylePtr &frameStyle, const Color frameColor) -> Char {

    if (frameStyle == Char16Style::outerHalfBlockFrame()) {
        return outerHalfBlockForFrame(rect, pos, frameColor);
    }
    if (frameStyle == Char16Style::innerHalfBlockFrame()) {
        return innerHalfBlockForFrame(rect, pos, frameColor);
    }
    const auto bitMask = pos.cardinalFourBitmask([&](const Position deltaPos) -> bool { return rect.isFrame(deltaPos); });
    return applyFrameColor(frameStyle->block(bitMask), frameColor);
}


auto Buffer::outerHalfBlockForFrame(const Rectangle rect, const Position pos, const Color frameColor) -> Char {
    const auto isTop = pos.y() == rect.y1();
    const auto isBottom = pos.y() == rect.y2() - 1;
    const auto isLeft = pos.x() == rect.x1();
    const auto isRight = pos.x() == rect.x2() - 1;
    if (isTop && isLeft) {
        return Char{"▛", frameColor};
    }
    if (isTop && isRight) {
        return Char{"▜", frameColor};
    }
    if (isBottom && isLeft) {
        return Char{"▙", frameColor};
    }
    if (isBottom && isRight) {
        return Char{"▟", frameColor};
    }
    if (isTop) {
        return Char{"▀", frameColor};
    }
    if (isBottom) {
        return Char{"▄", frameColor};
    }
    if (isLeft) {
        return Char{"▌", frameColor};
    }
    return Char{"▐", frameColor};
}


auto Buffer::innerHalfBlockForFrame(const Rectangle rect, const Position pos, const Color frameColor) -> Char {
    const auto isTop = pos.y() == rect.y1();
    const auto isBottom = pos.y() == rect.y2() - 1;
    const auto isLeft = pos.x() == rect.x1();
    const auto isRight = pos.x() == rect.x2() - 1;
    if (isTop && isLeft) {
        return Char{"▗", frameColor};
    }
    if (isTop && isRight) {
        return Char{"▖", frameColor};
    }
    if (isBottom && isLeft) {
        return Char{"▝", frameColor};
    }
    if (isBottom && isRight) {
        return Char{"▘", frameColor};
    }
    if (isTop) {
        return Char{"▄", frameColor};
    }
    if (isBottom) {
        return Char{"▀", frameColor};
    }
    if (isLeft) {
        return Char{"▐", frameColor};
    }
    return Char{"▌", frameColor};
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
            const auto finalColor = colorForTextPosition(text, character, pos, animationCycle);
            set(pos, character.withColor(finalColor));
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
