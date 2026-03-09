// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../Input.hpp"

#include <memory>


namespace erbsland::cterm::impl {


/// Create the platform-specific input backend for the current process.
[[nodiscard]] auto createInputForPlatform() -> std::unique_ptr<Input>;


}
