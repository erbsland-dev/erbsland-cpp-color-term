// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/text/impl/text_node_renderer/InlineTextPlanner.hpp>
#include <erbsland/cterm/text/impl/text_node_renderer/PlanningStyleResolver.hpp>
#include <erbsland/cterm/text/Style.hpp>
#include <erbsland/cterm/text/TextNode.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <string>


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


TESTED_TARGETS(InlineTextPlanner)
class InlineTextPlannerTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testInlinePlannerPreservesNestedStyleOverlaysAndLineBreaks() {
        auto paragraph = text::TextNode::createParagraph();
        paragraph->addChild(text::TextNode::createText(U"A"));

        auto strong = text::TextNode::createStrong();
        strong->addChild(text::TextNode::createText(U"B"));
        paragraph->addChild(strong);

        paragraph->addChild(text::TextNode::createLineBreak());

        auto emphasis = text::TextNode::createEmphasis();
        emphasis->addChild(text::TextNode::createText(U"C"));
        paragraph->addChild(emphasis);

        const auto resolver = tnr::PlanningStyleResolver{text::Style::defaultStyle()};
        const auto planner = tnr::InlineTextPlanner{resolver};
        const auto rendered = planner.render(*paragraph, CharStyle{fg::White});

        REQUIRE_EQUAL(toPlainText(rendered), std::string{"AB\nC"});
        REQUIRE_FALSE(rendered[0].attributes().isBold());
        REQUIRE(rendered[1].attributes().isBold());
        REQUIRE(rendered[3].attributes().isItalic());
    }
};
