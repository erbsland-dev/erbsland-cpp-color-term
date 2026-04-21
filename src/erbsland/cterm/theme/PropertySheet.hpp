// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Properties.hpp"

#include "../Char.hpp"
#include "../CharStyle.hpp"
#include "../Tile9Style.hpp"

#include <memory>

namespace erbsland::cterm::theme {

class PropertySheet;
using PropertySheetPtr = std::shared_ptr<PropertySheet>;
using PropertySheetConstPtr = std::shared_ptr<const PropertySheet>;

/// Effective values for one resolved theme selector.
class PropertySheet final {
public:
    /// Create the root sheet with all default values.
    PropertySheet() noexcept;
    /// Create an effective sheet from a base sheet and authoring properties.
    PropertySheet(const PropertySheet &base, const Properties &properties) noexcept;

public:
    /// Resolve the color for an animation cycle.
    [[nodiscard]] auto color(std::size_t animationCycle = 0) const noexcept -> Color;
    /// Access the effective color sequence.
    [[nodiscard]] auto colorSequence() const noexcept -> const ColorSequence & { return _colorSequence; }
    /// Access the effective attributes.
    [[nodiscard]] auto attributes() const noexcept -> CharAttributes { return _attributes; }
    /// Resolve the style for an animation cycle.
    [[nodiscard]] auto style(std::size_t animationCycle = 0) const noexcept -> CharStyle;
    /// Resolve a block character for a role.
    [[nodiscard]] auto block(BlockRole role, std::size_t animationCycle = 0) const noexcept -> Char;
    /// Resolve a tile-9 style for an animation cycle.
    [[nodiscard]] auto tile9Style(std::size_t animationCycle = 0) const noexcept -> Tile9Style;
    /// Access the effective margins.
    [[nodiscard]] auto margins() const noexcept -> Margins { return _margins; }
    /// Access the effective padding.
    [[nodiscard]] auto padding() const noexcept -> Margins { return _padding; }

private:
    Color _color;                 ///< The effective static color.
    ColorSequence _colorSequence; ///< The effective color sequence.
    CharAttributes _attributes;   ///< The effective attributes.
    Properties::Blocks _blocks{}; ///< The effective block code points.
    Margins _margins;             ///< The effective margins.
    Margins _padding;             ///< The effective padding.
};

}
