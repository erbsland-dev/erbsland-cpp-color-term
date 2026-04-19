// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../../../CharStyle.hpp"
#include "../../../Margins.hpp"
#include "../../Style.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

/// Rendering context propagated while planning block output.
class PlanningContext final {
public:
    /// The inherited text style overlay for descendant blocks.
    CharStyle textStyle;
    /// The accumulated margins contributed by ancestor containers.
    Margins margins;

public:
    /// Create a child context for a semantic container.
    /// @param containerRule The resolved container rule.
    /// @return The derived child context.
    [[nodiscard]] auto withContainer(const StyleRule &containerRule) const -> PlanningContext;
    /// Create a child context that only inherits horizontal margins.
    /// @param edgeMargins The margins to contribute.
    /// @return The derived child context.
    [[nodiscard]] auto withHorizontalMargins(const Margins &edgeMargins) const -> PlanningContext;
    /// Add top and bottom edge margins for the first and last rendered child.
    /// @param edgeMargins The container margins to apply at the edges.
    /// @param isFirstChild Set if the child is the first rendered child.
    /// @param isLastChild Set if the child is the last rendered child.
    /// @return The derived child context.
    [[nodiscard]] auto withEdgeMargins(const Margins &edgeMargins, bool isFirstChild, bool isLastChild) const
        -> PlanningContext;
    /// Create a child context for the body of one list item.
    /// @param listItemRule The resolved list-item rule.
    /// @return The derived child context.
    [[nodiscard]] auto withListItem(const StyleRule &listItemRule) const -> PlanningContext;
    /// Resolve the effective text style for a block in this context.
    /// @param baseTextStyle The renderer base text style.
    /// @param blockStyle The block rule to apply.
    /// @return The effective text style.
    [[nodiscard]] auto resolvedTextStyle(const CharStyle &baseTextStyle, const StyleRule &blockStyle) const
        -> CharStyle;
    /// Resolve inherited margins onto a block rule.
    /// @param blockStyle The block rule to resolve.
    /// @return The block rule including inherited margins.
    [[nodiscard]] auto resolvedBlockStyle(const StyleRule &blockStyle) const -> StyleRule;
};

}
