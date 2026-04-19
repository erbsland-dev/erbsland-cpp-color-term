// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "RenderBlock.hpp"
#include "RenderToState.hpp"

#include "../../../CursorWriter.hpp"
#include "../../../ParagraphOptions.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

/// Emit planned render blocks to a cursor-writer target.
class BlockEmitter final {
public:
    /// Create an emitter for the given cursor writer.
    /// @param cursorWriter The output writer. `nullptr` disables output.
    explicit BlockEmitter(CursorWriterPtr cursorWriter);

    // defaults
    ~BlockEmitter() = default;
    BlockEmitter(const BlockEmitter &) = delete;
    BlockEmitter(BlockEmitter &&) = delete;
    auto operator=(const BlockEmitter &) -> BlockEmitter & = delete;
    auto operator=(BlockEmitter &&) -> BlockEmitter & = delete;

public:
    /// Render the planned blocks to the configured writer.
    /// @param blocks The blocks to emit.
    void render(const RenderBlocks &blocks);

private:
    void renderBlock(const RenderBlock &block, ParagraphOptions &paragraphOptions);
    void renderFilledLine(const RenderBlock &block);
    void renderHorizontalRule(const RenderBlock &block);
    void writeBlockTopMargin(const StyleRule &blockStyle);
    void finishRenderedBlock(const StyleRule &blockStyle) noexcept;
    [[nodiscard]] static auto paragraphText(const RenderBlock &block) noexcept -> const String &;
    static void updateParagraphOptions(ParagraphOptions &options, const StyleRule &blockStyle) noexcept;
    void writeSpaces(int count) noexcept;
    void writeBlankLines(int lineCount) noexcept;

private:
    CursorWriterPtr _cursorWriter; ///< The output target.
    RenderToState _state;          ///< The current streaming state.
};

}
