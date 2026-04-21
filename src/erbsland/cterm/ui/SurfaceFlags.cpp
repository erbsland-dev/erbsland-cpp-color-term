// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "SurfaceFlags.hpp"

#include "Surface.hpp"

namespace erbsland::cterm::ui {

SurfaceFlags::SurfaceFlags(Surface &owner) noexcept : _owner{owner} {
}

auto SurfaceFlags::isVisible() const noexcept -> bool {
    return _isVisible;
}

void SurfaceFlags::setVisible(const bool visible) {
    if (_isVisible == visible) {
        return;
    }
    const auto oldRect = _owner._rectangle;
    if (!visible) {
        _owner.clearSubtreeFocusIfNeeded();
    }
    _isVisible = visible;
    setLayoutOutdated();
    if (const auto parent = _owner.parent().lock(); parent != nullptr && !oldRect.size().isZero()) {
        parent->flags().setPaintOutdated(oldRect);
    }
    if (visible) {
        setPaintOutdated();
    }
}

auto SurfaceFlags::isVisibleInTree() const noexcept -> bool {
    if (!_isVisible) {
        return false;
    }
    for (auto parentSurface = _owner.parent().lock(); parentSurface != nullptr;
         parentSurface = parentSurface->parent().lock()) {
        if (!parentSurface->flags().isVisible()) {
            return false;
        }
    }
    return true;
}

auto SurfaceFlags::isFocusable() const noexcept -> bool {
    return _isFocusable;
}

void SurfaceFlags::setFocusable(const bool focusable) noexcept {
    if (_isFocusable == focusable) {
        return;
    }
    if (!focusable) {
        _owner.clearDirectFocusIfNeeded();
    }
    _isFocusable = focusable;
}

auto SurfaceFlags::isEnabled() const noexcept -> bool {
    return _isEnabled;
}

void SurfaceFlags::setEnabled(const bool enabled) noexcept {
    if (_isEnabled == enabled) {
        return;
    }
    if (!enabled) {
        _owner.clearDirectFocusIfNeeded();
    }
    _isEnabled = enabled;
    setThemeOutdated();
}

auto SurfaceFlags::isSelected() const noexcept -> bool {
    return _isSelected;
}

void SurfaceFlags::setSelected(const bool selected) {
    if (_isSelected == selected) {
        return;
    }
    _isSelected = selected;
    setThemeOutdated();
}

auto SurfaceFlags::isChecked() const noexcept -> bool {
    return _isChecked;
}

void SurfaceFlags::setChecked(const bool checked) {
    if (_isChecked == checked) {
        return;
    }
    _isChecked = checked;
    setThemeOutdated();
}

auto SurfaceFlags::hasFocus() const noexcept -> bool {
    return _hasFocus;
}

void SurfaceFlags::setHasFocus(const bool hasFocus) noexcept {
    _hasFocus = hasFocus;
}

auto SurfaceFlags::hasFocusWithin() const noexcept -> bool {
    return _hasFocusWithin;
}

void SurfaceFlags::setHasFocusWithin(const bool hasFocusWithin) noexcept {
    _hasFocusWithin = hasFocusWithin;
}

auto SurfaceFlags::themeStates() const noexcept -> theme::States {
    auto result = theme::States{};
    if (_hasFocus) {
        result.add(theme::State::Focused);
    }
    if (_hasFocusWithin) {
        result.add(theme::State::FocusWithin);
    }
    if (!_isEnabled) {
        result.add(theme::State::Disabled);
    }
    if (_isSelected) {
        result.add(theme::State::Selected);
    }
    if (_isChecked) {
        result.add(theme::State::Checked);
    }
    return result;
}

auto SurfaceFlags::isLayoutOutdated() const noexcept -> bool {
    return _isLayoutOutdated;
}

void SurfaceFlags::setLayoutOutdated() {
    _isLayoutOutdated = true;
    if (const auto parent = _owner.parent().lock(); parent != nullptr) {
        parent->flags().setLayoutOutdated();
    }
    setPaintOutdated();
}

void SurfaceFlags::clearLayoutOutdated() noexcept {
    _isLayoutOutdated = false;
}

auto SurfaceFlags::isPaintOutdated() const noexcept -> bool {
    return _isPaintOutdated;
}

void SurfaceFlags::setPaintOutdated() {
    _isPaintOutdated = true;
    _paintDirtyRect.reset();
}

void SurfaceFlags::setPaintOutdated(const Rectangle dirtyRect) {
    if (dirtyRect.size().isZero()) {
        return;
    }
    if (!_isPaintOutdated) {
        _paintDirtyRect = dirtyRect;
    } else if (_paintDirtyRect.has_value()) {
        *_paintDirtyRect |= dirtyRect;
    }
    _isPaintOutdated = true;
}

void SurfaceFlags::setThemeOutdated() {
    setLayoutOutdated();
    setPaintOutdated();
    for (const auto &child : _owner.surfaces()) {
        if (child != nullptr) {
            child->flags().setThemeOutdated();
        }
    }
}

void SurfaceFlags::clearPaintOutdated() noexcept {
    _isPaintOutdated = false;
    _paintDirtyRect.reset();
}

auto SurfaceFlags::paintDirtyRect() const noexcept -> const std::optional<Rectangle> & {
    return _paintDirtyRect;
}

}
