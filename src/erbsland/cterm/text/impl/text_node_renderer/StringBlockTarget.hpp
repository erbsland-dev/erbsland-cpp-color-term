// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "BlockTarget.hpp"

#include "../../../impl/StringBuilder.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

/// Block target that renders the stream to one terminal string.
class StringBlockTarget final : public BlockTarget {
public:
    /// Create an empty string target.
    StringBlockTarget() = default;

    // defaults
    ~StringBlockTarget() override = default;
    StringBlockTarget(const StringBlockTarget &) = delete;
    StringBlockTarget(StringBlockTarget &&) = delete;
    auto operator=(const StringBlockTarget &) -> StringBlockTarget & = delete;
    auto operator=(StringBlockTarget &&) -> StringBlockTarget & = delete;

public:
    /// Write one fully prepared block.
    /// @param block The block to write.
    void writeBlock(const RenderBlock &block) override {
        if (!_firstBlock) {
            _result.append(Char{U'\n'});
        }
        _result.append(block.renderString());
        _firstBlock = false;
    }
    /// Finish the block stream.
    void finish() override {}
    /// Materialize the final string.
    /// @return The rendered text.
    [[nodiscard]] auto takeString() -> String { return _result.takeString().trimmed(U"\n\r\t"); }

private:
    cterm::impl::StringBuilder _result; ///< The rendered text.
    bool _firstBlock{true};             ///< `true` until the first block has been written.
};

}
