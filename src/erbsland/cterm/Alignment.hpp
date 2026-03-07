// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>


namespace erbsland::cterm {


/// Alignment of text or graphics in a box.
enum class Alignment : uint8_t {
    Left = 1 << 0,                           ///< Aligned to the left edge of the box.
    HCenter = 1 << 1,                        ///< Aligned to the horizontal center of the box.
    Right = 1 << 2,                          ///< Aligned to the right edge of the box.
    Top = 1 << 4,                            ///< Aligned to the top edge of the box.
    VCenter = 1 << 5,                        ///< Aligned to the vertical center of the box.
    Bottom = 1 << 6,                         ///< Aligned to the bottom edge of the box.
    TopLeft = Top | Left,                    ///< Aligned to the top-left corner of the box.
    TopCenter = Top | HCenter,               ///< Aligned to the top-center of the box.
    TopRight = Top | Right,                  ///< Aligned to the top-right corner of the box.
    CenterLeft = VCenter | Left,             ///< Aligned to the center-left of the box.
    Center = VCenter | HCenter,              ///< Aligned to the center of the box.
    CenterRight = VCenter | Right,           ///< Aligned to the center-right of the box.
    BottomLeft = Bottom | Left,              ///< Aligned to the bottom-left corner of the box.
    BottomCenter = Bottom | HCenter,         ///< Aligned to the bottom-center of the box.
    BottomRight = Bottom | Right,            ///< Aligned to the bottom-right corner of the box.
    HorizontalMask = Left | HCenter | Right, ///< Mask for extracting the horizontal component.
    VerticalMask = Top | VCenter | Bottom,   ///< Mask for extracting the vertical component.
};


/// Bitwise OR for combining alignments.
/// Example: Alignment::Top | Alignment::HCenter -> TopCenter
[[nodiscard]] constexpr auto operator|(const Alignment lhs, const Alignment rhs) noexcept -> Alignment {
    return static_cast<Alignment>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

/// Bitwise AND for masking/extracting components of an alignment.
/// Example: (alignment & Alignment::VerticalMask) yields the vertical component.
[[nodiscard]] constexpr auto operator&(const Alignment lhs, const Alignment rhs) noexcept -> Alignment {
    return static_cast<Alignment>(static_cast<int>(lhs) & static_cast<int>(rhs));
}


}
