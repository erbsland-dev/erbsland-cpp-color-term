// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ScrollingBufferView.hpp"

#include <algorithm>

namespace erbsland::cterm::ui::surface {

ScrollingBufferView::ScrollingBufferView(ReadableBufferPtr source, ProtectedTag) noexcept :
    Surface{Geometry{Size{}, Size::maximum(), Size{}, SizePolicy{SizePolicy::Grow}}},
    _source{std::move(source)},
    _view{_source, Rectangle{Position{0, 0}, Size{1, 1}}} {
}

auto ScrollingBufferView::create(ReadableBufferPtr source) noexcept -> ScrollingBufferViewPtr {
    return std::make_shared<ScrollingBufferView>(std::move(source), ProtectedTag{});
}

auto ScrollingBufferView::source() const noexcept -> const ReadableBufferPtr & {
    return _source;
}

void ScrollingBufferView::setSource(ReadableBufferPtr source) noexcept {
    _source = std::move(source);
    _view.setContent(_source);
    _viewOffset = clampedViewOffset(_viewOffset, rectangle().size());
    setPaintOutdated();
}

auto ScrollingBufferView::viewOffset() const noexcept -> Position {
    return _viewOffset;
}

void ScrollingBufferView::setViewOffset(const Position offset) noexcept {
    const auto clamped = clampedViewOffset(offset, rectangle().size());
    if (_viewOffset == clamped) {
        return;
    }
    _viewOffset = clamped;
    setPaintOutdated();
}

auto ScrollingBufferView::viewRect() const noexcept -> Rectangle {
    return Rectangle{_viewOffset, rectangle().size()};
}

auto ScrollingBufferView::showCropCharacters() const noexcept -> bool {
    return _view.showCropCharacters();
}

void ScrollingBufferView::setShowCropCharacters(const bool show) noexcept {
    if (_view.showCropCharacters() == show) {
        return;
    }
    _view.setShowCropCharacters(show);
    setPaintOutdated();
}

auto ScrollingBufferView::cropCharacter(const Direction direction) const noexcept -> Char {
    return _view.cropCharacter(direction);
}

void ScrollingBufferView::setCropCharacter(const Direction direction, const Char character) noexcept {
    _view.setCropCharacter(direction, character);
    setPaintOutdated();
}

void ScrollingBufferView::scrollUp(const Coordinate count) noexcept {
    setViewOffset(_viewOffset + Position{0, -std::max(count, 0)});
}

void ScrollingBufferView::scrollDown(const Coordinate count) noexcept {
    setViewOffset(_viewOffset + Position{0, std::max(count, 0)});
}

void ScrollingBufferView::scrollLeft(const Coordinate count) noexcept {
    setViewOffset(_viewOffset + Position{-std::max(count, 0), 0});
}

void ScrollingBufferView::scrollRight(const Coordinate count) noexcept {
    setViewOffset(_viewOffset + Position{std::max(count, 0), 0});
}

void ScrollingBufferView::pageUp() noexcept {
    scrollUp(std::max(rectangle().height(), 1));
}

void ScrollingBufferView::pageDown() noexcept {
    scrollDown(std::max(rectangle().height(), 1));
}

void ScrollingBufferView::pageLeft() noexcept {
    scrollLeft(std::max(rectangle().width(), 1));
}

void ScrollingBufferView::pageRight() noexcept {
    scrollRight(std::max(rectangle().width(), 1));
}

void ScrollingBufferView::scrollToTop() noexcept {
    setViewOffset(Position{_viewOffset.x(), 0});
}

void ScrollingBufferView::scrollToBottom() noexcept {
    setViewOffset(Position{_viewOffset.x(), Position::maximum().y()});
}

void ScrollingBufferView::scrollToLeftEdge() noexcept {
    setViewOffset(Position{0, _viewOffset.y()});
}

void ScrollingBufferView::scrollToRightEdge() noexcept {
    setViewOffset(Position{Position::maximum().x(), _viewOffset.y()});
}

void ScrollingBufferView::onLayout(const Size newParentSize) noexcept {
    _viewOffset = clampedViewOffset(_viewOffset, newParentSize);
    Surface::onLayout(newParentSize);
}

void ScrollingBufferView::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    updateView(context.targetRect().size());
    if (_source != nullptr) {
        buffer.drawBuffer(_view, context.targetRect());
    }
    Surface::onPaint(buffer, context);
}

auto ScrollingBufferView::clampedViewOffset(const Position offset, const Size viewSize) const noexcept -> Position {
    if (_source == nullptr) {
        return Position{0, 0};
    }
    const auto contentSize = _source->size();
    const auto maximumX = std::max(contentSize.width() - std::max(viewSize.width(), 1), 0);
    const auto maximumY = std::max(contentSize.height() - std::max(viewSize.height(), 1), 0);
    return Position{
        std::clamp(offset.x(), 0, maximumX),
        std::clamp(offset.y(), 0, maximumY),
    };
}

void ScrollingBufferView::updateView(const Size viewSize) noexcept {
    _viewOffset = clampedViewOffset(_viewOffset, viewSize);
    _view.setViewRect(Rectangle{_viewOffset, viewSize});
}

}
