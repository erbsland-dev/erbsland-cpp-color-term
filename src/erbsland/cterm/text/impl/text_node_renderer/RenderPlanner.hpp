// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "InlineTextPlanner.hpp"
#include "ListBlockPlanner.hpp"
#include "PlanningContext.hpp"
#include "PlanningStyleResolver.hpp"
#include "RenderBlock.hpp"
#include "RenderBlockFactory.hpp"
#include "TextNodePlanningQueries.hpp"

#include "../../Style.hpp"
#include "../../TextNode.hpp"


namespace erbsland::cterm::text::impl::text_node_renderer {


/// Plan `TextNode` trees into a linear block representation.
class RenderPlanner final : private PlannedNodeAppender {
public:
    /// Create a planner for the given style.
    /// @param style The style to use when resolving block and inline rendering.
    explicit RenderPlanner(StyleConstPtr style);

    // defaults
    ~RenderPlanner() = default;
    RenderPlanner(const RenderPlanner &) = delete;
    RenderPlanner(RenderPlanner &&) = delete;
    auto operator=(const RenderPlanner &) -> RenderPlanner & = delete;
    auto operator=(RenderPlanner &&) -> RenderPlanner & = delete;

public:
    /// Build render blocks for the given document tree.
    /// @param document The document node to render. `nullptr` produces an empty block list.
    /// @return The planned render blocks.
    [[nodiscard]] auto build(const TextNodeConstPtr &document) const -> RenderBlocks;

private:
    void appendNode(const TextNode &node, const PlanningContext &context, RenderBlocks &blocks) const;
    void
    appendContainer(const TextNode &node, StyleRole role, const PlanningContext &context, RenderBlocks &blocks) const;
    void appendParagraphLikeNode(
        const TextNode &node, StyleRole role, const PlanningContext &context, RenderBlocks &blocks) const;
    void appendPlannedNode(const TextNode &node, const PlanningContext &context, RenderBlocks &blocks) const override;

private:
    StyleConstPtr _style;                 ///< The style used to plan the document.
    PlanningStyleResolver _styleResolver; ///< Shared style resolver.
    TextNodePlanningQueries _queries;     ///< Planning queries for child inspection.
    InlineTextPlanner _inlineTextPlanner; ///< Inline text planner.
    RenderBlockFactory _blockFactory;     ///< Factory for planned render blocks.
    ListBlockPlanner _listBlockPlanner;   ///< Planner for list containers and list items.
};


}
