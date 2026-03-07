// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


namespace erbsland::cterm {


/// Represents margins (top, right, bottom, left) around a rectangle.
/// - Immutable-like value type for representing padding or insets.
/// - Provides convenience constructors for uniform or symmetric margins.
class Margins final {
public: // ctors/dtor/assign/move
    /// Default construct with uninitialized values (useful for aggregate-style construction).
    Margins() = default;
    /// Construct margins with the same value on all sides.
    /// @param allSides Value applied to top, right, bottom and left.
    constexpr explicit Margins(const int allSides) :
        _top(allSides), _right(allSides), _bottom(allSides), _left(allSides) {}
    /// Construct margins with separate horizontal and vertical values.
    /// @param horizontal Value applied to left and right.
    /// @param vertical Value applied to top and bottom.
    constexpr Margins(const int horizontal, const int vertical) :
        _top(vertical), _right(horizontal), _bottom(vertical), _left(horizontal) {}
    /// Construct margins with individually specified sides.
    /// @param top Top margin.
    /// @param right Right margin.
    /// @param bottom Bottom margin.
    /// @param left Left margin.
    constexpr Margins(const int top, const int right, const int bottom, const int left) :
        _top(top), _right(right), _bottom(bottom), _left(left) {}

public: // operators
    /// Equality comparison (all sides must be equal).
    auto operator==(const Margins &other) const noexcept -> bool = default;
    /// Inequality comparison.
    auto operator!=(const Margins &other) const noexcept -> bool = default;

    /// Unary negation producing margins with all sides negated.
    /// Useful for reversing an inset/expansion operation.
    auto operator-() const noexcept -> Margins { return Margins(-_top, -_right, -_bottom, -_left); }

public: // accessors
    /// Get top margin.
    [[nodiscard]] constexpr auto top() const noexcept -> int { return _top; }
    /// Get right margin.
    [[nodiscard]] constexpr auto right() const noexcept -> int { return _right; }
    /// Get bottom margin.
    [[nodiscard]] constexpr auto bottom() const noexcept -> int { return _bottom; }
    /// Get left margin.
    [[nodiscard]] constexpr auto left() const noexcept -> int { return _left; }

private:
    int _top;
    int _right;
    int _bottom;
    int _left;
};


}
