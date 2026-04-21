// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "RenderBlock.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

/// Receives render blocks from the streaming tree renderer.
class BlockTarget {
public:
    // defaults
    virtual ~BlockTarget() = default;
    BlockTarget() = default;
    BlockTarget(const BlockTarget &) = delete;
    BlockTarget(BlockTarget &&) = delete;
    auto operator=(const BlockTarget &) -> BlockTarget & = delete;
    auto operator=(BlockTarget &&) -> BlockTarget & = delete;

public:
    /// Write one fully prepared block.
    /// @param block The block to write.
    virtual void writeBlock(const RenderBlock &block) = 0;
    /// Finish the block stream after the last block was written.
    virtual void finish() = 0;
};

}
