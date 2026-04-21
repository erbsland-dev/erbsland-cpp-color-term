// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "BlockRole.hpp"

#include "../CharAttributes.hpp"
#include "../Color.hpp"
#include "../ColorSequence.hpp"
#include "../geometry/Margins.hpp"

#include <array>
#include <optional>
#include <utility>

namespace erbsland::cterm::theme {

/// Authoring values for one theme property sheet.
class Properties final {
public:
    /// Number of block roles stored in a property sheet.
    constexpr static auto cBlockCount = std::size_t{16};
    /// The block code point table.
    using Blocks = std::array<char32_t, cBlockCount>;

public:
    /// Access the optional color replacement.
    [[nodiscard]] auto color() const noexcept -> const std::optional<Color> & { return _color; }
    /// Replace the optional color replacement.
    void setColor(Color color) noexcept { _color = color; }
    /// Access the optional color sequence.
    [[nodiscard]] auto colorSequence() const noexcept -> const std::optional<ColorSequence> & { return _colorSequence; }
    /// Replace the optional color sequence.
    void setColorSequence(ColorSequence colorSequence) noexcept { _colorSequence = std::move(colorSequence); }
    /// Access the optional attributes replacement.
    [[nodiscard]] auto attributes() const noexcept -> const std::optional<CharAttributes> & { return _attributes; }
    /// Replace the optional attributes replacement.
    void setAttributes(CharAttributes attributes) noexcept { _attributes = attributes; }
    /// Access the optional block table.
    [[nodiscard]] auto blocks() const noexcept -> const std::optional<Blocks> & { return _blocks; }
    /// Replace one block role.
    void setBlock(BlockRole role, char32_t codePoint) noexcept;
    /// Replace the complete block table.
    void setBlocks(Blocks blocks) noexcept { _blocks = blocks; }
    /// Replace the optional margins.
    void setMargins(Margins margins) noexcept { _margins = margins; }
    /// Access the optional margins.
    [[nodiscard]] auto margins() const noexcept -> const std::optional<Margins> & { return _margins; }
    /// Replace the optional padding.
    void setPadding(Margins padding) noexcept { _padding = padding; }
    /// Access the optional padding.
    [[nodiscard]] auto padding() const noexcept -> const std::optional<Margins> & { return _padding; }

private:
    std::optional<Color> _color;                 ///< The color replacement.
    std::optional<ColorSequence> _colorSequence; ///< The color animation sequence.
    std::optional<CharAttributes> _attributes;   ///< The attributes replacement.
    std::optional<Blocks> _blocks;               ///< The block code points.
    std::optional<Margins> _margins;             ///< The margins.
    std::optional<Margins> _padding;             ///< The padding.
};

}
