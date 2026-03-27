// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstddef>


namespace erbsland::cterm::impl::paragraph {


/// A contiguous range in the source string representing one logical input line.
struct LayoutSourceLineRange final {
    std::size_t startIndex = 0; ///< The first character index in the source text.
    std::size_t length = 0;     ///< The number of characters in the source line.
};


}
