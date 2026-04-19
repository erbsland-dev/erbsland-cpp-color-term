// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/text/impl/text_node_renderer/TextNodePlanningQueries.hpp>
#include <erbsland/cterm/text/TextNode.hpp>
#include <erbsland/unittest/UnitTest.hpp>

namespace text = erbsland::cterm::text;
namespace tnr = erbsland::cterm::text::impl::text_node_renderer;

TESTED_TARGETS(TextNodePlanningQueries)
class TextNodePlanningQueriesTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testQueriesDetectRenderableChildrenAndMatchingRanges() {
        auto document = text::TextNode::createDocument();
        document->addChild({});
        auto span = text::TextNode::createSpan();
        span->addChild(text::TextNode::createText(U"inline only"));
        document->addChild(span);
        auto paragraph = text::TextNode::createParagraph();
        paragraph->addChild(text::TextNode::createText(U"text"));
        document->addChild(paragraph);
        document->addChild({});
        document->addChild(std::make_shared<text::TextNode>(text::TextNode::Type::Error, std::u32string{U"broken"}));

        const auto queries = tnr::TextNodePlanningQueries{};
        const auto range =
            queries.findChildRange(*document, tnr::TextNodePlanningQueries::ChildSelection::RenderableBlocks);

        REQUIRE(range.has_value());
        REQUIRE_EQUAL(range->first, std::size_t{2});
        REQUIRE_EQUAL(range->last, std::size_t{4});
        REQUIRE_FALSE(queries.emitsBlocks(*span));
        REQUIRE(queries.emitsBlocks(*paragraph));
    }

    void testQueriesRecognizeListItemsSeparatelyFromRenderableBlocks() {
        const auto listItem = text::TextNode::createListItem();
        const auto paragraph = text::TextNode::createParagraph();

        const auto queries = tnr::TextNodePlanningQueries{};

        REQUIRE(queries.childMatchesSelection(*listItem, tnr::TextNodePlanningQueries::ChildSelection::ListItems));
        REQUIRE_FALSE(
            queries.childMatchesSelection(*paragraph, tnr::TextNodePlanningQueries::ChildSelection::ListItems));
    }
};
