// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Panel.hpp"

namespace erbsland::cterm::ui::surface {

Panel::Panel(ProtectedTag) noexcept : Surface{Geometry{Size{}, Size::maximum(), Size{}, SizePolicy{SizePolicy::Grow}}} {
}

auto Panel::create() noexcept -> PanelPtr {
    return std::make_shared<Panel>(ProtectedTag{});
}

void Panel::setBackground(Char background) noexcept {
    _background = background;
    setPaintOutdated();
}

void Panel::clearBackground() noexcept {
    _background = std::nullopt;
    setPaintOutdated();
}

auto Panel::background() const noexcept -> std::optional<Char> {
    return _background;
}

auto Panel::isOpaque() const noexcept -> bool {
    return _background.has_value();
}

void Panel::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    if (_background.has_value()) {
        buffer.fill(context.targetRect(), *_background);
    }
    Surface::onPaint(buffer, context);
}

}
