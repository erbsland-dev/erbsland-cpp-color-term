// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ScrollCorner.hpp"

#include "../../theme/ThemePainter.hpp"

namespace erbsland::cterm::ui::surface {

ScrollCorner::ScrollCorner(ProtectedTag) noexcept :
    Surface{LayoutMetrics{Size{1, 1}, Size{1, 1}, Size{1, 1}, SizePolicy{SizePolicy::Preferred}}} {
}

auto ScrollCorner::create() -> ScrollCornerPtr {
    auto result = std::make_shared<ScrollCorner>(ProtectedTag{});
    result->initializeUi();
    return result;
}

void ScrollCorner::initializeUi() {
    Surface::initializeUi();
    themeAttributes().setElement(theme::Element::ScrollCorner);
}

auto ScrollCorner::fill() const noexcept -> const Char & {
    return _fill;
}

void ScrollCorner::setFill(Char fill) noexcept {
    if (_fill == fill) {
        return;
    }
    _fill = fill;
    _fillOverride = true;
    flags().setPaintOutdated();
}

auto ScrollCorner::isOpaque() const noexcept -> bool {
    return true;
}

void ScrollCorner::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    if (_fillOverride) {
        buffer.fill(context.surfaceRect(), _fill);
        return;
    }
    theme::ThemePainter{buffer, context.theme().forPart(theme::Part::Background)}.fill(context.surfaceRect());
}

}
