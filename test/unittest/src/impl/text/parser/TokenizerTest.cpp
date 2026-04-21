// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TokenizerTestProbe.hpp"

#include "support/TestHelper.hpp"

#include <erbsland/cterm/impl/U8Buffer.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <array>
#include <string_view>

TESTED_TARGETS(Tokenizer)
class TokenizerTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testConstructorInitializesCurrentAndPeekAndReachesEnd() {
        const auto text = bytes({0x41, 0xC3, 0xA4});
        auto parser = TokenizerProbe{text};

        REQUIRE_EQUAL(parser.currentCodePoint(), U'A');
        REQUIRE_EQUAL(parser.peekCodePoint(), U'\u00e4');
        REQUIRE_FALSE(parser.atEnd());

        parser.advance();
        REQUIRE_EQUAL(parser.currentCodePoint(), U'\u00e4');
        REQUIRE_EQUAL(parser.peekCodePoint(), char32_t{0});
        REQUIRE_FALSE(parser.atEnd());

        parser.advance();
        REQUIRE_EQUAL(parser.currentCodePoint(), char32_t{0});
        REQUIRE_EQUAL(parser.peekCodePoint(), char32_t{0});
        REQUIRE(parser.atEnd());
    }

    void testConsumeIfOnlyAdvancesOnMatch() {
        auto parser = TokenizerProbe{"ab"};

        REQUIRE_FALSE(parser.consume(U'x'));
        REQUIRE_EQUAL(parser.currentCodePoint(), U'a');
        REQUIRE_EQUAL(parser.peekCodePoint(), U'b');

        REQUIRE(parser.consume(U'a'));
        REQUIRE_EQUAL(parser.currentCodePoint(), U'b');
        REQUIRE_EQUAL(parser.peekCodePoint(), char32_t{0});
    }

    void testSaveAndLoadRestoreThePreviousParserState() {
        auto parser = TokenizerProbe{"abc"};

        const auto snapshots = parser.advanceTwiceAndRestore();

        REQUIRE_EQUAL(snapshots[0], U'c');
        REQUIRE_EQUAL(snapshots[1], char32_t{0});
        REQUIRE_EQUAL(snapshots[2], U'a');
        REQUIRE_EQUAL(snapshots[3], U'b');
    }

    void testParserReplacesMalformedUtf8Bytes() {
        const auto text = bytes({0x41, 0xFF, 0x42});
        auto parser = TokenizerProbe{text};

        REQUIRE_EQUAL(parser.currentCodePoint(), U'A');
        REQUIRE_EQUAL(parser.peekCodePoint(), impl::U8Buffer<const char>::cReplacementCharacter);

        parser.advance();
        REQUIRE_EQUAL(parser.currentCodePoint(), impl::U8Buffer<const char>::cReplacementCharacter);
        REQUIRE_EQUAL(parser.peekCodePoint(), U'B');

        parser.advance();
        REQUIRE_EQUAL(parser.currentCodePoint(), U'B');
        REQUIRE_EQUAL(parser.peekCodePoint(), char32_t{0});
    }

    void testEmptyInputStartsAtTheEnd() {
        const auto parser = TokenizerProbe{""};

        REQUIRE_EQUAL(parser.currentCodePoint(), char32_t{0});
        REQUIRE_EQUAL(parser.peekCodePoint(), char32_t{0});
        REQUIRE(parser.atEnd());
    }
};
