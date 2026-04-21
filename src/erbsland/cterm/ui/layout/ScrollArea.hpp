// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Viewport.hpp"

#include "../surface/AbstractScrollArea.hpp"

namespace erbsland::cterm::ui::layout {

class ScrollArea;
using ScrollAreaPtr = std::shared_ptr<ScrollArea>;

/// A scroll area layout that displays one content surface with optional scroll bars.
class ScrollArea final : public surface::AbstractScrollArea {
public:
    /// Create a scroll area.
    explicit ScrollArea(ProtectedTag) noexcept;
    ~ScrollArea() override = default;

public:
    /// Create a scroll area.
    /// @return The new scroll area.
    [[nodiscard]] static auto create() -> ScrollAreaPtr;

public:
    /// Replace the content surface.
    /// @param contentSurface The new content surface, or an empty pointer to clear it.
    void setContentSurface(SurfacePtr contentSurface);
    /// Access the current content surface.
    /// @return The current content surface.
    [[nodiscard]] auto contentSurface() const noexcept -> const SurfacePtr &;
    /// Access the viewport.
    /// @return The viewport.
    [[nodiscard]] auto viewport() const noexcept -> const ViewportPtr &;
    /// Get the content alignment for undersized content.
    /// @return The content alignment.
    [[nodiscard]] auto alignment() const noexcept -> Alignment;
    /// Replace the content alignment for undersized content.
    /// @param alignment The new alignment.
    void setAlignment(Alignment alignment) noexcept;

public: // implement Surface
    void onLayout(LayoutScope &scope) noexcept override;

protected: // implement AbstractScrollArea
    [[nodiscard]] auto isManagedScrollAreaChild(const SurfacePtr &surface) const noexcept -> bool override;
    [[nodiscard]] auto contentSizeForViewport(Size viewportSize) const noexcept -> Size override;
    [[nodiscard]] auto measureContentSizeForViewport(Size viewportSize, LayoutScope &scope) noexcept -> Size override;
    void onPaintArea(
        Position scrollOffset,
        Rectangle targetRect,
        WritableBuffer &buffer,
        const PaintContext &context) noexcept override;
    void onScrollOffsetChanged(Position scrollOffset) noexcept override;

private:
    /// Add the internal child surfaces.
    void initializeUi();

private:
    ViewportPtr _viewport; ///< The viewport child surface.
};

}
