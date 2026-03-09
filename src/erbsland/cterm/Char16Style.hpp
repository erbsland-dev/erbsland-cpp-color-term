// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#pragma once


#include "Char.hpp"
#include "FrameStyle.hpp"
#include "String.hpp"

#include <array>
#include <memory>
#include <string_view>


namespace erbsland::cterm {


class Char16Style;
/// Shared pointer for Char16Style
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
    /// Create a new tile 16 style from 16 terminal characters.
    /// Connection points/bits: E:0, S:1, W:2, N:3
    /// @param tiles A sequence of exactly 16 terminal characters.
    /// @throws std::invalid_argument If `tiles` does not contain exactly 16 terminal characters.
    explicit Char16Style(std::u32string_view tiles);

public: // accessors
    /// Access the block for a given bit combination.
    /// @param bitMask The bit-mask E:0, S:1, W:2, N:3.
    /// @return The block for the given bit combination, or an empty block if the bits are out of bounds.
    [[nodiscard]] auto block(uint32_t bitMask) const noexcept -> Char;

public:
    /// Create a new shared style from 16 terminal characters.
    /// @param tiles A sequence of exactly 16 terminal characters.
    /// @return A shared style instance.
    /// @throws std::invalid_argument If `tiles` does not contain exactly 16 terminal characters.
    [[nodiscard]] static auto create(std::string_view tiles) -> Char16StylePtr;
    /// Create a new shared style from 16 terminal characters.
    /// @param tiles A sequence of exactly 16 terminal characters.
    /// @return A shared style instance.
    /// @throws std::invalid_argument If `tiles` does not contain exactly 16 terminal characters.
    [[nodiscard]] static auto create(std::u32string_view tiles) -> Char16StylePtr;
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
    /// Get the style for the given frame style.
    /// @param frameStyle The frame style to resolve.
    /// @return The shared style instance, or `nullptr` if `frameStyle` requires `Tile9Style`.
    [[nodiscard]] static auto forStyle(FrameStyle frameStyle) -> Char16StylePtr;

private:
    [[nodiscard]] static auto toTiles(const String &tiles) -> std::array<Char, 16>;

private:
    std::array<Char, 16> _tiles;
};


}
