// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StatusLine.hpp"

#include <algorithm>

namespace erbsland::cterm::ui::surface {

auto emptyText() -> const String & {
    static const auto cEmptyText = String{};
    return cEmptyText;
}

StatusLine::StatusLine(ProtectedTag protectedTag) noexcept : AbstractStatusLine{protectedTag} {
}

auto StatusLine::create() noexcept -> StatusLinePtr {
    return std::make_shared<StatusLine>(ProtectedTag{});
}

void StatusLine::setText(const Section section, String text) {
    fieldOrCreate(section).text = std::move(text);
    setPaintOutdated();
}

void StatusLine::setText(const Section section, const std::string_view text) {
    setText(section, String{text});
}

void StatusLine::clearText(const Section section) {
    fieldOrCreate(section).text.clear();
    setPaintOutdated();
}

void StatusLine::setMargins(const Section section, const Margins margins) noexcept {
    fieldOrCreate(section).margins = margins;
    setPaintOutdated();
}

void StatusLine::setCollapseBehavior(const Section section, const CollapseBehavior behavior) noexcept {
    fieldOrCreate(section).collapseBehavior = behavior;
    setPaintOutdated();
}

auto StatusLine::collapseBehavior(const Section section) const noexcept -> CollapseBehavior {
    if (const auto &field = fieldStorage(section); field.has_value()) {
        return field->collapseBehavior;
    }
    return CollapseBehavior::Ellipsis;
}

void StatusLine::setUpdateMode(const Section section, const UpdateMode updateMode) noexcept {
    fieldOrCreate(section).updateMode = updateMode;
    resetResizeTracking(section);
    setPaintOutdated();
}

auto StatusLine::updateMode(const Section section) const noexcept -> UpdateMode {
    if (const auto &field = fieldStorage(section); field.has_value()) {
        return field->updateMode;
    }
    return UpdateMode::Static;
}

void StatusLine::setUpdateFn(const Section section, UpdateFn updateFn) {
    fieldOrCreate(section).updateFn = std::move(updateFn);
    resetResizeTracking(section);
    setPaintOutdated();
}

void StatusLine::clearUpdateFn(const Section section) noexcept {
    fieldOrCreate(section).updateFn = {};
    resetResizeTracking(section);
    setPaintOutdated();
}

auto StatusLine::text(const Section section) const -> const String & {
    if (const auto &field = fieldStorage(section); field.has_value()) {
        return field->text;
    }
    return emptyText();
}

auto StatusLine::hasText(const Section section) const -> bool {
    if (const auto &field = fieldStorage(section); field.has_value()) {
        return !field->text.empty();
    }
    return false;
}

auto StatusLine::margins(const Section section) const -> Margins {
    if (const auto &field = fieldStorage(section); field.has_value()) {
        return field->margins;
    }
    return {};
}

void StatusLine::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    const auto totalWidth = std::max(context.targetRect().width(), 0);
    updateField(Section::Left, totalWidth);
    refreshSectionLayouts(totalWidth);
    updateField(Section::Right, totalWidth);
    refreshSectionLayouts(totalWidth);
    updateField(Section::Middle, totalWidth);
    AbstractStatusLine::onPaint(buffer, context);
}

auto StatusLine::fieldOrCreate(const Section section) noexcept -> Field & {
    auto &field = fieldStorage(section);
    if (!field.has_value()) {
        field.emplace();
    }
    return *field;
}

auto StatusLine::fieldStorage(const Section section) noexcept -> std::optional<Field> & {
    return _fields[indexForSection(section)];
}

auto StatusLine::fieldStorage(const Section section) const noexcept -> const std::optional<Field> & {
    return _fields[indexForSection(section)];
}

auto StatusLine::updateWidthForSection(const Section section, const Coordinate totalWidth) const noexcept
    -> Coordinate {
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

void StatusLine::updateField(const Section section, const Coordinate totalWidth) noexcept {
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

auto StatusLine::indexForSection(const Section section) noexcept -> std::size_t {
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

void StatusLine::resetResizeTracking(const Section section) noexcept {
    _lastWidths[indexForSection(section)] = std::nullopt;
}

}
