// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "PlanningContext.hpp"


namespace erbsland::cterm::text::impl::text_node_renderer {


auto PlanningContext::withContainer(const StyleRule &containerRule) const -> PlanningContext {
    return PlanningContext{
        .textStyle = textStyle.withOverlay(containerRule.textStyle()),
        .margins = Margins{
            margins.top(),
            margins.right() + containerRule.margins().right(),
            margins.bottom(),
            margins.left() + containerRule.margins().left()}};
}


auto PlanningContext::withHorizontalMargins(const Margins &edgeMargins) const -> PlanningContext {
    return PlanningContext{
        .textStyle = textStyle,
        .margins = Margins{
            margins.top(),
            margins.right() + edgeMargins.right(),
            margins.bottom(),
            margins.left() + edgeMargins.left()}};
}


auto PlanningContext::withEdgeMargins(const Margins &edgeMargins, const bool isFirstChild, const bool isLastChild) const
    -> PlanningContext {
    return PlanningContext{
        .textStyle = textStyle,
        .margins = Margins{
            margins.top() + (isFirstChild ? edgeMargins.top() : 0),
            margins.right(),
            margins.bottom() + (isLastChild ? edgeMargins.bottom() : 0),
            margins.left()}};
}


auto PlanningContext::withListItem(const StyleRule &listItemRule) const -> PlanningContext {
    return PlanningContext{
        .textStyle = textStyle.withOverlay(listItemRule.textStyle()),
        .margins = Margins{
            0, margins.right() + listItemRule.margins().right(), 0, margins.left() + listItemRule.margins().left()}};
}


auto PlanningContext::resolvedTextStyle(const CharStyle &baseTextStyle, const StyleRule &blockStyle) const
    -> CharStyle {
    return baseTextStyle.withOverlay(textStyle).withOverlay(blockStyle.textStyle());
}


auto PlanningContext::resolvedBlockStyle(const StyleRule &blockStyle) const -> StyleRule {
    auto result = blockStyle;
    result.setMargins(
        Margins{
            blockStyle.margins().top() + margins.top(),
            blockStyle.margins().right() + margins.right(),
            blockStyle.margins().bottom() + margins.bottom(),
            blockStyle.margins().left() + margins.left()});
    return result;
}


}
