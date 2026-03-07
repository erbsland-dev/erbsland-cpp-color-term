// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "ColorPart.hpp"


#include <stdexcept>


namespace erbsland::cterm {


auto ColorBase::enumFromString(std::string_view str) -> Value {
    static const std::unordered_map<std::string_view, Value> strMap = {
        {"black", Value::Black},
        {"red", Value::Red},
        {"green", Value::Green},
        {"yellow", Value::Yellow},
        {"blue", Value::Blue},
        {"magenta", Value::Magenta},
        {"cyan", Value::Cyan},
        {"white", Value::White},
        {"default", Value::Default},
        {"bright_black", Value::BrightBlack},
        {"bright_red", Value::BrightRed},
        {"bright_green", Value::BrightGreen},
        {"bright_yellow", Value::BrightYellow},
        {"bright_blue", Value::BrightBlue},
        {"bright_magenta", Value::BrightMagenta},
        {"bright_cyan", Value::BrightCyan},
        {"bright_white", Value::BrightWhite},
    };
    auto it = strMap.find(str);
    if (it == strMap.end()) {
        throw std::invalid_argument("Unknown color '" + std::string(str) + "'.");
    }
    return it->second;
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
