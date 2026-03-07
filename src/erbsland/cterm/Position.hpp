// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <numeric>
#include <string>
#include <string_view>
#include <utility>


namespace erbsland::cterm {


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

public: // accessors
    /// Get the x coordinate.
    [[nodiscard]] constexpr auto x() const noexcept -> int { return _x; }
    /// Get the y coordinate.
    [[nodiscard]] constexpr auto y() const noexcept -> int { return _y; }

public: // modifiers
    /// Set the x coordinate.
    /// @param x New x value.
    void setX(int x) noexcept;
    /// Set the y coordinate.
    /// @param y New y value.
    void setY(int y) noexcept;
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
    /// Get the four cardinal position deltas.
    /// Order: right, down, left, up
    [[nodiscard]] static auto cardinalFourDeltas() noexcept -> const std::array<Position, 4> &;
    /// Create a bitmask testing the four cardinal positions.
    /// @param fn The function to test each cardinal delta position, relative to this one.
    template <typename Fn>
        requires std::invocable<Fn, Position> && std::convertible_to<std::invoke_result_t<Fn, Position>, bool>
    [[nodiscard]] auto cardinalFourBitmask(Fn fn) const noexcept -> uint32_t;

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
struct std::formatter<erbsland::cterm::Position> : std::formatter<std::string> {
    auto format(const erbsland::cterm::Position &pos, std::format_context &ctx) const {
        return std::formatter<std::string>::format(std::format("{},{}", pos.x(), pos.y()), ctx);
    }
};
