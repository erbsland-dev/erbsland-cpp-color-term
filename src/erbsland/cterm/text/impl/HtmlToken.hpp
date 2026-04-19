// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "HtmlTokenType.hpp"

#include <string>
#include <utility>

namespace erbsland::cterm::text::impl {

/// One token produced by the tokenizer.
struct HtmlToken {
    HtmlTokenType type{HtmlTokenType::End}; ///< The token type.
    std::u32string value;                   ///< The token value.
};

/// Swap two tokens without copying their payload.
inline void swap(HtmlToken &left, HtmlToken &right) noexcept {
    using std::swap;

    swap(left.type, right.type);
    swap(left.value, right.value);
}

}
