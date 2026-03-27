// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../../Style.hpp"
#include "../../TextNode.hpp"

#include <optional>
#include <string_view>
#include <vector>


namespace erbsland::cterm::text::impl::text_node_renderer {


/// Resolve planner-facing style information for text nodes.
class PlanningStyleResolver final {
public:
    /// Create a resolver for the given style.
    /// @param style The style used during planning. `nullptr` falls back to the default style.
    explicit PlanningStyleResolver(StyleConstPtr style);

    // defaults
    ~PlanningStyleResolver() = default;
    PlanningStyleResolver(const PlanningStyleResolver &) = default;
    PlanningStyleResolver(PlanningStyleResolver &&) = default;
    auto operator=(const PlanningStyleResolver &) -> PlanningStyleResolver & = delete;
    auto operator=(PlanningStyleResolver &&) -> PlanningStyleResolver & = delete;

public:
    /// Access the renderer base text style.
    /// @return The base text style.
    [[nodiscard]] auto baseTextStyle() const noexcept -> const CharStyle &;
    /// Resolve the block rule for one text node.
    /// @param node The node to resolve.
    /// @param role The semantic role.
    /// @param listKind Optional list kind qualifier.
    /// @param levelOverride Optional explicit level override.
    /// @return The resolved rule.
    [[nodiscard]] auto blockRule(
        const TextNode &node,
        StyleRole role,
        StyleListKind listKind = StyleListKind::Any,
        std::optional<int> levelOverride = std::nullopt) const -> StyleRule;
    /// Resolve the horizontal-rule style.
    /// @return The resolved rule.
    [[nodiscard]] auto horizontalRuleRule() const -> StyleRule;
    /// Resolve the semantic inline role for one node type.
    /// @param type The node type.
    /// @return The matching inline role, if any.
    [[nodiscard]] static auto inlineRole(TextNode::Type type) -> std::optional<StyleRole>;
    /// Split and normalize style tokens.
    /// @param value The raw style value.
    /// @return The unique sorted tokens.
    [[nodiscard]] static auto splitStyleTokens(std::string_view value) -> std::vector<std::string>;

private:
    StyleConstPtr _style; ///< The style used for all selector resolution.
};


}
