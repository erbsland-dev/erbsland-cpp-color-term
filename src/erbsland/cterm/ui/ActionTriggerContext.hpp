// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "SurfacePtr.hpp"

#include "../Key.hpp"

namespace erbsland::cterm::ui {

/// Context passed to an action when it is triggered.
class ActionTriggerContext final {
public:
    /// Create a trigger context.
    /// @param key The key that triggered the action.
    /// @param sourceSurface The surface whose action container triggered the action.
    explicit ActionTriggerContext(Key key, SurfaceWeakPtr sourceSurface = {}) noexcept;

public:
    /// Get the key that triggered the action.
    [[nodiscard]] auto key() const noexcept -> const Key & { return _key; }
    /// Get the surface whose action container triggered the action.
    [[nodiscard]] auto sourceSurface() const noexcept -> const SurfaceWeakPtr & { return _sourceSurface; }

private:
    Key _key;                      ///< The key that triggered the action.
    SurfaceWeakPtr _sourceSurface; ///< The source surface.
};

}
