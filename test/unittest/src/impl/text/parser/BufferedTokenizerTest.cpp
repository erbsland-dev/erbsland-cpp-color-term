// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "BufferedTokenizerTestProbe.hpp"

#include "support/TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <string_view>

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
