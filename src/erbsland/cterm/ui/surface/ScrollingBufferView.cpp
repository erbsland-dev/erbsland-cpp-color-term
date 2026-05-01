// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ScrollingBufferView.hpp"

namespace erbsland::cterm::ui::surface {

ScrollingBufferView::ScrollingBufferView(ReadableBufferPtr source, ProtectedTag) noexcept :
    AbstractScrollArea{ProtectedTag{}},
    _source{std::move(source)},
    _view{_source, Rectangle{Position{0, 0}, Size{1, 1}}} {
}

auto ScrollingBufferView::create(ReadableBufferPtr source) -> ScrollingBufferViewPtr {
    auto view = std::make_shared<ScrollingBufferView>(std::move(source), ProtectedTag{});
    view->initializeUi();
    return view;
}

auto ScrollingBufferView::source() const noexcept -> const ReadableBufferPtr & {
    return _source;
}

void ScrollingBufferView::setSource(ReadableBufferPtr source) noexcept {
    _source = std::move(source);
    _view.setContent(_source);
    flags().setLayoutOutdated();
}

auto ScrollingBufferView::showCropCharacters() const noexcept -> bool {
    return _view.showCropCharacters();
}

void ScrollingBufferView::setShowCropCharacters(const bool show) noexcept {
    if (_view.showCropCharacters() == show) {
        return;
    }
    _view.setShowCropCharacters(show);
    flags().setPaintOutdated();
}

auto ScrollingBufferView::cropCharacter(const Direction direction) const noexcept -> Char {
    return _view.cropCharacter(direction);
}

void ScrollingBufferView::setCropCharacter(const Direction direction, const Char character) noexcept {
    _view.setCropCharacter(direction, character);
    flags().setPaintOutdated();
}

auto ScrollingBufferView::contentSizeForViewport([[maybe_unused]] const Size viewportSize) const noexcept -> Size {
    if (_source == nullptr) {
        return {};
    }
    return _source->size();
}

auto ScrollingBufferView::measureContentSizeForViewport(
    const Size viewportSize, [[maybe_unused]] LayoutScope &scope) noexcept -> Size {
    return contentSizeForViewport(viewportSize);
}

void ScrollingBufferView::onPaintArea(
    const Position scrollOffset,
    const Rectangle targetRect,
    WritableBuffer &buffer,
    [[maybe_unused]] const PaintContext &context) noexcept {
    _view.setViewRect(Rectangle{scrollOffset, targetRect.size()});
    if (_source != nullptr) {
        buffer.drawBuffer(_view, targetRect);
    }
}

}
