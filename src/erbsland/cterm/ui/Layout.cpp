// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Layout.hpp"

namespace erbsland::cterm::ui {

Layout::Layout() noexcept : Surface{cDefaultGeometry} {
}

void Layout::setBackground(Char background) noexcept {
    _background = background;
    setPaintOutdated();
}

void Layout::clearBackground() noexcept {
    _background = std::nullopt;
    setPaintOutdated();
}

auto Layout::background() const noexcept -> std::optional<Char> {
    return _background;
}

auto Layout::isOpaque() const noexcept -> bool {
    return _background.has_value();
}

void Layout::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    if (_background.has_value()) {
        buffer.fill(context.targetRect(), *_background);
    }
    Surface::onPaint(buffer, context);
}

}
