// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <cstdint>

namespace erbsland::cterm::text {

/// The list kind for list-specific selectors.
enum class StyleListKind : uint8_t {
    Any,      ///< Match any list kind or non-list roles.
    Bullet,   ///< Match bullet lists and bullet list items.
    Numbered, ///< Match numbered lists and numbered list items.
};

}
