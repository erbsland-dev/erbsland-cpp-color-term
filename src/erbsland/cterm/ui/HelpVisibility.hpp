// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <cstdint>

namespace erbsland::cterm::ui {

/// Visibility of action help metadata.
enum class HelpVisibility : uint8_t {
    Hidden,   ///< Hide this action from generated help.
    Footer,   ///< Show this action only in compact footer help.
    HelpPage, ///< Show this action only in detailed help.
    All,      ///< Show this action in compact footer help and detailed help.
};

}
