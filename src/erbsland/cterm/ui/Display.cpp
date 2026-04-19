// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Display.hpp"

#include "event/impl/EventClockAccess.hpp"

#include <algorithm>
#include <ranges>
#include <stdexcept>

namespace erbsland::cterm::ui {

Display::Display(TerminalPtr terminal, PagePtr mainPage) :
    _terminal(std::move(terminal)),
    _displayBuffer(_terminal != nullptr ? _terminal->size() : Size{1, 1}),
    _terminalSize(_terminal != nullptr ? _terminal->size() : Size{1, 1}),
    _nextTerminalSizePollTime(impl::EventClockAccess::now()),
    _pageStack({std::move(mainPage)}),
    _nextRenderTime(impl::EventClockAccess::now()) {
    if (_terminal == nullptr) {
        throw std::invalid_argument{"terminal must not be null"};
    }
    if (_pageStack.front() == nullptr) {
        throw std::invalid_argument{"mainPage must not be null"};
    }
    _pageStack.front()->setRectangle(_displayBuffer.rect());
    _pageStack.front()->setLayoutOutdated();
    _pageStack.front()->setPaintOutdated();
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
    page->setPaintOutdated();
    page->setLayoutOutdated();
    _pageStack.emplace_back(std::move(page));
}

void Display::popPage() {
    if (_pageStack.size() == 1) {
        throw std::logic_error("Cannot pop the main page from the page stack.");
    }
    _pageStack.pop_back();
    _pageStack.back()->setPaintOutdated();
}

void Display::pollTerminalResize() {
    const auto now = impl::EventClockAccess::now();
    if (now < _nextTerminalSizePollTime) {
        return;
    }
    _nextTerminalSizePollTime = now + cTerminalSizePollInterval;
    _terminal->testScreenSize();
    if (_terminal->size() != _terminalSize) {
        _terminalSize = _terminal->size();
        _hasPendingResize = true;
    }
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
    _terminal->updateScreen(_displayBuffer);
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

auto Display::hasVisibleDirtyPage() const noexcept -> bool {
    for (const auto &page : std::views::reverse(_pageStack)) {
        if (page->isLayoutOutdated() || page->isPaintOutdated()) {
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
            page->setLayoutOutdated();
            page->setPaintOutdated();
        }
        _hasPendingResize = false;
    }
    // Trigger layout rebuild on all pages with outdated layout.
    for (auto &page : _pageStack) {
        if (page->isLayoutOutdated()) {
            page->onLayout(_terminalSize);
        }
    }
}

void Display::render() {
    for (auto &page : std::views::reverse(_pageStack)) {
        if (page->isPaintOutdated()) {
            page->onPaint(_displayBuffer, PaintContext{_displayBuffer.rect()});
        }
        if (page->isOpaque()) {
            // if a page is opaque, no need to paint further pages from the stack.
            break;
        }
    }
}

}
