// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Size.hpp"

namespace erbsland::cterm {

auto Size::anchor(const Anchor anchor) const noexcept -> Position {
    Coordinate x = 0;
    Coordinate y = 0;
    switch (anchor & Anchor::HMask) {
    case Anchor::Left:
        x = 0;
        break;
    case Anchor::HCenter:
        x = halfWidthForPosition();
        break;
    case Anchor::Right:
        x = widthForPosition();
        break;
    default:
        break;
    };
    switch (anchor & Anchor::VMask) {
    case Anchor::Top:
        y = 0;
        break;
    case Anchor::VCenter:
        y = halfHeightForPosition();
        break;
    case Anchor::Bottom:
        y = heightForPosition();
        break;
    default:
        break;
    }
    return Position{x, y};
}

auto Size::alignmentOffset(const Size contentSize, const Alignment alignment) const noexcept -> Position {
    return {
        alignment.horizontalOffset(_width, contentSize.width()),
        alignment.verticalOffset(_height, contentSize.height())};
}

}
