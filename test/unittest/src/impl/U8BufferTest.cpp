// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/impl/U8Buffer.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <vector>

TESTED_TARGETS(U8Buffer)
class U8BufferTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testParseCodePointPrefixParsesAsciiAndUnicode() {
        const auto asciiText = std::string_view{"ab"};
        const auto ascii = term::impl::U8Buffer<const char>{asciiText}.parseCodePointPrefix();
        REQUIRE_EQUAL(ascii.status(), term::impl::U8ParseStatus::Parsed);
        REQUIRE_EQUAL(ascii.codePoint(), U'a');
        REQUIRE_EQUAL(ascii.consumedByteCount(), std::size_t{1});

        const auto unicodeText = bytes({0xC3, 0xA4, 0x2B});
        const auto unicode = term::impl::U8Buffer<const char>{unicodeText}.parseCodePointPrefix();
        REQUIRE_EQUAL(unicode.status(), term::impl::U8ParseStatus::Parsed);
        REQUIRE_EQUAL(unicode.codePoint(), U'\u00e4');
        REQUIRE_EQUAL(unicode.consumedByteCount(), std::size_t{2});
    }

    void testParseCodePointPrefixReportsNeedMoreDataAndInvalidSequences() {
        const auto incompleteText = bytes({0xE2, 0x82});
        REQUIRE_EQUAL(
            term::impl::U8Buffer<const char>{incompleteText}.parseCodePointPrefix().status(),
            term::impl::U8ParseStatus::NeedMoreData);

        const auto invalidText = bytes({0xFF});
        const auto invalid = term::impl::U8Buffer<const char>{invalidText}.parseCodePointPrefix();
        REQUIRE_EQUAL(invalid.status(), term::impl::U8ParseStatus::Invalid);
        REQUIRE_EQUAL(invalid.consumedByteCount(), std::size_t{1});
    }

    void testParseCodePointPrefixDiscardsOnlyTheBrokenLeadByte() {
        const auto brokenThenAsciiText = bytes({0xC3, 0x71});
        const auto brokenThenAscii = term::impl::U8Buffer<const char>{brokenThenAsciiText}.parseCodePointPrefix();
        REQUIRE_EQUAL(brokenThenAscii.status(), term::impl::U8ParseStatus::Invalid);
        REQUIRE_EQUAL(brokenThenAscii.consumedByteCount(), std::size_t{1});

        const auto brokenThenUtf8Text = bytes({0xE2, 0x28, 0xA1});
        const auto brokenThenUtf8 = term::impl::U8Buffer<const char>{brokenThenUtf8Text}.parseCodePointPrefix();
        REQUIRE_EQUAL(brokenThenUtf8.status(), term::impl::U8ParseStatus::Invalid);
        REQUIRE_EQUAL(brokenThenUtf8.consumedByteCount(), std::size_t{1});
    }

    void testEncodedByteCountMatchesUtf8EncodingWidth() {
        REQUIRE_EQUAL(term::impl::U8Buffer<const char>::encodedByteCount(U'A'), std::size_t{1});
        REQUIRE_EQUAL(term::impl::U8Buffer<const char>::encodedByteCount(U'\u00e4'), std::size_t{2});
        REQUIRE_EQUAL(term::impl::U8Buffer<const char>::encodedByteCount(U'\u20ac'), std::size_t{3});
        REQUIRE_EQUAL(term::impl::U8Buffer<const char>::encodedByteCount(U'\U0001f642'), std::size_t{4});
    }

    void testDecodeAllReplaceReplacesEachErroneousByte() {
        const auto input = bytes({0x41, 0xE2, 0x28, 0xA1, 0x42, 0xC3});

        const auto decoded = decodeAll(std::string_view{input}, term::EncodingErrors::Replace);

        REQUIRE_EQUAL(decoded, (std::vector<char32_t>{U'A', U'\uFFFD', U'(', U'\uFFFD', U'B', U'\uFFFD'}));
    }

    void testDecodeAllIgnoreSkipsAllErroneousBytes() {
        const auto input = bytes({0x41, 0xE2, 0x28, 0xA1, 0x42, 0xC3});

        const auto decoded = decodeAll(std::string_view{input}, term::EncodingErrors::Ignore);

        REQUIRE_EQUAL(decoded, (std::vector<char32_t>{U'A', U'(', U'B'}));
    }

    void testDecodeAllThrowStillFailsOnInvalidUtf8() {
        const auto input = bytes({0x41, 0xC3});
        REQUIRE_THROWS_AS(std::invalid_argument, decodeAll(std::string_view{input}, term::EncodingErrors::Throw));
    }

    void testDecodeAllReplaceTreatsIncompleteTrailingBytesIndividually() {
        const auto input = bytes({0xE2, 0x82});
        const auto decoded = decodeAll(std::string_view{input}, term::EncodingErrors::Replace);

        REQUIRE_EQUAL(decoded, (std::vector<char32_t>{U'\uFFFD', U'\uFFFD'}));
    }

    void testDecodeAllIgnoreDropsIncompleteTrailingBytes() {
        const auto input = bytes({0x41, 0xE2, 0x82});
        const auto decoded = decodeAll(std::string_view{input}, term::EncodingErrors::Ignore);

        REQUIRE_EQUAL(decoded, (std::vector<char32_t>{U'A'}));
    }

    void testDecodeCharReplacingErrorsConsumesOneErroneousByte() {
        const auto input = bytes({0xE2, 0x28, 0xA1});
        auto position = std::size_t{0};

        const auto codePoint = term::impl::U8Buffer<const char>::decodeCharReplacingErrors(
            std::span(input.data(), input.size()), position);

        REQUIRE_EQUAL(codePoint, U'\uFFFD');
        REQUIRE_EQUAL(position, std::size_t{1});
    }

    void testDecodeCharIgnoringErrorsStopsWithoutProducingTrailingReplacement() {
        const auto input = bytes({0xE2, 0x82});
        auto position = std::size_t{0};

        const auto codePoint =
            term::impl::U8Buffer<const char>::decodeCharIgnoringErrors(std::span(input.data(), input.size()), position);

        REQUIRE_EQUAL(codePoint, char32_t{0});
        REQUIRE_EQUAL(position, std::size_t{0});
    }

    void testEncodeStringEncodesUtf32TextAsUtf8() {
        const auto encoded = term::impl::U8Buffer<const char>::encodeString(U"A\u00e4\u20ac\U0001f642");

        REQUIRE_EQUAL(encoded, std::string{"Aä€🙂"});
    }

private:
    [[nodiscard]] static auto decodeAll(const std::string_view text, const term::EncodingErrors encodingErrors)
        -> std::vector<char32_t> {
        auto result = std::vector<char32_t>{};
        term::impl::U8Buffer<const char>{text}.decodeAll(
            [&](const char32_t codePoint) -> void { result.push_back(codePoint); }, encodingErrors);
        return result;
    }
};
