// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "LayoutData.hpp"
#include "SurfacePtr.hpp"
#include "ui_namespace.hpp"

#include <cstddef>
#include <limits>

namespace erbsland::cterm::ui {

/// Policy and notification interface for child surface changes.
class SurfaceManager {
public:
    // defaults
    virtual ~SurfaceManager() = default;

public:
    /// Get the maximum number of child surfaces accepted by this manager.
    [[nodiscard]] virtual auto maximumSurfaceCount() const noexcept -> std::size_t {
        return std::numeric_limits<std::size_t>::max();
    }
    /// Create default layout data for a child surface added through generic APIs.
    [[nodiscard]] virtual auto defaultLayoutData(const SurfacePtr &surface) const -> LayoutDataPtr;
    /// Called before a child surface is added.
    virtual void willAdd(const SurfacePtr &surface, std::size_t index, const LayoutDataPtr &data);
    /// Called after a child surface was added.
    virtual void didAdd(const SurfacePtr &surface, std::size_t index, const LayoutDataPtr &data) noexcept;
    /// Called before a child surface is removed.
    virtual void willRemove(const SurfacePtr &surface, std::size_t index, const LayoutDataPtr &data);
    /// Called after a child surface was removed.
    virtual void didRemove(const SurfacePtr &surface, std::size_t index, const LayoutDataPtr &data) noexcept;
    /// Called before all child surfaces are removed.
    virtual void willRemoveAll(std::size_t count);
    /// Called after all child surfaces were removed.
    virtual void didRemoveAll() noexcept;
    /// Called before a child surface is replaced.
    virtual void willReplace(
        const SurfacePtr &oldSurface,
        const SurfacePtr &newSurface,
        std::size_t index,
        const LayoutDataPtr &oldData,
        const LayoutDataPtr &newData);
    /// Called after a child surface was replaced.
    virtual void didReplace(
        const SurfacePtr &oldSurface,
        const SurfacePtr &newSurface,
        std::size_t index,
        const LayoutDataPtr &oldData,
        const LayoutDataPtr &newData) noexcept;
    /// Called before a child surface is moved.
    virtual void willMove(const SurfacePtr &surface, std::size_t fromIndex, std::size_t toIndex);
    /// Called after a child surface was moved.
    virtual void didMove(const SurfacePtr &surface, std::size_t fromIndex, std::size_t toIndex) noexcept;
};

}
