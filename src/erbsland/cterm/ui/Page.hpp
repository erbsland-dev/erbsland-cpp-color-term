// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Surface.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace erbsland::cterm::ui {

class Page;
using PagePtr = std::shared_ptr<Page>;

/// A single page on the screen.
class Page : public Surface {
public:
    /// Create a page instance through the protected construction path.
    explicit Page(ProtectedTag) : Surface{theme::Element::Page} {}
    ~Page() override = default;

public:
    /// Create a new page.
    [[nodiscard]] static auto create() -> PagePtr;

public:
    /// Replace the theme for this page and its descendants.
    /// @param theme The page theme. A null pointer clears the override.
    void setTheme(theme::ThemeConstPtr theme) noexcept;
    /// Clear the page theme override.
    void clearTheme() noexcept;
    /// Access the page theme override.
    /// @return The page theme override, or null if the application theme is used.
    [[nodiscard]] auto themeOverride() const noexcept -> const theme::ThemeConstPtr &;
    /// Test if the page has a focus surface.
    [[nodiscard]] auto hasFocusSurface() const noexcept -> bool;
    /// Get the current focus surface.
    [[nodiscard]] auto focusSurface() const noexcept -> const SurfaceWeakPtr &;
    /// Move the focus to another surface.
    /// Passing an expired, foreign, hidden, or non-focusable surface clears the current focus.
    /// @param surface The new focus surface.
    /// @return `true` if the surface received focus.
    auto focusTo(const SurfaceWeakPtr &surface) noexcept -> bool;
    /// Clear keyboard focus from this page.
    void clearFocus() noexcept;

public: // implement Surface
    /// Layout top-level page children inside the page.
    /// @param scope The layout scope for this page.
    void onLayout(LayoutScope &scope) noexcept override;
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;
    /// Route the key press to the focused surface chain and finally to the page itself.
    /// @param keyPressEvent The key press event.
    void onKeyPress(KeyPressEvent &keyPressEvent) noexcept override;
    [[nodiscard]] auto isOpaque() const noexcept -> bool override;
    [[nodiscard]] auto isPage() const noexcept -> bool override;

private:
    enum class FocusRelation : uint8_t {
        None,
        Within,
        Direct,
    };
    using FocusPath = std::vector<SurfacePtr>;

private:
    /// Test if the given surface belongs to this page.
    /// @param surface The surface to test.
    /// @return `true` if the surface is a descendant of this page.
    [[nodiscard]] auto isFocusSurfaceValid(const SurfacePtr &surface) const noexcept -> bool;
    /// Clear the stored focus if it is no longer valid.
    /// @return The valid focus surface, or null if no focus remains.
    [[nodiscard]] auto validFocusSurface() noexcept -> SurfacePtr;
    /// Build a page-relative path from a focus surface to this page.
    /// @param surface The focus surface.
    /// @return The path from page child to focus surface.
    [[nodiscard]] auto focusPathFor(const SurfacePtr &surface) const -> FocusPath;
    /// Get one surface relation for a path.
    /// @param path The focus path.
    /// @param surface The surface to query.
    /// @return The focus relation for the surface.
    [[nodiscard]] static auto relationFor(const FocusPath &path, const SurfacePtr &surface) noexcept -> FocusRelation;
    /// Apply all relation changes between two paths.
    /// @param oldPath The previous focus path.
    /// @param newPath The new focus path.
    void updateFocusPath(const FocusPath &oldPath, const FocusPath &newPath) noexcept;
    /// Convert one relation into a focus callback change.
    /// @param relation The new relation.
    /// @return The corresponding focus change.
    [[nodiscard]] static auto focusChangeFor(FocusRelation relation) noexcept -> FocusChange;

    SurfaceWeakPtr _focusSurface; ///< The surface that currently has the main focus.
    theme::ThemeConstPtr _theme;  ///< Optional page theme override.
};

}
