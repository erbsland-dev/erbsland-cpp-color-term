// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <cstdint>

namespace erbsland::cterm::ui {

/// Format used for detailed help descriptions.
enum class HelpFormat : uint8_t {
    Text, ///< Plain text that must be escaped before embedding in HTML help pages.
    Html, ///< HTML fragment that can be embedded directly in HTML help pages.
};

}
