// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TextPainter.hpp"

#include "StringWrapper.hpp"

#include "paragraph/Layout.hpp"
#include "paragraph/Painter.hpp"

namespace erbsland::cterm::impl {

void TextPainter::drawText(Position pos, const StringView &str) {
    if (str.empty()) {
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
        if (rect().contains(pos)) {
            set(pos, character.withBase(get(pos).color()));
        }
        pos += Position{character.displayWidth(), 0};
    }
}

void TextPainter::drawText(const Text &text, const std::size_t animationCycle) {
    drawText(text.text(), text.rectangle(), text.textOptions(), animationCycle);
}

auto TextPainter::simpleTextOptions(const Alignment alignment, const Color color) noexcept -> TextOptions {
    auto options = TextOptions{alignment};
    options.setColor(color);
    return options;
}

void TextPainter::drawText(
    const StringView &text, const Rectangle rect, const TextOptions &options, std::size_t animationCycle) {
    const auto textRect = contentRect(rect, options.paragraphOptions());
    if (textRect.width() <= 0 || textRect.height() <= 0) {
        return;
    }
    if (options.font() != nullptr) {
        auto lines = StringLines{};
        for (const auto &paragraph : text.splitLines()) {
            const auto fontLines = buildFontTextLines(options, paragraph);
            lines.insert(lines.end(), fontLines.begin(), fontLines.end());
        }
        applyTextLines(textRect, options, lines, animationCycle);
        return;
    }
    const auto layout =
        paragraph::Layout{
            text, textRect.width(), options.paragraphOptions(), paragraph::LayoutNewlineMode::ParagraphBreak}
            .build();
    if (!layout.valid()) {
        if (options.onError() == ParagraphOnError::Empty) {
            return;
        }
        applyTextLines(
            textRect, options, buildSimpleTextLines(text, textRect, options.paragraphSpacing()), animationCycle);
        return;
    }
    paragraph::Painter{
        _buffer,
        textRect,
        options.alignment(),
        layout,
        text,
        options.paragraphOptions(),
        options.backgroundMode(),
        [&](const Char &character, const Position position) -> Color {
            return colorForTextPosition(options, character, position, animationCycle);
        }}
        .paint();
}

void TextPainter::drawText(
    const std::string_view text,
    const Rectangle rect,
    const Alignment alignment,
    const Color color,
    const std::size_t animationCycle) {
    drawText(String{text, EncodingErrors::Replace}, rect, simpleTextOptions(alignment, color), animationCycle);
}

void TextPainter::drawText(
    const std::u32string_view text,
    const Rectangle rect,
    const Alignment alignment,
    const Color color,
    const std::size_t animationCycle) {
    drawText(String{text}, rect, simpleTextOptions(alignment, color), animationCycle);
}

void TextPainter::drawText(
    const StringView &text,
    const Rectangle rect,
    const Alignment alignment,
    const Color color,
    const std::size_t animationCycle) {
    drawText(text, rect, simpleTextOptions(alignment, color), animationCycle);
}

auto TextPainter::contentRect(const Rectangle rect, const ParagraphOptions &options) noexcept -> Rectangle {
    return rect.insetBy(options.margins());
}

auto TextPainter::buildSimpleTextLines(const StringView &text, const Rectangle rect, ParagraphSpacing spacing) const
    -> StringLines {
    return StringWrapper{text}.wrapIntoLines(rect.width(), spacing);
}

auto TextPainter::buildFontTextLines(const TextOptions &options, const StringView &paragraph) const -> StringLines {
    static constexpr auto pixelMap = std::array<std::string_view, 16>{
        " ", "▘", "▝", "▀", "▖", "▌", "▞", "▛", "▗", "▚", "▐", "▜", "▄", "▙", "▟", "█"};
    if (options.font() == nullptr) {
        return {};
    }
    const auto &font = *options.font();
    auto bitmapWidth = 0;
    auto renderedGlyphs = 0;
    for (const auto &character : paragraph) {
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
    for (const auto &character : paragraph) {
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
    auto lines = StringLines(static_cast<std::size_t>(rowCount));
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

void TextPainter::applyTextLines(
    const Rectangle rect,
    const TextOptions &options,
    const StringLines &lines,
    const std::size_t animationCycle) noexcept {
    const auto maxLines = std::min(static_cast<int>(lines.size()), rect.height());
    auto yStart = rect.topLeft().y();
    switch (options.alignment() & Alignment::VerticalMask) {
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
        switch (options.alignment() & Alignment::HorizontalMask) {
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
                auto finalColor = colorForTextPosition(options, character, pos, animationCycle);
                finalColor = get(pos).color().overlayWith(finalColor);
                set(pos, character.withOverlay(finalColor));
            }
            pos = pos + Position{characterWidth, 0};
        }
    }
}

auto TextPainter::colorForTextPosition(
    const TextOptions &options,
    const Char &character,
    const Position position,
    const std::size_t animationCycle) const noexcept -> Color {

    auto color = Color{};
    if (!options.colorSequence().empty()) {
        auto sequenceIndex = std::size_t{0};
        if (options.animation() == TextAnimation::ColorDiagonal) {
            sequenceIndex = animationCycle + static_cast<std::size_t>(std::max(0, position.x() + position.y()));
        }
        color = options.colorSequence().color(sequenceIndex);
    }
    return color.overlayWith(character.color());
}

}
