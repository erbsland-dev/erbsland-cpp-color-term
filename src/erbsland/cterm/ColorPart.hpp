// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ColorRole.hpp"

#include <string>
#include <string_view>
#include <unordered_map>


namespace erbsland::cterm {


/// Shared implementation for foreground and background color values.
class ColorBase {
public:
    /// Internal color identifiers used for ANSI conversion and parsing.
    enum class Value : uint8_t {
        Black = 0,
        Red = 1,
        Green = 2,
        Yellow = 3,
        Blue = 4,
        Magenta = 5,
        Cyan = 6,
        White = 7,
        Default = 9,
        BrightBlack = 60,
        BrightRed = 61,
        BrightGreen = 62,
        BrightYellow = 63,
        BrightBlue = 64,
        BrightMagenta = 65,
        BrightCyan = 66,
        BrightWhite = 67,
    };

protected:
    ColorBase() = default;
    constexpr explicit ColorBase(const Value value) : _value{value} {}

public: // operators
    /// Compare two color base values for equality.
    auto operator==(const ColorBase &) const noexcept -> bool = default;
    /// Compare two color base values for inequality.
    auto operator!=(const ColorBase &) const noexcept -> bool = default;

protected:
    /// Create a color enum from the given string.
    /// @return The color enum.
    /// @throws std::invalid_argument if the color does not exist.
    [[nodiscard]] static auto enumFromString(std::string_view str) -> Value;

    /// Create brighter enum
    [[nodiscard]] static auto brighterEnum(Value value) -> Value;

protected:
    Value _value{Value::Default};
};


/// A foreground or background color.
template <ColorRole tColorType>
class ColorPart : public ColorBase {
public:
    /// Base ANSI escape code for this color role.
    constexpr static auto cCodeBase = (tColorType == ColorRole::Foreground ? 30 : 40);

public: // public types
    /// Tag type used by the predefined named color constants.
    struct Hue {
        /// The encoded color value.
        Value value;
    };
    constexpr static auto Black = Hue{Value::Black};
    constexpr static auto Red = Hue{Value::Red};
    constexpr static auto Green = Hue{Value::Green};
    constexpr static auto Yellow = Hue{Value::Yellow};
    constexpr static auto Blue = Hue{Value::Blue};
    constexpr static auto Magenta = Hue{Value::Magenta};
    constexpr static auto Cyan = Hue{Value::Cyan};
    constexpr static auto White = Hue{Value::White};
    constexpr static auto Default = Hue{Value::Default};
    constexpr static auto BrightBlack = Hue{Value::BrightBlack};
    constexpr static auto BrightRed = Hue{Value::BrightRed};
    constexpr static auto BrightGreen = Hue{Value::BrightGreen};
    constexpr static auto BrightYellow = Hue{Value::BrightYellow};
    constexpr static auto BrightBlue = Hue{Value::BrightBlue};
    constexpr static auto BrightMagenta = Hue{Value::BrightMagenta};
    constexpr static auto BrightCyan = Hue{Value::BrightCyan};
    constexpr static auto BrightWhite = Hue{Value::BrightWhite};

public: // ctors/dtor/assign/move
    /// Create the default terminal color for this role.
    ColorPart() = default;
    /// Create a color from one of the predefined hue constants.
    /// @param color The named hue.
    constexpr ColorPart(const Hue color) : ColorBase{color.value} {}

public: // operators
    /// Compare two color parts for equality.
    auto operator==(const ColorPart &) const noexcept -> bool = default;
    /// Compare two color parts for inequality.
    auto operator!=(const ColorPart &) const noexcept -> bool = default;

public: // conversion
    /// Convert this color part to its ANSI SGR numeric code.
    /// @return The numeric ANSI code for this color role.
    [[nodiscard]] auto ansiCode() const noexcept -> int { return static_cast<int>(_value) + cCodeBase; }
    /// Create the bright variant of this color.
    /// @return The brightened color, or the unchanged color if no brighter variant exists.
    [[nodiscard]] auto brighter() const noexcept -> ColorPart { return ColorPart{Hue{brighterEnum(_value)}}; }

public: // tools
    /// Create a color from the given string.
    /// @param str The lowercase color name.
    /// @return The parsed color.
    /// @throws std::invalid_argument if the color does not exist.
    [[nodiscard]] static auto fromString(const std::string_view str) -> ColorPart {
        return ColorPart{Hue{enumFromString(str)}};
    }
};


/// The background color.
using Background = ColorPart<ColorRole::Background>;
/// Short alias for `Background`.
using bg = Background;

/// The foreground color.
using Foreground = ColorPart<ColorRole::Foreground>;
/// Short alias for `Foreground`.
using fg = Foreground;


}
