// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "RenderContext.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

auto RenderContext::withContainer(const StyleRule &containerRule) const -> RenderContext {
    return RenderContext{
        .textStyle = textStyle.withOverlay(containerRule.textStyle()),
        .horizontalMargins = Margins{
            0,
            horizontalMargins.right() + containerRule.margins().right(),
            0,
            horizontalMargins.left() + containerRule.margins().left()}};
}

auto RenderContext::withListItem(const StyleRule &listItemRule) const -> RenderContext {
    return RenderContext{
        .textStyle = textStyle.withOverlay(listItemRule.textStyle()),
        .horizontalMargins = Margins{
            0,
            horizontalMargins.right() + listItemRule.margins().right(),
            0,
            horizontalMargins.left() + listItemRule.margins().left()}};
}

auto RenderContext::resolvedTextStyle(const CharStyle &baseTextStyle, const StyleRule &blockRule) const -> CharStyle {
    return baseTextStyle.withOverlay(textStyle).withOverlay(blockRule.textStyle());
}

auto RenderContext::resolvedBlockStyle(const StyleRule &blockRule) const -> StyleRule {
    auto result = blockRule;
    result.setMargins(
        Margins{
            blockRule.margins().top(),
            blockRule.margins().right() + horizontalMargins.right(),
            blockRule.margins().bottom(),
            blockRule.margins().left() + horizontalMargins.left()});
    return result;
}

}
