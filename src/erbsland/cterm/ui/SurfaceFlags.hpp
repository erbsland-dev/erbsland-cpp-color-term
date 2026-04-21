// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "ui_namespace.hpp"

#include "../geometry/Rectangle.hpp"
#include "../theme/States.hpp"

#include <optional>

namespace erbsland::cterm::ui {

class Surface;
class Display;

/// Framework-managed state flags for one surface.
class SurfaceFlags final {
    friend class Display;
    friend class Surface;

public:
    /// Create flags for the given owner surface.
    /// @param owner The owning surface.
    explicit SurfaceFlags(Surface &owner) noexcept;
    ~SurfaceFlags() = default;

    // defaults
    SurfaceFlags(const SurfaceFlags &) = delete;
    SurfaceFlags(SurfaceFlags &&) = delete;
    auto operator=(const SurfaceFlags &) -> SurfaceFlags & = delete;
    auto operator=(SurfaceFlags &&) -> SurfaceFlags & = delete;

public:
    /// Test if the owner surface is locally visible.
    [[nodiscard]] auto isVisible() const noexcept -> bool;
    /// Change local visibility for the owner surface.
    void setVisible(bool visible);
    /// Test if the owner surface and all ancestors are visible.
    [[nodiscard]] auto isVisibleInTree() const noexcept -> bool;
    /// Test if the owner surface can receive direct keyboard focus.
    [[nodiscard]] auto isFocusable() const noexcept -> bool;
    /// Change whether the owner surface can receive direct keyboard focus.
    void setFocusable(bool focusable) noexcept;
    /// Test if the owner surface is enabled.
    [[nodiscard]] auto isEnabled() const noexcept -> bool;
    /// Change whether the owner surface is enabled.
    void setEnabled(bool enabled) noexcept;
    /// Test if the owner surface is selected.
    [[nodiscard]] auto isSelected() const noexcept -> bool;
    /// Change whether the owner surface is selected.
    void setSelected(bool selected);
    /// Test if the owner surface is checked.
    [[nodiscard]] auto isChecked() const noexcept -> bool;
    /// Change whether the owner surface is checked.
    void setChecked(bool checked);
    /// Test if the owner surface has direct keyboard focus.
    [[nodiscard]] auto hasFocus() const noexcept -> bool;
    /// Test if the owner surface or any descendant has keyboard focus.
    [[nodiscard]] auto hasFocusWithin() const noexcept -> bool;
    /// Get the theme states represented by these flags.
    [[nodiscard]] auto themeStates() const noexcept -> theme::States;
    /// Test if the owner surface requires a layout pass.
    [[nodiscard]] auto isLayoutOutdated() const noexcept -> bool;
    /// Mark the owner surface and its ancestors as requiring layout.
    void setLayoutOutdated();
    /// Test if the owner surface requires repainting.
    [[nodiscard]] auto isPaintOutdated() const noexcept -> bool;
    /// Mark the whole owner surface as requiring repainting.
    void setPaintOutdated();
    /// Mark a local rectangle of the owner surface as requiring repainting.
    /// @param dirtyRect The dirty rectangle in the owner's local coordinates.
    void setPaintOutdated(Rectangle dirtyRect);
    /// Mark the owner subtree as requiring layout and repainting after a theme-scope change.
    void setThemeOutdated();
    /// Get the dirty rectangle in local coordinates, or an empty value for a full repaint.
    [[nodiscard]] auto paintDirtyRect() const noexcept -> const std::optional<Rectangle> &;

private:
    /// Change the direct keyboard focus state.
    void setHasFocus(bool hasFocus) noexcept;
    /// Change the focus-within state.
    void setHasFocusWithin(bool hasFocusWithin) noexcept;
    /// Clear the layout invalidation state on the owner surface.
    void clearLayoutOutdated() noexcept;
    /// Clear the paint invalidation state on the owner surface.
    void clearPaintOutdated() noexcept;

private:
    Surface &_owner;                          ///< The owning surface.
    bool _isVisible{true};                    ///< The local surface visibility state.
    bool _isFocusable{false};                 ///< Whether this surface can receive direct keyboard focus.
    bool _isEnabled{true};                    ///< Whether this surface is enabled.
    bool _isSelected{false};                  ///< Whether this surface is selected.
    bool _isChecked{false};                   ///< Whether this surface is checked.
    bool _hasFocus{false};                    ///< Direct focus state.
    bool _hasFocusWithin{false};              ///< Focus path state.
    bool _isLayoutOutdated{false};            ///< Indicates if the layout needs to be recalculated.
    bool _isPaintOutdated{false};             ///< Indicates if the surface needs to be repainted on the display.
    std::optional<Rectangle> _paintDirtyRect; ///< The dirty rectangle for partial repainting.
};

}
