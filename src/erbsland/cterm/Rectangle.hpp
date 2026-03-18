// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Direction.hpp"
#include "Margins.hpp"
#include "Position.hpp"
#include "Size.hpp"

#include <cstdint>
#include <functional>
#include <type_traits>
#include <vector>


namespace erbsland::cterm {


class Rectangle;
using RectangleList = std::vector<Rectangle>;


/// Axis-aligned rectangle represented by a top-left position and size.
/// Provides geometry utilities such as containment tests, expansion and iteration.
/// @tested `RectangleTest`
class Rectangle {
public:
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
    /// Construct from a top-left and bottom-right (exclusive) corner position.
    /// If the bottom right is left or above the top-left corner, the rectangle will be empty.
    /// @param topLeft The top-left corner *inside* the new rectangle.
    /// @param bottomRight The bottom-right corner *outside* the new rectangle.
    constexpr Rectangle(const Position topLeft, const Position bottomRight) noexcept :
        _pos{topLeft}, _size{bottomRight.x() - topLeft.x(), bottomRight.y() - topLeft.y()} {}

public: // operators
    /// Compare two rectangles.
    auto operator==(const Rectangle &other) const noexcept -> bool = default;
    /// Compare two rectangles.
    auto operator!=(const Rectangle &other) const noexcept -> bool = default;
    /// Merge two rectangles into a larger one that holds both rectangles.
    auto operator|(const Rectangle &other) const noexcept -> Rectangle;
    /// Expand this rectangle to include another rectangle.
    /// @param other The other rectangle to merge.
    /// @return Reference to this rectangle.
    auto operator|=(const Rectangle &other) noexcept -> Rectangle &;
    /// Intersect two rectangles to get only the overlapping part.
    /// If the two rectangles don't overlap, return an empty rectangle.
    auto operator&(const Rectangle &other) const noexcept -> Rectangle;
    /// Change this rectangle to the intersection of both rectangles.
    /// If the two rectangles don't overlap, its size will be (0,0).
    auto operator&=(const Rectangle &other) noexcept -> Rectangle &;

public: // attributes
    /// Get the top-left corner position.
    [[nodiscard]] constexpr auto pos() const noexcept -> Position { return _pos; }
    /// Set the top-left corner position.
    /// @param pos New position value.
    void setPos(Position pos) noexcept { _pos = pos; }
    /// Get the size of the rectangle.
    [[nodiscard]] constexpr auto size() const noexcept -> Size { return _size; }
    /// Set the size of the rectangle.
    /// @param size New size value.
    void setSize(Size size) noexcept { _size = size; }

public: // accessors
    /// Left x-coordinate.
    [[nodiscard]] constexpr auto x1() const noexcept -> int { return _pos.x(); }
    /// Top y-coordinate.
    [[nodiscard]] constexpr auto y1() const noexcept -> int { return _pos.y(); }
    /// Right x-coordinate (exclusive).
    [[nodiscard]] constexpr auto x2() const noexcept -> int { return _pos.x() + _size.width(); }
    /// Bottom y-coordinate (exclusive).
    [[nodiscard]] constexpr auto y2() const noexcept -> int { return _pos.y() + _size.height(); }
    /// Top-left corner position.
    /// Is equivalent to `pos()`.
    [[nodiscard]] constexpr auto topLeft() const noexcept -> Position { return _pos; }
    /// Top-right corner position (x-coordinate exclusive).
    [[nodiscard]] constexpr auto topRight() const noexcept -> Position { return {x2(), y1()}; }
    /// Bottom-left corner position (y-coordinate exclusive).
    [[nodiscard]] constexpr auto bottomLeft() const noexcept -> Position { return {x1(), y2()}; }
    /// Bottom-right corner position (x-coordinate exclusive, y-coordinate exclusive).
    [[nodiscard]] constexpr auto bottomRight() const noexcept -> Position { return {x2(), y2()}; }
    /// Rectangle width.
    [[nodiscard]] constexpr auto width() const noexcept -> int { return _size.width(); }
    /// Rectangle height.
    [[nodiscard]] constexpr auto height() const noexcept -> int { return _size.height(); }
    /// Position of a given anchor within this rectangle.
    /// @param anchor Anchor to query.
    /// @return The position *inside* this rectangle matching the requested anchor.
    [[nodiscard]] auto anchor(Anchor anchor = Anchor::TopLeft) const noexcept -> Position;
    /// Center position.
    /// This is equal to the position of the `Anchor::Center` anchor.
    [[nodiscard]] auto center() const noexcept -> Position { return anchor(Anchor::Center); }

public: // tests
    /// Check if a position is inside the rectangle.
    /// @param testedPosition Position to test.
    /// @return `true` if the position lies inside the rectangle bounds.
    [[nodiscard]] auto contains(Position testedPosition) const noexcept -> bool;
    /// Checks if another rectangle fits into this one.
    /// Only `true` if every position of the tested rectangle is inside this one.
    /// @param testedRectangle The rectangle to test for containment.
    /// @return `true` if the tested rectangle is fully contained within this one.
    [[nodiscard]] auto contains(Rectangle testedRectangle) const noexcept -> bool;
    /// Check if another rectangle overlaps this one.
    /// Overlapping is when both rectangles share at least one position.
    /// @param testedRectangle The rectangle to test for overlap.
    [[nodiscard]] auto overlaps(Rectangle testedRectangle) const noexcept -> bool;
    /// Check if a position lies on the rectangle frame.
    /// @param testedPosition Position to test.
    /// @return `true` if the position lies on the outer frame of the rectangle.
    [[nodiscard]] auto isFrame(Position testedPosition) const noexcept -> bool;

public: // tools
    /// Get a hash for this rectangle.
    [[nodiscard]] constexpr auto hash() const noexcept -> std::size_t {
        return impl::hashCreate(x1(), y1(), width(), height());
    }
    /// Clamp a position to this rectangle.
    /// @param position The position to clamp.
    /// @return The clamped position where (x1 <= position.x <= x2) && (y1 <= position.y <= y2)
    [[nodiscard]] auto clamp(Position position) const noexcept -> Position;
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
    /// Get the clockwise border index for a frame position.
    /// The top-left corner has index `0`, then the index increases clockwise around the perimeter.
    /// Degenerate rectangles with width or height `1` still produce a continuous index sequence.
    /// @param testedPosition The position on the frame.
    /// @return The clockwise border index, or `-1` if the position is not on the frame.
    [[nodiscard]] auto frameIndex(Position testedPosition) const noexcept -> int64_t;
    /// Get the frame direction for a given position in this rectangle.
    /// @return The direction of the frame at the given position, or Direction::None if the position is not on the
    /// frame.
    [[nodiscard]] auto frameDirection(Position testedPosition) const noexcept -> Direction;
    /// Divide this rectangle into equally spaced grid cells.
    /// Each cell must be at least 1x1 in size, if this isn't possible, `std::invalid_argument` is thrown.
    /// @param rows The number of rows. Minimum 1.
    /// @param columns The number of columns. Minimum 1
    /// @param horizontalSpacing The spacing between cells horizontally.
    /// @param verticalSpacing The spacing between cells vertically.
    /// @return A vector of rectangles representing the grid cells from left to right, top to bottom.
    /// @throws std::invalid_argument if rows or columns are less than 1 or the chosen division is impossible.
    [[nodiscard]] auto gridCells(int rows, int columns, int horizontalSpacing = 0, int verticalSpacing = 0) const
        -> std::vector<Rectangle>;
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
    /// Get the bounds from the given positions.
    /// @param positions The positions to get the bounds from.
    /// @return A rectangle that contains all positions.
    [[nodiscard]] static auto bounds(const PositionList &positions) noexcept -> Rectangle;

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
struct std::hash<erbsland::cterm::Rectangle> {
    auto operator()(const erbsland::cterm::Rectangle &rect) const noexcept -> std::size_t { return rect.hash(); }
};


template <>
struct std::formatter<erbsland::cterm::Rectangle> : std::formatter<std::string> {
    auto format(const erbsland::cterm::Rectangle &rect, format_context &ctx) const {
        return std::formatter<std::string>::format(std::format("{}:{}", rect.topLeft(), rect.size()), ctx);
    }
};
