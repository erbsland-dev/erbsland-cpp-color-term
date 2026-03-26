// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ParagraphPrinter.hpp"


namespace erbsland::cterm::impl {


ParagraphPrinter::ParagraphPrinter(
    CursorWriter &writer,
    const int width,
    const Alignment alignment,
    const ParagraphLayout::Result &layout,
    const ParagraphBackgroundMode backgroundMode) noexcept :
    ParagraphRendererBase{alignment, layout, backgroundMode},
    _writer{writer},
    _width{width},
    _baseColor{writer.color()} {
}

auto ParagraphPrinter::print() -> int {
    auto previousWrapColor = std::optional<Color>{};
    for (const auto &line : layout().lines) {
        const auto placement = linePlacement(line, 0, _width);
        if (placement.textX > 0) {
            writeSpaces(placement.textX, _baseColor);
        }
        if (line.indentWidth > 0) {
            auto indentColor = _baseColor;
            if (previousWrapColor.has_value() && usesLeftFill()) {
                indentColor = backgroundFillColor(*previousWrapColor);
            }
            writeSpaces(line.indentWidth, indentColor);
        }
        auto lastColor = writeResolved(resolvedWithoutIndent(line));
        if (!line.endMark.empty()) {
            const auto gapWidth = placement.endMarkX - (placement.textX + placement.textWidth);
            if (gapWidth > 0) {
                auto gapColor = _baseColor;
                if (usesRightFill() && lastColor.has_value()) {
                    gapColor = backgroundFillColor(*lastColor);
                }
                writeSpaces(gapWidth, gapColor);
            }
            if (const auto markColor = writeResolved(line.endMark); markColor.has_value()) {
                lastColor = markColor;
            }
        } else if (usesRightFillForLine(line) && lastColor.has_value()) {
            const auto fillWidth = _width - (placement.textX + placement.textWidth);
            writeSpaces(fillWidth, backgroundFillColor(*lastColor));
        }
        _writer.setBackground(Background::Default);
        _writer.writeLineBreak();
        if (line.wrapsToNext && usesLeftFill()) {
            previousWrapColor = lastColor;
        } else {
            previousWrapColor.reset();
        }
    }
    _writer.setColor(_baseColor);
    return static_cast<int>(layout().lines.size());
}

auto ParagraphPrinter::backgroundFillColor(const Color color) const noexcept -> Color {
    return _baseColor.overlayWith(Color{Foreground::Inherited, color.bg()});
}

auto ParagraphPrinter::resolvedWithoutIndent(const ParagraphLayout::Line &line) const noexcept -> String {
    if (line.indentWidth <= 0) {
        return line.text;
    }
    return line.text.substr(static_cast<std::size_t>(line.indentWidth));
}

auto ParagraphPrinter::writeResolved(const String &text) -> std::optional<Color> {
    if (text.empty()) {
        return std::nullopt;
    }
    auto resolvedText = String{};
    resolvedText.reserve(text.size());
    auto lastColor = std::optional<Color>{};
    for (const auto &character : text) {
        const auto resolvedCharacter = character.withBaseColor(_baseColor);
        resolvedText.append(resolvedCharacter);
        if (resolvedCharacter.displayWidth() > 0) {
            lastColor = resolvedCharacter.color();
        }
    }
    _writer.write(resolvedText);
    return lastColor;
}

void ParagraphPrinter::writeSpaces(const int count, const Color color) {
    if (count <= 0) {
        return;
    }
    _writer.write(String{static_cast<std::size_t>(count), Char{U' ', color}});
}


}
