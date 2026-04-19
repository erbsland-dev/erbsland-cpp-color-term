// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "AbstractStatusLine.hpp"

#include <algorithm>

namespace erbsland::cterm::ui::surface {

AbstractStatusLine::AbstractStatusLine(ProtectedTag protectedTag) noexcept : Panel{protectedTag} {
    geometry().setFixedHeight(1);
}

auto AbstractStatusLine::hasText(const Section section) const -> bool {
    return !text(section).empty();
}

auto AbstractStatusLine::margins(const Section section) const -> Margins {
    static_cast<void>(section);
    return {};
}

auto AbstractStatusLine::collapseBehavior(const Section section) const noexcept -> CollapseBehavior {
    static_cast<void>(section);
    return CollapseBehavior::Ellipsis;
}

auto AbstractStatusLine::sectionWidth(const Section section) const noexcept -> Coordinate {
    return layoutFor(section).availableWidth;
}

void AbstractStatusLine::onLayout(const Size newParentSize) noexcept {
    refreshSectionLayouts(rectangle().width());
    Panel::onLayout(newParentSize);
}

void AbstractStatusLine::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    if (const auto panelBackground = background(); panelBackground.has_value()) {
        buffer.fill(context.targetRect(), *panelBackground);
    }
    refreshSectionLayouts(context.targetRect().width());
    if (context.targetRect().size().isZero()) {
        Surface::onPaint(buffer, context);
        return;
    }

    const auto &leftLayout = layoutFor(Section::Left);
    const auto &middleLayout = layoutFor(Section::Middle);
    const auto &rightLayout = layoutFor(Section::Right);
    const auto gapStart = leftLayout.reservedWidth;

    if (leftLayout.textDisplayWidth > 0) {
        drawClippedText(
            buffer,
            context.targetRect(),
            context.targetRect().x1() + std::max(leftLayout.margins.left(), 0),
            leftLayout.visibleText.empty() ? *leftLayout.text : leftLayout.visibleText);
    }
    if (middleLayout.textDisplayWidth > 0) {
        drawClippedText(
            buffer,
            context.targetRect(),
            context.targetRect().x1() + gapStart + std::max(middleLayout.margins.left(), 0) +
                std::max(middleLayout.availableWidth - middleLayout.textDisplayWidth, 0) / 2,
            middleLayout.visibleText.empty() ? *middleLayout.text : middleLayout.visibleText);
    }
    if (rightLayout.textDisplayWidth > 0) {
        drawClippedText(
            buffer,
            context.targetRect(),
            context.targetRect().x2() - std::max(rightLayout.margins.right(), 0) - rightLayout.textDisplayWidth,
            rightLayout.visibleText.empty() ? *rightLayout.text : rightLayout.visibleText);
    }
    Surface::onPaint(buffer, context);
}

void AbstractStatusLine::refreshSectionLayouts(const Coordinate totalWidth) const noexcept {
    const auto normalizedWidth = std::max(totalWidth, 0);
    _sectionLayouts[indexForSection(Section::Left)] = layoutSection(Section::Left, normalizedWidth);
    _sectionLayouts[indexForSection(Section::Right)] =
        layoutSection(Section::Right, std::max(normalizedWidth - reservedWidth(Section::Left), 0));
    _sectionLayouts[indexForSection(Section::Middle)] = layoutSection(
        Section::Middle, std::max(normalizedWidth - reservedWidth(Section::Left) - reservedWidth(Section::Right), 0));
}

auto AbstractStatusLine::reservedWidth(const Section section) const noexcept -> Coordinate {
    return layoutFor(section).reservedWidth;
}

auto AbstractStatusLine::horizontalMargins(const Margins &margins) noexcept -> Coordinate {
    return std::max(margins.left(), 0) + std::max(margins.right(), 0);
}

auto AbstractStatusLine::layoutSection(const Section section, const Coordinate availableWidth) const -> SectionLayout {
    if (!hasText(section)) {
        return {};
    }
    const auto effectiveMargins = margins(section);
    const auto textAreaWidth = std::max(availableWidth - horizontalMargins(effectiveMargins), 0);
    const auto &sourceText = text(section);
    const auto sourceTextWidth = sourceText.displayWidth();
    const auto behavior = collapseBehavior(section);
    auto layout = SectionLayout{
        .text = &sourceText,
        .visibleText = {},
        .margins = effectiveMargins,
        .availableWidth = textAreaWidth,
        .reservedWidth = 0,
        .textDisplayWidth = sourceTextWidth,
    };
    if (behavior != CollapseBehavior::Never && sourceTextWidth > textAreaWidth) {
        if (behavior == CollapseBehavior::Hide) {
            layout.text = nullptr;
            layout.textDisplayWidth = 0;
            return layout;
        }
        layout.visibleText = ellipsizedText(sourceText, textAreaWidth, anchorForSection(section));
        layout.textDisplayWidth = layout.visibleText.displayWidth();
    }
    if (behavior != CollapseBehavior::Never && layout.textDisplayWidth > 0) {
        layout.reservedWidth = layout.textDisplayWidth + horizontalMargins(layout.margins);
    }
    return layout;
}

auto AbstractStatusLine::ellipsizedText(
    const StringView &text, const Coordinate availableWidth, const SectionAnchor anchor) noexcept -> String {
    if (availableWidth <= 0) {
        return {};
    }
    constexpr auto cEllipsis = Char{U'…'};
    if (availableWidth == 1) {
        return String{1, cEllipsis};
    }
    const auto preservedWidth = std::max(availableWidth - cEllipsis.displayWidth(), 0);
    if (anchor == SectionAnchor::Right) {
        auto result = String{1, cEllipsis};
        result += suffixForWidth(text, preservedWidth);
        return result;
    }
    auto result = prefixForWidth(text, preservedWidth);
    result += cEllipsis;
    return result;
}

auto AbstractStatusLine::prefixForWidth(const StringView &text, const Coordinate maximumWidth) noexcept -> String {
    auto result = String{};
    auto usedWidth = Coordinate{0};
    for (const auto &character : text) {
        const auto characterWidth = character.displayWidth();
        if (characterWidth <= 0) {
            continue;
        }
        if (usedWidth + characterWidth > maximumWidth) {
            break;
        }
        result += character;
        usedWidth += characterWidth;
    }
    return result;
}

auto AbstractStatusLine::suffixForWidth(const StringView &text, const Coordinate maximumWidth) noexcept -> String {
    auto reverseCharacters = std::vector<Char>{};
    auto usedWidth = Coordinate{0};
    for (auto it = text.rbegin(); it != text.rend(); ++it) {
        const auto characterWidth = it->displayWidth();
        if (characterWidth <= 0) {
            continue;
        }
        if (usedWidth + characterWidth > maximumWidth) {
            break;
        }
        reverseCharacters.push_back(*it);
        usedWidth += characterWidth;
    }
    auto result = String{};
    result.reserve(reverseCharacters.size());
    for (auto it = reverseCharacters.rbegin(); it != reverseCharacters.rend(); ++it) {
        result += *it;
    }
    return result;
}

auto AbstractStatusLine::anchorForSection(const Section section) noexcept -> SectionAnchor {
    switch (section) {
    case Section::Left:
        return SectionAnchor::Left;
    case Section::Middle:
        return SectionAnchor::Center;
    case Section::Right:
        return SectionAnchor::Right;
    }
    return SectionAnchor::Left;
}

auto AbstractStatusLine::layoutFor(const Section section) const noexcept -> const SectionLayout & {
    return _sectionLayouts[indexForSection(section)];
}

auto AbstractStatusLine::indexForSection(const Section section) noexcept -> std::size_t {
    switch (section) {
    case Section::Left:
        return 0;
    case Section::Middle:
        return 1;
    case Section::Right:
        return 2;
    }
    return 0;
}

void AbstractStatusLine::drawClippedText(
    WritableBuffer &buffer, const Rectangle targetRect, const Coordinate x, const StringView &text) noexcept {
    auto drawX = x;
    for (const auto &character : text) {
        const auto characterWidth = character.displayWidth();
        if (characterWidth <= 0) {
            continue;
        }
        if (drawX >= targetRect.x2()) {
            return;
        }
        if (drawX >= targetRect.x1() && drawX + characterWidth <= targetRect.x2()) {
            buffer.set(Position{drawX, targetRect.y1()}, character);
        }
        drawX += characterWidth;
    }
}

}
