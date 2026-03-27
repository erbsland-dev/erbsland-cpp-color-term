// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "BufferedTokenizer.hpp"
#include "Generator.hpp"
#include "HtmlToken.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>


namespace erbsland::cterm::text::impl {

/// A basic HTML tokenizer for parsing HTML text.
/// This tokenizer gracefully ignores any malformed HTML and provides a stream of tokens.
class HtmlTokenizer : public BufferedTokenizer {
public:
    using Token = HtmlToken;
    using TokenType = HtmlTokenType;
    using TokenGenerator = Generator<HtmlToken>;

public:
    /// Create a tokenizer for the given HTML source text.
    /// @param text The UTF-8 encoded HTML fragment to tokenize.
    explicit HtmlTokenizer(std::string_view text);

    // defaults
    ~HtmlTokenizer() override = default;
    HtmlTokenizer(const HtmlTokenizer &) = delete;
    HtmlTokenizer(HtmlTokenizer &&) = delete;
    auto operator=(const HtmlTokenizer &) -> HtmlTokenizer & = delete;
    auto operator=(HtmlTokenizer &&) -> HtmlTokenizer & = delete;

public:
    /// Tokenize the configured HTML input.
    /// Malformed markup is returned as plain text tokens instead of causing an error.
    [[nodiscard]] auto tokenize() -> TokenGenerator;

private:
    /// A lightweight token description referencing a range in the inherited buffer.
    struct HtmlTokenRef final {
        HtmlTokenType type; ///< The token type.
        IndexRange range;   ///< The referenced text range in `_buffer`.
    };

private:
    [[nodiscard]] auto tokenizeTag(std::vector<HtmlTokenRef> &tokens) -> bool;
    [[nodiscard]] auto tokenizeDeclaration(std::vector<HtmlTokenRef> &tokens) -> bool;
    [[nodiscard]] auto tokenizeClosingTag(std::vector<HtmlTokenRef> &tokens) -> bool;
    [[nodiscard]] auto tokenizeOpeningTag(std::vector<HtmlTokenRef> &tokens) -> bool;
    auto tokenizeText() -> HtmlToken;
    auto tokenizeBufferedLiteralTagText() -> HtmlToken;
    [[nodiscard]] auto tokenizeComment() -> bool;
    [[nodiscard]] auto tokenizeDocType(std::vector<HtmlTokenRef> &tokens) -> bool;
    [[nodiscard]] auto decodeEntity(char32_t &decodedCharacter) -> bool;
    [[nodiscard]] auto parseAttributeValue(IndexRange &value) -> bool;
    [[nodiscard]] auto parseName() -> std::optional<IndexRange>;
    void skipWhitespace();
    [[nodiscard]] auto materializeToken(const HtmlTokenRef &token) const -> HtmlToken;
    [[nodiscard]] auto bufferTextToken() const -> HtmlToken;

    [[nodiscard]] static auto isNameTerminator(char32_t character) noexcept -> bool;
    [[nodiscard]] static auto isAttributeValueTerminator(char32_t character) noexcept -> bool;
};

}
