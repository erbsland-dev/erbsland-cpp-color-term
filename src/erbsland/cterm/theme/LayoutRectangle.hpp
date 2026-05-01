// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../geometry/Rectangle.hpp"

namespace erbsland::cterm::theme {

/// Rectangle set for a themed layout with margins and padding.
struct LayoutRectangles final {
    Rectangle outerRect;   ///< The complete assigned rectangle, including margins.
    Rectangle partRect;    ///< The rectangle owned and painted by the themed part.
    Rectangle contentRect; ///< The rectangle inside the themed part padding.
};

}
