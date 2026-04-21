// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "HtmlParserTestSupport.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(HtmlParser)
class HtmlParserTest final : public UNITTEST_SUBCLASS(HtmlParserTestSupport) {
public:
    void testParseBuildsInlineStructureForFragments() {
        const auto root = parse(R"(This is a <strong>text</strong> with <span class="hl"><em>highlights</em></span>.)");

        requireTreeEqual(
            root,
            {
                "Document",
                "  Paragraph",
                "    Text text=\"This is a \"",
                "    Strong",
                "      Text text=\"text\"",
                "    Text text=\" with \"",
                "    Span style=\"hl\"",
                "      Emphasis",
                "        Text text=\"highlights\"",
                "    Text text=\".\"",
            });
    }

    void testParseDropsIgnoredDocumentBoilerplateAndKeepsBodyContent() {
        const auto root = parse(
            "<!doctype html><html lang=en><head><title>Ignored</title><script>hidden()</script></head>"
            "<body><h1>Title</h1><p>Visible <b>text</b></p></body></html>");

        requireTreeEqual(
            root,
            {
                "Document",
                "  Heading level=1",
                "    Text text=\"Title\"",
                "  Paragraph",
                "    Text text=\"Visible \"",
                "    Strong",
                "      Text text=\"text\"",
            });
    }

    void testParseRecoversMissingListItemEndTags() {
        const auto root = parse("A test with a list.<ul><li>Entry<li>Another entry</li></ul>");

        requireTreeEqual(
            root,
            {
                "Document",
                "  Paragraph",
                "    Text text=\"A test with a list.\"",
                "  BulletList",
                "    ListItem",
                "      Paragraph",
                "        Text text=\"Entry\"",
                "    ListItem",
                "      Paragraph",
                "        Text text=\"Another entry\"",
            });
    }

    void testParseClosesNestedChildrenWhenAParentEndTagAppears() {
        const auto root = parse("<p><strong>bold</p>tail");

        requireTreeEqual(
            root,
            {
                "Document",
                "  Paragraph",
                "    Strong",
                "      Text text=\"bold\"",
                "  Paragraph",
                "    Text text=\"tail\"",
            });
    }

    void testParseTreatsTransparentAndSuppressedTagsDifferently() {
        const auto root = parse("<figure><p>shown</p></figure><script>hidden</script><tr><span>cell</span></tr>");

        requireTreeEqual(
            root,
            {
                "Document",
                "  Paragraph",
                "    Text text=\"shown\"",
                "  Paragraph",
                "    Span",
                "      Text text=\"cell\"",
            });
    }

    void testParseCreatesUnsupportedPlaceholdersOncePerUnsupportedElement() {
        const auto root = parse(
            R"(<img class="hero"/><table><tr><td>hidden</td></tr></table><form><input>ignored</form><svg><text>ignored</text></svg>)");

        requireTreeEqual(
            root,
            {
                "Document",
                "  Unsupported style=\"hero\" text=\"image\"",
                "  Unsupported text=\"table\"",
                "  Unsupported text=\"form\"",
                "  Unsupported text=\"svg\"",
            });
    }

    void testParsePreservesWhitespaceInsidePreformattedBlocks() {
        const auto root = parse("<pre>  a\n  b</pre>");

        requireTreeEqual(
            root,
            {
                "Document",
                "  CodeBlock",
                "    Text text=\"  a\\n  b\"",
            });
    }

    void testParseMapsAttributesToTheCreatedNodes() {
        const auto root = parse(R"(<a id="link-id" class="cta" href="/target">Go</a><hr id="rule" class="sep">)");

        requireTreeEqual(
            root,
            {
                "Document",
                "  Paragraph",
                "    Link id=\"link-id\" style=\"cta\" data=\"/target\"",
                "      Text text=\"Go\"",
                "  HorizontalLine id=\"rule\" style=\"sep\"",
            });
    }

    void testParseSkipsWhitespaceOnlyTokensBeforeBlockStarts() {
        const auto root = parse("<p>alpha</p> <div>beta</div>");

        requireTreeEqual(
            root,
            {
                "Document",
                "  Paragraph",
                "    Text text=\"alpha\"",
                "  Section",
                "    Paragraph",
                "      Text text=\"beta\"",
            });
    }

    void testParsePreservesWhitespaceOnlyTokensInsideInlineContainers() {
        const auto root = parse("<p>Hello<span> </span><em>world</em></p>");

        requireTreeEqual(
            root,
            {
                "Document",
                "  Paragraph",
                "    Text text=\"Hello\"",
                "    Span",
                "      Text text=\" \"",
                "    Emphasis",
                "      Text text=\"world\"",
            });
    }

    void testParseIgnoresMalformedCloseTagsWithoutCreatingErrorNodes() {
        const auto root = parse("text</bogus><div>more</span>");

        requireTreeEqual(
            root,
            {
                "Document",
                "  Paragraph",
                "    Text text=\"text\"",
                "  Section",
                "    Paragraph",
                "      Text text=\"more\"",
            });
    }
};
