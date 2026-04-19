// Copyright © 2026 by Tobias Erbsland / Erbsland DEV.
// SPDX-License-Identifier: CC-BY-4.0
#include "UnicodeWidth.hpp"

#include "U8Buffer.hpp"

#include <algorithm>

namespace erbsland::cterm::impl {

auto consoleCharacterWidth(const char32_t codePoint) noexcept -> uint8_t {
    // Fast-path frequently used narrow ranges to avoid the table lookup in common terminal text.
    if (codePoint <= 0x1fU) {
        return 0;
    }
    if (codePoint <= 0x7eU) {
        return 1;
    }
    if (codePoint <= 0x9fU) {
        return 0;
    }
    if (codePoint == 0xadU) {
        return 0;
    }
    if (codePoint <= 0x24fU) {
        return 1;
    }
    if (codePoint >= 0x2500U && codePoint <= 0x25fbU) {
        return 1;
    }

    const auto table = charWidthTable();
    const auto match = std::ranges::lower_bound(table, codePoint, {}, &CharWidth::end);
    if (match != table.end() && match->begin <= codePoint) {
        return match->width;
    }
    return 1;
}

auto calculateDisplayWidth(const std::string_view charStr) noexcept -> uint32_t {
    uint32_t result = 0;
    U8Buffer{charStr}.decodeAllReplacingErrors([&](const char32_t codePoint) noexcept -> void {
        result += static_cast<uint32_t>(consoleCharacterWidth(codePoint));
    });
    return result;
}

}
