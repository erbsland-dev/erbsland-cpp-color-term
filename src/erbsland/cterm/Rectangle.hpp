// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Margins.hpp"
#include "Position.hpp"
#include "Size.hpp"


namespace erbsland::cterm {


/// Axis-aligned rectangle represented by a top-left position and size.
/// Provides geometry utilities such as containment tests, expansion and iteration.
class Rectangle {
public: // ctors/dtor/assign/move
    /// Construct an empty rectangle at (0,0).
    Rectangle() = default;
    /// Construct from explicit position and size values.
    /// @param x X-coordinate of the top-left corner.
    /// @param y Y-coordinate of the top-left corner.
    /// @param width Rectangle width.
    /// @param height Rectangle height.
    constexpr Rectangle(int x, int y, int width, int height) noexcept : _pos{x, y}, _size{width, height} {}
    /// Construct from position and size objects.
    /// @param pos Top-left corner position.
    /// @param size Rectangle size.
    constexpr Rectangle(Position pos, Size size) noexcept : _pos{pos}, _size{size} {}

public: // operators
    /// Expand this rectangle to include another rectangle.
    /// @param other Other rectangle to merge.
    /// @return Reference to this rectangle.
    auto operator|=(const Rectangle &other) noexcept -> Rectangle &;

public: // accessors
    /// Left x-coordinate.
    [[nodiscard]] auto x1() const noexcept -> int { return _pos.x(); }
    /// Top y-coordinate.
    [[nodiscard]] auto y1() const noexcept -> int { return _pos.y(); }
    /// Right x-coordinate (exclusive).
    [[nodiscard]] auto x2() const noexcept -> int { return _pos.x() + _size.width(); }
    /// Bottom y-coordinate (exclusive).
    [[nodiscard]] auto y2() const noexcept -> int { return _pos.y() + _size.height(); }
    /// Top-left corner position.
    [[nodiscard]] auto topLeft() const noexcept -> Position { return _pos; }
    /// Top-right corner position.
    [[nodiscard]] auto topRight() const noexcept -> Position { return {x2(), y1()}; }
    /// Bottom-left corner position.
    [[nodiscard]] auto bottomLeft() const noexcept -> Position { return {x1(), y2()}; }
    /// Bottom-right corner position.
    [[nodiscard]] auto bottomRight() const noexcept -> Position { return {x2(), y2()}; }
    /// Rectangle width.
    [[nodiscard]] auto width() const noexcept -> int { return _size.width(); }
    /// Rectangle height.
    [[nodiscard]] auto height() const noexcept -> int { return _size.height(); }
    /// Get the size of the rectangle.
    [[nodiscard]] constexpr auto size() const noexcept -> Size { return _size; }
    /// Position of a given anchor within this rectangle.
    /// @param anchor Anchor to query.
    /// @return The position inside this rectangle matching the requested anchor.
    [[nodiscard]] auto anchor(Anchor anchor = Anchor::TopLeft) const noexcept -> Position;

public: // modifiers
    /// Set the top-left corner position.
    /// @param pos New position value.
    void setPos(Position pos) noexcept { _pos = pos; }

    /// Set the size of the rectangle.
    /// @param size New size value.
    void setSize(Size size) noexcept { _size = size; }

public: // tools
    /// Create a rectangle expanded by the provided margins.
    /// @param margins Margins to apply; positive values expand outward.
    /// @return The expanded rectangle.
    [[nodiscard]] auto expandedBy(Margins margins) const noexcept -> Rectangle;
    /// Create a rectangle inset by the provided margins.
    /// @param margins Margins to remove from each side.
    /// @return The inset rectangle.
    [[nodiscard]] auto insetBy(Margins margins) const noexcept -> Rectangle;
    /// Create a sub-rectangle inside this rectangle.
    /// @param anchor The anchor of the rectangle.
    /// @param size The size. Zero means full width/height.
    /// @param margins The margins around the sub rectangle.
    /// @return The aligned sub-rectangle.
    [[nodiscard]] auto subRectangle(Anchor anchor, Size size, Margins margins) const noexcept -> Rectangle;
    /// Check if a position is inside the rectangle.
    /// @param testedPosition Position to test.
    /// @return `true` if the position lies inside the rectangle bounds.
    [[nodiscard]] auto contains(Position testedPosition) const noexcept -> bool;
    /// Check if a position lies on the rectangle frame.
    /// @param testedPosition Position to test.
    /// @return `true` if the position lies on the outer frame of the rectangle.
    [[nodiscard]] auto isFrame(Position testedPosition) const noexcept -> bool;
    /// Call a function for each position contained in the rectangle.
    /// @tparam Fn A callable with signature `void(Position)`.
    /// The function definition must be `void fn(Position pos)`.
    template <typename Fn>
    void forEach(Fn fn) const;
    /// Call a function for each position around the frame, clockwise with index.
    /// @tparam Fn A callable with signature `void(Position, int)`.
    /// The function definition must be `void fn(Position pos, int index)`.
    template <typename Fn>
        requires(std::is_invocable_r_v<void, Fn, Position, int> || std::is_invocable_r_v<void, Fn, Position>)
    void forEachInFrame(Fn fn) const;

private:
    Position _pos;
    Size _size;
};


template <typename Fn>
void Rectangle::forEach(Fn fn) const {
    for (int y = 0; y < _size.height(); ++y) {
        for (int x = 0; x < _size.width(); ++x) {
            fn(Position{x, y} + _pos);
        }
    }
}


template <typename Fn>
    requires(std::is_invocable_r_v<void, Fn, Position, int> || std::is_invocable_r_v<void, Fn, Position>)
void Rectangle::forEachInFrame(Fn fn) const {
    if (_size.width() <= 0 || _size.height() <= 0) {
        return;
    }
    if constexpr (std::is_invocable_r_v<void, Fn, Position, int>) {
        int index = 0;
        for (int x = 0; x < _size.width(); ++x) {
            fn(Position{x1() + x, y1()}, index++);
        }
        if (_size.height() > 1) {
            for (int y = 1; y < _size.height(); ++y) {
                fn(Position{x2() - 1, y1() + y}, index++);
            }
            for (int x = _size.width() - 2; x >= 0; --x) {
                fn(Position{x1() + x, y2() - 1}, index++);
            }
            if (_size.width() > 1) {
                for (int y = _size.height() - 2; y > 0; --y) {
                    fn(Position{x1(), y1() + y}, index++);
                }
            }
        }
    } else {
        for (int x = 0; x < _size.width(); ++x) {
            fn(Position{x1() + x, y1()});
        }
        if (_size.height() > 1) {
            for (int y = 1; y < _size.height(); ++y) {
                fn(Position{x2() - 1, y1() + y});
            }
            for (int x = _size.width() - 2; x >= 0; --x) {
                fn(Position{x1() + x, y2() - 1});
            }
            if (_size.width() > 1) {
                for (int y = _size.height() - 2; y > 0; --y) {
                    fn(Position{x1(), y1() + y});
                }
            }
        }
    }
}


}


template <>
struct std::formatter<erbsland::cterm::Rectangle> : std::formatter<std::string> {
    auto format(const erbsland::cterm::Rectangle &rect, format_context &ctx) const {
        return std::formatter<std::string>::format(std::format("{}:{}", rect.topLeft(), rect.size()), ctx);
    }
};
