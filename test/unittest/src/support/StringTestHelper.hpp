// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "TestHelper.hpp"

#include <format>
#include <stdexcept>
#include <string>
#include <vector>

class StringTestHelper : public TestHelper {
public:
    [[nodiscard]] auto render(const String &text) -> std::string {
        auto result = std::string{};
        for (const auto &character : text) {
            character.appendTo(result);
        }
        return result;
    }

    [[nodiscard]] auto render(const StringView text) -> std::string {
        auto result = std::string{};
        for (const auto &character : text) {
            character.appendTo(result);
        }
        return result;
    }

    [[nodiscard]] auto render(const std::u32string &text) -> std::string {
        auto result = std::string{};
        for (const auto character : text) {
            Char{character}.appendTo(result);
        }
        return result;
    }

    [[nodiscard]] auto renderLines(const StringLines &lines) -> std::vector<std::string> {
        auto result = std::vector<std::string>{};
        result.reserve(lines.size());
        for (const auto &line : lines) {
            result.push_back(render(line));
        }
        return result;
    }

    [[nodiscard]] auto renderWords(const std::vector<String> &words) -> std::vector<std::string> {
        auto result = std::vector<std::string>{};
        result.reserve(words.size());
        for (const auto &word : words) {
            result.push_back(render(word));
        }
        return result;
    }

    [[nodiscard]] auto renderWords(const std::vector<StringView> &words) -> std::vector<std::string> {
        auto result = std::vector<std::string>{};
        result.reserve(words.size());
        for (const auto word : words) {
            result.push_back(render(word));
        }
        return result;
    }

    [[nodiscard]] auto toPlainText(const String &text) -> std::string {
        auto result = std::string{};
        for (const auto &character : text) {
            result += character.charStr();
        }
        return result;
    }

    void requireStringEqual(const String &actual, const std::u32string_view expected) {
        if (actual.size() != expected.size()) {
            throw std::runtime_error(
                std::format("String length mismatch: actual={} expected={}", actual.size(), expected.size()));
        }
        for (std::size_t index = 0; index < expected.size(); ++index) {
            if (actual[index] != expected[index]) {
                throw std::runtime_error(std::format("String mismatch at index {}", index));
            }
        }
    }
};
