// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Direction.hpp"


#include "impl/TextUtil.hpp"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <tuple>


namespace erbsland::cterm {


namespace {

using DirectionToDeltaEntry = std::tuple<Direction::Enum, Position>;
using DirectionToStringEntry = std::tuple<Direction::Enum, std::string_view>;
using StringToDirectionEntry = std::tuple<std::string_view, Direction>;

constexpr std::array<DirectionToDeltaEntry, 9> cDirectionToDeltaMap = {
    std::make_tuple(Direction::None, Position{0, 0}),
    std::make_tuple(Direction::North, Position{0, -1}),
    std::make_tuple(Direction::NorthEast, Position{1, -1}),
    std::make_tuple(Direction::East, Position{1, 0}),
    std::make_tuple(Direction::SouthEast, Position{1, 1}),
    std::make_tuple(Direction::South, Position{0, 1}),
    std::make_tuple(Direction::SouthWest, Position{-1, 1}),
    std::make_tuple(Direction::West, Position{-1, 0}),
    std::make_tuple(Direction::NorthWest, Position{-1, -1}),
};

constexpr std::array<DirectionToStringEntry, 9> cDirectionToStringMap = {
    std::make_tuple(Direction::None, "none"),
    std::make_tuple(Direction::North, "north"),
    std::make_tuple(Direction::NorthEast, "north_east"),
    std::make_tuple(Direction::East, "east"),
    std::make_tuple(Direction::SouthEast, "south_east"),
    std::make_tuple(Direction::South, "south"),
    std::make_tuple(Direction::SouthWest, "south_west"),
    std::make_tuple(Direction::West, "west"),
    std::make_tuple(Direction::NorthWest, "north_west"),
};

constexpr std::array<StringToDirectionEntry, 22> cStringToDirectionMap = {
    std::make_tuple("", Direction::None),
    std::make_tuple("none", Direction::None),
    std::make_tuple("n", Direction::North),
    std::make_tuple("north", Direction::North),
    std::make_tuple("ne", Direction::NorthEast),
    std::make_tuple("northeast", Direction::NorthEast),
    std::make_tuple("north_east", Direction::NorthEast),
    std::make_tuple("e", Direction::East),
    std::make_tuple("east", Direction::East),
    std::make_tuple("se", Direction::SouthEast),
    std::make_tuple("southeast", Direction::SouthEast),
    std::make_tuple("south_east", Direction::SouthEast),
    std::make_tuple("s", Direction::South),
    std::make_tuple("south", Direction::South),
    std::make_tuple("sw", Direction::SouthWest),
    std::make_tuple("southwest", Direction::SouthWest),
    std::make_tuple("south_west", Direction::SouthWest),
    std::make_tuple("w", Direction::West),
    std::make_tuple("west", Direction::West),
    std::make_tuple("nw", Direction::NorthWest),
    std::make_tuple("northwest", Direction::NorthWest),
    std::make_tuple("north_west", Direction::NorthWest),
};

} // namespace


auto Direction::toDelta() const noexcept -> Position {
    auto it = std::ranges::find_if(cDirectionToDeltaMap, [this](const DirectionToDeltaEntry &entry) -> bool {
        return std::get<0>(entry) == _value;
    });
    if (it == cDirectionToDeltaMap.end()) {
        return {};
    }
    return std::get<1>(*it);
}


auto Direction::toString() const noexcept -> std::string_view {
    auto it = std::ranges::find_if(cDirectionToStringMap, [this](const DirectionToStringEntry &entry) -> bool {
        return std::get<0>(entry) == _value;
    });
    if (it == cDirectionToStringMap.end()) {
        return "none";
    }
    return std::get<1>(*it);
}


auto Direction::fromString(const std::string_view text) -> Direction {
    const auto normalized = impl::toNormalizedIdentifier(text);
    auto it = std::ranges::find_if(cStringToDirectionMap, [&normalized](const StringToDirectionEntry &entry) -> bool {
        return std::get<0>(entry) == normalized;
    });
    if (it == cStringToDirectionMap.end()) {
        throw std::invalid_argument("Invalid direction");
    }
    return std::get<1>(*it);
}


}
