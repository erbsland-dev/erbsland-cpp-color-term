// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Rectangle.hpp"

namespace erbsland::cterm {

auto Rectangle::operator|=(const Rectangle &other) noexcept -> Rectangle & {
    auto newPos1 = _pos.componentMin(other._pos);
    auto newPos2 = bottomRight().componentMax(other.bottomRight());
    _pos = newPos1;
    _size = Size{newPos1, newPos2};
    return *this;
}

auto Rectangle::anchor(const Anchor anchor) const noexcept -> Position {
    return _pos + _size.anchor(anchor);
}

auto Rectangle::expandedBy(const Margins margins) const noexcept -> Rectangle {
    return Rectangle{
        _pos.x() - margins.left(),
        _pos.y() - margins.top(),
        _size.width() + margins.left() + margins.right(),
        _size.height() + margins.top() + margins.bottom()};
}

auto Rectangle::insetBy(const Margins margins) const noexcept -> Rectangle {
    return expandedBy(-margins);
}

auto Rectangle::subRectangle(const Anchor anchor, Size size, const Margins margins) const noexcept -> Rectangle {
    const auto innerRect = insetBy(margins);
    size = size.componentMin(innerRect.size());
    if (size.width() == 0) {
        size.setWidth(innerRect.width());
    }
    if (size.height() == 0) {
        size.setHeight(innerRect.height());
    }
    auto topLeftRect = Rectangle(innerRect.topLeft(), size.componentMin(innerRect.size()));
    const auto offset = innerRect.anchor(anchor) - topLeftRect.anchor(anchor);
    return Rectangle{innerRect.topLeft() + offset, size};
}

auto Rectangle::contains(const Position testedPosition) const noexcept -> bool {
    return testedPosition.x() >= _pos.x() && testedPosition.y() >= _pos.y() && testedPosition.x() < x2() &&
        testedPosition.y() < y2();
}

auto Rectangle::isFrame(const Position testedPosition) const noexcept -> bool {
    return contains(testedPosition) &&
        (testedPosition.x() == _pos.x() || testedPosition.y() == _pos.y() || testedPosition.x() == x2() - 1 ||
         testedPosition.y() == y2() - 1);
}

}
