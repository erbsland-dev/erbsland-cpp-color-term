// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "BlockTarget.hpp"
#include "RenderToState.hpp"

#include "../../../CursorWriter.hpp"
#include "../../../impl/StringBuilder.hpp"
#include "../../../ParagraphOptions.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

/// Block target that renders the stream to a cursor writer.
class CursorBlockTarget final : public BlockTarget {
public:
    /// Create a target for the given cursor writer.
    /// @param cursorWriter The output writer. `nullptr` disables output.
    explicit CursorBlockTarget(CursorWriterPtr cursorWriter);

    // defaults
    ~CursorBlockTarget() override = default;
    CursorBlockTarget(const CursorBlockTarget &) = delete;
    CursorBlockTarget(CursorBlockTarget &&) = delete;
    auto operator=(const CursorBlockTarget &) -> CursorBlockTarget & = delete;
    auto operator=(CursorBlockTarget &&) -> CursorBlockTarget & = delete;

public:
    /// Write one fully prepared block.
    /// @param block The block to write.
    void writeBlock(const RenderBlock &block) override;
    /// Finish the block stream and write the final bottom margin.
    void finish() override;

private:
    void renderFilledLine(const RenderBlock &block);
    void renderHorizontalRule(const RenderBlock &block);
    void writeBlockTopMargin(const StyleRule &blockStyle);
    void finishRenderedBlock(const StyleRule &blockStyle) noexcept;
    [[nodiscard]] auto paragraphText(const RenderBlock &block) -> const String &;
    static void updateParagraphOptions(ParagraphOptions &options, const StyleRule &blockStyle) noexcept;
    void writeSpaces(int count) noexcept;
    void writeBlankLines(int lineCount) noexcept;

private:
    CursorWriterPtr _cursorWriter;           ///< The output target.
    RenderToState _state;                    ///< The current streaming state.
    ParagraphOptions _paragraphOptions;      ///< Reused options for paragraph output.
    cterm::impl::StringBuilder _textScratch; ///< Scratch buffer for prefixed paragraph text.
    String _textScratchString;               ///< Materialized prefixed paragraph text.
};

}
