// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "AbstractLine.hpp"

#include <algorithm>
#include <vector>

namespace erbsland::cterm::ui::surface {

AbstractLine::AbstractLine(ProtectedTag protectedTag) noexcept : Panel{protectedTag} {
    themeAttributes().setElement(theme::Element::StatusLine);
    editLayoutMetrics().setFixedHeight(1);
}

auto AbstractLine::hasText(const Section section) const -> bool {
    return !text(section).empty();
}

auto AbstractLine::margins([[maybe_unused]] const Section section) const -> Margins {
    return {};
}

auto AbstractLine::collapseBehavior([[maybe_unused]] const Section section) const noexcept -> CollapseBehavior {
    return CollapseBehavior::Ellipsis;
}

auto AbstractLine::sectionWidth(const Section section) const noexcept -> Coordinate {
    return layoutFor(section).availableWidth;
}

void AbstractLine::onLayout(LayoutScope &scope) noexcept {
    refreshSectionLayouts(scope.size().width());
}

void AbstractLine::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    const auto surfaceRect = context.surfaceRect();
    Panel::onPaint(buffer, context);
    refreshSectionLayouts(surfaceRect.width());
    if (surfaceRect.size().isZero()) {
        return;
    }

    const auto &leftLayout = layoutFor(Section::Left);
    const auto &middleLayout = layoutFor(Section::Middle);
    const auto &rightLayout = layoutFor(Section::Right);
    const auto gapStart = leftLayout.reservedWidth;
    const auto textStyle = context.theme().forPart(theme::Part::Background).style();
    const auto drawText = [&](const StringView &source, const Rectangle rect) -> void {
        auto styledText = String{};
        styledText.appendWithBaseStyle(source, textStyle);
        buffer.drawText(styledText, rect);
    };

    if (leftLayout.textDisplayWidth > 0) {
        const auto x = surfaceRect.x1() + std::max(leftLayout.margins.left(), 0);
        drawText(
            leftLayout.visibleText.empty() ? *leftLayout.text : leftLayout.visibleText,
            Rectangle{{x, surfaceRect.y1()}, Size{surfaceRect.x2() - x, 1}});
    }
    if (middleLayout.textDisplayWidth > 0) {
        const auto x = surfaceRect.x1() + gapStart + std::max(middleLayout.margins.left(), 0) +
            std::max(middleLayout.availableWidth - middleLayout.textDisplayWidth, 0) / 2;
        drawText(
            middleLayout.visibleText.empty() ? *middleLayout.text : middleLayout.visibleText,
            Rectangle{{x, surfaceRect.y1()}, Size{surfaceRect.x2() - x, 1}});
    }
    if (rightLayout.textDisplayWidth > 0) {
        const auto x = surfaceRect.x2() - std::max(rightLayout.margins.right(), 0) - rightLayout.textDisplayWidth;
        drawText(
            rightLayout.visibleText.empty() ? *rightLayout.text : rightLayout.visibleText,
            Rectangle{{x, surfaceRect.y1()}, Size{surfaceRect.x2() - x, 1}});
    }
}

void AbstractLine::refreshSectionLayouts(const Coordinate totalWidth) const noexcept {
    const auto normalizedWidth = std::max(totalWidth, 0);
    _sectionLayouts[indexForSection(Section::Left)] = layoutSection(Section::Left, normalizedWidth);
    _sectionLayouts[indexForSection(Section::Right)] =
        layoutSection(Section::Right, std::max(normalizedWidth - reservedWidth(Section::Left), 0));
    _sectionLayouts[indexForSection(Section::Middle)] = layoutSection(
        Section::Middle, std::max(normalizedWidth - reservedWidth(Section::Left) - reservedWidth(Section::Right), 0));
}

auto AbstractLine::reservedWidth(const Section section) const noexcept -> Coordinate {
    return layoutFor(section).reservedWidth;
}

auto AbstractLine::horizontalMargins(const Margins &margins) noexcept -> Coordinate {
    return std::max(margins.left(), 0) + std::max(margins.right(), 0);
}

auto AbstractLine::layoutSection(const Section section, const Coordinate availableWidth) const -> SectionLayout {
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

auto AbstractLine::ellipsizedText(
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

auto AbstractLine::prefixForWidth(const StringView &text, const Coordinate maximumWidth) noexcept -> String {
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

auto AbstractLine::suffixForWidth(const StringView &text, const Coordinate maximumWidth) noexcept -> String {
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

auto AbstractLine::anchorForSection(const Section section) noexcept -> SectionAnchor {
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

auto AbstractLine::layoutFor(const Section section) const noexcept -> const SectionLayout & {
    return _sectionLayouts[indexForSection(section)];
}

auto AbstractLine::indexForSection(const Section section) noexcept -> std::size_t {
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

}
