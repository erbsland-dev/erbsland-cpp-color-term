// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Layout.hpp"

namespace erbsland::cterm::ui::layout {

/// Base class for layouts that manage one optional content surface.
class SingleContentLayout : public Layout {
protected:
    /// Create a single-content layout.
    SingleContentLayout() noexcept = default;

public:
    ~SingleContentLayout() override = default;

public:
    /// Replace the content surface.
    /// @param contentSurface The new content surface, or an empty pointer to clear it.
    void setContentSurface(SurfacePtr contentSurface);
    /// Access the current content surface.
    /// @return The current content surface.
    [[nodiscard]] auto contentSurface() const noexcept -> const SurfacePtr &;

protected:
    /// React to a content surface change.
    /// @param oldContentSurface The previous content surface.
    /// @param newContentSurface The new content surface.
    virtual void
    onContentSurfaceChanged(const SurfacePtr &oldContentSurface, const SurfacePtr &newContentSurface) noexcept;

protected: // implement SurfaceManager
    [[nodiscard]] auto maximumSurfaceCount() const noexcept -> std::size_t override;
    void didAdd(const SurfacePtr &surface, std::size_t index, const LayoutDataPtr &data) noexcept override;
    void didRemove(const SurfacePtr &surface, std::size_t index, const LayoutDataPtr &data) noexcept override;
    void didReplace(
        const SurfacePtr &oldSurface,
        const SurfacePtr &newSurface,
        std::size_t index,
        const LayoutDataPtr &oldData,
        const LayoutDataPtr &newData) noexcept override;

private:
    /// Replace the cached content surface and notify subclasses.
    void updateContentSurface(SurfacePtr contentSurface) noexcept;

private:
    SurfacePtr _contentSurface; ///< The optional content surface.
};

}
