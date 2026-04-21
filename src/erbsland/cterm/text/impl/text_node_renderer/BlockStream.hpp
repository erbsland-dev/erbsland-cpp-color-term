// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "BlockTarget.hpp"
#include "RenderBlock.hpp"

#include <optional>
#include <utility>

namespace erbsland::cterm::text::impl::text_node_renderer {

/// Streams render blocks while keeping the most recent block patchable.
class BlockStream final {
public:
    /// Create a stream for the given target.
    /// @param target The target that receives flushed blocks.
    explicit BlockStream(BlockTarget &target) noexcept : _target{target} {}

    // defaults
    ~BlockStream() = default;
    BlockStream(const BlockStream &) = delete;
    BlockStream(BlockStream &&) = delete;
    auto operator=(const BlockStream &) -> BlockStream & = delete;
    auto operator=(BlockStream &&) -> BlockStream & = delete;

public:
    /// Append one block and flush the previously pending block.
    /// @param block The new block.
    void append(RenderBlock &&block) {
        flushPending();
        _pending.emplace(std::move(block));
    }
    /// Access the pending block that can still receive closing-scope changes.
    /// @return The pending block, or `nullptr` when no block is pending.
    [[nodiscard]] auto pendingBlock() noexcept -> RenderBlock * {
        if (!_pending.has_value()) {
            return nullptr;
        }
        return &*_pending;
    }
    /// Flush all remaining output and finish the target.
    void finish() {
        flushPending();
        _target.finish();
    }

private:
    void flushPending() {
        if (!_pending.has_value()) {
            return;
        }
        _target.writeBlock(*_pending);
        _pending.reset();
    }

private:
    BlockTarget &_target;                ///< The target that receives flushed blocks.
    std::optional<RenderBlock> _pending; ///< The newest block, retained for scope finalization.
};

}
