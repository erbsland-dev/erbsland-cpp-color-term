// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "InlineTextPlanner.hpp"
#include "PlanningContext.hpp"
#include "RenderBlock.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

/// Create fully planned render blocks from semantic inputs.
class RenderBlockFactory final {
public:
    /// Create a render-block factory.
    /// @param styleResolver The shared planning style resolver.
    /// @param inlineTextPlanner The inline text planner used for text payloads.
    RenderBlockFactory(const PlanningStyleResolver &styleResolver, const InlineTextPlanner &inlineTextPlanner);

    // defaults
    ~RenderBlockFactory() = default;
    RenderBlockFactory(const RenderBlockFactory &) = default;
    RenderBlockFactory(RenderBlockFactory &&) = default;
    auto operator=(const RenderBlockFactory &) -> RenderBlockFactory & = delete;
    auto operator=(RenderBlockFactory &&) -> RenderBlockFactory & = delete;

public:
    /// Create a paragraph-like block from one semantic node.
    /// @param node The source node.
    /// @param blockStyle The resolved block rule.
    /// @param context The active planning context.
    /// @return The planned block.
    [[nodiscard]] auto
    paragraph(const TextNode &node, const StyleRule &blockStyle, const PlanningContext &context) const -> RenderBlock;
    /// Create a paragraph-like block from already rendered text.
    /// @param text The text payload.
    /// @param blockStyle The resolved block rule.
    /// @param context The active planning context.
    /// @return The planned block.
    [[nodiscard]] auto paragraph(String text, const StyleRule &blockStyle, const PlanningContext &context) const
        -> RenderBlock;
    /// Create a heading block.
    /// @param node The heading node.
    /// @param context The active planning context.
    /// @return The planned block.
    [[nodiscard]] auto heading(const TextNode &node, const PlanningContext &context) const -> RenderBlock;
    /// Create a horizontal-rule block.
    /// @param context The active planning context.
    /// @return The planned block.
    [[nodiscard]] auto horizontalRule(const PlanningContext &context) const -> RenderBlock;

private:
    [[nodiscard]] auto decorateText(String text, const StyleRule &blockStyle, const CharStyle &textStyle) const
        -> String;
    [[nodiscard]] auto resolvedDecoration(const std::optional<String> &decoration, const CharStyle &textStyle) const
        -> std::optional<String>;

private:
    const PlanningStyleResolver &_styleResolver; ///< Shared planning style resolver.
    const InlineTextPlanner &_inlineTextPlanner; ///< Inline text planner.
};

}
