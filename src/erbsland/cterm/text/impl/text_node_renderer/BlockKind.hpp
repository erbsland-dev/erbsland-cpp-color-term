// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <cstdint>

namespace erbsland::cterm::text::impl::text_node_renderer {

/// The supported physical block kinds emitted by the planner.
enum class BlockKind : uint8_t {
    Paragraph,      ///< A regular paragraph-like text block.
    HorizontalRule, ///< A horizontal separator line.
    FilledLine,     ///< A line filled with a character and optional overlay text.
};

}
