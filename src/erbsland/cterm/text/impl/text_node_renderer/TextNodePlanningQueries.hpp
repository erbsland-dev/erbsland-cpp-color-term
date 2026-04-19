// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../../TextNode.hpp"

#include <optional>

namespace erbsland::cterm::text::impl::text_node_renderer {

/// Planning-specific queries on `TextNode` trees.
class TextNodePlanningQueries final {
public:
    /// The supported child selections used during planning.
    enum class ChildSelection : uint8_t {
        RenderableBlocks, ///< Children that emit physical render blocks.
        ListItems,        ///< Children that are list items.
    };

    /// The first and last matching child index.
    struct ChildRange final {
        std::size_t first{0};
        std::size_t last{0};
    };

public:
    /// Test if a node emits physical render blocks.
    /// @param node The node to test.
    /// @return `true` if the node emits render blocks.
    [[nodiscard]] auto emitsBlocks(const TextNode &node) const -> bool;
    /// Test if a node matches the given child selection.
    /// @param node The node to test.
    /// @param selection The selection to match.
    /// @return `true` if the node matches.
    [[nodiscard]] auto childMatchesSelection(const TextNode &node, ChildSelection selection) const -> bool;
    /// Find the first and last matching child for one node.
    /// @param node The node whose children are inspected.
    /// @param selection The child selection to match.
    /// @return The matching range, if any children match.
    [[nodiscard]] auto findChildRange(const TextNode &node, ChildSelection selection) const
        -> std::optional<ChildRange>;
};

}
