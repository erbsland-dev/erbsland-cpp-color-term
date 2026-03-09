// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Rectangle.hpp"


#include <stdexcept>


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

auto Rectangle::gridCells(
    const int rows, const int columns, const int horizontalSpacing, const int verticalSpacing) const
    -> std::vector<Rectangle> {
    if (rows < 1) {
        throw std::invalid_argument{"Rectangle::gridCells() requires at least one row."};
    }
    if (columns < 1) {
        throw std::invalid_argument{"Rectangle::gridCells() requires at least one column."};
    }
    if (horizontalSpacing < 0 || verticalSpacing < 0) {
        throw std::invalid_argument{"Rectangle::gridCells() spacing must not be negative."};
    }
    const auto usableWidth = width() - (columns - 1) * horizontalSpacing;
    const auto usableHeight = height() - (rows - 1) * verticalSpacing;
    if (usableWidth < columns || usableHeight < rows) {
        throw std::invalid_argument{"Rectangle::gridCells() cannot create cells of at least 1x1."};
    }
    const auto baseCellWidth = usableWidth / columns;
    const auto extraWidthCells = usableWidth % columns;
    const auto baseCellHeight = usableHeight / rows;
    const auto extraHeightCells = usableHeight % rows;
    auto result = std::vector<Rectangle>{};
    result.reserve(static_cast<std::size_t>(rows) * static_cast<std::size_t>(columns));
    auto y = y1();
    for (int row = 0; row < rows; ++row) {
        const auto cellHeight = baseCellHeight + (row < extraHeightCells ? 1 : 0);
        auto x = x1();
        for (int column = 0; column < columns; ++column) {
            const auto cellWidth = baseCellWidth + (column < extraWidthCells ? 1 : 0);
            result.emplace_back(x, y, cellWidth, cellHeight);
            x += cellWidth + horizontalSpacing;
        }
        y += cellHeight + verticalSpacing;
    }
    return result;
}

}
