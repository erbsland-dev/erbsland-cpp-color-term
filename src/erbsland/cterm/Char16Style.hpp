// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#pragma once


#include "Char.hpp"
#include "FrameStyle.hpp"

#include <array>
#include <memory>
#include <string_view>


namespace erbsland::cterm {


class Char16Style;
using Char16StylePtr = std::shared_ptr<Char16Style>;


/// Defines a style for drawing tiles.
class Char16Style {
public:
    /// Create a new tile 16 style.
    /// Connection points/bits: E:0, S:1, W:2, N:3
    explicit Char16Style(std::array<Char, 16> tiles) noexcept : _tiles(std::move(tiles)) {}
    /// Create a new tile 16 style from 16 terminal characters.
    /// Connection points/bits: E:0, S:1, W:2, N:3
    /// @param tiles A sequence of exactly 16 terminal characters.
    /// @throws std::invalid_argument If `tiles` does not contain exactly 16 terminal characters.
    explicit Char16Style(std::string_view tiles);

public: // accessors
    /// Access the block for a given bit combination.
    /// @param bitMask The bit-mask E:0, S:1, W:2, N:3.
    /// @return The block for the given bit combination, or an empty block if the bits are out of bounds.
    [[nodiscard]] auto block(uint32_t bitMask) const noexcept -> Char;

public:
    /// For drawing light frames.
    [[nodiscard]] static auto lightFrame() -> Char16StylePtr;
    /// For drawing light frames with double-dashed lines.
    [[nodiscard]] static auto lightDoubleDashFrame() -> Char16StylePtr;
    /// For drawing light frames with triple-dashed lines.
    [[nodiscard]] static auto lightTripleDashFrame() -> Char16StylePtr;
    /// For drawing light frames with quadruple-dashed lines.
    [[nodiscard]] static auto lightQuadrupleDashFrame() -> Char16StylePtr;
    /// For drawing light frames with rounded corners.
    [[nodiscard]] static auto lightRoundedFrame() -> Char16StylePtr;
    /// For drawing heavy frames.
    [[nodiscard]] static auto heavyFrame() -> Char16StylePtr;
    /// For drawing heavy frames with double-dashed lines.
    [[nodiscard]] static auto heavyDoubleDashFrame() -> Char16StylePtr;
    /// For drawing heavy frames with triple-dashed lines.
    [[nodiscard]] static auto heavyTripleDashFrame() -> Char16StylePtr;
    /// For drawing heavy frames with quadruple-dashed lines.
    [[nodiscard]] static auto heavyQuadrupleDashFrame() -> Char16StylePtr;
    /// For drawing double frames.
    [[nodiscard]] static auto doubleFrame() -> Char16StylePtr;
    /// For drawing solid block frames.
    [[nodiscard]] static auto fullBlockFrame() -> Char16StylePtr;
    /// For drawing solid block frames with chamfered corners.
    [[nodiscard]] static auto fullBlockWithChamferFrame() -> Char16StylePtr;
    /// For drawing half-block frames on the outer cell edges.
    [[nodiscard]] static auto outerHalfBlockFrame() -> Char16StylePtr;
    /// For drawing half-block frames on the inner cell edges.
    [[nodiscard]] static auto innerHalfBlockFrame() -> Char16StylePtr;
    /// Get the style for the given frame style.
    [[nodiscard]] static auto forStyle(FrameStyle frameStyle) -> Char16StylePtr;

private:
    [[nodiscard]] static auto splitTiles(std::string_view tiles) -> std::array<Char, 16>;
    [[nodiscard]] static auto styleFromTiles(std::string_view tiles) -> Char16StylePtr;

private:
    std::array<Char, 16> _tiles;
};


}
