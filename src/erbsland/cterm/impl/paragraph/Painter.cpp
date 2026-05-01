// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Painter.hpp"

#include <optional>

namespace erbsland::cterm::impl::paragraph {

void Painter::paint() {
    const auto maxLines = std::min(static_cast<int>(layout().size()), _rect.height());
    const auto leftFillEnabled = usesLeftFill();
    const auto rightFillEnabled = usesRightFill();
    const auto textAlignment = alignment();
    auto yStart = _rect.y1();
    if (textAlignment.isVerticalCenter()) {
        yStart += (_rect.height() - maxLines) / 2;
    } else if (textAlignment.isBottom()) {
        yStart = _rect.y2() - maxLines;
    }
    auto previousWrapColor = std::optional<Color>{};
    for (auto lineIndex = 0; lineIndex < maxLines; ++lineIndex) {
        const auto &line = layout().lines()[static_cast<std::size_t>(lineIndex)];
        const auto y = yStart + lineIndex;
        const auto placement = linePlacement(line, _rect.x1(), _rect.width());
        auto lastColor = drawLine(line, Position{placement.textX + line.indentWidth, y});
        auto rightFillColor = std::optional<Color>{};
        auto rightFillStart = 0;
        auto rightFillEnd = 0;
        if (line.wrapsToNext) {
            auto endMarkPosition = Position{placement.endMarkX, y};
            for (const auto &character : options().lineBreakEndMark()) {
                if (const auto markColor = drawCharacter(character, endMarkPosition); markColor.has_value()) {
                    lastColor = markColor;
                }
            }
            if (rightFillEnabled && lastColor.has_value()) {
                rightFillColor = lastColor;
                rightFillStart = placement.textX + placement.textWidth;
                rightFillEnd = placement.endMarkX;
            }
        } else if (lastColor.has_value() && usesRightFillForLine(line)) {
            rightFillColor = lastColor;
            rightFillStart = placement.textX + placement.textWidth;
            rightFillEnd = _rect.x2();
        }
        if (previousWrapColor.has_value() && leftFillEnabled && line.indentWidth > 0) {
            fillBackgroundRange(y, _rect.x1(), _rect.x1() + line.indentWidth, *previousWrapColor);
        }
        if (rightFillColor.has_value()) {
            fillBackgroundRange(y, rightFillStart, rightFillEnd, *rightFillColor);
        }
        if (line.wrapsToNext && leftFillEnabled) {
            previousWrapColor = lastColor;
        } else {
            previousWrapColor.reset();
        }
    }
}

auto Painter::drawLine(const LayoutLine &line, Position pos) -> std::optional<Color> {
    auto lastColor = std::optional<Color>{};
    for (const auto &fragment : line.fragments) {
        if (const auto fragmentColor = drawFragment(fragment, pos); fragmentColor.has_value()) {
            lastColor = fragmentColor;
        }
    }
    return lastColor;
}

auto Painter::drawFragment(const LayoutFragment &fragment, Position &pos) -> std::optional<Color> {
    switch (fragment.type()) {
    case LayoutFragment::Type::SourceRange: {
        auto lastColor = std::optional<Color>{};
        const auto &text = sourceText();
        for (auto index = fragment.startIndex(); index < fragment.startIndex() + fragment.length(); ++index) {
            if (const auto characterColor = drawCharacter(text[index], pos); characterColor.has_value()) {
                lastColor = characterColor;
            }
        }
        return lastColor;
    }
    case LayoutFragment::Type::Spaces: {
        auto lastColor = std::optional<Color>{};
        for (auto index = 0; index < fragment.width(); ++index) {
            if (const auto characterColor = drawCharacter(Char{U' ', fragment.color()}, pos);
                characterColor.has_value()) {
                lastColor = characterColor;
            }
        }
        return lastColor;
    }
    case LayoutFragment::Type::LineBreakStartMark: {
        auto lastColor = std::optional<Color>{};
        for (const auto &character : options().lineBreakStartMark()) {
            if (const auto characterColor = drawCharacter(character, pos); characterColor.has_value()) {
                lastColor = characterColor;
            }
        }
        return lastColor;
    }
    case LayoutFragment::Type::WordBreakMark:
        return drawCharacter(options().wordBreakMark(), pos);
    case LayoutFragment::Type::ParagraphEllipsis: {
        auto lastColor = std::optional<Color>{};
        for (const auto &character : options().paragraphEllipsisMark()) {
            if (const auto characterColor = drawCharacter(character, pos); characterColor.has_value()) {
                lastColor = characterColor;
            }
        }
        return lastColor;
    }
    }
    return std::nullopt;
}

auto Painter::drawCharacter(const Char &character, Position &pos) -> std::optional<Color> {
    const auto characterWidth = character.displayWidth();
    if (characterWidth <= 0) {
        return std::nullopt;
    }
    if (pos.x() + characterWidth > _rect.x2()) {
        return std::nullopt;
    }
    auto lastColor = std::optional<Color>{};
    if (_buffer.rect().contains(pos)) {
        auto renderedCharacter = character;
        if (_colorResolver) {
            const auto appliedColor = _buffer.get(pos).color().overlayWith(_colorResolver(character, pos));
            renderedCharacter = character.withOverlay(appliedColor);
        } else {
            renderedCharacter = character.withBase(_buffer.get(pos).color());
        }
        _buffer.set(pos, renderedCharacter);
        lastColor = renderedCharacter.color();
    }
    pos += Position{characterWidth, 0};
    return lastColor;
}

void Painter::fillBackgroundRange(const int y, const int x1, const int x2, const Color color) {
    for (auto x = x1; x < x2; ++x) {
        const auto pos = Position{x, y};
        if (!_buffer.rect().contains(pos)) {
            continue;
        }
        const auto appliedColor = _buffer.get(pos).color().overlayWith(Color{Foreground::Inherited, color.bg()});
        _buffer.set(pos, Char{U' '}.withOverlay(appliedColor));
    }
}

}
