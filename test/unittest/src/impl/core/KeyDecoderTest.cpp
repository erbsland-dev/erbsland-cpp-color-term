// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/cterm/impl/KeyDecoder.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <array>
#include <format>
#include <string_view>

TESTED_TARGETS(KeyDecoder)
class KeyDecoderTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testParseConsoleInputPrefixConsumesOnlyTheFirstBufferedKey() {
        const auto ascii = impl::KeyDecoder{"ab"}.parseConsoleInputPrefix();
        REQUIRE_EQUAL(ascii.status(), impl::U8ParseStatus::Parsed);
        REQUIRE_EQUAL(ascii.key(), (Key{Key::Character, U'a'}));
        REQUIRE_EQUAL(ascii.consumedByteCount(), std::size_t{1});

        const auto escapeSequence = impl::KeyDecoder{"\x1b[Aq"}.parseConsoleInputPrefix();
        REQUIRE_EQUAL(escapeSequence.status(), impl::U8ParseStatus::Parsed);
        REQUIRE_EQUAL(escapeSequence.key(), Key{Key::Up});
        REQUIRE_EQUAL(escapeSequence.consumedByteCount(), std::size_t{3});
    }

    void testParseConsoleInputPrefixSupportsUnicodeAndCombinedCharacters() {
        const auto unicode = impl::KeyDecoder{bytes({0xC3, 0xA4, 0x2B})}.parseConsoleInputPrefix();
        REQUIRE_EQUAL(unicode.status(), impl::U8ParseStatus::Parsed);
        REQUIRE_EQUAL(unicode.key(), (Key{Key::Character, U'\u00e4'}));
        REQUIRE_EQUAL(unicode.consumedByteCount(), std::size_t{2});

        const auto combined = impl::KeyDecoder{bytes({0x65, 0xCC, 0x81, 0x78})}.parseConsoleInputPrefix();
        REQUIRE_EQUAL(combined.status(), impl::U8ParseStatus::Parsed);
        REQUIRE_EQUAL(combined.key(), (Key{Key::Combined, U"e\u0301"}));
        REQUIRE_EQUAL(combined.consumedByteCount(), std::size_t{3});
    }

    void testDecodeConsoleInputRejectsIncompleteOrMultiKeyInput() {
        REQUIRE_EQUAL(impl::KeyDecoder{bytes({0x65, 0xCC})}.decodeConsoleInput(), Key{Key::None});
        REQUIRE_EQUAL(impl::KeyDecoder{"ab"}.decodeConsoleInput(), Key{Key::None});
        REQUIRE_EQUAL(impl::KeyDecoder{"\x1b[A"}.decodeConsoleInput(), Key{Key::Up});
        REQUIRE_EQUAL(impl::KeyDecoder{"\x1b"}.decodeConsoleInput(), Key{Key::Escape});
    }

    void testParseConsoleInputPrefixResynchronizesAfterBrokenUtf8LeadByte() {
        const auto brokenThenAscii = impl::KeyDecoder{bytes({0xC3, 0x71})}.parseConsoleInputPrefix();
        REQUIRE_EQUAL(brokenThenAscii.status(), impl::U8ParseStatus::Invalid);
        REQUIRE_EQUAL(brokenThenAscii.consumedByteCount(), std::size_t{1});
    }

    void testParseConsoleInputPrefixReportsNeedMoreDataForBareEscape() {
        const auto result = impl::KeyDecoder{"\x1b"}.parseConsoleInputPrefix();

        REQUIRE_EQUAL(result.status(), impl::U8ParseStatus::NeedMoreData);
        REQUIRE_EQUAL(result.consumedByteCount(), std::size_t{0});
    }

    void testParseConsoleInputPrefixSupportsModifiedSpecialKeys() {
        struct Entry {
            const char *input;
            Key expected;
        };
        const std::array<Entry, 13> entries{{
            {"\x1b[1;2A", Key{Key::Up, KeyModifier::Shift}},
            {"\x1b[1;3B", Key{Key::Down, KeyModifier::Alt}},
            {"\x1b[1;5D", Key{Key::Left, KeyModifier::Control}},
            {"\x1b[1;8C", Key{Key::Right, KeyModifier::Shift | KeyModifier::Alt | KeyModifier::Control}},
            {"\x1b[1;2H", Key{Key::Home, KeyModifier::Shift}},
            {"\x1b[1;5F", Key{Key::End, KeyModifier::Control}},
            {"\x1b[2;2~", Key{Key::Insert, KeyModifier::Shift}},
            {"\x1b[3;3~", Key{Key::Delete, KeyModifier::Alt}},
            {"\x1b[5;5~", Key{Key::PageUp, KeyModifier::Control}},
            {"\x1b[6;6~", Key{Key::PageDown, KeyModifier::Shift | KeyModifier::Control}},
            {"\x1b[1;2P", Key{Key::F1, KeyModifier::Shift}},
            {"\x1b[1;3S", Key{Key::F4, KeyModifier::Alt}},
            {"\x1b[15;5~", Key{Key::F5, KeyModifier::Control}},
        }};
        for (std::size_t index = 0; index < entries.size(); ++index) {
            const auto &entry = entries[index];
            runWithContext(
                SOURCE_LOCATION(),
                [&]() {
                    const auto result = impl::KeyDecoder{entry.input}.parseConsoleInputPrefix();
                    REQUIRE_EQUAL(result.status(), impl::U8ParseStatus::Parsed);
                    REQUIRE_EQUAL(result.key(), entry.expected);
                    REQUIRE_EQUAL(result.consumedByteCount(), std::string_view{entry.input}.size());
                },
                [&]() -> std::string {
                    return std::format("index = {} / expectedKey = {}", index, entry.expected.toString());
                });
        }
    }

    void testParseConsoleInputPrefixConsumesUnsupportedEscapeSequences() {
        auto result = impl::KeyDecoder{"\x1b[999~q"}.parseConsoleInputPrefix();
        REQUIRE_EQUAL(result.status(), impl::U8ParseStatus::Invalid);
        REQUIRE_EQUAL(result.consumedByteCount(), std::size_t{6});

        result = impl::KeyDecoder{"\x1bxq"}.parseConsoleInputPrefix();
        REQUIRE_EQUAL(result.status(), impl::U8ParseStatus::Invalid);
        REQUIRE_EQUAL(result.consumedByteCount(), std::size_t{2});

        result = impl::KeyDecoder{"\x1b[1;2"}.parseConsoleInputPrefix();
        REQUIRE_EQUAL(result.status(), impl::U8ParseStatus::NeedMoreData);
        REQUIRE_EQUAL(result.consumedByteCount(), std::size_t{0});
    }
};
