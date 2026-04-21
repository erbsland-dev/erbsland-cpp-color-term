// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "ui_namespace.hpp"

#include <cstdint>

namespace erbsland::cterm::ui {

/// Visibility mode for scroll bars in scrollable surfaces.
enum class ScrollBarMode : uint8_t {
    Hidden,    ///< The scroll bar is hidden and consumes no space.
    Automatic, ///< The scroll bar is visible only when content overflows.
    Visible,   ///< The scroll bar is visible whenever the surface size permits it.
};

}
