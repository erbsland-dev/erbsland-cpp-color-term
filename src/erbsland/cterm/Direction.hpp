// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Position.hpp"

#include <cstdint>
#include <functional>
#include <string_view>

namespace erbsland::cterm {

/// A direction in a 2D grid.
class Direction final {
public:
    /// The enum for the direction.
    enum Enum : uint8_t {
        None = 0,   ///< No direction.
        North,      ///< North.
        NorthEast,  ///< North-east.
        East,       ///< East.
        SouthEast,  ///< South-east.
        South,      ///< South.
        SouthWest,  ///< South-west.
        West,       ///< West.
        NorthWest,  ///< North-west.

        _EnumCount, ///< The number of directions enums.
    };

    /// The number of directions enums.
    constexpr static auto cCount = static_cast<std::size_t>(_EnumCount);

public:
    /// Create a direction with value `None`.
    constexpr Direction() noexcept = default;

    /// Create a direction from an enum value.
    constexpr Direction(const Enum value) noexcept : _value{value} {} // NOLINT(*-explicit-constructor)

    /// Default destructor.
    ~Direction() = default;
    /// Default copy constructor.
    Direction(const Direction &) = default;
    /// Default copy assignment.
    auto operator=(const Direction &) -> Direction & = default;

public: // operators
    /// Assign an enum value.
    auto operator=(const Enum value) noexcept -> Direction & {
        _value = value;
        return *this;
    }
    /// Convert to the enum value.
    constexpr operator Enum() const noexcept { return _value; } // NOLINT(*-explicit-constructor)
    /// Compare two directions.
    constexpr auto operator==(const Direction &) const noexcept -> bool = default;
    /// Compare with an enum value.
    [[nodiscard]] constexpr auto operator==(const Enum value) const noexcept -> bool { return _value == value; }
    /// Compare an enum value with a direction.
    [[nodiscard]] friend constexpr auto operator==(const Enum value, const Direction &direction) noexcept -> bool {
        return direction == value;
    }

public: // tests
    /// Test if this direction contains (lexically) another direction.
    /// Examples:
    /// - NW contains N
    /// - NW contains W
    /// - NW contains NW
    /// - NW does not contain S
    /// - NW does not contain SW (they just overlap).
    [[nodiscard]] auto contains(Direction direction) const noexcept -> bool;

public: // conversion
    /// Get a hash for this direction.
    [[nodiscard]] constexpr auto hash() const noexcept -> std::size_t {
        return impl::hashCreate(static_cast<uint8_t>(_value));
    }
    /// Convert this direction into a position delta.
    /// @return The unit delta for this direction, or `(0,0)` for `None`.
    [[nodiscard]] auto toDelta() const noexcept -> Position;

    /// Convert a position delta into a direction.
    /// Only tests the signs of the x and y value in the given position.
    /// @param delta The position delta to convert.
    /// @return The direction for the given delta, or `None` if the delta is zero.
    [[nodiscard]] static auto fromDelta(Position delta) noexcept -> Direction;

    /// Convert this direction into a canonical lowercase string.
    /// @return The normalized direction name.
    [[nodiscard]] auto toString() const noexcept -> std::string_view;

    /// Parse a direction from text.
    /// Accepts empty text, abbreviations and normalized names.
    /// @param text The direction text.
    /// @return The parsed direction.
    [[nodiscard]] static auto fromString(std::string_view text) -> Direction;

private:
    using DirectionToDeltaEntry = std::tuple<Enum, Position>;
    using DirectionToDeltaMap = std::array<DirectionToDeltaEntry, static_cast<std::size_t>(_EnumCount)>;
    using DirectionToStringEntry = std::tuple<Enum, std::string_view>;
    using DirectionToStringMap = std::array<DirectionToStringEntry, static_cast<std::size_t>(_EnumCount)>;
    using StringToDirectionEntry = std::tuple<std::string_view, Direction>;
    using StringToDirectionMap = std::array<StringToDirectionEntry, 22>;

    [[nodiscard]] static auto directionToDeltaMap() noexcept -> const DirectionToDeltaMap &;
    [[nodiscard]] static auto directionToStringMap() noexcept -> const DirectionToStringMap &;
    [[nodiscard]] static auto stringToDirectionMap() noexcept -> const StringToDirectionMap &;

private:
    Enum _value{None}; ///< The internal enum value.
};

}

template <>
struct std::hash<erbsland::cterm::Direction> {
    auto operator()(const erbsland::cterm::Direction &direction) const noexcept -> std::size_t {
        return direction.hash();
    }
};
