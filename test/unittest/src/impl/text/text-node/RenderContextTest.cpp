// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/cterm/text/impl/text_node_renderer/RenderContext.hpp>
#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(RenderContext)
class RenderContextTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testContainerInheritanceAndResolvedStylesUseTheOwningContextMethods() {
        auto context = tnr::RenderContext{};
        context.textStyle = CharStyle{Color{fg::Inherited, bg::Inherited}, CharAttributes::Underline};
        context.horizontalMargins = Margins{0, 2, 0, 4};

        auto containerRule = text::StyleRule{};
        containerRule.setTextStyle(Color{fg::BrightRed, bg::Inherited}, CharAttributes::Italic);
        containerRule.setMargins(5, 6, 7, 8);

        const auto childContext = context.withContainer(containerRule);
        REQUIRE_EQUAL(childContext.horizontalMargins.top(), 0);
        REQUIRE_EQUAL(childContext.horizontalMargins.right(), 8);
        REQUIRE_EQUAL(childContext.horizontalMargins.bottom(), 0);
        REQUIRE_EQUAL(childContext.horizontalMargins.left(), 12);

        auto blockRule = text::StyleRule{};
        blockRule.setTextStyle(Color{fg::Inherited, bg::Inherited}, CharAttributes::Bold);
        blockRule.setMargins(1, 1, 1, 1);

        const auto resolvedTextStyle = childContext.resolvedTextStyle(CharStyle{fg::White}, blockRule);
        REQUIRE_EQUAL(resolvedTextStyle.color().fg(), fg::BrightRed);
        REQUIRE(resolvedTextStyle.attributes().isBold());
        REQUIRE(resolvedTextStyle.attributes().isItalic());
        REQUIRE(resolvedTextStyle.attributes().isUnderline());

        const auto resolvedBlockStyle = childContext.resolvedBlockStyle(blockRule);
        REQUIRE_EQUAL(resolvedBlockStyle.margins().top(), 1);
        REQUIRE_EQUAL(resolvedBlockStyle.margins().right(), 9);
        REQUIRE_EQUAL(resolvedBlockStyle.margins().bottom(), 1);
        REQUIRE_EQUAL(resolvedBlockStyle.margins().left(), 13);
    }

    void testListItemContextsKeepOnlyHorizontalMarginsAndInheritTextStyle() {
        auto context = tnr::RenderContext{};
        context.textStyle = CharStyle{Color{fg::BrightBlue}, CharAttributes::Underline};
        context.horizontalMargins = Margins{0, 2, 0, 4};

        auto listItemRule = text::StyleRule{};
        listItemRule.setTextStyle(Color{fg::Inherited, bg::Inherited}, CharAttributes::Italic);
        listItemRule.setMargins(9, 10, 11, 12);

        const auto listItemContext = context.withListItem(listItemRule);
        REQUIRE_EQUAL(listItemContext.horizontalMargins.top(), 0);
        REQUIRE_EQUAL(listItemContext.horizontalMargins.right(), 12);
        REQUIRE_EQUAL(listItemContext.horizontalMargins.bottom(), 0);
        REQUIRE_EQUAL(listItemContext.horizontalMargins.left(), 16);
        REQUIRE(listItemContext.textStyle.attributes().isItalic());
        REQUIRE(listItemContext.textStyle.attributes().isUnderline());
    }
};
