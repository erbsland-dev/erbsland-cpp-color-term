// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "ColorPart.hpp"


#include "impl/TextUtil.hpp"

#include <algorithm>
#include <ranges>
#include <stdexcept>


namespace erbsland::cterm {


auto ColorBase::tableEntry() const noexcept -> const TableEntry & {
    return colorTable()[static_cast<std::size_t>(_value)];
}


auto ColorBase::colorTable() noexcept -> const ColorTable & {
    static const auto table = ColorTable{{
        {Value::Black, 0, "black"},
        {Value::Red, 1, "red"},
        {Value::Green, 2, "green"},
        {Value::Yellow, 3, "yellow"},
        {Value::Blue, 4, "blue"},
        {Value::Magenta, 5, "magenta"},
        {Value::Cyan, 6, "cyan"},
        {Value::White, 7, "white"},
        {Value::BrightBlack, 60, "bright_black"},
        {Value::BrightRed, 61, "bright_red"},
        {Value::BrightGreen, 62, "bright_green"},
        {Value::BrightYellow, 63, "bright_yellow"},
        {Value::BrightBlue, 64, "bright_blue"},
        {Value::BrightMagenta, 65, "bright_magenta"},
        {Value::BrightCyan, 66, "bright_cyan"},
        {Value::BrightWhite, 67, "bright_white"},
        {Value::Default, 9, "default"},
        {Value::Inherited, 9, "inherited"},
    }};
    return table;
}


auto ColorBase::toString() const -> std::string {
    return std::string{tableEntry().name};
}


auto ColorBase::enumFromString(const std::string_view str) -> Value {
    auto normalizedIdentifier = impl::toNormalizedIdentifier(str);
    const auto &table = colorTable();
    auto it = std::ranges::find_if(table, [&](const auto &entry) { return entry.name == normalizedIdentifier; });
    if (it == table.end()) {
        throw std::invalid_argument("Unknown color '" + std::string(str) + "'.");
    }
    return it->value;
}


auto ColorBase::brighterEnum(Value value) -> Value {
    switch (value) {
    case Value::Black:
        return Value::BrightBlack;
    case Value::Red:
        return Value::BrightRed;
    case Value::Green:
        return Value::BrightGreen;
    case Value::Yellow:
        return Value::BrightYellow;
    case Value::Blue:
        return Value::BrightBlue;
    case Value::Magenta:
        return Value::BrightMagenta;
    case Value::Cyan:
        return Value::BrightCyan;
    case Value::White:
        return Value::BrightWhite;
    default:
        return value;
    }
}


}
