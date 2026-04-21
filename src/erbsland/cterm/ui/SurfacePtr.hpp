// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <memory>

namespace erbsland::cterm::ui {

class Surface;
/// Shared pointer to a surface.
using SurfacePtr = std::shared_ptr<Surface>;
/// Weak pointer to a surface.
using SurfaceWeakPtr = std::weak_ptr<Surface>;

}
