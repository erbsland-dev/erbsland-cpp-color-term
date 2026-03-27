// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ListItemLayout.hpp"
#include "PlanningContext.hpp"
#include "PlanningStyleResolver.hpp"
#include "RenderBlock.hpp"
#include "RenderBlockFactory.hpp"
#include "TextNodePlanningQueries.hpp"


namespace erbsland::cterm::text::impl::text_node_renderer {


/// Interface used by the list planner to recurse into non-list nodes.
class PlannedNodeAppender {
public:
    virtual ~PlannedNodeAppender() = default;

public:
    /// Append the planned blocks for one node.
    /// @param node The node to render.
    /// @param context The active planning context.
    /// @param blocks The destination block list.
    virtual void
    appendPlannedNode(const TextNode &node, const PlanningContext &context, RenderBlocks &blocks) const = 0;
};


/// Plan list containers and list-item block rewrites.
class ListBlockPlanner final {
public:
    /// Create a list block planner.
    /// @param styleResolver The shared planning style resolver.
    /// @param blockFactory The render-block factory.
    /// @param queries The planning queries used for child selection.
    ListBlockPlanner(
        const PlanningStyleResolver &styleResolver,
        const RenderBlockFactory &blockFactory,
        const TextNodePlanningQueries &queries);

    // defaults
    ~ListBlockPlanner() = default;
    ListBlockPlanner(const ListBlockPlanner &) = default;
    ListBlockPlanner(ListBlockPlanner &&) = default;
    auto operator=(const ListBlockPlanner &) -> ListBlockPlanner & = delete;
    auto operator=(ListBlockPlanner &&) -> ListBlockPlanner & = delete;

public:
    /// Append one list node to the block plan.
    /// @param node The list node.
    /// @param numbered Set for numbered lists.
    /// @param context The active planning context.
    /// @param blocks The destination block list.
    /// @param appender The recursion entry for nested non-list nodes.
    void appendList(
        const TextNode &node,
        bool numbered,
        const PlanningContext &context,
        RenderBlocks &blocks,
        const PlannedNodeAppender &appender) const;
    /// Create the neutral block style used for list-item paragraphs.
    /// @return The neutral paragraph rule.
    [[nodiscard]] static auto listItemParagraphRule() -> StyleRule;

private:
    void appendListItem(
        const TextNode &node,
        const StyleRule &listItemRule,
        const PlanningContext &context,
        ListItemLayout &listItemLayout,
        RenderBlocks &blocks,
        const PlannedNodeAppender &appender) const;
    [[nodiscard]] auto collectListItemBlocks(
        const TextNode &node, const PlanningContext &context, const PlannedNodeAppender &appender) const
        -> RenderBlocks;
    void appendListItemChild(
        const TextNode &node,
        const PlanningContext &context,
        RenderBlocks &blocks,
        const PlannedNodeAppender &appender) const;
    void
    applyListItemMargins(RenderBlocks &itemBlocks, const StyleRule &listItemRule, const PlanningContext &context) const;
    void
    appendListItemBlocks(RenderBlocks &itemBlocks, const ListItemLayout &listItemLayout, RenderBlocks &blocks) const;
    [[nodiscard]] static auto emptyListItemRule() -> StyleRule;
    [[nodiscard]] auto emptyListItemBlock(const PlanningContext &context) const -> RenderBlock;
    [[nodiscard]] auto
    makeListItemLayout(const StyleRule &listItemRule, const PlanningContext &context, std::size_t number) const
        -> ListItemLayout;

private:
    const PlanningStyleResolver &_styleResolver; ///< Shared planning style resolver.
    const RenderBlockFactory &_blockFactory;     ///< Shared render-block factory.
    const TextNodePlanningQueries &_queries;     ///< Shared planning queries.
};


}
