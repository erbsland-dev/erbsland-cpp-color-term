// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "VerticalScrollBar.hpp"

namespace erbsland::cterm::ui::surface {

VerticalScrollBar::VerticalScrollBar(ProtectedTag protectedTag) noexcept :
    AbstractScrollBar{
        Orientation::Vertical,
        LayoutMetrics{
            Size{1, 1},
            Size::maximum(),
            Size{1, 20},
            SizePolicy{DimensionPolicy{DimensionPolicy::Preferred}, DimensionPolicy{DimensionPolicy::Grow}}},
        protectedTag} {
    editLayoutMetrics().setFixedWidth(1);
}

auto VerticalScrollBar::create() -> VerticalScrollBarPtr {
    auto result = std::make_shared<VerticalScrollBar>(ProtectedTag{});
    result->initializeUi();
    return result;
}

void VerticalScrollBar::initializeUi() {
    AbstractScrollBar::initializeUi();
    themeAttributes().setElement(theme::Element::VerticalScrollBar);
}

}
