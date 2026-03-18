// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Rectangle.hpp"

#include <stdexcept>


namespace erbsland::cterm {

auto Rectangle::operator|(const Rectangle &other) const noexcept -> Rectangle {
    return Rectangle{topLeft().componentMin(other.topLeft()), bottomRight().componentMax(other.bottomRight())};
}

auto Rectangle::operator|=(const Rectangle &other) noexcept -> Rectangle & {
    *this = *this | other;
    return *this;
}

auto Rectangle::operator&(const Rectangle &other) const noexcept -> Rectangle {
    if (!overlaps(other)) {
        return Rectangle{};
    }
    const auto newTopLeft = topLeft().componentMax(other.topLeft());
    const auto newBottomRight = bottomRight().componentMin(other.bottomRight());
    return Rectangle{newTopLeft, newBottomRight};
}

auto Rectangle::operator&=(const Rectangle &other) noexcept -> Rectangle & {
    *this = *this & other;
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

auto Rectangle::contains(const Rectangle testedRectangle) const noexcept -> bool {
    return testedRectangle.x1() >= x1() && testedRectangle.x2() <= x2() && testedRectangle.y1() >= y1() &&
        testedRectangle.y2() <= y2();
}

auto Rectangle::overlaps(const Rectangle testedRectangle) const noexcept -> bool {
    return x1() < testedRectangle.x2() && x2() > testedRectangle.x1() && y1() < testedRectangle.y2() &&
        y2() > testedRectangle.y1();
}

auto Rectangle::isFrame(const Position testedPosition) const noexcept -> bool {
    return contains(testedPosition) &&
        (testedPosition.x() == _pos.x() || testedPosition.y() == _pos.y() || testedPosition.x() == x2() - 1 ||
         testedPosition.y() == y2() - 1);
}

auto Rectangle::clamp(Position position) const noexcept -> Position {
    return Position{std::clamp(position.x(), x1(), x2() - 1), std::clamp(position.y(), y1(), y2() - 1)};
}

auto Rectangle::frameDirection(const Position testedPosition) const noexcept -> Direction {
    if (!isFrame(testedPosition)) {
        return Direction::None;
    }
    // We can't reliably return a frame direction for 1x1 or degenerate rectangles.
    if (width() < 2 || height() < 2) {
        return Direction::None;
    }

    const bool north = testedPosition.y() == y1();
    const bool east = testedPosition.x() == x2() - 1;
    const bool south = testedPosition.y() == y2() - 1;
    const bool west = testedPosition.x() == x1();

    if (north && west) {
        return Direction::NorthWest;
    }
    if (north && east) {
        return Direction::NorthEast;
    }
    if (south && east) {
        return Direction::SouthEast;
    }
    if (south && west) {
        return Direction::SouthWest;
    }
    if (north) {
        return Direction::North;
    }
    if (east) {
        return Direction::East;
    }
    if (south) {
        return Direction::South;
    }
    return Direction::West;
}

auto Rectangle::frameIndex(const Position testedPosition) const noexcept -> int64_t {
    if (!isFrame(testedPosition)) {
        return -1;
    }
    if (width() <= 1) {
        return testedPosition.y() - y1();
    }
    if (height() <= 1) {
        return testedPosition.x() - x1();
    }
    if (testedPosition.y() == y1()) {
        return testedPosition.x() - x1();
    }
    if (testedPosition.x() == x2() - 1) {
        return (width() - 1) + (testedPosition.y() - y1());
    }
    if (testedPosition.y() == y2() - 1) {
        return (width() - 1) + (height() - 1) + (x2() - 1 - testedPosition.x());
    }
    return static_cast<int64_t>(2 * (width() - 1) + (height() - 1) + (y2() - 1 - testedPosition.y()));
}

auto Rectangle::gridCells(const int rows, const int columns, const int horizontalSpacing, const int verticalSpacing)
    const -> std::vector<Rectangle> {
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

auto Rectangle::bounds(const PositionList &positions) noexcept -> Rectangle {
    if (positions.empty()) {
        return Rectangle{};
    }
    auto topLeft = Position::maximum();
    auto bottomRight = Position::minimum();
    for (const auto &position : positions) {
        topLeft = topLeft.componentMin(position);
        bottomRight = bottomRight.componentMax(position);
    }
    return Rectangle{topLeft, bottomRight + Position{1, 1}};
}

}
