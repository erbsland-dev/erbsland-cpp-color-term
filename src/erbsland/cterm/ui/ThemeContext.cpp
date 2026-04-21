// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ThemeContext.hpp"

namespace erbsland::cterm::ui {

ThemeContext::ThemeContext() noexcept : ThemeContext{theme::Theme::dark()} {
}

ThemeContext::ThemeContext(
    theme::ThemeConstPtr theme, const theme::Element element, const theme::States state, theme::Tags tags) noexcept :
    _theme{theme != nullptr ? std::move(theme) : theme::Theme::dark()},
    _element{element},
    _state{state},
    _tags{std::move(tags)} {
}

auto ThemeContext::withTheme(theme::ThemeConstPtr theme) const noexcept -> ThemeContext {
    return ThemeContext{std::move(theme), _element, _state, _tags};
}

auto ThemeContext::withSurface(const theme::Element element, const theme::States state, theme::Tags tags) const noexcept
    -> ThemeContext {
    return ThemeContext{_theme, element, state, std::move(tags)};
}

auto ThemeContext::withState(const theme::States state) const noexcept -> ThemeContext {
    return ThemeContext{_theme, _element, _state | state, _tags};
}

auto ThemeContext::theme() const noexcept -> theme::ThemeAccessor {
    return theme::ThemeAccessor{_theme, theme::Selector{_element}.withStates(_state).withTags(_tags)};
}

}
