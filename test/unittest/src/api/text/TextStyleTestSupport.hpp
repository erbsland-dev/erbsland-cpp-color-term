// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/StringTestHelper.hpp"

#include <erbsland/cterm/text/Style.hpp>

#include <span>
#include <vector>

class TextStyleTestSupport : public StringTestHelper {
public:
    [[nodiscard]] auto
    matchContext(const text::StyleSelector &selector, const std::initializer_list<std::string> styleTokens = {})
        -> text::StyleMatchContext {
        _tokens = std::vector<std::string>{styleTokens};
        return text::StyleMatchContext{
            selector.styleRole(), selector.level(), selector.listKind(), std::span<const std::string>{_tokens}};
    }

private:
    std::vector<std::string> _tokens;
};
