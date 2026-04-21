// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Orientation.hpp"
#include "Size.hpp"

#include <algorithm>
#include <cstdint>

namespace erbsland::cterm {

/// Represents margins (top, right, bottom, left) around a rectangle.
/// - Immutable-like value type for representing padding or insets.
/// - Provides convenience constructors for uniform or symmetric margins.
class Margins final {
public:
    /// The side of the margin.
    enum class Side : uint8_t { Top = 0, Right, Bottom, Left };

public:
    /// Default construct with uninitialized values (useful for aggregate-style construction).
    Margins() = default;
    /// Construct margins with the same value on all sides.
    /// @param allSides Value applied to top, right, bottom and left.
    constexpr explicit Margins(const Coordinate allSides) :
        _top(allSides), _right(allSides), _bottom(allSides), _left(allSides) {}
    /// Construct margins with separate horizontal and vertical values.
    /// @param horizontal Value applied to left and right.
    /// @param vertical Value applied to top and bottom.
    constexpr Margins(const Coordinate horizontal, const Coordinate vertical) :
        _top(vertical), _right(horizontal), _bottom(vertical), _left(horizontal) {}
    /// Construct margins with individually specified sides.
    /// @param top Top margin.
    /// @param right Right margin.
    /// @param bottom Bottom margin.
    /// @param left Left margin.
    constexpr Margins(const Coordinate top, const Coordinate right, const Coordinate bottom, const Coordinate left) :
        _top(top), _right(right), _bottom(bottom), _left(left) {}

public: // operators
    /// Equality comparison (all sides must be equal).
    auto operator==(const Margins &other) const noexcept -> bool = default;
    /// Inequality comparison.
    auto operator!=(const Margins &other) const noexcept -> bool = default;

    /// Unary negation producing margins with all sides negated.
    /// Useful for reversing an inset/expansion operation.
    auto operator-() const noexcept -> Margins { return {-_top, -_right, -_bottom, -_left}; }
    /// Get the margin at the given side.
    [[nodiscard]] constexpr auto operator[](const Side side) const noexcept -> Coordinate { return at(side); }

public: // accessors
    /// Get top margin.
    [[nodiscard]] constexpr auto top() const noexcept -> Coordinate { return _top; }
    /// Set the top margin.
    constexpr void setTop(const Coordinate value) noexcept { _top = value; }
    /// Get the right margin.
    [[nodiscard]] constexpr auto right() const noexcept -> Coordinate { return _right; }
    /// Set the right margin.
    constexpr void setRight(const Coordinate value) noexcept { _right = value; }
    /// Get the bottom margin.
    [[nodiscard]] constexpr auto bottom() const noexcept -> Coordinate { return _bottom; }
    /// Set the bottom margin.
    constexpr void setBottom(const Coordinate value) noexcept { _bottom = value; }
    /// Get the left margin.
    [[nodiscard]] constexpr auto left() const noexcept -> Coordinate { return _left; }
    /// Set the left margin.
    constexpr void setLeft(const Coordinate value) noexcept { _left = value; }
    /// Get the margin at the given side.
    [[nodiscard]] constexpr auto at(const Side side) const noexcept -> Coordinate {
        switch (side) {
        case Side::Top:
            return top();
        case Side::Right:
            return right();
        case Side::Bottom:
            return bottom();
        case Side::Left:
            return left();
        }
        return 0;
    }
    /// Set the margin at the given side.
    constexpr void set(const Side side, Coordinate value) noexcept {
        switch (side) {
        case Side::Top:
            _top = value;
            return;
        case Side::Right:
            _right = value;
            return;
        case Side::Bottom:
            _bottom = value;
            return;
        case Side::Left:
            _left = value;
            return;
        default:
            break;
        }
    }
    /// Get the positive horizontal extent consumed by the margins.
    /// @return `left() + right()` with negative sides treated as zero.
    [[nodiscard]] constexpr auto horizontalExtent() const noexcept -> Coordinate {
        return std::max(_left, 0) + std::max(_right, 0);
    }
    /// Get the positive vertical extent consumed by the margins.
    /// @return `top() + bottom()` with negative sides treated as zero.
    [[nodiscard]] constexpr auto verticalExtent() const noexcept -> Coordinate {
        return std::max(_top, 0) + std::max(_bottom, 0);
    }
    /// Get the positive extent consumed by the margins.
    /// @return A size containing the horizontal and vertical extents.
    [[nodiscard]] constexpr auto extent() const noexcept -> Size { return {horizontalExtent(), verticalExtent()}; }
    /// Get the extent for the selected orientation.
    /// @param orientation The orientation that selects horizontal or vertical extent.
    /// @return `horizontalExtent()` for `Horizontal`, otherwise `verticalExtent()`.
    [[nodiscard]] constexpr auto extent(const Orientation orientation) const noexcept -> Coordinate {
        return orientation == Orientation::Horizontal ? horizontalExtent() : verticalExtent();
    }
    /// Get the horizontal spacing represented by the larger positive horizontal side.
    [[nodiscard]] constexpr auto horizontalSpacing() const noexcept -> Coordinate {
        return std::max<Coordinate>({0, _left, _right});
    }
    /// Get the vertical spacing represented by the larger positive vertical side.
    [[nodiscard]] constexpr auto verticalSpacing() const noexcept -> Coordinate {
        return std::max<Coordinate>({0, _top, _bottom});
    }
    /// Get the spacing represented by the margins.
    /// @return A size containing horizontal and vertical spacing.
    [[nodiscard]] constexpr auto spacing() const noexcept -> Size { return {horizontalSpacing(), verticalSpacing()}; }
    /// Get the spacing for the selected orientation.
    /// @param orientation The orientation that selects horizontal or vertical spacing.
    /// @return `horizontalSpacing()` for `Horizontal`, otherwise `verticalSpacing()`.
    [[nodiscard]] constexpr auto spacing(const Orientation orientation) const noexcept -> Coordinate {
        return orientation == Orientation::Horizontal ? horizontalSpacing() : verticalSpacing();
    }
    /// Component-wise maximum with another margin set.
    /// @param other The other margins.
    /// @return Margins containing the max of each side.
    [[nodiscard]] constexpr auto componentMax(const Margins other) const noexcept -> Margins {
        return {
            std::max(_top, other._top),
            std::max(_right, other._right),
            std::max(_bottom, other._bottom),
            std::max(_left, other._left)};
    }
    /// Component-wise maximum with another margin set, limited to one side.
    /// @param other The other margins.
    /// @param side The side to compare.
    /// @return Margins with only the selected side replaced by the maximum value.
    [[nodiscard]] constexpr auto componentMax(const Margins other, const Side side) const noexcept -> Margins {
        auto result = *this;
        result.set(side, std::max(at(side), other.at(side)));
        return result;
    }
    /// Component-wise minimum with another margin set.
    /// @param other The other margins.
    /// @return Margins containing the min of each side.
    [[nodiscard]] constexpr auto componentMin(const Margins other) const noexcept -> Margins {
        return {
            std::min(_top, other._top),
            std::min(_right, other._right),
            std::min(_bottom, other._bottom),
            std::min(_left, other._left)};
    }
    /// Component-wise minimum with another margin set, limited to one side.
    /// @param other The other margins.
    /// @param side The side to compare.
    /// @return Margins with only the selected side replaced by the minimum value.
    [[nodiscard]] constexpr auto componentMin(const Margins other, const Side side) const noexcept -> Margins {
        auto result = *this;
        result.set(side, std::min(at(side), other.at(side)));
        return result;
    }

private:
    Coordinate _top{};
    Coordinate _right{};
    Coordinate _bottom{};
    Coordinate _left{};
};

}
