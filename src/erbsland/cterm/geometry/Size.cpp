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
    const auto axisOffset = [](const Coordinate availableSize,
                               const Coordinate renderedSize,
                               const Alignment axisAlignment) noexcept -> Coordinate {
        const auto freeSpace = availableSize - renderedSize;
        switch (axisAlignment) {
        case Alignment::HCenter:
        case Alignment::VCenter:
            return freeSpace / 2;
        case Alignment::Right:
        case Alignment::Bottom:
            return freeSpace;
        case Alignment::Left:
        case Alignment::Top:
        default:
            return 0;
        }
    };
    return {
        axisOffset(_width, contentSize.width(), alignment & Alignment::HorizontalMask),
        axisOffset(_height, contentSize.height(), alignment & Alignment::VerticalMask)};
}

}
