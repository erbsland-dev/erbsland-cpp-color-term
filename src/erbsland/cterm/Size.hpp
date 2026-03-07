// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#pragma once


#include "Anchor.hpp"
#include "Position.hpp"

#include <algorithm>
#include <format>


namespace erbsland::cterm {


/// A non-negative 2D size (width × height).
/// - Width and height are clamped to be >= 0.
/// - Many operations assume a grid indexed from (0,0) to (width-1,height-1).
class Size {
public: // ctors/dtor/assign/move
    /// Construct a zero size (0 × 0).
    Size() = default;

    /// Construct a size from explicit width and height.
    /// Negative inputs are clamped to 0.
    /// @param width The desired width (clamped to >= 0).
    /// @param height The desired height (clamped to >= 0).
    constexpr Size(int width, int height) noexcept : _width{std::max(0, width)}, _height{std::max(0, height)} {}

    /// Construct a size from the axis-aligned distance between two positions.
    /// @param pos1 First position.
    /// @param pos2 Second position.
    /// @note The order of the positions does not matter; absolute differences are used.
    constexpr Size(Position pos1, Position pos2) noexcept :
        _width{absoluteDifference(pos1.x(), pos2.x())}, _height{absoluteDifference(pos1.y(), pos2.y())} {}

public: // operators
    /// Equality comparison on width and height.
    auto operator==(const Size &other) const noexcept -> bool = default;

    /// Inequality comparison on width and height.
    auto operator!=(const Size &other) const noexcept -> bool = default;

    /// Add two sizes.
    /// @param other The size to add.
    /// @return The component-wise sum.
    auto operator+(const Size &other) const noexcept -> Size {
        return {_width + other._width, _height + other._height};
    }

    /// Subtract two sizes.
    /// @param other The size to subtract.
    /// @return The component-wise difference, clamped to non-negative values by the constructor.
    auto operator-(const Size &other) const noexcept -> Size {
        return {_width - other._width, _height - other._height};
    }

public: // accessors
    /// Get the width (>= 0).
    [[nodiscard]] constexpr auto width() const noexcept -> int { return _width; }

public: // modifiers
    /// Set the width. Negative values are clamped to 0.
    /// @param width New width (clamped to >= 0).
    void setWidth(int width) noexcept { _width = std::max(0, width); }

public: // accessors
    /// Get the height (>= 0).
    [[nodiscard]] constexpr auto height() const noexcept -> int { return _height; }

public: // modifiers
    /// Set the height. Negative values are clamped to 0.
    /// @param height New height (clamped to >= 0).
    void setHeight(int height) noexcept { _height = std::max(0, height); }

public: // tests
    /// Check if this size fits completely into another size (component-wise <=).
    /// @param other The candidate container size.
    /// @return true if width <= other.width and height <= other.height.
    [[nodiscard]] auto fitsInto(const Size other) const noexcept -> bool {
        return _width <= other._width && _height <= other._height;
    }

    /// Compute the area (width * height).
    /// @return The area. Note: returns 0 if either dimension is 0.
    [[nodiscard]] auto area() const noexcept -> int { return _width * _height; }

    /// Compute a position inside the rectangle defined by this size for a given anchor.
    /// Bottom-right resolves to (width-1, height-1), top-left to (0,0), etc.
    /// @param anchor The anchor describing the target corner/edge/center.
    /// @return The position inside the [0,width-1]×[0,height-1] grid (or (0,0) for empty dimensions).
    [[nodiscard]] auto anchor(const Anchor anchor) const noexcept -> Position {
        int x = 0;
        int y = 0;
        switch (anchor & Anchor::HMask) {
        case Anchor::Left:
            x = 0;
            break;
        case Anchor::HCenter:
            x = halfWidthForPosition();
            break;
        case Anchor::Right:
            x = widthForPosition();
            break;
        default:
            break;
        };
        switch (anchor & Anchor::VMask) {
        case Anchor::Top:
            y = 0;
            break;
        case Anchor::VCenter:
            y = halfHeightForPosition();
            break;
        case Anchor::Bottom:
            y = heightForPosition();
            break;
        default:
            break;
        }
        return Position{x, y};
    }

    /// Test if the width and height of this size is in the given range.
    /// @param minimum The minimum size.
    /// @param maximum The maximum size.
    /// @return true If this size is in the range `minimum`-`maximum`.
    [[nodiscard]] auto isInRange(const Size minimum, const Size maximum) const noexcept -> bool {
        return _width >= minimum._width && _height >= minimum._height && _width <= maximum._width &&
            _height <= maximum._height;
    }

    /// Component-wise maximum with another size.
    /// @param other Other size.
    /// @return A size whose width is max(this.width, other.width) and height is max(this.height, other.height).
    [[nodiscard]] auto componentMax(Size other) const noexcept -> Size {
        return {std::max(_width, other._width), std::max(_height, other._height)};
    }

    /// Component-wise minimum with another size.
    /// @param other Other size.
    /// @return A size whose width is min(this.width, other.width) and height is min(this.height, other.height).
    [[nodiscard]] auto componentMin(Size other) const noexcept -> Size {
        return {std::min(_width, other._width), std::min(_height, other._height)};
    }

    /// Check if a position lies strictly inside the bounds [0,width) × [0,height).
    /// @param pos The position to test.
    /// @return true if 0 <= x < width and 0 <= y < height.
    [[nodiscard]] constexpr auto contains(const Position &pos) const noexcept -> bool {
        return pos.x() >= 0 && pos.y() >= 0 && pos.x() < _width && pos.y() < _height;
    }

public: // conversion
    /// Convert a 2D position to a row-major linear index.
    /// @param pos The position. Behavior is undefined if not contained by this size.
    /// @return y * width + x.
    [[nodiscard]] constexpr auto index(const Position &pos) const noexcept -> int { return pos.y() * _width + pos.x(); }

public: // tools
    /// Visit all positions inside the size in row-major order.
    /// @tparam Fn A callable taking Position.
    /// @param fn The function to invoke for each Position (x from 0..width-1, y from 0..height-1).
    template <typename Fn>
    void forEach(Fn fn) const;

private:
    /// Compute the absolute difference between two integer values.
    [[nodiscard]] static constexpr auto absoluteDifference(int value1, int value2) noexcept -> int {
        return (value1 >= value2) ? (value1 - value2) : (value2 - value1);
    }

private:
    /// The maximum valid x coordinate inside the size, or 0 if width == 0.
    [[nodiscard]] constexpr auto widthForPosition() const noexcept -> int { return std::max(0, _width - 1); }

    /// Half of the valid x range endpoint used for horizontal center positioning.
    [[nodiscard]] constexpr auto halfWidthForPosition() const noexcept -> int { return widthForPosition() / 2; }

    /// The maximum valid y coordinate inside the size, or 0 if height == 0.
    [[nodiscard]] constexpr auto heightForPosition() const noexcept -> int { return std::max(0, _height - 1); }

    /// Half of the valid y range endpoint used for vertical center positioning.
    [[nodiscard]] constexpr auto halfHeightForPosition() const noexcept -> int { return heightForPosition() / 2; }

private:
    int _width{};
    int _height{};
};


template <typename Fn>
void Size::forEach(Fn fn) const {
    for (int y = 0; y < height(); ++y) {
        for (int x = 0; x < width(); ++x) {
            fn(Position{x, y});
        }
    }
}


}


template <>
struct std::formatter<erbsland::cterm::Size> : std::formatter<std::string> {
    auto format(const erbsland::cterm::Size &size, format_context &ctx) const {
        return std::formatter<std::string>::format(std::format("{}x{}", size.width(), size.height()), ctx);
    }
};
