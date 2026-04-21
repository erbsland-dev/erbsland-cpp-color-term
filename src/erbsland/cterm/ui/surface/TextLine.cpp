// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TextLine.hpp"

#include <algorithm>

namespace erbsland::cterm::ui::surface {

auto emptyLineText() -> const String & {
    static const auto cEmptyText = String{};
    return cEmptyText;
}

TextLine::TextLine(ProtectedTag protectedTag) noexcept : AbstractLine{protectedTag} {
}

auto TextLine::create() noexcept -> TextLinePtr {
    return std::make_shared<TextLine>(ProtectedTag{});
}

void TextLine::setText(const Section section, String text) {
    fieldOrCreate(section).text = std::move(text);
    flags().setPaintOutdated();
}

void TextLine::setText(const Section section, const std::string_view text) {
    setText(section, String{text});
}

void TextLine::clearText(const Section section) {
    fieldOrCreate(section).text.clear();
    flags().setPaintOutdated();
}

void TextLine::setMargins(const Section section, const Margins margins) noexcept {
    fieldOrCreate(section).margins = margins;
    flags().setPaintOutdated();
}

void TextLine::setCollapseBehavior(const Section section, const CollapseBehavior behavior) noexcept {
    fieldOrCreate(section).collapseBehavior = behavior;
    flags().setPaintOutdated();
}

auto TextLine::collapseBehavior(const Section section) const noexcept -> CollapseBehavior {
    if (const auto &field = fieldStorage(section); field.has_value()) {
        return field->collapseBehavior;
    }
    return CollapseBehavior::Ellipsis;
}

void TextLine::setUpdateMode(const Section section, const UpdateMode updateMode) noexcept {
    fieldOrCreate(section).updateMode = updateMode;
    resetResizeTracking(section);
    flags().setPaintOutdated();
}

auto TextLine::updateMode(const Section section) const noexcept -> UpdateMode {
    if (const auto &field = fieldStorage(section); field.has_value()) {
        return field->updateMode;
    }
    return UpdateMode::Static;
}

void TextLine::setUpdateFn(const Section section, UpdateFn updateFn) {
    fieldOrCreate(section).updateFn = std::move(updateFn);
    resetResizeTracking(section);
    flags().setPaintOutdated();
}

void TextLine::clearUpdateFn(const Section section) noexcept {
    fieldOrCreate(section).updateFn = {};
    resetResizeTracking(section);
    flags().setPaintOutdated();
}

auto TextLine::text(const Section section) const -> const String & {
    if (const auto &field = fieldStorage(section); field.has_value()) {
        return field->text;
    }
    return emptyLineText();
}

auto TextLine::hasText(const Section section) const -> bool {
    if (const auto &field = fieldStorage(section); field.has_value()) {
        return !field->text.empty();
    }
    return false;
}

auto TextLine::margins(const Section section) const -> Margins {
    if (const auto &field = fieldStorage(section); field.has_value()) {
        return field->margins;
    }
    return {};
}

void TextLine::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    const auto totalWidth = std::max(context.surfaceRect().width(), 0);
    updateField(Section::Left, totalWidth);
    refreshSectionLayouts(totalWidth);
    updateField(Section::Right, totalWidth);
    refreshSectionLayouts(totalWidth);
    updateField(Section::Middle, totalWidth);
    AbstractLine::onPaint(buffer, context);
}

auto TextLine::fieldOrCreate(const Section section) noexcept -> Field & {
    auto &field = fieldStorage(section);
    if (!field.has_value()) {
        field.emplace();
    }
    return *field;
}

auto TextLine::fieldStorage(const Section section) noexcept -> std::optional<Field> & {
    return _fields[indexForSection(section)];
}

auto TextLine::fieldStorage(const Section section) const noexcept -> const std::optional<Field> & {
    return _fields[indexForSection(section)];
}

auto TextLine::updateWidthForSection(const Section section, const Coordinate totalWidth) const noexcept -> Coordinate {
    const auto &field = fieldStorage(section);
    if (!field.has_value()) {
        return 0;
    }
    switch (section) {
    case Section::Left:
        return std::max(totalWidth - horizontalMargins(field->margins), 0);
    case Section::Right:
        return std::max(totalWidth - reservedWidth(Section::Left) - horizontalMargins(field->margins), 0);
    case Section::Middle:
        return std::max(
            totalWidth - reservedWidth(Section::Left) - reservedWidth(Section::Right) -
                horizontalMargins(field->margins),
            0);
    }
    return 0;
}

void TextLine::updateField(const Section section, const Coordinate totalWidth) noexcept {
    auto &field = fieldStorage(section);
    if (!field.has_value() || field->updateMode == UpdateMode::Static || !field->updateFn) {
        return;
    }
    const auto currentWidth = updateWidthForSection(section, totalWidth);
    auto &lastWidth = _lastWidths[indexForSection(section)];
    if (field->updateMode == UpdateMode::OnResize && lastWidth.has_value() && *lastWidth == currentWidth) {
        return;
    }
    field->updateFn(field->text, currentWidth);
    lastWidth = currentWidth;
}

auto TextLine::indexForSection(const Section section) noexcept -> std::size_t {
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

void TextLine::resetResizeTracking(const Section section) noexcept {
    _lastWidths[indexForSection(section)] = std::nullopt;
}

}
