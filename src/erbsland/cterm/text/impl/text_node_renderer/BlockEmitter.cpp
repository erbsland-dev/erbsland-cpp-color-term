// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "BlockEmitter.hpp"

#include "../../../Buffer.hpp"

#include <algorithm>


namespace erbsland::cterm::text::impl::text_node_renderer {


BlockEmitter::BlockEmitter(CursorWriterPtr cursorWriter) : _cursorWriter{std::move(cursorWriter)} {
}


void BlockEmitter::render(const RenderBlocks &blocks) {
    if (!_cursorWriter) {
        return;
    }
    auto paragraphOptions = ParagraphOptions{};
    for (const auto &block : blocks) {
        renderBlock(block, paragraphOptions);
    }
    writeBlankLines(_state.previousBottomMargin);
}


void BlockEmitter::renderBlock(const RenderBlock &block, ParagraphOptions &paragraphOptions) {
    writeBlockTopMargin(block.style());
    switch (block.kind()) {
    case BlockKind::Paragraph:
        updateParagraphOptions(paragraphOptions, block.style());
        _cursorWriter->printParagraph(paragraphText(block), paragraphOptions);
        break;
    case BlockKind::HorizontalRule:
        renderHorizontalRule(block);
        break;
    case BlockKind::FilledLine:
        renderFilledLine(block);
        break;
    }
    finishRenderedBlock(block.style());
}


void BlockEmitter::renderFilledLine(const RenderBlock &block) {
    const auto margins = block.style().margins();
    const auto leftMargin = std::max(margins.left(), 0);
    const auto contentWidth =
        std::max(_cursorWriter->size().width() - std::max(margins.left(), 0) - std::max(margins.right(), 0), 0);
    if (leftMargin > 0) {
        writeSpaces(leftMargin);
    }
    if (contentWidth <= 0 || !block.fillCharacter().has_value() || block.fillCharacter()->displayWidth() != 1) {
        _cursorWriter->writeLineBreak();
        return;
    }
    auto line = Buffer{Size{contentWidth, 1}, *block.fillCharacter()};
    line.drawText(Position{std::min(block.style().indents().firstLineIndent(), contentWidth - 1), 0}, block.text());
    _cursorWriter->write(line);
}


void BlockEmitter::renderHorizontalRule(const RenderBlock &block) {
    const auto margins = block.style().margins();
    const auto leftMargin = std::max(margins.left() + block.style().indents().firstLineIndent(), 0);
    const auto contentWidth = std::max(
        _cursorWriter->size().width() - std::max(margins.left(), 0) - std::max(margins.right(), 0) -
            std::max(block.style().indents().firstLineIndent(), 0),
        0);
    if (leftMargin > 0) {
        writeSpaces(leftMargin);
    }
    if (contentWidth <= 0 || !block.fillCharacter().has_value() || block.fillCharacter()->displayWidth() != 1) {
        _cursorWriter->writeLineBreak();
        return;
    }
    auto line = Buffer{Size{contentWidth, 1}, *block.fillCharacter()};
    if (block.leadingText().has_value()) {
        line.drawText(Position{0, 0}, *block.leadingText());
    }
    if (block.trailingText().has_value()) {
        line.drawText(
            Position{std::max(contentWidth - block.trailingText()->displayWidth(), 0), 0}, *block.trailingText());
    }
    _cursorWriter->write(line);
}


void BlockEmitter::writeBlockTopMargin(const StyleRule &blockStyle) {
    const auto currentTopMargin = std::max(blockStyle.margins().top(), 0);
    writeBlankLines(_state.firstBlock ? currentTopMargin : std::max(_state.previousBottomMargin, currentTopMargin));
}


void BlockEmitter::finishRenderedBlock(const StyleRule &blockStyle) noexcept {
    _state.previousBottomMargin = std::max(blockStyle.margins().bottom(), 0);
    _state.firstBlock = false;
}


auto BlockEmitter::paragraphText(const RenderBlock &block) noexcept -> const String & {
    if (block.terminalText().has_value()) {
        return *block.terminalText();
    }
    return block.text();
}


void BlockEmitter::updateParagraphOptions(ParagraphOptions &options, const StyleRule &blockStyle) noexcept {
    options.setIndents(blockStyle.indents());
    options.setParagraphSpacing(ParagraphSpacing::SingleLine);
}


void BlockEmitter::writeSpaces(const int count) noexcept {
    if (count <= 0) {
        return;
    }
    _cursorWriter->writeRepeated(Char{U' '}, count);
}


void BlockEmitter::writeBlankLines(const int lineCount) noexcept {
    for (auto index = 0; index < lineCount; ++index) {
        _cursorWriter->writeLineBreak();
    }
}


}
