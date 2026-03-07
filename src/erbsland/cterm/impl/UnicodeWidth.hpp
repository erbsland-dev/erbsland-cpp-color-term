// Copyright © 2026 by Tobias Erbsland / Erbsland DEV.
// SPDX-License-Identifier: CC-BY-4.0
#pragma once


#include <cstdint>
#include <span>
#include <string_view>


namespace erbsland::cterm::impl {


/// Width information for a contiguous Unicode range.
struct CharWidth {
    char32_t begin;
    char32_t end;
    uint8_t width;
};

/// Get the generated Unicode width table.
/// @return A sorted table of contiguous Unicode ranges with console widths.
[[nodiscard]] auto charWidthTable() noexcept -> std::span<const CharWidth>;

/// Get the terminal cell width for a single Unicode code-point.
/// @param codePoint The Unicode code-point.
/// @return The width in console cells: `0`, `1` or `2`.
[[nodiscard]] auto consoleCharacterWidth(char32_t codePoint) noexcept -> uint8_t;

/// Calculate the display width of the given string.
/// @param charStr The string to calculate the display width for.
/// @return The number of "blocks" on the terminal.
[[nodiscard]] auto calculateDisplayWidth(std::string_view charStr) -> uint32_t;


}
