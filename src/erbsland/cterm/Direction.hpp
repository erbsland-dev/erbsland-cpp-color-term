// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Position.hpp"

#include <cstdint>
#include <string_view>


namespace erbsland::cterm {


/// A direction in a 2D grid.
class Direction final {
public: // ctors/dtor/assign/move
    /// The enum for the direction.
    enum Enum : uint8_t {
        None = 0,  ///< No direction.
        North,     ///< North.
        NorthEast, ///< North-east.
        East,      ///< East.
        SouthEast, ///< South-east.
        South,     ///< South.
        SouthWest, ///< South-west.
        West,      ///< West.
        NorthWest, ///< North-west.
    };

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

public: // conversion
    /// Convert this direction into a position delta.
    /// @return The unit delta for this direction, or `(0,0)` for `None`.
    [[nodiscard]] auto toDelta() const noexcept -> Position;

    /// Convert this direction into a canonical lowercase string.
    /// @return The normalized direction name.
    [[nodiscard]] auto toString() const noexcept -> std::string_view;

    /// Parse a direction from text.
    /// Accepts empty text, abbreviations and normalized names.
    /// @param text The direction text.
    /// @return The parsed direction.
    [[nodiscard]] static auto fromString(std::string_view text) -> Direction;

private:
    Enum _value{None}; ///< The internal enum value.
};


}
