// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/impl/U8Buffer.hpp>
#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(U8Buffer)
class U8BufferTest final : public el::UnitTest {
public:
    void testParseCodePointPrefixParsesAsciiAndUnicode() {
        const auto asciiText = std::string_view{"ab"};
        const auto ascii = term::impl::U8Buffer<const char>{asciiText}.parseCodePointPrefix();
        REQUIRE_EQUAL(ascii.status, term::impl::U8ParseStatus::Parsed);
        REQUIRE_EQUAL(ascii.codePoint, U'a');
        REQUIRE_EQUAL(ascii.consumedByteCount, std::size_t{1});

        const auto unicodeText = std::string_view{"\xC3\xA4+"};
        const auto unicode = term::impl::U8Buffer<const char>{unicodeText}.parseCodePointPrefix();
        REQUIRE_EQUAL(unicode.status, term::impl::U8ParseStatus::Parsed);
        REQUIRE_EQUAL(unicode.codePoint, U'\u00e4');
        REQUIRE_EQUAL(unicode.consumedByteCount, std::size_t{2});
    }

    void testParseCodePointPrefixReportsNeedMoreDataAndInvalidSequences() {
        const auto incompleteText = std::string_view{"\xE2\x82"};
        REQUIRE_EQUAL(
            term::impl::U8Buffer<const char>{incompleteText}.parseCodePointPrefix().status,
            term::impl::U8ParseStatus::NeedMoreData);

        const auto invalidText = std::string_view{"\xFF"};
        const auto invalid = term::impl::U8Buffer<const char>{invalidText}.parseCodePointPrefix();
        REQUIRE_EQUAL(invalid.status, term::impl::U8ParseStatus::Invalid);
        REQUIRE_EQUAL(invalid.consumedByteCount, std::size_t{1});
    }

    void testParseCodePointPrefixDiscardsOnlyTheBrokenLeadByte() {
        const auto brokenThenAsciiText = std::string_view{"\xC3q"};
        const auto brokenThenAscii = term::impl::U8Buffer<const char>{brokenThenAsciiText}.parseCodePointPrefix();
        REQUIRE_EQUAL(brokenThenAscii.status, term::impl::U8ParseStatus::Invalid);
        REQUIRE_EQUAL(brokenThenAscii.consumedByteCount, std::size_t{1});

        const auto brokenThenUtf8Text = std::string_view{"\xE2\x28\xA1"};
        const auto brokenThenUtf8 = term::impl::U8Buffer<const char>{brokenThenUtf8Text}.parseCodePointPrefix();
        REQUIRE_EQUAL(brokenThenUtf8.status, term::impl::U8ParseStatus::Invalid);
        REQUIRE_EQUAL(brokenThenUtf8.consumedByteCount, std::size_t{1});
    }

    void testEncodedByteCountMatchesUtf8EncodingWidth() {
        REQUIRE_EQUAL(term::impl::U8Buffer<const char>::encodedByteCount(U'A'), std::size_t{1});
        REQUIRE_EQUAL(term::impl::U8Buffer<const char>::encodedByteCount(U'\u00e4'), std::size_t{2});
        REQUIRE_EQUAL(term::impl::U8Buffer<const char>::encodedByteCount(U'\u20ac'), std::size_t{3});
        REQUIRE_EQUAL(term::impl::U8Buffer<const char>::encodedByteCount(U'\U0001f642'), std::size_t{4});
    }
};
