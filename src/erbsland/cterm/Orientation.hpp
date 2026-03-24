// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>


namespace erbsland::cterm {

/// Represents the orientation of a layout or a direction.
enum class Orientation : uint8_t {
    Horizontal, ///< Horizontal orientation, e.g. for horizontal layouts or horizontal scrolling.
    Vertical,   ///< Vertical orientation, e.g. for vertical layouts or vertical scrolling.
};


}
