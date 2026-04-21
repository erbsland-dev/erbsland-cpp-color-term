// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ListItemLayout.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

void ListItemLayout::applyPrefixTo(RenderBlock &block) const {
    auto indents = block.style().indents();
    indents.setLineIndent(indents.lineIndent() + continuationOffset());
    indents.setFirstLineIndent(indents.firstLineIndent() + firstLineOffset());
    indents.setWrappedLineIndent(indents.wrappedLineIndent() + continuationOffset());
    block.style().setIndents(indents);
    block.addStringFirstLineIndent(plainFirstLineOffset());
    block.addStringWrappedLineIndent(plainContinuationIndent());
    block.setListPrefix(prefix());
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
