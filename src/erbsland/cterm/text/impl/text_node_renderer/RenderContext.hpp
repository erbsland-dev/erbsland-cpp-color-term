// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../../../CharStyle.hpp"
#include "../../../geometry/Margins.hpp"
#include "../../Style.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

/// Inherited rendering context for descendant blocks.
class RenderContext final {
public:
    /// The inherited text style overlay for descendant blocks.
    CharStyle textStyle;
    /// The accumulated horizontal margins contributed by ancestor containers.
    Margins horizontalMargins;

public:
    /// Create a child context for a semantic container.
    /// @param containerRule The resolved container rule.
    /// @return The child context.
    [[nodiscard]] auto withContainer(const StyleRule &containerRule) const -> RenderContext;
    /// Create a child context for the body of one list item.
    /// @param listItemRule The resolved list-item rule.
    /// @return The child context.
    [[nodiscard]] auto withListItem(const StyleRule &listItemRule) const -> RenderContext;
    /// Resolve the effective text style for a block in this context.
    /// @param baseTextStyle The renderer base text style.
    /// @param blockRule The block rule to apply.
    /// @return The effective text style.
    [[nodiscard]] auto resolvedTextStyle(const CharStyle &baseTextStyle, const StyleRule &blockRule) const -> CharStyle;
    /// Resolve inherited horizontal margins onto a block rule.
    /// @param blockRule The block rule to resolve.
    /// @return The block rule including inherited horizontal margins.
    [[nodiscard]] auto resolvedBlockStyle(const StyleRule &blockRule) const -> StyleRule;
};

}
