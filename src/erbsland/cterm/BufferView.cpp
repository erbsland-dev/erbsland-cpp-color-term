// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "BufferView.hpp"

#include "Buffer.hpp"

namespace erbsland::cterm {

auto BufferViewBase::size() const noexcept -> Size {
    return _viewRect.size();
}

auto BufferViewBase::rect() const noexcept -> Rectangle {
    return Rectangle{Position{0, 0}, size()};
}

auto BufferViewBase::clone() const -> WritableBufferPtr {
    auto buffer = std::make_shared<Buffer>(size());
    size().forEach([&](const Position pos) -> void { buffer->set(pos, get(pos)); });
    return buffer;
}

auto BufferView::content() const noexcept -> const ReadableBufferPtr & {
    return _content;
}

void BufferView::setContent(ReadableBufferPtr buffer) noexcept {
    _content = buffer;
}

auto BufferViewBase::viewRect() const noexcept -> const Rectangle & {
    return _viewRect;
}

void BufferViewBase::setViewRect(Rectangle rect) noexcept {
    _viewRect = rect;
}

auto BufferViewBase::showCropCharacters() const noexcept -> bool {
    return _showCropCharacters;
}

void BufferViewBase::setShowCropCharacters(bool show) noexcept {
    _showCropCharacters = show;
}

auto BufferViewBase::cropCharacter(const Direction direction) const noexcept -> Char {
    if (static_cast<std::size_t>(direction) >= _cropCharacters.size()) {
        return Char{};
    }
    return _cropCharacters[direction];
}

void BufferViewBase::setCropCharacter(const Direction direction, const Char character) noexcept {
    if (static_cast<std::size_t>(direction) >= _cropCharacters.size()) {
        return;
    }
    _cropCharacters[direction] = character;
}

}
