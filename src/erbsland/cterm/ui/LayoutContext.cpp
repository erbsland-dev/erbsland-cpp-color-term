// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "LayoutContext.hpp"

namespace erbsland::cterm::ui {

LayoutContext::LayoutContext() noexcept : LayoutContext{theme::Theme::dark()} {
}

LayoutContext::LayoutContext(
    theme::ThemeConstPtr theme,
    const theme::Element themeElement,
    const theme::States themeState,
    const theme::Tags themeTags) noexcept :
    _themeContext{std::move(theme), themeElement, themeState, themeTags} {
}

LayoutContext::LayoutContext(ThemeContext themeContext) noexcept : _themeContext{std::move(themeContext)} {
}

}
