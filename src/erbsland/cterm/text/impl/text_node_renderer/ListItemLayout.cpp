// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ListItemLayout.hpp"

#include "../../../impl/StringBuilder.hpp"
#include "../../../StringView.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

void ListItemLayout::applyPrefixTo(RenderBlock &block) const {
    auto indents = block.style().indents();
    indents.setLineIndent(indents.lineIndent() + continuationOffset());
    indents.setFirstLineIndent(indents.firstLineIndent() + firstLineOffset());
    indents.setWrappedLineIndent(indents.wrappedLineIndent() + continuationOffset());
    block.style().setIndents(indents);
    block.addStringFirstLineIndent(plainFirstLineOffset());
    block.addStringWrappedLineIndent(plainContinuationIndent());

    auto prefixedText = cterm::impl::StringBuilder{};
    prefixedText.reserve(prefix().text().size() + block.text().size());
    prefixedText.append(prefix().text());
    prefixedText.append(block.text());
    auto terminalText = cterm::impl::StringBuilder{};
    terminalText.reserve(prefix().terminalText().size() + block.text().size());
    terminalText.append(prefix().terminalText());
    terminalText.append(block.text());
    block.setTerminalText(terminalText.takeString());
    block.setText(prefixedText.takeString());
}

void ListItemLayout::applyContinuationTo(RenderBlock &block) const {
    auto indents = block.style().indents();
    indents.setLineIndent(indents.lineIndent() + continuationOffset());
    indents.setFirstLineIndent(indents.firstLineIndent() + continuationOffset());
    indents.setWrappedLineIndent(indents.wrappedLineIndent() + continuationOffset());
    block.style().setIndents(indents);
    block.addStringFirstLineIndent(plainContinuationIndent());
    block.addStringWrappedLineIndent(plainContinuationIndent());
}

}
