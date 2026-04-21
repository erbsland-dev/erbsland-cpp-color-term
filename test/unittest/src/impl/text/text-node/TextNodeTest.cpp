// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/cterm/text/TextNode.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <vector>

TESTED_TARGETS(TextNode)
class TextNodeTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testCreateUnsupportedBuildsTheExpectedPlaceholderNode() {
        const auto node = text::TextNode::createUnsupported(U"image");
        const auto expected = std::vector<std::string>{"Unsupported text=\"image\""};

        REQUIRE_EQUAL(node->type(), text::TextNode::Type::Unsupported);
        REQUIRE_EQUAL_LINES(node->toDiagnosticTree(), expected);
    }

    void testCreateFactoryBuildsTypedNodesWithExpectedDefaults() {
        const auto paragraph = text::TextNode::create(text::TextNode::Type::Paragraph);
        const auto link = text::TextNode::create(text::TextNode::Type::Link);

        REQUIRE_EQUAL(paragraph->type(), text::TextNode::Type::Paragraph);
        REQUIRE_EQUAL(link->type(), text::TextNode::Type::Link);
        REQUIRE(link->data().has_value());
        REQUIRE_EQUAL(link->data().value(), "");
    }

    void testNodeTypeHelpersClassifyKnownNodeKinds() {
        REQUIRE(text::TextNode::isInlineNodeType(text::TextNode::Type::Strong));
        REQUIRE(text::TextNode::isTextContainerType(text::TextNode::Type::CodeBlock));
        REQUIRE(text::TextNode::isListNodeType(text::TextNode::Type::BulletList));
        REQUIRE_FALSE(text::TextNode::isInlineNodeType(text::TextNode::Type::Paragraph));
    }
};
