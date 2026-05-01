// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Layout.hpp"

namespace erbsland::cterm::ui {

Layout::Layout() noexcept : Surface{cDefaultLayoutSize} {
    childStorage().setManager(*this);
}

void Layout::initializeUi() {
    Surface::initializeUi();
    themeAttributes().setElement(theme::Element::Layout);
}

auto Layout::isOpaque() const noexcept -> bool {
    return false;
}

void Layout::onPaint([[maybe_unused]] WritableBuffer &buffer, [[maybe_unused]] const PaintContext &context) noexcept {
}

}
