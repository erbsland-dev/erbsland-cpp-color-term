// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "CursorBlockTarget.hpp"

#include "../../../Buffer.hpp"

#include <algorithm>
#include <utility>

namespace erbsland::cterm::text::impl::text_node_renderer {

CursorBlockTarget::CursorBlockTarget(CursorWriterPtr cursorWriter) : _cursorWriter{std::move(cursorWriter)} {
}

void CursorBlockTarget::writeBlock(const RenderBlock &block) {
    if (!_cursorWriter) {
        return;
    }
    writeBlockTopMargin(block.style());
    switch (block.kind()) {
    case BlockKind::Paragraph:
        updateParagraphOptions(_paragraphOptions, block.style());
        _cursorWriter->printParagraph(paragraphText(block), _paragraphOptions);
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

void CursorBlockTarget::finish() {
    if (!_cursorWriter) {
        return;
    }
    writeBlankLines(_state.previousBottomMargin);
}

void CursorBlockTarget::renderFilledLine(const RenderBlock &block) {
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
    line.drawText(
        Position{std::min(block.style().indents().firstLineIndent(), contentWidth - 1), 0}, paragraphText(block));
    _cursorWriter->write(line);
}

void CursorBlockTarget::renderHorizontalRule(const RenderBlock &block) {
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

void CursorBlockTarget::writeBlockTopMargin(const StyleRule &blockStyle) {
    const auto currentTopMargin = std::max(blockStyle.margins().top(), 0);
    writeBlankLines(_state.firstBlock ? currentTopMargin : std::max(_state.previousBottomMargin, currentTopMargin));
}

void CursorBlockTarget::finishRenderedBlock(const StyleRule &blockStyle) noexcept {
    _state.previousBottomMargin = std::max(blockStyle.margins().bottom(), 0);
    _state.firstBlock = false;
}

auto CursorBlockTarget::paragraphText(const RenderBlock &block) -> const String & {
    if (!block.listPrefix().has_value()) {
        return block.text();
    }
    _textScratch.clear();
    _textScratch.reserve(block.listPrefix()->terminalText().size() + block.text().size());
    _textScratch.append(block.listPrefix()->terminalText());
    _textScratch.append(block.text());
    _textScratchString = _textScratch.toString();
    return _textScratchString;
}

void CursorBlockTarget::updateParagraphOptions(ParagraphOptions &options, const StyleRule &blockStyle) noexcept {
    options.setIndents(blockStyle.indents());
    options.setParagraphSpacing(ParagraphSpacing::SingleLine);
}

void CursorBlockTarget::writeSpaces(const int count) noexcept {
    if (count <= 0) {
        return;
    }
    _cursorWriter->writeRepeated(Char{U' '}, count);
}

void CursorBlockTarget::writeBlankLines(const int lineCount) noexcept {
    for (auto index = 0; index < lineCount; ++index) {
        _cursorWriter->writeLineBreak();
    }
}

}
