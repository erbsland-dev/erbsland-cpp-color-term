// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Coordinate.hpp"

#include "../impl/HashHelper.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>

namespace erbsland::cterm {

/// Alignment of text or graphics in a box.
class Alignment final {
public:
    /// The enum values for alignment.
    enum class Enum : uint8_t {
        Left = 1U << 0,    ///< Aligned to the left edge of the box.
        HCenter = 1U << 1, ///< Aligned to the horizontal center of the box.
        Right = 1U << 2,   ///< Aligned to the right edge of the box.
        Top = 1U << 4,     ///< Aligned to the top edge of the box.
        VCenter = 1U << 5, ///< Aligned to the vertical center of the box.
        Bottom = 1U << 6,  ///< Aligned to the bottom edge of the box.
        TopLeft = static_cast<uint8_t>(Top) | static_cast<uint8_t>(Left),
        TopCenter = static_cast<uint8_t>(Top) | static_cast<uint8_t>(HCenter),
        TopRight = static_cast<uint8_t>(Top) | static_cast<uint8_t>(Right),
        CenterLeft = static_cast<uint8_t>(VCenter) | static_cast<uint8_t>(Left),
        Center = static_cast<uint8_t>(VCenter) | static_cast<uint8_t>(HCenter),
        CenterRight = static_cast<uint8_t>(VCenter) | static_cast<uint8_t>(Right),
        BottomLeft = static_cast<uint8_t>(Bottom) | static_cast<uint8_t>(Left),
        BottomCenter = static_cast<uint8_t>(Bottom) | static_cast<uint8_t>(HCenter),
        BottomRight = static_cast<uint8_t>(Bottom) | static_cast<uint8_t>(Right),
        HorizontalMask = static_cast<uint8_t>(Left) | static_cast<uint8_t>(HCenter) | static_cast<uint8_t>(Right),
        VerticalMask = static_cast<uint8_t>(Top) | static_cast<uint8_t>(VCenter) | static_cast<uint8_t>(Bottom),
    };

public:
    static const Alignment Left;           ///< Aligned to the left edge of the box.
    static const Alignment HCenter;        ///< Aligned to the horizontal center of the box.
    static const Alignment Right;          ///< Aligned to the right edge of the box.
    static const Alignment Top;            ///< Aligned to the top edge of the box.
    static const Alignment VCenter;        ///< Aligned to the vertical center of the box.
    static const Alignment Bottom;         ///< Aligned to the bottom edge of the box.
    static const Alignment TopLeft;        ///< Aligned to the top-left corner of the box.
    static const Alignment TopCenter;      ///< Aligned to the top-center of the box.
    static const Alignment TopRight;       ///< Aligned to the top-right corner of the box.
    static const Alignment CenterLeft;     ///< Aligned to the center-left of the box.
    static const Alignment Center;         ///< Aligned to the center of the box.
    static const Alignment CenterRight;    ///< Aligned to the center-right of the box.
    static const Alignment BottomLeft;     ///< Aligned to the bottom-left corner of the box.
    static const Alignment BottomCenter;   ///< Aligned to the bottom-center of the box.
    static const Alignment BottomRight;    ///< Aligned to the bottom-right corner of the box.
    static const Alignment HorizontalMask; ///< Mask for extracting the horizontal component.
    static const Alignment VerticalMask;   ///< Mask for extracting the vertical component.

public:
    /// Create a top-left alignment.
    constexpr Alignment() noexcept = default;

    /// Create an alignment from an enum value.
    constexpr Alignment(const Enum value) noexcept : _value{value} {} // NOLINT(*-explicit-constructor)

    /// Default destructor.
    ~Alignment() = default;
    /// Default copy constructor.
    Alignment(const Alignment &) = default;
    /// Default copy assignment.
    auto operator=(const Alignment &) -> Alignment & = default;

public: // operators
    /// Assign an enum value.
    auto operator=(const Enum value) noexcept -> Alignment & {
        _value = value;
        return *this;
    }
    /// Convert to the enum value.
    constexpr operator Enum() const noexcept { return _value; } // NOLINT(*-explicit-constructor)
    /// Compare two alignments.
    constexpr auto operator==(const Alignment &) const noexcept -> bool = default;
    /// Compare with an enum value.
    [[nodiscard]] constexpr auto operator==(const Enum value) const noexcept -> bool { return _value == value; }
    /// Compare an enum value with an alignment.
    [[nodiscard]] friend constexpr auto operator==(const Enum value, const Alignment &alignment) noexcept -> bool {
        return alignment == value;
    }
    /// Bitwise OR for combining alignments.
    /// Example: Alignment::Top | Alignment::HCenter -> TopCenter
    [[nodiscard]] friend constexpr auto operator|(const Alignment lhs, const Alignment rhs) noexcept -> Alignment {
        return Alignment{static_cast<Enum>(lhs.value() | rhs.value())};
    }
    /// Bitwise AND for masking/extracting components of an alignment.
    /// Example: (alignment & Alignment::VerticalMask) yields the vertical component.
    [[nodiscard]] friend constexpr auto operator&(const Alignment lhs, const Alignment rhs) noexcept -> Alignment {
        return Alignment{static_cast<Enum>(lhs.value() & rhs.value())};
    }

public: // tests
    /// Test if this alignment is on the left side.
    [[nodiscard]] constexpr auto isLeft() const noexcept -> bool { return horizontal() == Enum::Left; }
    /// Test if this alignment is horizontally centered.
    [[nodiscard]] constexpr auto isHorizontalCenter() const noexcept -> bool { return horizontal() == Enum::HCenter; }
    /// Test if this alignment is on the right side.
    [[nodiscard]] constexpr auto isRight() const noexcept -> bool { return horizontal() == Enum::Right; }
    /// Test if this alignment is at the top.
    [[nodiscard]] constexpr auto isTop() const noexcept -> bool { return vertical() == Enum::Top; }
    /// Test if this alignment is vertically centered.
    [[nodiscard]] constexpr auto isVerticalCenter() const noexcept -> bool { return vertical() == Enum::VCenter; }
    /// Test if this alignment is at the bottom.
    [[nodiscard]] constexpr auto isBottom() const noexcept -> bool { return vertical() == Enum::Bottom; }

public:
    /// Extract the horizontal alignment component.
    [[nodiscard]] constexpr auto horizontal() const noexcept -> Alignment {
        return *this & Alignment{Enum::HorizontalMask};
    }
    /// Extract the vertical alignment component.
    [[nodiscard]] constexpr auto vertical() const noexcept -> Alignment {
        return *this & Alignment{Enum::VerticalMask};
    }
    /// Calculate the horizontal offset for content aligned in an available width.
    [[nodiscard]] constexpr auto
    horizontalOffset(const Coordinate availableWidth, const Coordinate contentWidth) const noexcept -> Coordinate {
        const auto freeSpace = availableWidth - contentWidth;
        if (isHorizontalCenter()) {
            return freeSpace / 2;
        }
        if (isRight()) {
            return freeSpace;
        }
        return 0;
    }
    /// Calculate the vertical offset for content aligned in an available height.
    [[nodiscard]] constexpr auto
    verticalOffset(const Coordinate availableHeight, const Coordinate contentHeight) const noexcept -> Coordinate {
        const auto freeSpace = availableHeight - contentHeight;
        if (isVerticalCenter()) {
            return freeSpace / 2;
        }
        if (isBottom()) {
            return freeSpace;
        }
        return 0;
    }
    /// Get a hash for this alignment.
    [[nodiscard]] constexpr auto hash() const noexcept -> std::size_t {
        return impl::hashCreate(static_cast<uint8_t>(_value));
    }

private:
    /// Get the raw enum value as integer.
    [[nodiscard]] constexpr auto value() const noexcept -> uint8_t { return static_cast<uint8_t>(_value); }

private:
    Enum _value{Enum::TopLeft}; ///< The internal enum value.
};

inline constexpr Alignment Alignment::Left{Alignment::Enum::Left};
inline constexpr Alignment Alignment::HCenter{Alignment::Enum::HCenter};
inline constexpr Alignment Alignment::Right{Alignment::Enum::Right};
inline constexpr Alignment Alignment::Top{Alignment::Enum::Top};
inline constexpr Alignment Alignment::VCenter{Alignment::Enum::VCenter};
inline constexpr Alignment Alignment::Bottom{Alignment::Enum::Bottom};
inline constexpr Alignment Alignment::TopLeft{Alignment::Enum::TopLeft};
inline constexpr Alignment Alignment::TopCenter{Alignment::Enum::TopCenter};
inline constexpr Alignment Alignment::TopRight{Alignment::Enum::TopRight};
inline constexpr Alignment Alignment::CenterLeft{Alignment::Enum::CenterLeft};
inline constexpr Alignment Alignment::Center{Alignment::Enum::Center};
inline constexpr Alignment Alignment::CenterRight{Alignment::Enum::CenterRight};
inline constexpr Alignment Alignment::BottomLeft{Alignment::Enum::BottomLeft};
inline constexpr Alignment Alignment::BottomCenter{Alignment::Enum::BottomCenter};
inline constexpr Alignment Alignment::BottomRight{Alignment::Enum::BottomRight};
inline constexpr Alignment Alignment::HorizontalMask{Alignment::Enum::HorizontalMask};
inline constexpr Alignment Alignment::VerticalMask{Alignment::Enum::VerticalMask};

}

template <>
struct std::hash<erbsland::cterm::Alignment> {
    auto operator()(const erbsland::cterm::Alignment &alignment) const noexcept -> std::size_t {
        return alignment.hash();
    }
};
