// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "ThemeContext.hpp"

#include <utility>

namespace erbsland::cterm::ui {

/// The theme scope for one layout pass.
class LayoutContext {
public:
    /// Create a layout context using the default dark theme.
    LayoutContext() noexcept;
    /// Create a layout context for a resolved theme scope.
    /// @param theme The active theme. A null pointer uses the default dark theme.
    /// @param themeElement The current theme element.
    /// @param themeState The current theme state.
    /// @param themeTags The current theme tags.
    explicit LayoutContext(
        theme::ThemeConstPtr theme,
        theme::Element themeElement = {},
        theme::States themeState = {},
        theme::Tags themeTags = {}) noexcept;
    /// Create a layout context from a resolved theme context.
    /// @param themeContext The resolved theme context.
    explicit LayoutContext(ThemeContext themeContext) noexcept;

    // defaults
    ~LayoutContext() = default;
    LayoutContext(const LayoutContext &) = default;
    LayoutContext(LayoutContext &&) = default;
    auto operator=(const LayoutContext &) -> LayoutContext & = delete;
    auto operator=(LayoutContext &&) -> LayoutContext & = delete;

public:
    /// Access the current theme context.
    /// @return The current theme context.
    [[nodiscard]] auto themeContext() const noexcept -> const ThemeContext & { return _themeContext; }
    /// Access the theme for this layout pass.
    /// @return The read-only theme accessor.
    [[nodiscard]] auto theme() const noexcept -> theme::ThemeAccessor { return _themeContext.theme(); }

private:
    const ThemeContext _themeContext; ///< The current theme context.
};

}
