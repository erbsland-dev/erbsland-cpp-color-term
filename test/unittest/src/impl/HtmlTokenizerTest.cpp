// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/text/impl/HtmlTokenizer.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <initializer_list>
#include <string_view>
#include <utility>
#include <vector>

namespace textimpl = erbsland::cterm::text::impl;

TESTED_TARGETS(HtmlTokenizer)
class HtmlTokenizerTest final : public el::UnitTest {
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

private:
    struct ExpectedToken final {
        textimpl::HtmlTokenizer::TokenType type;
        std::u32string value;
    };

    [[nodiscard]] static auto tokenize(const std::string_view text) -> std::vector<textimpl::HtmlTokenizer::Token> {
        auto result = std::vector<textimpl::HtmlTokenizer::Token>{};
        auto tokenizer = textimpl::HtmlTokenizer{text};
        for (auto token : tokenizer.tokenize()) {
            result.push_back(std::move(token));
        }
        return result;
    }

    void requireTokensEqual(
        const std::vector<textimpl::HtmlTokenizer::Token> &actual,
        const std::initializer_list<ExpectedToken> expected) {
        REQUIRE_EQUAL(actual.size(), expected.size());
        auto index = std::size_t{0};
        for (const auto &expectedToken : expected) {
            runWithContext(
                SOURCE_LOCATION(),
                [&]() {
                    REQUIRE_EQUAL(actual[index].type, expectedToken.type);
                    REQUIRE_EQUAL(actual[index].value, expectedToken.value);
                },
                [&]() -> std::string {
                    return std::format(
                        "index = {} / actualType = {} / actualValue = {} / expectedType = {} / expectedValue = {}",
                        index,
                        static_cast<int>(actual[index].type),
                        render(actual[index].value),
                        static_cast<int>(expectedToken.type),
                        render(expectedToken.value));
                });
            index += 1;
        }
    }

    [[nodiscard]] static auto render(const std::u32string &text) -> std::string {
        auto result = std::string{};
        for (const auto character : text) {
            Char{character}.appendTo(result);
        }
        return result;
    }
};
