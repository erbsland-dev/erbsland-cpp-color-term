// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "impl/HashHelper.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <functional>
#include <limits>
#include <numeric>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


namespace erbsland::cterm {


class Position;
using PositionList = std::vector<Position>;


/// Represents a 2D integer position or vector (x, y).
/// - Lightweight value type with default construction to (0,0).
/// - Useful both for coordinates in a grid and for 2D vector arithmetic.
class Position {
public:
    /// Default construct to (0,0).
    Position() = default;
    /// Construct from explicit coordinates.
    /// @param x The x-coordinate.
    /// @param y The y-coordinate.
    constexpr Position(int x, int y) noexcept : _x{x}, _y{y} {}

public: // operators
    /// Equality comparison (component-wise).
    auto operator==(const Position &other) const noexcept -> bool = default;
    /// Inequality comparison (component-wise).
    auto operator!=(const Position &other) const noexcept -> bool = default;
    /// Vector addition (component-wise).
    /// @param other The other position to add.
    /// @return A Position with coordinates (_x + other._x, _y + other._y).
    [[nodiscard]] auto operator+(const Position &other) const noexcept -> Position;
    /// Vector subtraction (component-wise).
    /// @param other The other position to subtract.
    /// @return A Position with coordinates (_x - other._x, _y - other._y).
    [[nodiscard]] auto operator-(const Position &other) const noexcept -> Position;
    /// Add another position to this one in-place.
    /// @param other The other position to add.
    /// @return Reference to this position.
    auto operator+=(const Position &other) noexcept -> Position &;
    /// Subtract another position from this one in-place.
    /// @param other The other position to subtract.
    /// @return Reference to this position.
    auto operator-=(const Position &other) noexcept -> Position &;

public: // attributes
    /// Get the x coordinate.
    [[nodiscard]] constexpr auto x() const noexcept -> int { return _x; }
    /// Set the x coordinate.
    /// @param x New x value.
    void setX(int x) noexcept;
    /// Get the y coordinate.
    [[nodiscard]] constexpr auto y() const noexcept -> int { return _y; }
    /// Set the y coordinate.
    /// @param y New y value.
    void setY(int y) noexcept;

public: // tools
    /// Get a hash for this position.
    /// This hash is designed to be fast and uniform for both 32-bit and 64-bit platforms.
    /// It is not only optimized to be used in a map but also as a source for pseudo-randomness.
    [[nodiscard]] constexpr auto hash() const noexcept -> std::size_t { return impl::hashCreate(_x, _y); }
    /// Manhattan (L1) distance to another position.
    /// @param other The other position.
    /// @return |x - other.x| + |y - other.y|.
    [[nodiscard]] auto distanceTo(Position other) const noexcept -> int;
    /// Component-wise maximum with another position.
    /// @param other The other position.
    /// @return A Position containing the max of each component.
    [[nodiscard]] auto componentMax(Position other) const noexcept -> Position;
    /// Component-wise minimum with another position.
    /// @param other The other position.
    /// @return A Position containing the min of each component.
    [[nodiscard]] auto componentMin(Position other) const noexcept -> Position;
    /// Get the four cardinal positions, relative to this one.
    /// Order: right, down, left, up
    [[nodiscard]] auto cardinalFour() const noexcept -> std::array<Position, 4> {
        return {Position{_x + 1, _y}, Position{_x, _y + 1}, Position{_x - 1, _y}, Position{_x, _y - 1}};
    }
    /// Get the four cardinal position deltas.
    /// Order: right, down, left, up
    [[nodiscard]] static auto cardinalFourDeltas() noexcept -> const std::array<Position, 4> &;
    /// Create a bitmask testing the four cardinal positions.
    /// @param fn The function to test each cardinal delta position, relative to this one.
    template <typename Fn>
        requires std::invocable<Fn, Position> && std::convertible_to<std::invoke_result_t<Fn, Position>, bool>
    [[nodiscard]] auto cardinalFourBitmask(Fn fn) const noexcept -> uint32_t;
    /// Get the eight positions that form a ring around this position.
    /// Clockwise order: 0:E, 1:SE, 2:S, 3:SW, 4:W, 5:NW, 6:N, 7:NE
    /// @return An array with all the eight positions.
    [[nodiscard]] auto ringEight() const noexcept -> std::array<Position, 8U> {
        return {
            Position{_x + 1, _y},
            Position{_x + 1, _y + 1},
            Position{_x, _y + 1},
            Position{_x - 1, _y + 1},
            Position{_x - 1, _y},
            Position{_x - 1, _y - 1},
            Position{_x, _y - 1},
            Position{_x + 1, _y - 1}};
    }
    /// Get the eight deltas that form a ring around this position.
    /// Clockwise order: 0:E, 1:SE, 2:S, 3:SW, 4:W, 5:NW, 6:N, 7:NE
    [[nodiscard]] static auto ringEightDeltas() noexcept -> const std::array<Position, 8U> &;

public: // useful constants
    /// Get the point with the minimum coordinates.
    static auto minimum() noexcept -> Position {
        return Position{std::numeric_limits<int>::min(), std::numeric_limits<int>::min()};
    }
    /// Get the point with the maximum coordinates.
    static auto maximum() noexcept -> Position {
        return Position{std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};
    }

private:
    int _x{};
    int _y{};
};


template <typename Fn>
    requires std::invocable<Fn, Position> && std::convertible_to<std::invoke_result_t<Fn, Position>, bool>
auto Position::cardinalFourBitmask(Fn fn) const noexcept -> uint32_t {
    const auto &deltas = cardinalFourDeltas();
    return std::accumulate(
        deltas.rbegin(), deltas.rend(), 0U, [&](const uint32_t acc, const Position delta) -> uint32_t {
            return (acc << 1) | (fn(*this + delta) ? 1 : 0);
        });
}


}


template <>
struct std::hash<erbsland::cterm::Position> {
    auto operator()(const erbsland::cterm::Position &pos) const noexcept -> std::size_t { return pos.hash(); }
};


template <>
struct std::formatter<erbsland::cterm::Position> : std::formatter<std::string> {
    auto format(const erbsland::cterm::Position &pos, std::format_context &ctx) const {
        return std::formatter<std::string>::format(std::format("{},{}", pos.x(), pos.y()), ctx);
    }
};
