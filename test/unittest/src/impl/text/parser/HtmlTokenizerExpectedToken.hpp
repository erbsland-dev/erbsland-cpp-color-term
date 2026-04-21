// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/ColorTermIncludes.hpp"

#include <erbsland/cterm/text/impl/HtmlTokenizer.hpp>

#include <string>

struct HtmlTokenizerExpectedToken final {
    textimpl::HtmlTokenizer::TokenType type;
    std::u32string value;
};
