// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/TestHelper.hpp"

#include <erbsland/cterm/text/impl/HtmlParser.hpp>

#include <initializer_list>
#include <string_view>
#include <vector>

class HtmlParserTestSupport : public UNITTEST_SUBCLASS(TestHelper) {
public:
    [[nodiscard]] auto parse(const std::string_view html) -> text::TextNodePtr {
        auto parser = textimpl::HtmlParser{html};
        return parser.parse();
    }

    void requireTreeEqual(const text::TextNodePtr &root, const std::initializer_list<std::string_view> expectedLines) {
        auto expected = std::vector<std::string>{};
        expected.reserve(expectedLines.size());
        for (const auto line : expectedLines) {
            expected.emplace_back(line);
        }
        REQUIRE_EQUAL_LINES(root->toDiagnosticTree(2), expected);
    }
};
