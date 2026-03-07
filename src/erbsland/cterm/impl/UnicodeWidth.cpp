// Copyright © 2026 by Tobias Erbsland / Erbsland DEV.
// SPDX-License-Identifier: CC-BY-4.0
#include "UnicodeWidth.hpp"

#include "U8Buffer.hpp"

#include <algorithm>


namespace erbsland::cterm::impl {


auto consoleCharacterWidth(const char32_t codePoint) noexcept -> uint8_t {
    const auto table = charWidthTable();
    const auto match = std::ranges::lower_bound(table, codePoint, {}, &CharWidth::end);
    if (match != table.end() && match->begin <= codePoint) {
        return match->width;
    }
    return 1;
}


auto calculateDisplayWidth(const std::string_view charStr) -> uint32_t {
    uint32_t result = 0;
    U8Buffer{charStr}.decodeAll(
        [&](const char32_t codePoint) -> void { result += static_cast<uint32_t>(consoleCharacterWidth(codePoint)); });
    return result;
}


}
