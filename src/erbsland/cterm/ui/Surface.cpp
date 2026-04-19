// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Surface.hpp"

#include "Layout.hpp"

#include "layout/impl/LayoutConstraints.hpp"

#include <algorithm>
#include <ranges>

namespace erbsland::cterm::ui {

Surface::Surface(Geometry geometry) : _geometry{std::move(geometry)} {
}

auto Surface::parent() const noexcept -> const SurfaceWeakPtr & {
    return _parent;
}

void Surface::setParent(const SurfacePtr &parent) {
    _parent = parent;
}

auto Surface::isInParentChain(const SurfacePtr &surface) const noexcept -> bool {
    if (surface == shared_from_this()) {
        return true;
    }
    if (_parent.expired()) {
        return false;
    }
    return _parent.lock()->isInParentChain(surface);
}

auto Surface::schedulerIfExists() noexcept -> Scheduler * {
    return _scheduler.get();
}

auto Surface::schedulerIfExists() const noexcept -> const Scheduler * {
    return _scheduler.get();
}

auto Surface::isPage() const noexcept -> bool {
    return false;
}

auto Surface::isOpaque() const noexcept -> bool {
    return false;
}

void Surface::onLayout(const Size newParentSize) noexcept {
    for (const auto &child : _children) {
        const auto childSize = layout::impl::LayoutConstraints::fromGeometry(child->geometry()).resolve(newParentSize);
        child->setRectangle(Rectangle{{0, 0}, childSize});
        child->onLayout(childSize);
    }
    _isLayoutOutdated = false;
}

void Surface::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    for (const auto &child : _children) {
        const auto childRect = context.targetRect() & effectiveChildRect(child->rectangle());
        if (childRect.size().isZero()) {
            continue;
        }
        child->onPaint(buffer, PaintContext{childRect});
    }
    _isPaintOutdated = false;
}

void Surface::onKeyPress(KeyPressEvent &keyPressEvent) noexcept {
    if (_keyBindings != nullptr) {
        _keyBindings->onKeyPress(keyPressEvent);
    }
}

auto Surface::children() const noexcept -> const std::vector<SurfacePtr> & {
    return _children;
}

void Surface::addChild(SurfacePtr child) {
    if (isInParentChain(child)) {
        throw std::invalid_argument("Cannot add child surface to its own parent chain");
    }
    child->setParent(shared_from_this());
    _children.push_back(std::move(child));
    setLayoutOutdated();
}

void Surface::removeChild(const SurfacePtr &child) {
    const auto it = std::ranges::find(_children, child);
    if (it == _children.end()) {
        return;
    }
    _children.erase(it);
    child->setParent(SurfacePtr{});
    setLayoutOutdated();
}

auto Surface::effectiveRect() const noexcept -> Rectangle {
    if (_parent.expired()) {
        return _rectangle;
    }
    return _parent.lock()->effectiveChildRect(_rectangle);
}

auto Surface::effectiveChildRect(const Rectangle childRect) const noexcept -> Rectangle {
    return Rectangle{_rectangle.pos() + childRect.pos(), childRect.size()};
}

auto Surface::isLayoutOutdated() const noexcept -> bool {
    return _isLayoutOutdated;
}

void Surface::setLayoutOutdated() {
    if (!_isLayoutOutdated) {
        _isLayoutOutdated = true;
        if (!_parent.expired()) {
            _parent.lock()->setLayoutOutdated();
        }
    }
    setPaintOutdated();
}

auto Surface::isPaintOutdated() const noexcept -> bool {
    return _isPaintOutdated;
}

void Surface::setPaintOutdated() {
    if (!_isPaintOutdated) {
        _isPaintOutdated = true;
        if (!_parent.expired()) {
            _parent.lock()->setPaintOutdated();
        }
    }
}

auto Surface::geometry() noexcept -> Geometry & {
    return _geometry;
}

auto Surface::geometry() const noexcept -> const Geometry & {
    return _geometry;
}

void Surface::setGeometry(Geometry geometry) {
    _geometry = std::move(geometry);
    setLayoutOutdated();
}

auto Surface::keyBindings() noexcept -> KeyBindings & {
    if (_keyBindings == nullptr) {
        _keyBindings = std::make_unique<KeyBindings>();
    }
    return *_keyBindings;
}

auto Surface::scheduler() noexcept -> Scheduler & {
    if (_scheduler == nullptr) {
        _scheduler = std::make_unique<Scheduler>(weak_from_this());
    }
    return *_scheduler;
}

}
