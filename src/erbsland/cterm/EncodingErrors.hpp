// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>


namespace erbsland::cterm {

/// How to handle encoding or Unicode normalization errors.
enum class EncodingErrors : uint8_t {
    Throw = 0, ///< Throw an `std::invalid_argument` when invalid input is encountered.
    Ignore,    ///< Skip unsupported input when the calling API allows lossy recovery.
    Replace,   ///< Replace unsupported input with the Unicode replacement character when supported.
};

}
