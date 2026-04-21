// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Panel.hpp"

#include "../../theme/ThemePainter.hpp"

namespace erbsland::cterm::ui::surface {

Panel::Panel(ProtectedTag) noexcept :
    Surface{theme::Element::Panel, LayoutMetrics{Size{}, Size::maximum(), Size{}, SizePolicy{SizePolicy::Grow}}} {
}

auto Panel::create() noexcept -> PanelPtr {
    return std::make_shared<Panel>(ProtectedTag{});
}

auto Panel::isOpaque() const noexcept -> bool {
    return true;
}

void Panel::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    theme::ThemePainter{buffer, context.theme().forPart(theme::Part::Background)}.fill(context.surfaceRect());
}

}
