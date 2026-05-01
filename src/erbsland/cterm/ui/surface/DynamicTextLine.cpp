// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "DynamicTextLine.hpp"

#include <algorithm>
#include <utility>

namespace erbsland::cterm::ui::surface {

DynamicTextLine::DynamicTextLine(ProtectedTag protectedTag) noexcept : Panel{protectedTag} {
    editLayoutMetrics().setFixedHeight(1);
    childStorage().setManager(*this);
}

auto DynamicTextLine::create() -> DynamicTextLinePtr {
    auto result = std::make_shared<DynamicTextLine>(ProtectedTag{});
    result->initializeUi();
    return result;
}

auto DynamicTextLine::dynamicText(const Section section) const noexcept -> const DynamicTextPtr & {
    return _dynamicTexts[indexForSection(section)];
}

auto DynamicTextLine::text(const Section section) const noexcept -> const String & {
    return dynamicText(section)->text();
}

void DynamicTextLine::setText(const Section section, String text) {
    dynamicText(section)->setText(std::move(text));
}

void DynamicTextLine::setText(const Section section, const std::string_view text) {
    dynamicText(section)->setText(text);
}

void DynamicTextLine::clearText(const Section section) {
    dynamicText(section)->setText(String{});
}

void DynamicTextLine::setMargins(const Section section, const Margins margins) noexcept {
    sectionData(section).margins = margins.expandedWith(Margins{});
    flags().setLayoutOutdated();
}

auto DynamicTextLine::margins(const Section section) const noexcept -> Margins {
    return sectionData(section).margins;
}

void DynamicTextLine::setSpacePriority(const Section section, const SpacePriority priority) noexcept {
    sectionData(section).spacePriority = priority;
    flags().setLayoutOutdated();
}

auto DynamicTextLine::spacePriority(const Section section) const noexcept -> SpacePriority {
    return sectionData(section).spacePriority;
}

auto DynamicTextLine::sectionWidth(const Section section) const noexcept -> Coordinate {
    return sectionData(section).assignedWidth;
}

void DynamicTextLine::onLayout(LayoutScope &scope) noexcept {
    const auto totalWidth = scope.size().width();
    auto leftLayout = layoutSection(Section::Left, totalWidth, scope);
    auto rightLayout = layoutSection(Section::Right, totalWidth - leftLayout.reservedWidth, scope);
    const auto middleAvailableWidth = totalWidth - leftLayout.reservedWidth - rightLayout.reservedWidth;
    auto middleLayout = layoutSection(Section::Middle, middleAvailableWidth, scope);

    if (leftLayout.visible) {
        placeSection(Section::Left, leftLayout, leftLayout.leadingExtra, scope);
    }
    if (rightLayout.visible) {
        const auto x = totalWidth - rightLayout.trailingExtra - rightLayout.assignedWidth;
        placeSection(Section::Right, rightLayout, x, scope);
    }
    if (middleLayout.visible) {
        const auto gapStart = leftLayout.reservedWidth;
        const auto gapWidth = std::max(middleAvailableWidth, Coordinate{0});
        const auto contentWidth =
            std::max(gapWidth - middleLayout.leadingExtra - middleLayout.trailingExtra, Coordinate{0});
        const auto x = gapStart + middleLayout.leadingExtra +
            std::max(contentWidth - middleLayout.assignedWidth, Coordinate{0}) / Coordinate{2};
        placeSection(Section::Middle, middleLayout, x, scope);
    }
}

void DynamicTextLine::initializeUi() {
    Panel::initializeUi();
    themeAttributes().setElement(theme::Element::StatusLine);
    if (_dynamicTexts[indexForSection(Section::Left)] != nullptr) {
        return;
    }
    _dynamicTexts[indexForSection(Section::Left)] = DynamicText::create(String{}, Alignment::TopLeft);
    _dynamicTexts[indexForSection(Section::Middle)] = DynamicText::create(String{}, Alignment::TopCenter);
    _dynamicTexts[indexForSection(Section::Right)] = DynamicText::create(String{}, Alignment::TopRight);
    for (auto index = std::size_t{0}; index < _dynamicTexts.size(); ++index) {
        const auto section = static_cast<Section>(index);
        const auto &dynamicText = _dynamicTexts[index];
        // Copy our own element ID to the surfaces.
        // This will allow customizing the text elements via `DynamicTextLine::Text`.
        dynamicText->themeAttributes().setElement(themeAttributes().element());
        childStorage().add(dynamicText, std::make_shared<SectionData>(section));
    }
}

auto DynamicTextLine::isManagedChild(const SurfacePtr &surface) const noexcept -> bool {
    return std::ranges::find(_dynamicTexts, surface) != _dynamicTexts.end();
}

auto DynamicTextLine::sectionData(const Section section) noexcept -> SectionData & {
    const auto index = indexForSection(section);
    return *std::static_pointer_cast<SectionData>(childStorage().layoutData(_dynamicTexts[index]));
}

auto DynamicTextLine::sectionData(const Section section) const noexcept -> const SectionData & {
    const auto index = indexForSection(section);
    return *std::static_pointer_cast<SectionData>(childStorage().layoutData(_dynamicTexts[index]));
}

auto DynamicTextLine::layoutSection(
    const Section section, const Coordinate proposedAvailableWidth, LayoutScope &scope) noexcept -> SectionLayout {
    const auto availableWidth = std::max(proposedAvailableWidth, Coordinate{0});
    const auto index = indexForSection(section);
    auto &data = sectionData(section);
    const auto &dynamicText = _dynamicTexts[index];
    auto result = SectionLayout{};
    data.assignedWidth = 0;
    if (dynamicText->text().empty()) {
        dynamicText->flags().setVisible(false);
        return result;
    }
    dynamicText->flags().setVisible(true);

    const auto childMetrics = scope.measure(dynamicText, LayoutProposal::unconstrained());
    const auto sectionMargins = data.margins;
    const auto effectiveMargins = sectionMargins.expandedWith(childMetrics.margins(), Orientation::Horizontal);
    result.leadingExtra = effectiveMargins.left();
    result.trailingExtra = effectiveMargins.right();
    const auto contentWidth = std::max(availableWidth - result.leadingExtra - result.trailingExtra, Coordinate{0});
    result.preferredWidth = childMetrics.preferred().width();
    switch (data.spacePriority) {
    case SpacePriority::Keep:
        result.assignedWidth = result.preferredWidth;
        break;
    case SpacePriority::Shrink:
        result.assignedWidth = std::min(result.preferredWidth, contentWidth);
        break;
    case SpacePriority::Hide:
        if (result.preferredWidth > contentWidth) {
            dynamicText->flags().setVisible(false);
            return result;
        }
        result.assignedWidth = result.preferredWidth;
        break;
    }
    result.visible = result.assignedWidth > 0;
    dynamicText->flags().setVisible(result.visible);
    if (result.visible) {
        result.reservedWidth = result.assignedWidth + result.leadingExtra + result.trailingExtra;
        data.assignedWidth = result.assignedWidth;
    }
    return result;
}

void DynamicTextLine::placeSection(
    const Section section, const SectionLayout &layout, const Coordinate x, LayoutScope &scope) noexcept {
    const auto &dynamicText = _dynamicTexts[indexForSection(section)];
    dynamicText->flags().setVisible(true);
    scope.place(dynamicText, Rectangle{{x, 0}, Size{layout.assignedWidth, 1}});
}

auto DynamicTextLine::indexForSection(const Section section) noexcept -> std::size_t {
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
