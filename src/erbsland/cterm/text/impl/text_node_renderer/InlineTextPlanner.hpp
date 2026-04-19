// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "PlanningStyleResolver.hpp"

#include "../../../impl/StringBuilder.hpp"
#include "../../../String.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

/// Render one node subtree into styled inline text.
class InlineTextPlanner final {
public:
    /// Create an inline text planner.
    /// @param styleResolver The shared planning style resolver.
    explicit InlineTextPlanner(const PlanningStyleResolver &styleResolver);

    // defaults
    ~InlineTextPlanner() = default;
    InlineTextPlanner(const InlineTextPlanner &) = default;
    InlineTextPlanner(InlineTextPlanner &&) = default;
    auto operator=(const InlineTextPlanner &) -> InlineTextPlanner & = delete;
    auto operator=(InlineTextPlanner &&) -> InlineTextPlanner & = delete;

public:
    /// Render one node subtree into styled text.
    /// @param node The node subtree to render.
    /// @param style The active inline text style.
    /// @return The rendered text.
    [[nodiscard]] auto render(const TextNode &node, const CharStyle &style) const -> String;
    /// Append one node subtree to an existing output buffer.
    /// @param node The node subtree to render.
    /// @param style The active inline text style.
    /// @param text The destination text buffer.
    void appendTo(const TextNode &node, const CharStyle &style, cterm::impl::StringBuilder &text) const;

private:
    void appendChildren(const TextNode &node, const CharStyle &style, cterm::impl::StringBuilder &text) const;

private:
    const PlanningStyleResolver &_styleResolver; ///< Shared style-resolution helper.
};

}
