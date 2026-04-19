// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <erbsland/cterm/all.hpp>
#include <erbsland/cterm/impl/UnicodeWidth.hpp>
#include <erbsland/unittest/TextHelper.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <cstdint>
#include <initializer_list>
#include <string>

// import everything into the global namespace
using namespace erbsland::cterm;

// create an alias to resolve name conflicts.
namespace term = erbsland::cterm;

class TestHelper : public el::UnitTest {
public:
    [[nodiscard]] static auto bytes(const std::initializer_list<uint8_t> values) -> std::string {
        auto result = std::string{};
        result.reserve(values.size());
        for (const auto value : values) {
            result.push_back(static_cast<char>(value));
        }
        return result;
    }
};
