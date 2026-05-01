// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Layout.hpp"

#include <vector>

namespace erbsland::cterm::ui::layout {

class Pages;
using PagesPtr = std::shared_ptr<Pages>;

/// A layout that displays one visible child surface at a time.
class Pages final : public Layout {
public:
    /// Create a pages layout.
    explicit Pages(ProtectedTag) noexcept;
    ~Pages() override = default;

public:
    /// Create a pages layout.
    /// @return The new pages layout.
    [[nodiscard]] static auto create() -> PagesPtr;

public:
    /// Show the next visible page, clamped at the last page.
    void nextPage();
    /// Show the previous visible page, clamped at the first page.
    void previousPage();
    /// Get the current visible page index.
    /// @return The visible page index, or `-1` if no visible page exists.
    [[nodiscard]] auto currentPage() const noexcept -> int;
    /// Get the number of visible pages.
    /// @return The number of visible child surfaces.
    [[nodiscard]] auto pageCount() const noexcept -> int;
    /// Show the visible page at the given index.
    /// @param index The visible page index.
    /// @throws std::out_of_range If the index is not valid.
    void showPage(int index);
    /// Show the given surface if it is a visible child page.
    /// @param surface The surface to show.
    /// @throws std::out_of_range If the surface is not a visible child.
    void showPage(SurfacePtr surface);
    /// Get the visible page index for a surface.
    /// @param surface The child surface.
    /// @return The visible page index, or `-1` if it is not visible in this layout.
    [[nodiscard]] auto indexForPage(SurfacePtr surface) const noexcept -> int;

public: // implement Surface
    [[nodiscard]] auto isOpaque() const noexcept -> bool override;
    [[nodiscard]] auto onMeasure(MeasureScope &scope, const LayoutProposal &proposal) noexcept
        -> LayoutMetrics override;
    void onLayout(LayoutScope &scope) noexcept override;
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

private:
    /// Initialize theme attributes after construction.
    void initializeUi() override;
    /// Get all visible child pages in child-storage order.
    [[nodiscard]] auto visiblePages() const -> std::vector<SurfacePtr>;
    /// Clamp the current page to the visible page range.
    [[nodiscard]] auto clampedCurrentPage(const std::vector<SurfacePtr> &pages) const noexcept -> int;
    /// Place the current page inside the available layout area.
    static void placeCentered(const SurfacePtr &surface, const LayoutMetrics &metrics, LayoutScope &scope) noexcept;

private:
    int _currentPage{0}; ///< The current visible page index.
};

}
