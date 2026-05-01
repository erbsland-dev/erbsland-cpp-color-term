// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Surface.hpp"

#include "Page.hpp"

#include <functional>
#include <utility>

namespace erbsland::cterm::ui {

Surface::Surface(const theme::Element themeElement) noexcept : _themeAttributes{themeElement} {
}

Surface::Surface(LayoutMetrics layoutMetrics) : _layoutMetrics{std::move(layoutMetrics)} {
}

Surface::Surface(const theme::Element themeElement, LayoutMetrics layoutMetrics) :
    _layoutMetrics{std::move(layoutMetrics)}, _themeAttributes{themeElement} {
}

auto Surface::parent() const noexcept -> const SurfaceWeakPtr & {
    return _parent;
}

auto Surface::surfaces() noexcept -> AbstractSurfaceContainer & {
    return _surfaces;
}

auto Surface::surfaces() const noexcept -> const AbstractSurfaceContainer & {
    return _surfaces;
}

void Surface::addSurface(SurfacePtr surface) {
    surfaces().add(std::move(surface));
}

auto Surface::schedulerPtr() const noexcept -> const SchedulerPtr & {
    return _scheduler;
}

auto Surface::isPage() const noexcept -> bool {
    return false;
}

auto Surface::isOpaque() const noexcept -> bool {
    return false;
}

auto Surface::onMeasure([[maybe_unused]] MeasureScope &scope, [[maybe_unused]] const LayoutProposal &proposal) noexcept
    -> LayoutMetrics {
    return _layoutMetrics;
}

void Surface::onLayout([[maybe_unused]] LayoutScope &scope) noexcept {
}

void Surface::onPaint([[maybe_unused]] WritableBuffer &buffer, [[maybe_unused]] const PaintContext &context) noexcept {
}

void Surface::onKeyPress(KeyPressEvent &keyPressEvent) noexcept {
    if (_actions != nullptr) {
        _actions->onKeyPress(keyPressEvent);
    }
}

void Surface::onFocus([[maybe_unused]] const FocusChange focusChange) noexcept {
}

void Surface::initializeUi() {
}

auto Surface::localToScreen(Rectangle localRect) const noexcept -> Rectangle {
    auto screenPosition = localRect.pos() + _rectangle.pos();
    for (auto parentSurface = parent().lock(); parentSurface != nullptr;
         parentSurface = parentSurface->parent().lock()) {
        screenPosition += parentSurface->_rectangle.pos();
    }
    return Rectangle{screenPosition, localRect.size()};
}

auto Surface::screenToLocal(Rectangle screenRect) const noexcept -> Rectangle {
    auto localPosition = screenRect.pos() - _rectangle.pos();
    for (auto parentSurface = parent().lock(); parentSurface != nullptr;
         parentSurface = parentSurface->parent().lock()) {
        localPosition -= parentSurface->_rectangle.pos();
    }
    return Rectangle{localPosition, screenRect.size()};
}

auto Surface::visibleScreenRect(Rectangle screenRect) const noexcept -> Rectangle {
    if (!flags().isVisibleInTree()) {
        return {};
    }
    auto visibleRect = _rectangle;
    for (auto parentSurface = parent().lock(); parentSurface != nullptr;
         parentSurface = parentSurface->parent().lock()) {
        visibleRect &= parentSurface->localSurfaceRect();
        if (visibleRect.size().isZero()) {
            return {};
        }
        visibleRect = Rectangle{parentSurface->_rectangle.pos() + visibleRect.pos(), visibleRect.size()};
    }
    return visibleRect & screenRect;
}

auto Surface::nearestOpaqueSurface() noexcept -> SurfacePtr {
    for (auto surface = weak_from_this().lock(); surface != nullptr; surface = surface->parent().lock()) {
        if (surface->isOpaque()) {
            return surface;
        }
    }
    return {};
}

auto Surface::isAncestorOrSame(const SurfacePtr &surface) const noexcept -> bool {
    for (auto current = surface; current != nullptr; current = current->parent().lock()) {
        if (current.get() == this) {
            return true;
        }
    }
    return false;
}

auto Surface::flags() noexcept -> SurfaceFlags & {
    return _flags;
}

auto Surface::flags() const noexcept -> const SurfaceFlags & {
    return _flags;
}

auto Surface::themeAttributes() noexcept -> SurfaceThemeAttributes & {
    return _themeAttributes;
}

auto Surface::themeAttributes() const noexcept -> const SurfaceThemeAttributes & {
    return _themeAttributes;
}

void Surface::setRectangle(const Rectangle &rect) noexcept {
    if (_rectangle == rect) {
        return;
    }
    const auto oldRect = _rectangle;
    _rectangle = rect;
    flags().setPaintOutdated();
    if (const auto parent = this->parent().lock(); parent != nullptr) {
        if (oldRect.size().isZero()) {
            parent->flags().setPaintOutdated(rect);
        } else if (rect.size().isZero()) {
            parent->flags().setPaintOutdated(oldRect);
        } else {
            parent->flags().setPaintOutdated(oldRect | rect);
        }
    }
}

auto Surface::layoutMetrics() const noexcept -> const LayoutMetrics & {
    return _layoutMetrics;
}

auto Surface::editLayoutMetrics() noexcept -> LayoutMetricsEditor {
    return LayoutMetricsEditor{*this};
}

void Surface::setLayoutMetrics(LayoutMetrics layoutMetrics) {
    if (_layoutMetrics == layoutMetrics) {
        return;
    }
    _layoutMetrics = std::move(layoutMetrics);
    flags().setLayoutOutdated();
}

auto Surface::requestFocus() noexcept -> bool {
    if (const auto page = nearestPage(); page != nullptr) {
        return page->focusTo(weak_from_this());
    }
    return false;
}

auto Surface::actions() noexcept -> Actions & {
    if (_actions == nullptr) {
        _actions = std::make_unique<Actions>(weak_from_this());
    }
    return *_actions;
}

auto Surface::actionsPtr() const noexcept -> const ActionsPtr & {
    return _actions;
}

auto Surface::scheduler() noexcept -> Scheduler & {
    if (_scheduler == nullptr) {
        _scheduler = std::make_unique<Scheduler>(weak_from_this());
    }
    return *_scheduler;
}

void Surface::layout(const Size size, const LayoutContext &context) noexcept {
    using MeasureSurfaceFn = std::function<LayoutMetrics(const SurfacePtr &, const LayoutProposal &, LayoutContext)>;
    auto measureSurface = MeasureSurfaceFn{};
    measureSurface = [&](const SurfacePtr &surface,
                         const LayoutProposal &proposal,
                         const LayoutContext childContext) -> LayoutMetrics {
        if (surface == nullptr || !surface->flags().isVisible()) {
            return {};
        }
        auto measureScope = MeasureScope{
            [&](const SurfacePtr &child, const LayoutProposal &childProposal) -> LayoutMetrics {
                return measureSurface(child, childProposal, surface->childLayoutContext(child, childContext));
            },
            childContext.themeContext()};
        return surface->onMeasure(measureScope, proposal);
    };
    auto scope = LayoutScope{
        size,
        context.themeContext(),
        [&](const SurfacePtr &child, const LayoutProposal &proposal) -> LayoutMetrics {
            return measureSurface(child, proposal, childLayoutContext(child, context));
        },
        [](const SurfacePtr &child, const Rectangle rectangle) -> void {
            if (child != nullptr && child->flags().isVisible()) {
                child->setRectangle(rectangle);
            }
        }};
    onLayout(scope);
    flags().clearLayoutOutdated();
}

auto Surface::childStorage() noexcept -> SurfaceContainer & {
    return _surfaces;
}

auto Surface::childStorage() const noexcept -> const SurfaceContainer & {
    return _surfaces;
}

auto Surface::childLayoutContext(const SurfacePtr &child, const LayoutContext &parentContext) const -> LayoutContext {
    if (child == nullptr) {
        return parentContext;
    }
    return LayoutContext{child->themeContextFrom(parentContext.themeContext())};
}

void Surface::applyFocusChange(const FocusChange focusChange) noexcept {
    switch (focusChange) {
    case FocusChange::In:
        _flags.setHasFocus(true);
        _flags.setHasFocusWithin(true);
        _flags.setThemeOutdated();
        break;
    case FocusChange::InWithin:
        _flags.setHasFocus(false);
        _flags.setHasFocusWithin(true);
        _flags.setThemeOutdated();
        break;
    case FocusChange::Out:
        _flags.setHasFocus(false);
        _flags.setHasFocusWithin(false);
        _flags.setThemeOutdated();
        break;
    }
    onFocus(focusChange);
}

void Surface::clearDirectFocusIfNeeded() noexcept {
    if (!flags().hasFocus()) {
        return;
    }
    if (const auto page = nearestPage(); page != nullptr) {
        page->clearFocus();
    }
}

void Surface::clearSubtreeFocusIfNeeded() noexcept {
    if (!flags().hasFocusWithin()) {
        return;
    }
    if (const auto page = nearestPage(); page != nullptr) {
        page->clearFocus();
    }
}

auto Surface::nearestPage() noexcept -> PagePtr {
    for (auto surface = weak_from_this().lock(); surface != nullptr; surface = surface->parent().lock()) {
        if (surface->isPage()) {
            return std::dynamic_pointer_cast<Page>(surface);
        }
    }
    return {};
}

auto Surface::themeContextFrom(const ThemeContext &parentContext) const -> ThemeContext {
    auto tags = parentContext.tags();
    tags.add(themeAttributes().tags());
    return parentContext.withSurface(themeAttributes().element(), flags().themeStates(), std::move(tags));
}

}
