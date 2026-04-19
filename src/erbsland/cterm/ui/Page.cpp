// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Page.hpp"

namespace erbsland::cterm::ui {

auto Page::create() -> PagePtr {
    return std::make_shared<Page>(ProtectedTag{});
}

auto Page::background() const noexcept -> const Char & {
    return _background;
}

void Page::setBackground(Char background) noexcept {
    _background = background;
    setPaintOutdated();
}

auto Page::hasFocusSurface() const noexcept -> bool {
    return !_focusSurface.expired();
}

auto Page::focusSurface() const noexcept -> const SurfaceWeakPtr & {
    return _focusSurface;
}

void Page::focusTo(const SurfaceWeakPtr &surface) noexcept {
    const auto focusSurface = surface.lock();
    if (focusSurface == nullptr || !isFocusSurfaceValid(focusSurface)) {
        _focusSurface.reset();
        return;
    }
    _focusSurface = focusSurface;
}

void Page::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    buffer.fill(context.targetRect(), _background);
    Surface::onPaint(buffer, context);
}

void Page::onKeyPress(KeyPressEvent &keyPressEvent) noexcept {
    auto focusSurface = _focusSurface.lock();
    if (focusSurface != nullptr && !isFocusSurfaceValid(focusSurface)) {
        _focusSurface.reset();
        focusSurface.reset();
    }
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
    if (surface == nullptr) {
        return false;
    }
    for (auto parent = surface->parent().lock(); parent != nullptr; parent = parent->parent().lock()) {
        if (parent.get() == this) {
            return true;
        }
    }
    return false;
}

}
