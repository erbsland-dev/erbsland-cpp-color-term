// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/StringTestHelper.hpp"

#include <erbsland/cterm/text/impl/text_node_renderer/StyleResolver.hpp>
#include <erbsland/cterm/text/Style.hpp>
#include <erbsland/cterm/text/TextNode.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <string>
#include <vector>

TESTED_TARGETS(StyleResolver)
class StyleResolverTest final : public UNITTEST_SUBCLASS(StringTestHelper) {
public:
    void testSplitStyleTokensNormalizesWhitespaceSortsAndDeduplicates() {
        const auto tokens = tnr::StyleResolver::splitStyleTokens(" warning\tbeta warning \n hero  ");

        REQUIRE_EQUAL(tokens, std::vector<std::string>({"beta", "hero", "warning"}));
    }

    void testBlockRuleUsesStyleTokensAndHeadingLevels() {
        auto style = std::make_shared<text::Style>();
        style->edit(text::StyleSelector{text::StyleRole::Paragraph, {"warning"}}).setPrefix(U"[warning] ");
        style->edit(text::StyleSelector::heading(2)).setPrefix(U"[h2] ");

        auto paragraph = text::TextNode::createParagraph();
        paragraph->setStyle("warning warning");
        paragraph->addChild(text::TextNode::createText(U"Careful"));

        auto heading = text::TextNode::createHeading(2);
        heading->addChild(text::TextNode::createText(U"Title"));

        auto resolver = tnr::StyleResolver{style};
        const auto paragraphRule = resolver.blockRule(*paragraph, text::StyleRole::Paragraph);
        const auto headingRule = resolver.blockRule(*heading, text::StyleRole::Heading);

        REQUIRE(paragraphRule.prefix().has_value());
        REQUIRE_EQUAL(toPlainText(*paragraphRule.prefix()), std::string{"[warning] "});
        REQUIRE(headingRule.prefix().has_value());
        REQUIRE_EQUAL(toPlainText(*headingRule.prefix()), std::string{"[h2] "});
    }
};
