// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>


namespace erbsland::cterm::impl {


/// Status for parsing one UTF-8 code point from the front of a byte buffer.
enum class U8ParseStatus : uint8_t {
    /// A full code point was parsed successfully.
    Parsed,
    /// More bytes are required to decide.
    NeedMoreData,
    /// The leading bytes are invalid or unsupported.
    Invalid,
};


/// Result of parsing one UTF-8 code point from a byte buffer.
struct U8ParseResult final {
    U8ParseStatus status{U8ParseStatus::Invalid}; ///< The parsing status.
    char32_t codePoint{0};                        ///< The parsed code point for `Parsed`.
    std::size_t consumedByteCount{0};             ///< Number of bytes consumed or rejected.
};


/// A safe and reliable UTF-8 decoder/encoder.
template <typename T>
    requires(std::is_integral_v<T> && sizeof(T) == 1)
class U8Buffer final {
    using Byte = uint8_t;

public:
    explicit U8Buffer(const std::span<T> buffer) : _buffer{buffer} {}
    template <typename Char>
    explicit U8Buffer(const std::basic_string<Char> &text) : _buffer{std::span(text.data(), text.size())} {}
    template <typename Char>
    explicit U8Buffer(const std::basic_string_view<Char> text) : _buffer{std::span(text.data(), text.size())} {}

    // prevent using rvalues
    template <typename Char>
    explicit U8Buffer(std::basic_string<Char> &&text) = delete;
    template <typename Char>
    explicit U8Buffer(std::basic_string_view<Char> &&text) = delete;

    // prevent assigning and copy
    U8Buffer(const U8Buffer &) = delete;
    U8Buffer(U8Buffer &&) = delete;
    auto operator=(const U8Buffer &) -> U8Buffer & = delete;
    auto operator=(U8Buffer &&) -> U8Buffer & = delete;

public: // Accessor
    /// Access the buffer.
    [[nodiscard]] auto buffer() const noexcept -> const std::span<T> & { return _buffer; }

public:
    /// Decode all characters using a custom function.
    /// @throws std::invalid_argument if there is an encoding error in the data.
    template <typename Function>
        requires std::invocable<Function, char32_t>
    void decodeAll(Function &&decodeFn) const {
        std::size_t position = 0;
        while (position < _buffer.size()) {
            decodeFn(decodeChar(_buffer, position));
        }
    }

    /// Decode one code point.
    /// Expects that the given view contains exactly one UTF-8 encoded code point.
    /// @return The decoded code point.
    /// @throws std::invalid_argument if the view does not contain exactly one code point.
    [[nodiscard]] auto decodeOneChar() const -> char32_t {
        auto position = std::size_t{0};
        const auto codePoint = decodeChar(_buffer, position);
        if (position != _buffer.size()) {
            throw std::invalid_argument{"decodeOneChar() expects a single code point."};
        }
        return codePoint;
    }

    /// Parse one UTF-8 code point from the buffer at the given byte offset.
    /// @param offset The byte offset into the buffer.
    /// @return The parsing status, code point, and consumed byte count.
    [[nodiscard]] auto parseCodePointPrefix(const std::size_t offset = 0) const noexcept -> U8ParseResult {
        if (offset >= _buffer.size()) {
            return {.status = U8ParseStatus::Invalid, .consumedByteCount = offset};
        }
        const auto firstByte = static_cast<Byte>(_buffer[offset]);
        const auto byteCount = expectedByteCount(firstByte);
        if (byteCount == 0) {
            return {.status = U8ParseStatus::Invalid, .consumedByteCount = offset + 1};
        }
        if ((offset + byteCount) > _buffer.size()) {
            return {.status = U8ParseStatus::NeedMoreData, .consumedByteCount = offset};
        }
        try {
            auto position = offset;
            const auto codePoint = decodeChar(_buffer, position);
            return {.status = U8ParseStatus::Parsed, .codePoint = codePoint, .consumedByteCount = position};
        } catch (...) {
            // Resynchronize one byte at a time so a broken multi-byte prefix does not swallow following valid input.
            return {.status = U8ParseStatus::Invalid, .consumedByteCount = offset + 1};
        }
    }

    /// Throw an encoding error.
    /// @param message The diagnostic text.
    /// @throws std::invalid_argument Always thrown.
    static void throwEncodingError(const std::string &message) { throw std::invalid_argument(message); }

    /// Test if the given Unicode code point is valid.
    [[nodiscard]] constexpr static auto isValidUnicode(const char32_t unicode) noexcept -> bool {
        return unicode <= 0x10FFFFU && (unicode < 0xD800U || unicode > 0xDFFFU);
    }

    /// Get the number of UTF-8 bytes required to encode a Unicode code point.
    /// @param unicode The Unicode code point.
    /// @return The encoded byte count.
    [[nodiscard]] constexpr static auto encodedByteCount(const char32_t unicode) noexcept -> std::size_t {
        const auto value = static_cast<uint32_t>(unicode);
        if (value <= 0x7FU) {
            return 1;
        }
        if (value <= 0x7FFU) {
            return 2;
        }
        if (value <= 0xFFFFU) {
            return 3;
        }
        return 4;
    }

    /// Get the number of bytes expected for a UTF-8 sequence from its first byte.
    /// @param firstByte The leading byte of the sequence.
    /// @return The expected byte count, or `0` for an invalid leading byte.
    [[nodiscard]] constexpr static auto expectedByteCount(const Byte firstByte) noexcept -> std::size_t {
        if (firstByte < 0x80U) {
            return 1;
        }
        if ((firstByte & 0b11100000U) == 0b11000000U && firstByte >= 0b11000010U) {
            return 2;
        }
        if ((firstByte & 0b11110000U) == 0b11100000U) {
            return 3;
        }
        if ((firstByte & 0b11111000U) == 0b11110000U && firstByte < 0b11110101U) {
            return 4;
        }
        return 0;
    }

    /// Decode a single UTF-8 character in the buffer and advance the position.
    /// @param buffer The byte buffer to decode.
    /// @param position The read position that is advanced after a successful read.
    /// @throws std::invalid_argument if there is an encoding error in the data.
    [[nodiscard]] static auto decodeChar(std::span<T> buffer, std::size_t &position) -> char32_t {
        if (position >= buffer.size()) {
            return static_cast<char32_t>(0);
        }
        auto index = position;
        auto c = static_cast<Byte>(buffer[index]);
        index += 1;
        if (c < Byte{0x80U}) { // 7-bit ASCII?
            const auto result = static_cast<char32_t>(c);
            position = index;
            return result;
        }
        const auto cSize = expectedByteCount(c);
        if (cSize == 0) {
            throwEncodingError("Invalid or out-of-range start byte sequence.");
        }
        char32_t unicodeValue{};
        if (cSize == 2) {
            unicodeValue = static_cast<char32_t>(c & Byte{0b00011111U});
        } else if (cSize == 3) {
            unicodeValue = static_cast<char32_t>(c & Byte{0b00001111U});
        } else if (cSize == 4) {
            unicodeValue = static_cast<char32_t>(c & Byte{0b00000111U});
        } else {
            const auto result = static_cast<char32_t>(c);
            position = index;
            return result;
        }
        for (std::size_t i = 1; i < cSize; ++i) {
            if (index >= buffer.size()) {
                throwEncodingError("Unexpected end of the data.");
            }
            c = static_cast<Byte>(buffer[index]);
            index += 1;
            if ((c & Byte{0b11000000U}) != Byte{0b10000000U}) {
                throwEncodingError("Unexpected continuation byte.");
            }
            unicodeValue <<= 6;
            unicodeValue |= static_cast<char32_t>(c & Byte{0b00111111U});
        }
        if ((cSize == 3 && unicodeValue < 0x800) || (cSize == 4 && unicodeValue < 0x10000)) {
            throwEncodingError("Overlong encoding.");
        }
        const auto result = char32_t{unicodeValue};
        if (!isValidUnicode(result)) {
            throwEncodingError("Invalid Unicode character.");
        }
        position = index;
        return result;
    }

    /// Encode a Unicode character into UTF-8 format and append it to the string.
    template <typename tStr>
        requires std::is_convertible_v<T, typename tStr::value_type>
    static void encodeChar(tStr &str, const char32_t unicode) noexcept {
        const auto u = static_cast<uint32_t>(unicode);
        if (u <= 0x0000007FU) {
            str.push_back(static_cast<T>(u));
            return;
        }
        if (u <= 0x000007FFU) {
            str.push_back(static_cast<T>(((u >> 6) & 0b00011111U) | 0b11000000U));
            str.push_back(static_cast<T>((u & 0b00111111U) | 0b10000000U));
            return;
        }
        if (u <= 0x0000FFFFU) {
            str.push_back(static_cast<T>(((u >> 12) & 0b00001111U) | 0b11100000U));
            str.push_back(static_cast<T>(((u >> 6) & 0b00111111U) | 0b10000000U));
            str.push_back(static_cast<T>((u & 0b00111111U) | 0b10000000U));
            return;
        }
        if (u <= 0x0010FFFFU) {
            str.push_back(static_cast<T>(((u >> 18) & 0b00000111U) | 0b11110000U));
            str.push_back(static_cast<T>(((u >> 12) & 0b00111111U) | 0b10000000U));
            str.push_back(static_cast<T>(((u >> 6) & 0b00111111U) | 0b10000000U));
            str.push_back(static_cast<T>((u & 0b00111111U) | 0b10000000U));
            return;
        }
        // Placeholder for Unicode characters that are out of the valid range.
        str.push_back(static_cast<T>(0xefU));
        str.push_back(static_cast<T>(0xbfU));
        str.push_back(static_cast<T>(0xbdU));
    }

private:
    std::span<T> _buffer;
};


U8Buffer(const std::u8string &) -> U8Buffer<const char8_t>;
U8Buffer(std::u8string_view) -> U8Buffer<const char8_t>;
U8Buffer(const std::string &) -> U8Buffer<const char>;
U8Buffer(std::string_view) -> U8Buffer<const char>;


}
