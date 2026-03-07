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
    auto decodeOneChar() -> char32_t {
        auto position = std::size_t{0};
        const auto codePoint = decodeChar(_buffer, position);
        if (position != _buffer.size()) {
            throw std::invalid_argument{"decodeOneChar() expects a single code point."};
        }
        return codePoint;
    }

    /// Throw an encoding error.
    /// @param message The diagnostic text.
    /// @throws std::invalid_argument Always thrown.
    static void throwEncodingError(const std::string &message) { throw std::invalid_argument(message); }

    /// Test if the given Unicode code point is valid.
    [[nodiscard]] constexpr static auto isValidUnicode(const char32_t unicode) noexcept -> bool {
        return unicode <= 0x10FFFFU && (unicode < 0xD800U || unicode > 0xDFFFU);
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
        std::size_t cSize = 0;
        char32_t unicodeValue{};
        if ((c & Byte{0b11100000U}) == Byte{0b11000000U} && c >= Byte{0b11000010U}) {
            cSize = 2; // 2-byte sequence
            unicodeValue = static_cast<char32_t>(c & Byte{0b00011111U});
        } else if ((c & Byte{0b11110000U}) == Byte{0b11100000U}) {
            cSize = 3; // 3-byte sequence
            unicodeValue = static_cast<char32_t>(c & Byte{0b00001111U});
        } else if ((c & Byte{0b11111000U}) == Byte{0b11110000U} && c < Byte{0b11110101U}) {
            cSize = 4; // 4-byte sequence
            unicodeValue = static_cast<char32_t>(c & Byte{0b00000111U});
        } else {
            throwEncodingError("Invalid or out-of-range start byte sequence.");
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
