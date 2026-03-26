// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../Color.hpp"

#include <concepts>
#include <type_traits>


namespace erbsland::cterm {


class Char;
class CharAttributes;
class CharStyle;
class String;


/// A value accepted by `Terminal::print()` and `Terminal::printLine()` or `String::append()`.
template <typename T>
concept PrintableArg = std::constructible_from<Foreground, T> || std::constructible_from<Background, T> ||
    std::same_as<Color, T> || std::same_as<CharAttributes, T> || std::same_as<CharStyle, T> || std::same_as<Char, T> ||
    std::same_as<String, T> || std::same_as<std::decay_t<T>, std::string> ||
    std::same_as<std::decay_t<T>, std::string_view> || std::same_as<T, const char *>;

template <typename T>
concept ColorArg = std::constructible_from<Color, T>;

/// Constructor arguments that can build a `Color` without matching the explicit `CharAttributes` overloads.
template <typename... T>
concept CharColorConstructorArgs = sizeof...(T) >= 1 && std::constructible_from<Color, T...> &&
    (!std::same_as<std::remove_cvref_t<T>, CharAttributes> && ...);


}
