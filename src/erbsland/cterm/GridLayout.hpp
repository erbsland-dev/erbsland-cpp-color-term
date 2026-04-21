// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "FrameBorder.hpp"

#include "geometry/Coordinate.hpp"
#include "geometry/Position.hpp"
#include "geometry/Rectangle.hpp"
#include "geometry/Size.hpp"

#include <initializer_list>
#include <string_view>
#include <vector>

namespace erbsland::cterm {

/// Geometry for a grid of content cells.
///
/// This class stores only content sizes.
/// @tested `GridLayoutTest`
class GridLayout final {
public:
    /// Create a grid layout.
    /// @param columnWidths The content width of each column. Each value must be positive.
    /// @param rowHeights The content height of each row. Each value must be positive.
    /// @throws std::invalid_argument if either list is empty or contains a non-positive size.
    GridLayout(std::vector<Coordinate> columnWidths, std::vector<Coordinate> rowHeights);
    /// Create a grid layout from initializer lists.
    /// @param columnWidths The content width of each column. Each value must be positive.
    /// @param rowHeights The content height of each row. Each value must be positive.
    /// @throws std::invalid_argument if either list is empty or contains a non-positive size.
    GridLayout(std::initializer_list<Coordinate> columnWidths, std::initializer_list<Coordinate> rowHeights);

    // defaults
    ~GridLayout() = default;
    GridLayout(const GridLayout &) = default;
    GridLayout(GridLayout &&) noexcept = default;
    auto operator=(const GridLayout &) -> GridLayout & = default;
    auto operator=(GridLayout &&) noexcept -> GridLayout & = default;

public: // operators
    auto operator==(const GridLayout &other) const noexcept -> bool = default;

public: // accessors
    /// Number of rows in the grid.
    [[nodiscard]] auto rowCount() const noexcept -> std::size_t;
    /// Number of columns in the grid.
    [[nodiscard]] auto columnCount() const noexcept -> std::size_t;
    /// Access one row height.
    /// @param row The row index.
    /// @return The row content height.
    /// @throws std::out_of_range if `row` is outside the layout.
    [[nodiscard]] auto rowHeight(std::size_t row) const -> Coordinate;
    /// Access one column width.
    /// @param column The column index.
    /// @return The column content width.
    /// @throws std::out_of_range if `column` is outside the layout.
    [[nodiscard]] auto columnWidth(std::size_t column) const -> Coordinate;
    /// Access all row heights.
    [[nodiscard]] auto rowHeights() const noexcept -> const std::vector<Coordinate> &;
    /// Access all column widths.
    [[nodiscard]] auto columnWidths() const noexcept -> const std::vector<Coordinate> &;

public:
    /// Calculate the complete size of this layout for the given border.
    /// @param border Border styles that decide which frame lines occupy cells.
    /// @return The total grid size including active frame lines.
    [[nodiscard]] auto size(const FrameBorder &border) const noexcept -> Size;
    /// Calculate the content rectangle for one cell.
    /// @param row The row index.
    /// @param column The column index.
    /// @param origin The top-left position of the full grid.
    /// @param border Border styles that decide which frame lines occupy cells.
    /// @return The cell content rectangle.
    /// @throws std::out_of_range if `row` or `column` is outside the layout.
    [[nodiscard]] auto cellRect(std::size_t row, std::size_t column, Position origin, const FrameBorder &border) const
        -> Rectangle;

private:
    [[nodiscard]] static auto borderSize(const FrameBorder &border, FrameBorder::Element element) noexcept
        -> Coordinate;
    static void validateSizes(const std::vector<Coordinate> &sizes, std::string_view name);

private:
    std::vector<Coordinate> _columnWidths;
    std::vector<Coordinate> _rowHeights;
};

}
