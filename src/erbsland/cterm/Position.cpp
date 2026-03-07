// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Position.hpp"


namespace erbsland::cterm {


auto Position::operator+(const Position &other) const noexcept -> Position {
    return {_x + other._x, _y + other._y};
}

auto Position::operator-(const Position &other) const noexcept -> Position {
    return {_x - other._x, _y - other._y};
}

auto Position::operator+=(const Position &other) noexcept -> Position & {
    _x += other._x;
    _y += other._y;
    return *this;
}

auto Position::operator-=(const Position &other) noexcept -> Position & {
    _x -= other._x;
    _y -= other._y;
    return *this;
}

void Position::setX(int x) noexcept {
    _x = x;
}

void Position::setY(int y) noexcept {
    _y = y;
}

auto Position::distanceTo(Position other) const noexcept -> int {
    return std::abs(_x - other._x) + std::abs(_y - other._y);
}

auto Position::componentMax(Position other) const noexcept -> Position {
    return {std::max(_x, other._x), std::max(_y, other._y)};
}

auto Position::componentMin(Position other) const noexcept -> Position {
    return {std::min(_x, other._x), std::min(_y, other._y)};
}

auto Position::cardinalFourDeltas() noexcept -> const std::array<Position, 4> & {
    static std::array<Position, 4> deltas = {{{1, 0}, {0, 1}, {-1, 0}, {0, -1}}};
    return deltas;
}


}
