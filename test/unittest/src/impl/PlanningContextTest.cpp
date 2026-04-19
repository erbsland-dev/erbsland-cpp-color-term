// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/text/impl/text_node_renderer/PlanningContext.hpp>
#include <erbsland/unittest/UnitTest.hpp>

namespace text = erbsland::cterm::text;
namespace tnr = erbsland::cterm::text::impl::text_node_renderer;

TESTED_TARGETS(PlanningContext)
class PlanningContextTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testContainerInheritanceAndResolvedStylesUseTheOwningContextMethods() {
        auto context = tnr::PlanningContext{};
        context.textStyle = CharStyle{Color{fg::Inherited, bg::Inherited}, CharAttributes::Underline};
        context.margins = Margins{1, 2, 3, 4};

        auto containerRule = text::StyleRule{};
        containerRule.setTextStyle(Color{fg::BrightRed, bg::Inherited}, CharAttributes::Italic);
        containerRule.setMargins(5, 6, 7, 8);

        const auto childContext = context.withContainer(containerRule);
        REQUIRE_EQUAL(childContext.margins.top(), 1);
        REQUIRE_EQUAL(childContext.margins.right(), 8);
        REQUIRE_EQUAL(childContext.margins.bottom(), 3);
        REQUIRE_EQUAL(childContext.margins.left(), 12);

        auto blockRule = text::StyleRule{};
        blockRule.setTextStyle(Color{fg::Inherited, bg::Inherited}, CharAttributes::Bold);
        blockRule.setMargins(1, 1, 1, 1);

        const auto resolvedTextStyle = childContext.resolvedTextStyle(CharStyle{fg::White}, blockRule);
        REQUIRE_EQUAL(resolvedTextStyle.color().fg(), fg::BrightRed);
        REQUIRE(resolvedTextStyle.attributes().isBold());
        REQUIRE(resolvedTextStyle.attributes().isItalic());
        REQUIRE(resolvedTextStyle.attributes().isUnderline());

        const auto resolvedBlockStyle = childContext.resolvedBlockStyle(blockRule);
        REQUIRE_EQUAL(resolvedBlockStyle.margins().top(), 2);
        REQUIRE_EQUAL(resolvedBlockStyle.margins().right(), 9);
        REQUIRE_EQUAL(resolvedBlockStyle.margins().bottom(), 4);
        REQUIRE_EQUAL(resolvedBlockStyle.margins().left(), 13);
    }

    void testEdgeMarginsAndListItemContextsKeepOnlyTheIntendedMargins() {
        auto context = tnr::PlanningContext{};
        context.textStyle = CharStyle{Color{fg::BrightBlue}, CharAttributes::Underline};
        context.margins = Margins{1, 2, 3, 4};

        const auto firstChild = context.withEdgeMargins(Margins{5, 6, 7, 8}, true, false);
        REQUIRE_EQUAL(firstChild.margins.top(), 6);
        REQUIRE_EQUAL(firstChild.margins.right(), 2);
        REQUIRE_EQUAL(firstChild.margins.bottom(), 3);
        REQUIRE_EQUAL(firstChild.margins.left(), 4);

        auto listItemRule = text::StyleRule{};
        listItemRule.setTextStyle(Color{fg::Inherited, bg::Inherited}, CharAttributes::Italic);
        listItemRule.setMargins(9, 10, 11, 12);

        const auto listItemContext = context.withListItem(listItemRule);
        REQUIRE_EQUAL(listItemContext.margins.top(), 0);
        REQUIRE_EQUAL(listItemContext.margins.right(), 12);
        REQUIRE_EQUAL(listItemContext.margins.bottom(), 0);
        REQUIRE_EQUAL(listItemContext.margins.left(), 16);
        REQUIRE(listItemContext.textStyle.attributes().isItalic());
        REQUIRE(listItemContext.textStyle.attributes().isUnderline());
    }
};
