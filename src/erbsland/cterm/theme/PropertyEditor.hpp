// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Properties.hpp"

#include "../CharStyle.hpp"

#include <string_view>

namespace erbsland::cterm::theme {

/// Mutable editor for one property sheet while a theme is being built.
class PropertyEditor final {
public:
    /// Create an empty editor.
    PropertyEditor() = default;
    /// Create an editor for a property sheet.
    /// @param properties The edited properties. Must outlive this editor.
    explicit PropertyEditor(Properties &properties) noexcept : _properties{&properties} {}

public:
    /// Set the color replacement.
    auto setColor(Color color) noexcept -> PropertyEditor &;
    /// Set the color sequence.
    auto setColorSequence(ColorSequence colorSequence) noexcept -> PropertyEditor &;
    /// Set the attributes replacement.
    auto setAttributes(CharAttributes attributes) noexcept -> PropertyEditor &;
    /// Set color and attributes in one call.
    auto setStyle(CharStyle style) noexcept -> PropertyEditor &;
    /// Set one block code point.
    auto setBlock(BlockRole role, char32_t codePoint) noexcept -> PropertyEditor &;
    /// Set a 9-block or 16-block table from UTF-32 code points.
    auto setBlocks(std::u32string_view blocks) -> PropertyEditor &;
    /// Set the margins.
    auto setMargins(Margins margins) noexcept -> PropertyEditor &;
    /// Set the padding.
    auto setPadding(Margins padding) noexcept -> PropertyEditor &;

private:
    Properties *_properties{}; ///< The edited properties.
};

}
