// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Page.hpp"

#include "../theme/ThemePainter.hpp"

#include <algorithm>
#include <ranges>
#include <utility>

namespace erbsland::cterm::ui {

auto Page::create() -> PagePtr {
    return std::make_shared<Page>(ProtectedTag{});
}

void Page::setTheme(theme::ThemeConstPtr theme) noexcept {
    if (_theme == theme) {
        return;
    }
    _theme = std::move(theme);
    flags().setThemeOutdated();
}

void Page::clearTheme() noexcept {
    setTheme({});
}

auto Page::themeOverride() const noexcept -> const theme::ThemeConstPtr & {
    return _theme;
}

auto Page::hasFocusSurface() const noexcept -> bool {
    return !_focusSurface.expired();
}

auto Page::focusSurface() const noexcept -> const SurfaceWeakPtr & {
    return _focusSurface;
}

auto Page::focusTo(const SurfaceWeakPtr &surface) noexcept -> bool {
    const auto oldFocusSurface = _focusSurface.lock();
    const auto focusSurface = surface.lock();
    if (focusSurface == nullptr || !isFocusSurfaceValid(focusSurface)) {
        _focusSurface.reset();
        updateFocusPath(focusPathFor(oldFocusSurface), {});
        return false;
    }
    if (oldFocusSurface == focusSurface && focusSurface->flags().hasFocus()) {
        return true;
    }
    _focusSurface = focusSurface;
    updateFocusPath(focusPathFor(oldFocusSurface), focusPathFor(focusSurface));
    return true;
}

void Page::clearFocus() noexcept {
    const auto oldFocusSurface = _focusSurface.lock();
    if (oldFocusSurface == nullptr) {
        _focusSurface.reset();
        return;
    }
    _focusSurface.reset();
    updateFocusPath(focusPathFor(oldFocusSurface), {});
}

void Page::onLayout(LayoutScope &scope) noexcept {
    for (const auto &child : surfaces()) {
        if (child == nullptr || !child->flags().isVisible()) {
            continue;
        }
        const auto metrics = scope.measure(child, LayoutProposal::atMost(scope.size()));
        scope.place(child, Rectangle{{0, 0}, metrics.resolvedSize(LayoutProposal::atMost(scope.size()))});
    }
}

void Page::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    theme::ThemePainter{buffer, context.theme().forPart(theme::Part::Background)}.fill(context.surfaceRect());
}

void Page::onKeyPress(KeyPressEvent &keyPressEvent) noexcept {
    auto focusSurface = validFocusSurface();
    if (focusSurface != nullptr) {
        focusSurface->onKeyPress(keyPressEvent);
        for (auto parent = focusSurface->parent().lock(); !keyPressEvent.isHandled() && parent != nullptr;
             parent = parent->parent().lock()) {
            if (parent.get() == this) {
                break;
            }
            parent->onKeyPress(keyPressEvent);
        }
    }
    if (!keyPressEvent.isHandled()) {
        Surface::onKeyPress(keyPressEvent);
    }
}

auto Page::isOpaque() const noexcept -> bool {
    return true;
}

auto Page::isPage() const noexcept -> bool {
    return true;
}

auto Page::isFocusSurfaceValid(const SurfacePtr &surface) const noexcept -> bool {
    if (surface == nullptr || !surface->flags().isVisibleInTree() || !surface->flags().isFocusable() ||
        !surface->flags().isEnabled()) {
        return false;
    }
    for (auto parent = surface->parent().lock(); parent != nullptr; parent = parent->parent().lock()) {
        if (parent.get() == this) {
            return true;
        }
    }
    return false;
}

auto Page::validFocusSurface() noexcept -> SurfacePtr {
    auto focusSurface = _focusSurface.lock();
    if (focusSurface != nullptr && !isFocusSurfaceValid(focusSurface)) {
        clearFocus();
        focusSurface.reset();
    }
    return focusSurface;
}

auto Page::focusPathFor(const SurfacePtr &surface) const -> FocusPath {
    auto path = FocusPath{};
    if (surface == nullptr || !isFocusSurfaceValid(surface)) {
        return path;
    }
    for (auto current = surface; current != nullptr && current.get() != this; current = current->parent().lock()) {
        path.push_back(current);
    }
    std::ranges::reverse(path);
    return path;
}

auto Page::relationFor(const FocusPath &path, const SurfacePtr &surface) noexcept -> FocusRelation {
    if (surface == nullptr || path.empty()) {
        return FocusRelation::None;
    }
    const auto it = std::ranges::find(path, surface);
    if (it == path.end()) {
        return FocusRelation::None;
    }
    return surface == path.back() ? FocusRelation::Direct : FocusRelation::Within;
}

void Page::updateFocusPath(const FocusPath &oldPath, const FocusPath &newPath) noexcept {
    for (const auto &surface : std::views::reverse(oldPath)) {
        if (relationFor(oldPath, surface) != relationFor(newPath, surface)) {
            surface->applyFocusChange(FocusChange::Out);
        }
    }
    for (const auto &surface : newPath) {
        const auto oldRelation = relationFor(oldPath, surface);
        const auto newRelation = relationFor(newPath, surface);
        if (oldRelation != newRelation) {
            surface->applyFocusChange(focusChangeFor(newRelation));
        }
    }
}

auto Page::focusChangeFor(const FocusRelation relation) noexcept -> FocusChange {
    return relation == FocusRelation::Direct ? FocusChange::In : FocusChange::InWithin;
}

}
