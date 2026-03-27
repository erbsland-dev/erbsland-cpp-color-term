// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/impl/U8Buffer.hpp>
#include <erbsland/cterm/text/impl/Tokenizer.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <array>
#include <string_view>

namespace textimpl = erbsland::cterm::text::impl;

class TokenizerProbe final : public textimpl::Tokenizer {
public:
    explicit TokenizerProbe(const std::string_view text) : Tokenizer{text} {}

    [[nodiscard]] auto currentCodePoint() const noexcept -> char32_t { return current(); }
    [[nodiscard]] auto peekCodePoint() const noexcept -> char32_t { return peek(); }
    [[nodiscard]] auto atEnd() const noexcept -> bool { return isAtEnd(); }

    auto advance() noexcept -> void { next(); }
    auto consume(const char32_t expected) noexcept -> bool { return consumeIf(expected); }

    [[nodiscard]] auto advanceTwiceAndRestore() noexcept -> std::array<char32_t, 4> {
        const auto state = save();
        next();
        next();
        const auto currentAfterAdvance = current();
        const auto peekAfterAdvance = peek();
        load(state);
        return {currentAfterAdvance, peekAfterAdvance, current(), peek()};
    }
};

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
        REQUIRE_EQUAL(parser.peekCodePoint(), term::impl::U8Buffer<const char>::cReplacementCharacter);

        parser.advance();
        REQUIRE_EQUAL(parser.currentCodePoint(), term::impl::U8Buffer<const char>::cReplacementCharacter);
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
