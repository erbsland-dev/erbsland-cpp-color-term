// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ParagraphPainter.hpp"


#include <optional>


namespace erbsland::cterm::impl {


ParagraphPainter::ParagraphPainter(
    WritableBuffer &buffer,
    const Rectangle rect,
    const Alignment alignment,
    const ParagraphLayout::Result &layout,
    const ParagraphBackgroundMode backgroundMode,
    const ColorResolver &colorResolver) noexcept :
    _buffer{buffer},
    _rect{rect},
    _alignment{alignment},
    _layout{layout},
    _backgroundMode{backgroundMode},
    _colorResolver{colorResolver} {
}

void ParagraphPainter::paint() {
    const auto maxLines = std::min(static_cast<int>(_layout.lines.size()), _rect.height());
    const auto leftFillEnabled = usesLeftFill();
    const auto rightFillEnabled = usesRightFill();
    auto yStart = _rect.y1();
    switch (_alignment & Alignment::VerticalMask) {
    case Alignment::VCenter:
        yStart += (_rect.height() - maxLines) / 2;
        break;
    case Alignment::Bottom:
        yStart = _rect.y2() - maxLines;
        break;
    default:
        break;
    }
    auto previousWrapColor = std::optional<Color>{};
    for (auto lineIndex = 0; lineIndex < maxLines; ++lineIndex) {
        const auto &line = _layout.lines[static_cast<std::size_t>(lineIndex)];
        const auto y = yStart + lineIndex;
        const auto endMarkWidth = line.endMark.displayWidth();
        const auto availableWidth = _rect.width() - endMarkWidth;
        const auto textWidth = line.text.displayWidth();
        auto textX = _rect.x1();
        switch (_alignment & Alignment::HorizontalMask) {
        case Alignment::Right:
            textX = _rect.x1() + availableWidth - textWidth;
            break;
        case Alignment::HCenter:
            textX = _rect.x1() + (availableWidth - textWidth) / 2;
            break;
        default:
            break;
        }
        auto lastColor = drawSegment(line.text, Position{textX, y});
        auto rightFillColor = std::optional<Color>{};
        auto rightFillStart = 0;
        auto rightFillEnd = 0;
        if (!line.endMark.empty()) {
            const auto endMarkX = _rect.x2() - endMarkWidth;
            if (const auto markColor = drawSegment(line.endMark, Position{endMarkX, y}); markColor.has_value()) {
                lastColor = markColor;
            }
            if (rightFillEnabled && lastColor.has_value()) {
                rightFillColor = lastColor;
                rightFillStart = textX + textWidth;
                rightFillEnd = endMarkX;
            }
        } else if (lastColor.has_value() && usesRightFillForLine(line)) {
            rightFillColor = lastColor;
            rightFillStart = textX + textWidth;
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

auto ParagraphPainter::usesLeftFill() const noexcept -> bool {
    return _backgroundMode == ParagraphBackgroundMode::WrappedLeft ||
        _backgroundMode == ParagraphBackgroundMode::WrappedBoth || _backgroundMode == ParagraphBackgroundMode::FullBoth;
}

auto ParagraphPainter::usesRightFill() const noexcept -> bool {
    return _backgroundMode == ParagraphBackgroundMode::WrappedRight ||
        _backgroundMode == ParagraphBackgroundMode::WrappedBoth ||
        _backgroundMode == ParagraphBackgroundMode::FullRight || _backgroundMode == ParagraphBackgroundMode::FullBoth;
}

auto ParagraphPainter::usesRightFillForLine(const ParagraphLayout::Line &line) const noexcept -> bool {
    if (_backgroundMode == ParagraphBackgroundMode::FullRight || _backgroundMode == ParagraphBackgroundMode::FullBoth) {
        return true;
    }
    return line.wrapsToNext &&
        (_backgroundMode == ParagraphBackgroundMode::WrappedRight ||
         _backgroundMode == ParagraphBackgroundMode::WrappedBoth);
}

auto ParagraphPainter::drawSegment(const String &text, Position pos) -> std::optional<Color> {
    auto lastColor = std::optional<Color>{};
    for (const auto &character : text) {
        const auto characterWidth = character.displayWidth();
        if (characterWidth <= 0) {
            continue;
        }
        if (pos.x() + characterWidth > _rect.x2()) {
            break;
        }
        if (_buffer.rect().contains(pos)) {
            auto renderedCharacter = character;
            if (_colorResolver) {
                const auto appliedColor = _buffer.get(pos).color().overlayWith(_colorResolver(character, pos));
                renderedCharacter = character.withColorOverlay(appliedColor);
            } else {
                renderedCharacter = character.withBaseColor(_buffer.get(pos).color());
            }
            _buffer.set(pos, renderedCharacter);
            lastColor = renderedCharacter.color();
        }
        pos += Position{characterWidth, 0};
    }
    return lastColor;
}

void ParagraphPainter::fillBackgroundRange(const int y, const int x1, const int x2, const Color color) {
    for (auto x = x1; x < x2; ++x) {
        const auto pos = Position{x, y};
        if (!_buffer.rect().contains(pos)) {
            continue;
        }
        const auto appliedColor = _buffer.get(pos).color().overlayWith(Color{Foreground::Inherited, color.bg()});
        _buffer.set(pos, Char{U' '}.withColorOverlay(appliedColor));
    }
}


}
