// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "ListPrefix.hpp"
#include "RenderBlock.hpp"

#include <utility>

namespace erbsland::cterm::text::impl::text_node_renderer {

/// Layout data used while applying a list-item prefix to planned blocks.
class ListItemLayout final {
public:
    /// Create one list-item layout.
    /// @param prefix The rendered list prefix.
    /// @param firstLineOffset Additional indentation before the visible prefix on the first block.
    /// @param continuationOffset Additional indentation for wrapped lines and continuation blocks.
    /// @param plainFirstLineOffset Additional plain-text indent for the prefixed line.
    /// @param plainContinuationIndent Additional plain-text indent for continuation lines and blocks.
    ListItemLayout(
        ListPrefix prefix = {},
        const int firstLineOffset = 0,
        const int continuationOffset = 0,
        const int plainFirstLineOffset = 0,
        const int plainContinuationIndent = 0) :
        _prefix{std::move(prefix)},
        _firstLineOffset{firstLineOffset},
        _continuationOffset{continuationOffset},
        _plainFirstLineOffset{plainFirstLineOffset},
        _plainContinuationIndent{plainContinuationIndent} {}

public:
    /// Access the rendered list prefix.
    [[nodiscard]] auto prefix() const noexcept -> const ListPrefix & { return _prefix; }
    /// Access the additional indentation before the visible prefix on the first block.
    [[nodiscard]] auto firstLineOffset() const noexcept -> int { return _firstLineOffset; }
    /// Access the additional indentation for wrapped lines and continuation blocks.
    [[nodiscard]] auto continuationOffset() const noexcept -> int { return _continuationOffset; }
    /// Access the additional plain-text indent for the prefixed line.
    [[nodiscard]] auto plainFirstLineOffset() const noexcept -> int { return _plainFirstLineOffset; }
    /// Access the additional plain-text indent for continuation lines and blocks.
    [[nodiscard]] auto plainContinuationIndent() const noexcept -> int { return _plainContinuationIndent; }

    /// Attach the list prefix to the given block.
    /// @param block The block to update.
    void applyPrefixTo(RenderBlock &block) const;
    /// Shift the block as a continuation of an already prefixed list item.
    /// @param block The block to update.
    void applyContinuationTo(RenderBlock &block) const;

private:
    ListPrefix _prefix;              ///< The rendered list prefix.
    int _firstLineOffset{0};         ///< Additional indentation before the visible prefix on the first block.
    int _continuationOffset{0};      ///< Additional indentation for wrapped lines and continuation blocks.
    int _plainFirstLineOffset{0};    ///< Additional plain-text indent for the prefixed line.
    int _plainContinuationIndent{0}; ///< Additional plain-text indent for continuation lines and blocks.
};

}
