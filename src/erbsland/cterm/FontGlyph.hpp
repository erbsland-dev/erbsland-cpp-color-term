// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Bitmap.hpp"

#include <algorithm>
#include <bit>
#include <cstdint>
#include <vector>


namespace erbsland::cterm {


/// A bitmap glyph that can be used by a terminal text font.
class FontGlyph final : public Bitmap {
public:
    /// Maximum supported glyph width when importing numeric row masks.
    constexpr static auto cMaxGlyphWidth = 64;

public:
    /// Create an empty glyph.
    FontGlyph() = default;
    /// Create an empty glyph with the given size.
    /// @param size The glyph dimensions.
    explicit FontGlyph(const Size size) noexcept : Bitmap{size} {}
    /// Create a glyph from numeric row masks.
    /// @param data One 64-bit mask per bitmap row.
    template <typename T>
        requires std::is_integral_v<T> && std::is_unsigned_v<T>
    explicit FontGlyph(const std::vector<T> &data) :
        Bitmap{Size{calculateGlyphWidth(data), static_cast<int>(data.size())}} {
        draw(Position{0, 0}, data);
        flipHorizontal();
    }

private:
    template <typename T>
        requires std::is_integral_v<T> && std::is_unsigned_v<T>
    [[nodiscard]] constexpr static auto calculateGlyphWidth(const std::vector<T> &data) noexcept -> int {
        int width = 0;
        for (const auto &mask : data) {
            width = std::max(width, std::bit_width(mask));
        }
        return width;
    }
};


}
