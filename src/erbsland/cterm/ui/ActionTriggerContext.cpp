// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ActionTriggerContext.hpp"

#include <utility>

namespace erbsland::cterm::ui {

ActionTriggerContext::ActionTriggerContext(Key key, SurfaceWeakPtr sourceSurface) noexcept :
    _key{std::move(key)}, _sourceSurface{std::move(sourceSurface)} {
}

}
