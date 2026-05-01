// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "ColorTermIncludes.hpp"

#include <erbsland/cterm/all.hpp>
#include <erbsland/cterm/impl/UnicodeWidth.hpp>
#include <erbsland/unittest/TextHelper.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <cstdint>
#include <initializer_list>
#include <string>

class TestHelper : public el::UnitTest {
public:
    [[nodiscard]] static auto zeroThemeContext() noexcept -> ui::ThemeContext {
        return ui::ThemeContext{theme::Theme::zero()};
    }

    [[nodiscard]] static auto zeroLayoutContext() noexcept -> ui::LayoutContext {
        return ui::LayoutContext{zeroThemeContext()};
    }

    [[nodiscard]] static auto bytes(const std::initializer_list<uint8_t> values) -> std::string {
        auto result = std::string{};
        result.reserve(values.size());
        for (const auto value : values) {
            result.push_back(static_cast<char>(value));
        }
        return result;
    }
};
