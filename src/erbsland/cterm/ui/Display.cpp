// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Display.hpp"

#include "event/impl/EventClockAccess.hpp"

#include "../WriteClippedBuffer.hpp"

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <utility>

namespace erbsland::cterm::ui {

Display::Display(TerminalPtr terminal, PagePtr mainPage) :
    Display{std::move(terminal), std::move(mainPage), SizeLimits{}} {
}

Display::Display(TerminalPtr terminal, PagePtr mainPage, theme::ThemeConstPtr theme) :
    Display{std::move(terminal), std::move(mainPage), std::move(theme), SizeLimits{}} {
}

Display::Display(TerminalPtr terminal, PagePtr mainPage, SizeLimits sizeLimits) :
    Display{std::move(terminal), std::move(mainPage), theme::Theme::dark(), sizeLimits} {
}

Display::Display(TerminalPtr terminal, PagePtr mainPage, theme::ThemeConstPtr theme, SizeLimits sizeLimits) :
    _terminal(std::move(terminal)),
    _minimumDisplaySize{sizeLimits.minimumDisplaySize.expandedWith(sizeLimits.hardMinimumDisplaySize)},
    _hardMinimumDisplaySize(sizeLimits.hardMinimumDisplaySize),
    _nextTerminalSizePollTime(impl::EventClockAccess::now()),
    _pageStack({std::move(mainPage)}),
    _nextRenderTime(impl::EventClockAccess::now()),
    _theme{std::move(theme)} {
    if (_terminal == nullptr) {
        throw std::invalid_argument{"terminal must not be null"};
    }
    if (_theme == nullptr) {
        _theme = theme::Theme::dark();
    }
    if (_pageStack.front() == nullptr) {
        throw std::invalid_argument{"mainPage must not be null"};
    }
    _pageStack.front()->setRectangle(_displayBuffer.rect());
    _pageStack.front()->flags().setLayoutOutdated();
    _pageStack.front()->flags().setPaintOutdated();
    refreshTerminalSize();
    _displayBuffer.resize(_terminalSize);
    _updateSettings.setMinimumSize(_hardMinimumDisplaySize);
    _updateSettings.setMinimumSizeBackground(Char{U'╳', Color{fg::BrightRed, bg::Black}});
    _updateSettings.setMinimumSizeMessage(String{U"Terminal size too small!", fg::White});
    _updateSettings.setShowCropMarks(true);
}

void Display::pushPage(PagePtr page) {
    const auto it = std::ranges::find(_pageStack, page);
    if (it != _pageStack.end()) {
        throw std::logic_error("Cannot push duplicate page to the page stack.");
    }
    if (_pageStack.size() >= cMaximumPages) {
        throw std::logic_error("Cannot push more than 20 pages to the page stack.");
    }
    page->setRectangle(_displayBuffer.rect());
    page->flags().setPaintOutdated();
    page->flags().setLayoutOutdated();
    _pageStack.emplace_back(std::move(page));
}

void Display::popPage() {
    if (_pageStack.size() == 1) {
        throw std::logic_error("Cannot pop the main page from the page stack.");
    }
    _pageStack.pop_back();
    _pageStack.back()->flags().setPaintOutdated();
}

void Display::pollTerminalResize() {
    const auto now = impl::EventClockAccess::now();
    if (now < _nextTerminalSizePollTime) {
        return;
    }
    _nextTerminalSizePollTime = now + cTerminalSizePollInterval;
    _terminal->testScreenSize();
    refreshTerminalSize();
}

void Display::pollRender() {
    if (_hasPendingResize) {
        calculateLayout();
    }
    if (!hasVisibleDirtyPage()) {
        return;
    }
    const auto now = impl::EventClockAccess::now();
    if (now < _nextRenderTime) {
        return;
    }
    calculateLayout();
    render();
    _terminal->updateScreen(_displayBuffer, _updateSettings);
    _nextRenderTime = now + cMinimumRenderInterval;
}

auto Display::nextWakeTime() const noexcept -> EventTime {
    auto nextWakeTime = _nextTerminalSizePollTime;
    if (_hasPendingResize || hasVisibleDirtyPage()) {
        nextWakeTime = std::min(nextWakeTime, _nextRenderTime);
    }
    return nextWakeTime;
}

void Display::onKeyPress(KeyPressEvent &keyPressEvent) noexcept {
    if (_pageStack.empty()) {
        return;
    }
    _pageStack.back()->onKeyPress(keyPressEvent);
}

void Display::setTheme(const theme::ThemeConstPtr &theme) {
    _theme = theme == nullptr ? theme::Theme::dark() : theme;
    for (const auto &page : _pageStack) {
        page->flags().setThemeOutdated();
    }
}

void Display::setMinimumDisplaySize(const Size minimumDisplaySize) {
    const auto effectiveMinimumDisplaySize = minimumDisplaySize.expandedWith(_hardMinimumDisplaySize);
    if (effectiveMinimumDisplaySize == _minimumDisplaySize) {
        return;
    }
    _minimumDisplaySize = effectiveMinimumDisplaySize;
    refreshTerminalSize();
}

void Display::setHardMinimumDisplaySize(const Size hardMinimumDisplaySize) {
    if (hardMinimumDisplaySize == _hardMinimumDisplaySize) {
        return;
    }
    _hardMinimumDisplaySize = hardMinimumDisplaySize;
    _updateSettings.setMinimumSize(_hardMinimumDisplaySize);
    _minimumDisplaySize = _minimumDisplaySize.expandedWith(hardMinimumDisplaySize);
    refreshTerminalSize();
}

auto Display::hasVisibleDirtyPage() const noexcept -> bool {
    for (const auto &page : std::views::reverse(_pageStack)) {
        if (hasVisibleDirtySurface(page, _displayBuffer.rect())) {
            return true;
        }
        if (page->isOpaque()) {
            break;
        }
    }
    return false;
}

void Display::calculateLayout() {
    // If a resize is pending, invalidate the layouts on all pages.
    if (_hasPendingResize) {
        _displayBuffer.resize(_terminalSize);
        for (auto &page : _pageStack) {
            page->setRectangle(_displayBuffer.rect());
            page->flags().setLayoutOutdated();
            page->flags().setPaintOutdated();
        }
        _hasPendingResize = false;
    }
    // Trigger layout rebuild on all pages with outdated layout.
    for (auto &page : _pageStack) {
        if (page->flags().isLayoutOutdated()) {
            layoutSurfaceSubtree(page, LayoutContext{themeContextForSurface(page)});
        }
    }
}

auto Display::measureSurface(
    const SurfacePtr &surface, const LayoutProposal &proposal, const LayoutContext &context) noexcept -> LayoutMetrics {
    if (surface == nullptr || !surface->flags().isVisible()) {
        return {};
    }
    auto measureScope = MeasureScope{
        [&](const SurfacePtr &child, const LayoutProposal &childProposal) -> LayoutMetrics {
            return measureSurface(child, childProposal, surface->childLayoutContext(child, context));
        },
        context.themeContext()};
    return surface->onMeasure(measureScope, proposal);
}

void Display::layoutSurfaceSubtree(const SurfacePtr &surface, const LayoutContext &context) noexcept {
    if (surface == nullptr || !surface->flags().isVisible()) {
        return;
    }
    auto scope = LayoutScope{
        surface->rectangle().size(),
        context.themeContext(),
        [&](const SurfacePtr &child, const LayoutProposal &proposal) -> LayoutMetrics {
            return measureSurface(child, proposal, surface->childLayoutContext(child, context));
        },
        [](const SurfacePtr &child, const Rectangle rectangle) -> void {
            if (child != nullptr && child->flags().isVisible()) {
                child->setRectangle(rectangle);
            }
        }};
    surface->onLayout(scope);
    surface->flags().clearLayoutOutdated();
    for (const auto &child : surface->surfaces()) {
        if (child != nullptr && child->flags().isVisible()) {
            layoutSurfaceSubtree(child, surface->childLayoutContext(child, context));
        }
    }
}

void Display::render() {
    if (std::ranges::any_of(
            _pageStack, [](const PagePtr &page) -> bool { return page != nullptr && !page->isOpaque(); })) {
        renderVisiblePageStack();
        return;
    }
    auto paintRequests = PaintRequestList{};
    for (auto &page : std::views::reverse(_pageStack)) {
        collectPaintRequests(page, _displayBuffer.rect(), paintRequests);
        if (page->isOpaque()) {
            // if a page is opaque, no need to paint further pages from the stack.
            break;
        }
    }
    for (const auto &request : paintRequests) {
        paintRequest(request);
    }
}

void Display::renderVisiblePageStack() {
    auto baseIndex = _pageStack.empty() ? std::size_t{0} : _pageStack.size() - 1;
    while (baseIndex > 0 && (_pageStack[baseIndex] == nullptr || !_pageStack[baseIndex]->isOpaque())) {
        baseIndex -= 1;
    }
    for (auto index = baseIndex; index < _pageStack.size(); ++index) {
        const auto &page = _pageStack[index];
        if (page == nullptr || !page->flags().isVisible()) {
            continue;
        }
        paintSurfaceSubtree(page, _displayBuffer.rect(), _displayBuffer.rect());
    }
}

void Display::collectPaintRequests(
    const SurfacePtr &surface, const Rectangle parentVisibleRect, PaintRequestList &requests) const {
    if (surface == nullptr || !surface->flags().isVisible()) {
        return;
    }
    const auto visibleRect = surface->visibleScreenRect(parentVisibleRect);
    if (visibleRect.size().isZero()) {
        return;
    }
    if (surface->flags().isPaintOutdated()) {
        auto dirtyRect = surface->flags().paintDirtyRect().value_or(surface->localSurfaceRect());
        dirtyRect &= surface->localSurfaceRect();
        const auto damagedScreenRect = surface->localToScreen(dirtyRect) & visibleRect;
        if (!damagedScreenRect.size().isZero()) {
            addPaintRequest(requests, PaintRequest{surface->nearestOpaqueSurface(), damagedScreenRect});
        }
    }
    for (const auto &child : surface->surfaces()) {
        collectPaintRequests(child, visibleRect, requests);
    }
}

void Display::addPaintRequest(PaintRequestList &requests, PaintRequest request) const {
    if (request.baseSurface == nullptr || request.screenRect.size().isZero()) {
        return;
    }
    for (auto &existing : requests) {
        if (existing.baseSurface == request.baseSurface && existing.screenRect.overlaps(request.screenRect)) {
            existing.screenRect |= request.screenRect;
            return;
        }
        if (existing.baseSurface->isAncestorOrSame(request.baseSurface) &&
            existing.screenRect.contains(request.screenRect)) {
            return;
        }
    }
    std::erase_if(requests, [&](const PaintRequest &existing) -> bool {
        return request.baseSurface->isAncestorOrSame(existing.baseSurface) &&
            request.screenRect.contains(existing.screenRect);
    });
    requests.emplace_back(std::move(request));
}

void Display::paintRequest(const PaintRequest &request) {
    if (request.baseSurface == nullptr || request.screenRect.size().isZero()) {
        return;
    }
    const auto visibleRect = request.baseSurface->visibleScreenRect(_displayBuffer.rect());
    paintSurfaceSubtree(request.baseSurface, request.screenRect, visibleRect);
    paintLaterSiblings(request.baseSurface, request.screenRect);
}

void Display::paintSurfaceSubtree(
    const SurfacePtr &surface, const Rectangle damagedScreenRect, const Rectangle visibleScreenRect) {
    if (surface == nullptr || !surface->flags().isVisible() || visibleScreenRect.size().isZero()) {
        return;
    }
    const auto surfacePaintRect = damagedScreenRect & visibleScreenRect;
    if (surfacePaintRect.size().isZero()) {
        return;
    }
    paintSurface(surface, damagedScreenRect, visibleScreenRect);
    for (const auto &child : surface->surfaces()) {
        const auto childVisibleRect = child->visibleScreenRect(visibleScreenRect);
        paintSurfaceSubtree(child, damagedScreenRect, childVisibleRect);
    }
}

void Display::paintLaterSiblings(const SurfacePtr &surface, const Rectangle damagedScreenRect) {
    for (auto current = surface; current != nullptr;) {
        const auto parent = current->parent().lock();
        if (parent == nullptr) {
            return;
        }
        const auto parentVisibleRect = parent->visibleScreenRect(_displayBuffer.rect());
        auto paintSiblings = false;
        for (const auto &sibling : parent->surfaces()) {
            if (paintSiblings) {
                if (!sibling->flags().isVisible()) {
                    continue;
                }
                const auto siblingVisibleRect = sibling->visibleScreenRect(parentVisibleRect);
                paintSurfaceSubtree(sibling, damagedScreenRect, siblingVisibleRect);
            }
            if (sibling == current) {
                paintSiblings = true;
            }
        }
        current = parent;
    }
}

void Display::paintSurface(
    const SurfacePtr &surface, const Rectangle damagedScreenRect, const Rectangle visibleScreenRect) {
    const auto paintScreenRect = damagedScreenRect & visibleScreenRect;
    if (surface == nullptr || paintScreenRect.size().isZero()) {
        return;
    }
    const auto surfaceRect = surface->localSurfaceRect();
    const auto visibleLocalRect = surface->screenToLocal(visibleScreenRect) & surfaceRect;
    const auto dirtyLocalRect = surface->screenToLocal(paintScreenRect) & visibleLocalRect;
    if (dirtyLocalRect.size().isZero()) {
        return;
    }
    auto clippedBuffer = WriteClippedBufferRef{_displayBuffer, dirtyLocalRect.topLeft(), paintScreenRect};
    if (surface->actionsPtr() != nullptr) {
        surface->actionsPtr()->refreshForRepaint();
    }
    surface->flags().clearPaintOutdated();
    surface->onPaint(
        clippedBuffer, PaintContext{surfaceRect, visibleLocalRect, dirtyLocalRect, themeContextForSurface(surface)});
}

auto Display::hasVisibleDirtySurface(const SurfacePtr &surface, const Rectangle parentVisibleRect) const noexcept
    -> bool {
    if (surface == nullptr || !surface->flags().isVisible()) {
        return false;
    }
    const auto visibleRect = surface->visibleScreenRect(parentVisibleRect);
    if (visibleRect.size().isZero()) {
        return false;
    }
    if (surface->flags().isLayoutOutdated()) {
        return true;
    }
    if (surface->flags().isPaintOutdated()) {
        auto dirtyRect = surface->flags().paintDirtyRect().value_or(surface->localSurfaceRect());
        dirtyRect &= surface->localSurfaceRect();
        if (!(surface->localToScreen(dirtyRect) & visibleRect).size().isZero()) {
            return true;
        }
    }
    for (const auto &child : surface->surfaces()) {
        if (hasVisibleDirtySurface(child, visibleRect)) {
            return true;
        }
    }
    return false;
}

auto Display::themeContextForSurface(const SurfacePtr &surface) const -> ThemeContext {
    auto path = std::vector<SurfacePtr>{};
    for (auto current = surface; current != nullptr; current = current->parent().lock()) {
        path.push_back(current);
    }
    std::reverse(path.begin(), path.end());
    const auto resolvePath = [&path](auto &&self, const std::size_t index, ThemeContext context) -> ThemeContext {
        if (index >= path.size()) {
            return context;
        }
        const auto &pathSurface = path[index];
        const auto surfaceContext = pathSurface->themeContextFrom(context);
        if (const auto page = std::dynamic_pointer_cast<Page>(pathSurface);
            page != nullptr && page->themeOverride() != nullptr) {
            return self(self, index + 1, surfaceContext.withTheme(page->themeOverride()));
        }
        return self(self, index + 1, surfaceContext);
    };
    return resolvePath(resolvePath, 0, ThemeContext{_theme});
}

void Display::refreshTerminalSize() {
    const auto newTerminalSize = _terminal->size().expandedWith(_minimumDisplaySize);
    if (newTerminalSize != _terminalSize) {
        _terminalSize = newTerminalSize;
        _hasPendingResize = true;
    }
}

}
