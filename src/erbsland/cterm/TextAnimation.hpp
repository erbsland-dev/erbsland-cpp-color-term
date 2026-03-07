// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>


namespace erbsland::cterm {


/// Supported text animation styles used by `Buffer::renderText()`.
enum class TextAnimation : uint8_t {
    None,          ///< No animation.
    ColorDiagonal, ///< Diagonal color animation. Requires a color sequence.
};


}
