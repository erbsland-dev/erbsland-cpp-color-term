// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "HtmlDocumentPanel.hpp"

#include <erbsland/cterm/text/HtmlRenderer.hpp>

namespace demo::ui_html_viewer {

HtmlDocumentPanel::HtmlDocumentPanel(ProtectedTag protectedTag) noexcept : Panel{protectedTag} {
}

void HtmlDocumentPanel::initializeUi() {
    addSurface(_view);
}

void HtmlDocumentPanel::setHtml(std::string html) {
    _html = std::move(html);
    _documentDirty = true;
    renderDocumentIfRequired(_view->rectangle().width());
}

void HtmlDocumentPanel::setStyle(text::StyleConstPtr style) noexcept {
    _style = std::move(style);
    _documentDirty = true;
    renderDocumentIfRequired(_view->rectangle().width());
}

auto HtmlDocumentPanel::contentSize() const noexcept -> Size {
    return _documentBuffer->size();
}

auto HtmlDocumentPanel::view() const noexcept -> const ui::ScrollingBufferViewPtr & {
    return _view;
}

void HtmlDocumentPanel::onLayout(ui::LayoutScope &scope) noexcept {
    scope.place(_view, Rectangle{Position{}, scope.size()});
    renderDocumentIfRequired(scope.size().width());
}

void HtmlDocumentPanel::renderDocumentIfRequired(const Coordinate contentWidth) {
    const auto normalizedWidth = std::max(contentWidth, 1);
    if (!_documentDirty && _renderedContentWidth == normalizedWidth) {
        return;
    }
    const auto baseColor = _style == nullptr ? Color{fg::Default, bg::Black} : _style->baseTextStyle().color();
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

}
