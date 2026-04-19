// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/text/impl/BufferedTokenizer.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <string_view>

namespace textimpl = erbsland::cterm::text::impl;

class BufferedTokenizerProbe final : public textimpl::BufferedTokenizer {
public:
    explicit BufferedTokenizerProbe(const std::string_view text, const std::size_t initialBufferCapacity = 0) :
        BufferedTokenizer{text, initialBufferCapacity} {}

    [[nodiscard]] auto bufferText() const -> const std::u32string & { return _buffer; }
    [[nodiscard]] auto bufferCapacity() const -> std::size_t { return _buffer.capacity(); }
    [[nodiscard]] auto currentCodePoint() const noexcept -> char32_t { return current(); }

    void appendCharacter(const char32_t character) { append(character); }
    void clearBuffer() { resetBuffer(); }
    void appendCurrentCharacterAndAdvance() { appendCurrentAndAdvance(); }
    [[nodiscard]] auto consumeBufferedCharacter(const char32_t expected) noexcept -> bool {
        return consumeBufferedIf(expected);
    }
    [[nodiscard]] auto rangeFrom(const std::size_t start) const noexcept -> IndexRange { return makeRange(start); }
    [[nodiscard]] auto trim(IndexRange range) const noexcept -> IndexRange { return trimRange(range); }
};

TESTED_TARGETS(BufferedTokenizer)
class BufferedTokenizerTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testConstructorStartsWithAnEmptyBuffer() {
        const auto tokenizer = BufferedTokenizerProbe{"abc"};

        REQUIRE(tokenizer.bufferText().empty());
    }

    void testConstructorCanReserveTheRequestedCapacity() {
        const auto tokenizer = BufferedTokenizerProbe{"abc", 64};

        REQUIRE(tokenizer.bufferCapacity() >= 64);
    }

    void testAppendAndResetBufferManageTheReusableBuffer() {
        auto tokenizer = BufferedTokenizerProbe{"abc"};

        tokenizer.appendCharacter(U'A');
        tokenizer.appendCharacter(U'B');
        REQUIRE_EQUAL(tokenizer.bufferText(), U"AB");

        tokenizer.clearBuffer();
        REQUIRE(tokenizer.bufferText().empty());

        tokenizer.appendCharacter(U'C');
        REQUIRE_EQUAL(tokenizer.bufferText(), U"C");
    }

    void testAppendCurrentAndAdvanceAppendsTheCurrentCharacter() {
        auto tokenizer = BufferedTokenizerProbe{"ab"};

        tokenizer.appendCurrentCharacterAndAdvance();

        REQUIRE_EQUAL(tokenizer.bufferText(), U"a");
        REQUIRE_EQUAL(tokenizer.currentCodePoint(), U'b');
    }

    void testConsumeBufferedIfOnlyConsumesOnMatch() {
        auto tokenizer = BufferedTokenizerProbe{"ab"};

        REQUIRE_FALSE(tokenizer.consumeBufferedCharacter(U'x'));
        REQUIRE(tokenizer.bufferText().empty());
        REQUIRE_EQUAL(tokenizer.currentCodePoint(), U'a');

        REQUIRE(tokenizer.consumeBufferedCharacter(U'a'));
        REQUIRE_EQUAL(tokenizer.bufferText(), U"a");
        REQUIRE_EQUAL(tokenizer.currentCodePoint(), U'b');
    }

    void testMakeRangeReturnsTheSpanToTheCurrentBufferEnd() {
        auto tokenizer = BufferedTokenizerProbe{""};

        tokenizer.appendCharacter(U'A');
        tokenizer.appendCharacter(U'B');
        tokenizer.appendCharacter(U'C');

        const auto range = tokenizer.rangeFrom(1);

        REQUIRE_EQUAL(range.startIndex(), 1U);
        REQUIRE_EQUAL(range.length(), 2U);
    }

    void testTrimRangeRemovesOnlyLeadingAndTrailingAsciiWhitespace() {
        auto tokenizer = BufferedTokenizerProbe{""};

        tokenizer.appendCharacter(U' ');
        tokenizer.appendCharacter(U'\t');
        tokenizer.appendCharacter(U'A');
        tokenizer.appendCharacter(U' ');
        tokenizer.appendCharacter(U'B');
        tokenizer.appendCharacter(U'\n');

        const auto range = tokenizer.trim(tokenizer.rangeFrom(0));

        REQUIRE_EQUAL(range.startIndex(), 2U);
        REQUIRE_EQUAL(range.length(), 3U);
        REQUIRE_EQUAL(tokenizer.bufferText().substr(range.startIndex(), range.length()), U"A B");
    }
};
