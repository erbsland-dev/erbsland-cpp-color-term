// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <cstdint>

namespace erbsland::cterm::ui {

/// Describes how a surface's focus relation changed.
enum class FocusChange : uint8_t {
    In,       ///< The surface gained direct keyboard focus.
    InWithin, ///< A descendant of the surface gained keyboard focus.
    Out,      ///< The surface left the focused path.
};

}
