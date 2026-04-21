// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../theme/ThemeAccessor.hpp"

#include <utility>

namespace erbsland::cterm::ui {

/// The resolved theme scope for one surface operation.
class ThemeContext final {
public:
    /// Create a theme context using the default dark theme.
    ThemeContext() noexcept;
    /// Create a theme context.
    /// @param theme The active theme. A null pointer uses the default dark theme.
    /// @param element The current theme element.
    /// @param state The current theme state.
    /// @param tags The current theme tags.
    explicit ThemeContext(
        theme::ThemeConstPtr theme,
        theme::Element element = {},
        theme::States state = {},
        theme::Tags tags = {}) noexcept;

    // defaults
    ~ThemeContext() = default;
    ThemeContext(const ThemeContext &) = default;
    ThemeContext(ThemeContext &&) = default;
    auto operator=(const ThemeContext &) -> ThemeContext & = delete;
    auto operator=(ThemeContext &&) -> ThemeContext & = delete;

public:
    /// Access the theme for this context.
    [[nodiscard]] auto theme() const noexcept -> theme::ThemeAccessor;
    /// Access the current theme element.
    [[nodiscard]] auto element() const noexcept -> theme::Element { return _element; }
    /// Access the current theme state.
    [[nodiscard]] auto state() const noexcept -> theme::States { return _state; }
    /// Access the current theme tags.
    [[nodiscard]] auto tags() const noexcept -> const theme::Tags & { return _tags; }
    /// Create a copy with a different theme.
    /// @param theme The active theme. A null pointer uses the default dark theme.
    [[nodiscard]] auto withTheme(theme::ThemeConstPtr theme) const noexcept -> ThemeContext;
    /// Create a copy with a different surface scope.
    /// @param element The current theme element.
    /// @param state The current theme state.
    /// @param tags The current theme tags.
    [[nodiscard]] auto withSurface(theme::Element element, theme::States state, theme::Tags tags) const noexcept
        -> ThemeContext;
    /// Create a copy with additional state flags.
    /// @param state The state flags to add.
    [[nodiscard]] auto withState(theme::States state) const noexcept -> ThemeContext;

private:
    const theme::ThemeConstPtr _theme; ///< The active theme.
    const theme::Element _element;     ///< The current theme element.
    const theme::States _state;        ///< The current theme state.
    const theme::Tags _tags;           ///< The current theme tags.
};

}
