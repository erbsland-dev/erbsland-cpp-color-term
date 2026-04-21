// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Direction.hpp"

#include "../impl/TextUtil.hpp"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <tuple>

namespace erbsland::cterm {

auto Direction::contains(Direction direction) const noexcept -> bool {
    switch (_value) {
    case North:
        return direction == North;
    case NorthEast:
        return direction == NorthEast || direction == North || direction == East;
    case East:
        return direction == East;
    case SouthEast:
        return direction == SouthEast || direction == South || direction == East;
    case South:
        return direction == South;
    case SouthWest:
        return direction == SouthWest || direction == South || direction == West;
    case West:
        return direction == West;
    case NorthWest:
        return direction == NorthWest || direction == North || direction == West;
    default:
        return false;
    }
}

auto Direction::toDelta() const noexcept -> Position {
    auto it = std::ranges::find_if(directionToDeltaMap(), [this](const DirectionToDeltaEntry &entry) -> bool {
        return std::get<0>(entry) == _value;
    });
    if (it == directionToDeltaMap().end()) {
        return {};
    }
    return std::get<1>(*it);
}

auto Direction::fromDelta(const Position delta) noexcept -> Direction {
    const auto mask = (delta.x() > 0 ? 0b0001 : 0) | (delta.x() < 0 ? 0b0010 : 0) | (delta.y() > 0 ? 0b0100 : 0) |
        (delta.y() < 0 ? 0b1000 : 0);
    switch (mask) {
    case 0b0001:
        return East;
    case 0b0010:
        return West;
    case 0b0100:
        return South;
    case 0b0101:
        return SouthEast;
    case 0b0110:
        return SouthWest;
    case 0b1000:
        return North;
    case 0b1001:
        return NorthEast;
    case 0b1010:
        return NorthWest;
    default:
        return None;
    }
}

auto Direction::toString() const noexcept -> std::string_view {
    auto it = std::ranges::find_if(directionToStringMap(), [this](const DirectionToStringEntry &entry) -> bool {
        return std::get<0>(entry) == _value;
    });
    if (it == directionToStringMap().end()) {
        return "none";
    }
    return std::get<1>(*it);
}

auto Direction::fromString(const std::string_view text) -> Direction {
    const auto normalized = impl::toNormalizedIdentifier(text);
    auto it = std::ranges::find_if(stringToDirectionMap(), [&normalized](const StringToDirectionEntry &entry) -> bool {
        return std::get<0>(entry) == normalized;
    });
    if (it == stringToDirectionMap().end()) {
        throw std::invalid_argument("Invalid direction");
    }
    return std::get<1>(*it);
}

auto Direction::directionToDeltaMap() noexcept -> const DirectionToDeltaMap & {
    static const std::array<DirectionToDeltaEntry, _EnumCount> map = {
        std::make_tuple(None, Position{0, 0}),
        std::make_tuple(North, Position{0, -1}),
        std::make_tuple(NorthEast, Position{1, -1}),
        std::make_tuple(East, Position{1, 0}),
        std::make_tuple(SouthEast, Position{1, 1}),
        std::make_tuple(South, Position{0, 1}),
        std::make_tuple(SouthWest, Position{-1, 1}),
        std::make_tuple(West, Position{-1, 0}),
        std::make_tuple(NorthWest, Position{-1, -1}),
    };
    return map;
}

auto Direction::directionToStringMap() noexcept -> const DirectionToStringMap & {
    static const std::array<DirectionToStringEntry, _EnumCount> map = {
        std::make_tuple(None, "none"),
        std::make_tuple(North, "north"),
        std::make_tuple(NorthEast, "north_east"),
        std::make_tuple(East, "east"),
        std::make_tuple(SouthEast, "south_east"),
        std::make_tuple(South, "south"),
        std::make_tuple(SouthWest, "south_west"),
        std::make_tuple(West, "west"),
        std::make_tuple(NorthWest, "north_west"),
    };
    return map;
}

auto Direction::stringToDirectionMap() noexcept -> const StringToDirectionMap & {
    static const std::array<StringToDirectionEntry, 22> map = {
        std::make_tuple("", None),
        std::make_tuple("none", None),
        std::make_tuple("n", North),
        std::make_tuple("north", North),
        std::make_tuple("ne", NorthEast),
        std::make_tuple("northeast", NorthEast),
        std::make_tuple("north_east", NorthEast),
        std::make_tuple("e", East),
        std::make_tuple("east", East),
        std::make_tuple("se", SouthEast),
        std::make_tuple("southeast", SouthEast),
        std::make_tuple("south_east", SouthEast),
        std::make_tuple("s", South),
        std::make_tuple("south", South),
        std::make_tuple("sw", SouthWest),
        std::make_tuple("southwest", SouthWest),
        std::make_tuple("south_west", SouthWest),
        std::make_tuple("w", West),
        std::make_tuple("west", West),
        std::make_tuple("nw", NorthWest),
        std::make_tuple("northwest", NorthWest),
        std::make_tuple("north_west", NorthWest),
    };
    return map;
}

}
