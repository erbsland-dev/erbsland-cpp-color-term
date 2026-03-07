// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>


namespace erbsland::cterm {

/// Anchor describes a location inside a rectangular area using a combination of a vertical and a horizontal component.
/// - Vertical components occupy the lower two bits (VMask).
/// - Horizontal components occupy the next two bits (HMask).
/// Common composite anchors (e.g. TopLeft, Center) are provided for convenience.
enum class Anchor : uint8_t {
    /// Top aligned vertically.
    Top = 0U << 0,
    /// Vertically centered.
    VCenter = 1U << 0,
    /// Bottom aligned vertically.
    Bottom = 2U << 0,

    /// Left aligned horizontally.
    Left = 0U << 2,
    /// Horizontally centered.
    HCenter = 1U << 2,
    /// Right aligned horizontally.
    Right = 2U << 2,

    // Combined anchors
    TopLeft = Top | Left,
    TopCenter = Top | HCenter,
    TopRight = Top | Right,
    CenterLeft = VCenter | Left,
    Center = VCenter | HCenter,
    CenterRight = VCenter | Right,
    BottomLeft = Bottom | Left,
    BottomCenter = Bottom | HCenter,
    BottomRight = Bottom | Right,

    /// Mask for extracting the vertical component.
    VMask = 0b11U,
    /// Mask for extracting the horizontal component.
    HMask = 0b1100U,
};

/// Bitwise OR for combining anchors.
/// Example: Anchor::Top | Anchor::HCenter -> TopCenter
[[nodiscard]] constexpr auto operator|(const Anchor lhs, const Anchor rhs) noexcept -> Anchor {
    return static_cast<Anchor>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

/// Bitwise AND for masking/extracting components of an anchor.
/// Example: (anchor & Anchor::VMask) yields the vertical component.
[[nodiscard]] constexpr auto operator&(const Anchor lhs, const Anchor rhs) noexcept -> Anchor {
    return static_cast<Anchor>(static_cast<int>(lhs) & static_cast<int>(rhs));
}


}
