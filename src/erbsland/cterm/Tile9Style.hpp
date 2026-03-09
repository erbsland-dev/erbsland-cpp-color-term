// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#pragma once


#include "Char.hpp"
#include "FrameStyle.hpp"
#include "Rectangle.hpp"
#include "String.hpp"

#include <array>
#include <memory>
#include <string_view>


namespace erbsland::cterm {


class Tile9Style;
/// Shared pointer for Tile9Style
using Tile9StylePtr = std::shared_ptr<Tile9Style>;


/// Defines a style for repeating a 3x3 tile pattern across a rectangle.
///
/// The 9-tile layout uses this arrangement:
/// top-left, top, top-right,
/// left, center, right,
/// bottom-left, bottom, bottom-right.
///
/// Optionally, 7 additional tiles can be provided for degenerate rectangles:
/// single-row left, single-row center, single-row right,
/// single-column top, single-column center, single-column bottom,
/// and the single-cell tile.
class Tile9Style {
public:
    /// Create a new 9-tile style from the repeating 3x3 tile layout.
    /// @param tiles The 3x3 tiles in row-major order.
    explicit Tile9Style(std::array<Char, 9> tiles) noexcept;
    /// Create a new 9-tile style with explicit tiles for degenerate rectangles.
    /// @param tiles The 3x3 tiles followed by 7 degenerate tiles.
    explicit Tile9Style(std::array<Char, 16> tiles) noexcept;
    /// Create a new 9-tile style from 9 or 16 terminal characters.
    /// @param tiles A sequence of 9 tiles, or 16 tiles including the degenerate cases.
    /// @throws std::invalid_argument If `tiles` does not contain exactly 9 or 16 terminal characters.
    explicit Tile9Style(std::string_view tiles);
    /// Create a new 9-tile style from 9 or 16 terminal characters.
    /// @param tiles A sequence of 9 tiles, or 16 tiles including the degenerate cases.
    /// @throws std::invalid_argument If `tiles` does not contain exactly 9 or 16 terminal characters.
    explicit Tile9Style(std::u32string_view tiles);

public: // accessors
    /// Resolve the tile for a given position inside a rectangle.
    /// The center and edge tiles are repeated as needed.
    /// @param rect The styled rectangle.
    /// @param pos A position inside `rect`.
    /// @return The resolved tile, or an empty character if `pos` is outside `rect`.
    [[nodiscard]] auto block(Rectangle rect, Position pos) const noexcept -> Char;

public:
    /// Create a new shared style from 9 or 16 terminal characters.
    /// @param tiles A sequence of 9 tiles, or 16 tiles including the degenerate cases.
    /// @return A shared style instance.
    /// @throws std::invalid_argument If `tiles` does not contain exactly 9 or 16 terminal characters.
    [[nodiscard]] static auto create(std::string_view tiles) -> Tile9StylePtr;
    /// Create a new shared style from 9 or 16 terminal characters.
    /// @param tiles A sequence of 9 tiles, or 16 tiles including the degenerate cases.
    /// @return A shared style instance.
    /// @throws std::invalid_argument If `tiles` does not contain exactly 9 or 16 terminal characters.
    [[nodiscard]] static auto create(std::u32string_view tiles) -> Tile9StylePtr;
    /// For drawing half-block frames on the outer cell edges.
    [[nodiscard]] static auto outerHalfBlockFrame() -> Tile9StylePtr;
    /// For drawing half-block frames on the inner cell edges.
    [[nodiscard]] static auto innerHalfBlockFrame() -> Tile9StylePtr;
    /// Get the tile-9 style for a predefined frame style.
    /// @param frameStyle The frame style to resolve.
    /// @return The matching tile-9 style, or `nullptr` if the frame style uses `Char16Style`.
    [[nodiscard]] static auto forStyle(FrameStyle frameStyle) -> Tile9StylePtr;

private:
    struct ParsedTiles {
        std::array<Char, 16> tiles{};
        bool hasExtendedTiles = false;
    };

private:
    explicit Tile9Style(const ParsedTiles &parsed) noexcept;
    [[nodiscard]] static auto parseTiles(const String &tiles) -> ParsedTiles;
    [[nodiscard]] static auto toParsedTiles(const std::array<Char, 9> &tiles) noexcept -> ParsedTiles;
    [[nodiscard]] static auto toParsedTiles(const std::array<Char, 16> &tiles) noexcept -> ParsedTiles;

private:
    std::array<Char, 16> _tiles{};
    bool _hasExtendedTiles = false;
};


}
