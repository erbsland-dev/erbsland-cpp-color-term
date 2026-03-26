// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/impl/KeyDecoder.hpp>
#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(KeyDecoder)
class KeyDecoderTest final : public el::UnitTest {
public:
    void testParseConsoleInputPrefixConsumesOnlyTheFirstBufferedKey() {
        const auto ascii = term::impl::KeyDecoder{"ab"}.parseConsoleInputPrefix();
        REQUIRE_EQUAL(ascii.status, term::impl::U8ParseStatus::Parsed);
        REQUIRE_EQUAL(ascii.key, (Key{Key::Character, U'a'}));
        REQUIRE_EQUAL(ascii.consumedByteCount, std::size_t{1});

        const auto escapeSequence = term::impl::KeyDecoder{"\x1b[Aq"}.parseConsoleInputPrefix();
        REQUIRE_EQUAL(escapeSequence.status, term::impl::U8ParseStatus::Parsed);
        REQUIRE_EQUAL(escapeSequence.key, Key{Key::Up});
        REQUIRE_EQUAL(escapeSequence.consumedByteCount, std::size_t{3});
    }

    void testParseConsoleInputPrefixSupportsUnicodeAndCombinedCharacters() {
        const auto unicode = term::impl::KeyDecoder{"\xC3\xA4+"}.parseConsoleInputPrefix();
        REQUIRE_EQUAL(unicode.status, term::impl::U8ParseStatus::Parsed);
        REQUIRE_EQUAL(unicode.key, (Key{Key::Character, U'\u00e4'}));
        REQUIRE_EQUAL(unicode.consumedByteCount, std::size_t{2});

        const auto combined = term::impl::KeyDecoder{"e\xCC\x81x"}.parseConsoleInputPrefix();
        REQUIRE_EQUAL(combined.status, term::impl::U8ParseStatus::Parsed);
        REQUIRE_EQUAL(combined.key, (Key{Key::Combined, U"e\u0301"}));
        REQUIRE_EQUAL(combined.consumedByteCount, std::size_t{3});
    }

    void testDecodeConsoleInputRejectsIncompleteOrMultiKeyInput() {
        REQUIRE_EQUAL(term::impl::KeyDecoder{"e\xCC"}.decodeConsoleInput(), Key{Key::None});
        REQUIRE_EQUAL(term::impl::KeyDecoder{"ab"}.decodeConsoleInput(), Key{Key::None});
        REQUIRE_EQUAL(term::impl::KeyDecoder{"\x1b[A"}.decodeConsoleInput(), Key{Key::Up});
    }

    void testParseConsoleInputPrefixResynchronizesAfterBrokenUtf8LeadByte() {
        const auto brokenThenAscii = term::impl::KeyDecoder{"\xC3q"}.parseConsoleInputPrefix();
        REQUIRE_EQUAL(brokenThenAscii.status, term::impl::U8ParseStatus::Invalid);
        REQUIRE_EQUAL(brokenThenAscii.consumedByteCount, std::size_t{1});
    }
};
