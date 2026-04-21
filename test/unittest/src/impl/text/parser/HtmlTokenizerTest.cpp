// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "HtmlTokenizerTestSupport.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <string_view>

TESTED_TARGETS(HtmlTokenizer)
class HtmlTokenizerTest final : public UNITTEST_SUBCLASS(HtmlTokenizerTestSupport) {
public:
    void testTokenizeParsesPlainTextAndSimpleTags() {
        const auto tokens = tokenize("Hello <strong>world</strong>!");

        requireTokensEqual(
            tokens,
            {
                {textimpl::HtmlTokenizer::TokenType::Text, U"Hello "},
                {textimpl::HtmlTokenizer::TokenType::TagOpen, U"strong"},
                {textimpl::HtmlTokenizer::TokenType::Text, U"world"},
                {textimpl::HtmlTokenizer::TokenType::TagClose, U"strong"},
                {textimpl::HtmlTokenizer::TokenType::Text, U"!"},
            });
    }

    void testTokenizeParsesAttributesAndSelfClosingTags() {
        const auto tokens = tokenize(R"(<img alt="A &amp; B" src=test disabled />)");

        requireTokensEqual(
            tokens,
            {
                {textimpl::HtmlTokenizer::TokenType::TagOpen, U"img"},
                {textimpl::HtmlTokenizer::TokenType::AttributeName, U"alt"},
                {textimpl::HtmlTokenizer::TokenType::AttributeValue, U"A & B"},
                {textimpl::HtmlTokenizer::TokenType::AttributeName, U"src"},
                {textimpl::HtmlTokenizer::TokenType::AttributeValue, U"test"},
                {textimpl::HtmlTokenizer::TokenType::AttributeName, U"disabled"},
                {textimpl::HtmlTokenizer::TokenType::TagClose, U"img"},
            });
    }

    void testTokenizeParsesDocTypeCommentsAndEntities() {
        const auto tokens = tokenize("<!DOCTYPE html>Fish &amp; chips &#35; &#x41;<!-- note -->");

        requireTokensEqual(
            tokens,
            {
                {textimpl::HtmlTokenizer::TokenType::DocType, U"html"},
                {textimpl::HtmlTokenizer::TokenType::Text, U"Fish & chips # A"},
            });
    }

    void testTokenizeLeavesUnknownAndMalformedEntitiesAsLiteralText() {
        const auto tokens = tokenize("Text &bogus; &#x110000; &#x;");

        requireTokensEqual(
            tokens,
            {
                {textimpl::HtmlTokenizer::TokenType::Text, U"Text &bogus; &#x110000; &#x;"},
            });
    }

    void testTokenizeFallsBackToLiteralTextForMalformedOpenTags() {
        const auto tokens = tokenize(R"(prefix <strong title="broken suffix)");

        requireTokensEqual(
            tokens,
            {
                {textimpl::HtmlTokenizer::TokenType::Text, U"prefix "},
                {textimpl::HtmlTokenizer::TokenType::Text, U"<strong title=\"broken suffix"},
            });
    }

    void testTokenizeFallsBackToLiteralTextForMalformedCloseTagsAndDeclarations() {
        const auto tokens = tokenize("</><!not-html>");

        requireTokensEqual(
            tokens,
            {
                {textimpl::HtmlTokenizer::TokenType::Text, U"</>"},
                {textimpl::HtmlTokenizer::TokenType::Text, U"<!not-html>"},
            });
    }

    void testTokenizeFallsBackToLiteralTextForBrokenCommentsExactlyOnce() {
        const auto tokens = tokenize("A <!-- broken B");

        requireTokensEqual(
            tokens,
            {
                {textimpl::HtmlTokenizer::TokenType::Text, U"A "},
                {textimpl::HtmlTokenizer::TokenType::Text, U"<!-- broken B"},
            });
    }

    void testTokenizeKeepsDecodedEntitiesInMalformedTagFallback() {
        const auto tokens = tokenize(R"(prefix <strong title="A &amp; B suffix)");

        requireTokensEqual(
            tokens,
            {
                {textimpl::HtmlTokenizer::TokenType::Text, U"prefix "},
                {textimpl::HtmlTokenizer::TokenType::Text, U"<strong title=\"A & B suffix"},
            });
    }

    void testTokenizeExtendsMalformedTagFallbackUntilNextTag() {
        const auto tokens = tokenize("prefix </strong trailing <em>x</em>");

        requireTokensEqual(
            tokens,
            {
                {textimpl::HtmlTokenizer::TokenType::Text, U"prefix "},
                {textimpl::HtmlTokenizer::TokenType::Text, U"</strong trailing "},
                {textimpl::HtmlTokenizer::TokenType::TagOpen, U"em"},
                {textimpl::HtmlTokenizer::TokenType::Text, U"x"},
                {textimpl::HtmlTokenizer::TokenType::TagClose, U"em"},
            });
    }
};
