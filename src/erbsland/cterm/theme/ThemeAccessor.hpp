// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Theme.hpp"

#include "../geometry/Rectangle.hpp"

#include <cassert>

namespace erbsland::cterm::theme {

/// Read-only theme accessor for the current selector.
class ThemeAccessor final {
public:
    /// Create an accessor for the default theme root.
    ThemeAccessor() noexcept : ThemeAccessor{Theme::dark(), Selector{Element::Base}} {}
    /// Create an accessor.
    /// @param theme The active theme. Must not be null.
    /// @param selector The selector.
    ThemeAccessor(ThemeConstPtr theme, const Selector selector) noexcept :
        _theme{std::move(theme)}, _selector{selector} {
        assert(_theme != nullptr);
    }

public:
    /// Access the active theme.
    [[nodiscard]] auto theme() const noexcept -> const Theme & { return *_theme; }
    /// Access the selected selector.
    [[nodiscard]] auto selector() const noexcept -> Selector { return _selector; }
    /// Create an accessor for another part.
    [[nodiscard]] auto forPart(Part part) const noexcept -> ThemeAccessor;
    /// Create an accessor with additional states.
    [[nodiscard]] auto withStates(States states) const noexcept -> ThemeAccessor;
    /// Create an accessor with additional tags.
    [[nodiscard]] auto withTags(Tags tags) const noexcept -> ThemeAccessor;
    /// Resolve the color for an animation cycle.
    [[nodiscard]] auto color(std::size_t animationCycle = 0) const noexcept -> Color {
        return sheet()->color(animationCycle);
    }
    /// Access the effective color sequence.
    [[nodiscard]] auto colorSequence() const noexcept -> const ColorSequence & { return sheet()->colorSequence(); }
    /// Access the effective attributes.
    [[nodiscard]] auto attributes() const noexcept -> CharAttributes { return sheet()->attributes(); }
    /// Resolve the style for an animation cycle.
    [[nodiscard]] auto style(const std::size_t animationCycle = 0) const noexcept -> CharStyle {
        return sheet()->style(animationCycle);
    }
    /// Resolve a block character for a role.
    [[nodiscard]] auto
    block(const BlockRole role = BlockRole::Background, const std::size_t animationCycle = 0) const noexcept -> Char {
        return sheet()->block(role, animationCycle);
    }
    /// Resolve the tile-9 style for an animation cycle.
    [[nodiscard]] auto tile9Style(const std::size_t animationCycle = 0) const noexcept -> Tile9Style {
        return sheet()->tile9Style(animationCycle);
    }
    /// Access the effective margins.
    [[nodiscard]] auto margins() const noexcept -> Margins { return sheet()->margins(); }
    /// Access the effective padding.
    [[nodiscard]] auto padding() const noexcept -> Margins { return sheet()->padding(); }
    /// Resolve the content rectangle by applying margins.
    [[nodiscard]] auto contentRect(const Rectangle rect) const noexcept -> Rectangle { return rect.insetBy(margins()); }

private:
    [[nodiscard]] auto sheet() const noexcept -> PropertySheetConstPtr { return _theme->propertySheet(_selector); }

private:
    ThemeConstPtr _theme; ///< The active theme.
    Selector _selector;   ///< The current selector.
};

}
