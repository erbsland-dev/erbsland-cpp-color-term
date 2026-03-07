// Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <string>


/// Example Color class for unit test templates.
class Color {
public:
    enum Name {
        Red,
        Orange,
        Yellow,
        Green,
        Blue,
        Violet,
        Black,
        White,
    };

public:
    Color() = default;
    Color(Name name);
    Color(double r, double g, double b);

public:
    auto operator==(const Color &other) const -> bool;
    auto operator!=(const Color &other) const -> bool;

public:
    auto undefined() const -> bool;
    auto r() const -> double;
    auto g() const -> double;
    auto b() const -> double;

public:
    auto toString() const -> std::string;
    static auto fromString(const std::string &text) -> Color;
};