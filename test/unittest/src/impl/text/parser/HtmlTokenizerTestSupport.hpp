// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "HtmlTokenizerExpectedToken.hpp"

#include "support/StringTestHelper.hpp"

#include <format>
#include <initializer_list>
#include <utility>
#include <vector>

class HtmlTokenizerTestSupport : public StringTestHelper {
public:
    [[nodiscard]] auto tokenize(const std::string_view text) -> std::vector<textimpl::HtmlTokenizer::Token> {
        auto result = std::vector<textimpl::HtmlTokenizer::Token>{};
        auto tokenizer = textimpl::HtmlTokenizer{text};
        for (auto token : tokenizer.tokenize()) {
            result.push_back(std::move(token));
        }
        return result;
    }

    void requireTokensEqual(
        const std::vector<textimpl::HtmlTokenizer::Token> &actual,
        const std::initializer_list<HtmlTokenizerExpectedToken> expected) {
        REQUIRE_EQUAL(actual.size(), expected.size());
        auto index = std::size_t{0};
        for (const auto &expectedToken : expected) {
            runWithContext(
                SOURCE_LOCATION(),
                [&]() {
                    REQUIRE_EQUAL(actual[index].type, expectedToken.type);
                    REQUIRE_EQUAL(actual[index].value, expectedToken.value);
                },
                [&]() -> std::string {
                    return std::format(
                        "index = {} / actualType = {} / actualValue = {} / expectedType = {} / expectedValue = {}",
                        index,
                        static_cast<int>(actual[index].type),
                        render(actual[index].value),
                        static_cast<int>(expectedToken.type),
                        render(expectedToken.value));
                });
            index += 1;
        }
    }
};
