// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Layout.hpp"

namespace erbsland::cterm::ui {

Layout::Layout() noexcept : Surface{theme::Element::Layout, cDefaultLayoutSize} {
    childStorage().setManager(*this);
}

auto Layout::isOpaque() const noexcept -> bool {
    return false;
}

void Layout::onPaint([[maybe_unused]] WritableBuffer &buffer, [[maybe_unused]] const PaintContext &context) noexcept {
}

}
