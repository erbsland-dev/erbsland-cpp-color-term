// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/text/impl/text_node_renderer/PlanningStyleResolver.hpp>
#include <erbsland/cterm/text/Style.hpp>
#include <erbsland/cterm/text/TextNode.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <string>
#include <vector>

namespace text = erbsland::cterm::text;
namespace tnr = erbsland::cterm::text::impl::text_node_renderer;

namespace {

auto toPlainText(const String &textValue) -> std::string {
    auto result = std::string{};
    for (const auto &character : textValue) {
        result += character.charStr();
    }
    return result;
}

} // namespace

TESTED_TARGETS(PlanningStyleResolver)
class PlanningStyleResolverTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testSplitStyleTokensNormalizesWhitespaceSortsAndDeduplicates() {
        const auto tokens = tnr::PlanningStyleResolver::splitStyleTokens(" warning\tbeta warning \n hero  ");

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

        const auto resolver = tnr::PlanningStyleResolver{style};
        const auto paragraphRule = resolver.blockRule(*paragraph, text::StyleRole::Paragraph);
        const auto headingRule = resolver.blockRule(*heading, text::StyleRole::Heading);

        REQUIRE(paragraphRule.prefix().has_value());
        REQUIRE_EQUAL(toPlainText(*paragraphRule.prefix()), std::string{"[warning] "});
        REQUIRE(headingRule.prefix().has_value());
        REQUIRE_EQUAL(toPlainText(*headingRule.prefix()), std::string{"[h2] "});
    }

    void testInlineRoleMapsOnlySemanticInlineNodeTypes() {
        REQUIRE_EQUAL(tnr::PlanningStyleResolver::inlineRole(text::TextNode::Type::Strong), text::StyleRole::Strong);
        REQUIRE_EQUAL(tnr::PlanningStyleResolver::inlineRole(text::TextNode::Type::Code), text::StyleRole::Code);
        REQUIRE_FALSE(tnr::PlanningStyleResolver::inlineRole(text::TextNode::Type::Paragraph).has_value());
    }
};
