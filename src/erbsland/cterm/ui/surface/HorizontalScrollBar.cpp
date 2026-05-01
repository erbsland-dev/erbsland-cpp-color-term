// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "HorizontalScrollBar.hpp"

namespace erbsland::cterm::ui::surface {

HorizontalScrollBar::HorizontalScrollBar(ProtectedTag protectedTag) noexcept :
    AbstractScrollBar{
        Orientation::Horizontal,
        LayoutMetrics{Size{1, 1}, Size::maximum(), Size{20, 1}, SizePolicy{SizePolicy::Grow}},
        ProtectedTag{}} {
    static_cast<void>(protectedTag);
    editLayoutMetrics().setFixedHeight(1);
}

auto HorizontalScrollBar::create() -> HorizontalScrollBarPtr {
    auto result = std::make_shared<HorizontalScrollBar>(ProtectedTag{});
    result->initializeUi();
    return result;
}

void HorizontalScrollBar::initializeUi() {
    AbstractScrollBar::initializeUi();
    themeAttributes().setElement(theme::Element::HorizontalScrollBar);
}

}
