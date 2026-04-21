// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "HtmlHelpSection.hpp"

#include "../../text/HtmlRenderer.hpp"

#include <algorithm>

namespace erbsland::cterm::ui::surface {

HtmlHelpSection::HtmlHelpSection(ProtectedTag protectedTag) noexcept : AbstractHelpSection{protectedTag} {
}

auto HtmlHelpSection::create() -> HtmlHelpSectionPtr {
    auto result = std::make_shared<HtmlHelpSection>(ProtectedTag{});
    result->initializeUi();
    return result;
}

auto HtmlHelpSection::title() const noexcept -> std::string_view {
    return _title;
}

void HtmlHelpSection::setTitle(const std::string_view title) {
    _title = title;
    flags().setPaintOutdated();
}

auto HtmlHelpSection::html() const noexcept -> const std::string & {
    return _html;
}

void HtmlHelpSection::setHtml(const std::string_view html) {
    _html = html;
    _documentDirty = true;
    renderDocumentIfRequired(_view->rectangle().width());
}

auto HtmlHelpSection::view() const noexcept -> const ScrollingBufferViewPtr & {
    return _view;
}

auto HtmlHelpSection::contentSize() const noexcept -> Size {
    return _documentBuffer->size();
}

void HtmlHelpSection::scrollUp(const Coordinate count) noexcept {
    _view->scrollUp(count);
}

void HtmlHelpSection::scrollDown(const Coordinate count) noexcept {
    _view->scrollDown(count);
}

void HtmlHelpSection::pageUp() noexcept {
    _view->pageUp();
}

void HtmlHelpSection::pageDown() noexcept {
    _view->pageDown();
}

void HtmlHelpSection::scrollToTop() noexcept {
    _view->scrollToTop();
}

void HtmlHelpSection::scrollToBottom() noexcept {
    _view->scrollToBottom();
}

void HtmlHelpSection::onLayout(LayoutScope &scope) noexcept {
    scope.place(_view, Rectangle{Position{}, scope.size()});
    renderDocumentIfRequired(scope.size().width());
}

void HtmlHelpSection::initializeUi() {
    _style = text::Style::defaultStyled();
    _documentBuffer = createCursorBuffer();
    _view = ScrollingBufferView::create(_documentBuffer);
    addSurface(_view);
}

void HtmlHelpSection::renderDocumentIfRequired(const Coordinate contentWidth) {
    const auto normalizedWidth = std::max(contentWidth, 1);
    if (!_documentDirty && _renderedContentWidth == normalizedWidth) {
        return;
    }
    assert(_style != nullptr);
    const auto baseColor = _style->baseTextStyle().color();
    _documentBuffer->resize(Size{normalizedWidth, 1});
    _documentBuffer->setFillChar(Char{U' ', baseColor});
    _documentBuffer->fill(_documentBuffer->rect(), Char{U' ', baseColor});
    _documentBuffer->setColor(baseColor);
    _documentBuffer->clearScreen();
    _documentBuffer->moveHome();
    if (!_html.empty()) {
        text::HtmlRenderer{_html, _style}.renderTo(_documentBuffer);
    }
    _view->setSource(_documentBuffer);
    _renderedContentWidth = normalizedWidth;
    _documentDirty = false;
    flags().setPaintOutdated();
}

auto HtmlHelpSection::createCursorBuffer() -> std::shared_ptr<CursorBuffer> {
    return std::make_shared<CursorBuffer>(
        Size{1, 1}, CursorBuffer::OverflowMode::ExpandThenShift, Size{250, 10'000}, Char{U' ', fg::Default, bg::Black});
}

}
