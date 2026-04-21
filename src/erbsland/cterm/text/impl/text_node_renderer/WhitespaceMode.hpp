// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <cstdint>

namespace erbsland::cterm::text::impl::text_node_renderer {

/// Whitespace handling strategy for one render pass.
enum class WhitespaceMode : uint8_t {
    Preserve,  ///< Keep rendered inline whitespace unchanged.
    TrimOuter, ///< Trim leading and trailing collapsible inline whitespace once while rendering.
};

}
