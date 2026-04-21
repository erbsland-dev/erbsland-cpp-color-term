// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/TestHelper.hpp"

#include <erbsland/cterm/impl/U8Buffer.hpp>

#include <vector>

class U8BufferTestSupport : public TestHelper {
public:
    [[nodiscard]] auto decodeAll(const std::string_view text, const EncodingErrors encodingErrors)
        -> std::vector<char32_t> {
        auto result = std::vector<char32_t>{};
        impl::U8Buffer<const char>{text}.decodeAll(
            [&](const char32_t codePoint) -> void { result.push_back(codePoint); }, encodingErrors);
        return result;
    }
};
