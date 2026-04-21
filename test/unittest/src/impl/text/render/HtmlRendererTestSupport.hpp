// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/BufferTestHelper.hpp"

#include <format>
#include <string>

class HtmlRendererTestSupport : public BufferTestHelper {
public:
    [[nodiscard]] auto makeOrderedListHtml(const std::size_t itemCount) -> std::string {
        auto html = std::string{"<ol>"};
        html.reserve(itemCount * 20 + 16);
        for (auto index = std::size_t{1}; index <= itemCount; ++index) {
            html += std::format("<li>Item {}</li>", index);
        }
        html += "</ol>";
        return html;
    }
};
