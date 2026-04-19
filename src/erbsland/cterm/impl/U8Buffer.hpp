// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "TextUtil.hpp"

#include "../EncodingErrors.hpp"

#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

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
class U8ParseResult final {
public:
    /// Create one parse result without a decoded code point.
    /// @param status The parsing status.
    /// @param consumedByteCount The number of bytes consumed or rejected.
    U8ParseResult(const U8ParseStatus status = U8ParseStatus::Invalid, const std::size_t consumedByteCount = 0) noexcept
        :
        _status{status}, _consumedByteCount{consumedByteCount} {}

    /// Create one parse result with a decoded code point.
    /// @param status The parsing status.
    /// @param codePoint The decoded code point.
    /// @param consumedByteCount The number of bytes consumed or rejected.
    U8ParseResult(const U8ParseStatus status, const char32_t codePoint, const std::size_t consumedByteCount) noexcept :
        _status{status}, _codePoint{codePoint}, _consumedByteCount{consumedByteCount} {}

public:
    /// Access the parsing status.
    [[nodiscard]] auto status() const noexcept -> U8ParseStatus { return _status; }
    /// Access the parsed code point for `Parsed`.
    [[nodiscard]] auto codePoint() const noexcept -> char32_t { return _codePoint; }
    /// Access the number of bytes consumed or rejected.
    [[nodiscard]] auto consumedByteCount() const noexcept -> std::size_t { return _consumedByteCount; }

private:
    U8ParseStatus _status{U8ParseStatus::Invalid}; ///< The parsing status.
    char32_t _codePoint{0};                        ///< The parsed code point for `Parsed`.
    std::size_t _consumedByteCount{0};             ///< Number of bytes consumed or rejected.
};

/// A safe and reliable UTF-8 decoder/encoder.
template <typename T>
    requires(std::is_integral_v<T> && sizeof(T) == 1)
class U8Buffer final {
    using Byte = uint8_t;

public:
    constexpr static auto cReplacementCharacter = char32_t{0xFFFDU};

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
    /// @param decodeFn The function that receives each decoded code point.
    /// @param encodingErrors How UTF-8 decoding errors are handled.
    /// @throws std::invalid_argument If `encodingErrors` is `EncodingErrors::Throw` and the data is invalid UTF-8.
    template <typename Function>
        requires std::invocable<Function, char32_t>
    void decodeAll(Function &&decodeFn, const EncodingErrors encodingErrors = EncodingErrors::Replace) const {
        switch (encodingErrors) {
        case EncodingErrors::Throw:
            decodeAllOrThrow(std::forward<Function>(decodeFn));
            return;
        case EncodingErrors::Ignore:
            decodeAllIgnoringErrors(std::forward<Function>(decodeFn));
            return;
        case EncodingErrors::Replace:
            decodeAllReplacingErrors(std::forward<Function>(decodeFn));
            return;
        }
        throw std::invalid_argument{"Invalid encoding error handling mode."};
    }

    /// Decode all characters and throw on malformed UTF-8.
    /// @param decodeFn The function that receives each decoded code point.
    template <typename Function>
        requires std::invocable<Function, char32_t>
    void decodeAllOrThrow(Function &&decodeFn) const {
        decodeAllImpl<EncodingErrors::Throw>(std::forward<Function>(decodeFn));
    }

    /// Decode all characters and skip malformed UTF-8 bytes.
    /// @param decodeFn The function that receives each decoded code point.
    template <typename Function>
        requires std::invocable<Function, char32_t>
    void decodeAllIgnoringErrors(Function &&decodeFn) const noexcept(noexcept(std::declval<Function &>()(char32_t{}))) {
        decodeAllImpl<EncodingErrors::Ignore>(std::forward<Function>(decodeFn));
    }

    /// Decode all characters and replace each malformed UTF-8 byte with `U+FFFD`.
    /// @param decodeFn The function that receives each decoded code point.
    template <typename Function>
        requires std::invocable<Function, char32_t>
    void decodeAllReplacingErrors(Function &&decodeFn) const
        noexcept(noexcept(std::declval<Function &>()(char32_t{}))) {
        decodeAllImpl<EncodingErrors::Replace>(std::forward<Function>(decodeFn));
    }

    /// Decode one code point.
    /// Expects that the given view contains exactly one UTF-8 encoded code point.
    /// @param encodingErrors How UTF-8 decoding errors are handled.
    /// @return The decoded code point.
    /// @throws std::invalid_argument If the view does not contain exactly one decoded code point.
    [[nodiscard]] auto decodeOneChar(const EncodingErrors encodingErrors = EncodingErrors::Replace) const -> char32_t {
        switch (encodingErrors) {
        case EncodingErrors::Throw:
            return decodeOneCharOrThrow();
        case EncodingErrors::Ignore:
            return decodeOneCharIgnoringErrors();
        case EncodingErrors::Replace:
            return decodeOneCharReplacingErrors();
        }
        throw std::invalid_argument{"Invalid encoding error handling mode."};
    }

    /// Decode one code point and throw on malformed UTF-8.
    /// @return The decoded code point.
    /// @throws std::invalid_argument If the view does not contain exactly one decoded code point.
    [[nodiscard]] auto decodeOneCharOrThrow() const -> char32_t { return decodeOneCharImpl<EncodingErrors::Throw>(); }

    /// Decode one code point and skip malformed UTF-8 bytes.
    /// @return The decoded code point.
    /// @throws std::invalid_argument If the view does not contain exactly one decoded code point.
    [[nodiscard]] auto decodeOneCharIgnoringErrors() const -> char32_t {
        return decodeOneCharImpl<EncodingErrors::Ignore>();
    }

    /// Decode one code point and replace malformed UTF-8 bytes with `U+FFFD`.
    /// @return The decoded code point.
    /// @throws std::invalid_argument If the view does not contain exactly one decoded code point.
    [[nodiscard]] auto decodeOneCharReplacingErrors() const -> char32_t {
        return decodeOneCharImpl<EncodingErrors::Replace>();
    }

    /// Parse one UTF-8 code point from the buffer at the given byte offset.
    /// @param offset The byte offset into the buffer.
    /// @return The parsing status, code point, and consumed byte count.
    [[nodiscard]] auto parseCodePointPrefix(const std::size_t offset = 0) const noexcept -> U8ParseResult {
        if (offset >= _buffer.size()) {
            return U8ParseResult{U8ParseStatus::Invalid, offset};
        }
        const auto firstByte = static_cast<Byte>(_buffer[offset]);
        const auto byteCount = expectedByteCount(firstByte);
        if (byteCount == 0) {
            return U8ParseResult{U8ParseStatus::Invalid, offset + 1};
        }
        if ((offset + byteCount) > _buffer.size()) {
            return U8ParseResult{U8ParseStatus::NeedMoreData, offset};
        }
        try {
            auto position = offset;
            const auto codePoint = decodeCharOrThrow(_buffer, position);
            return U8ParseResult{U8ParseStatus::Parsed, codePoint, position};
        } catch (...) {
            // Resynchronize one byte at a time so a broken multi-byte prefix does not swallow following valid input.
            return U8ParseResult{U8ParseStatus::Invalid, offset + 1};
        }
    }

    /// Throw an encoding error.
    /// @param message The diagnostic text.
    /// @throws std::invalid_argument Always thrown.
    static void throwEncodingError(const char *message) { throw std::invalid_argument(message); }

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
    /// @param encodingErrors How malformed UTF-8 is handled.
    /// @return The decoded code point. For `Ignore`, returns `0` with an unchanged position if no further code point
    ///     can be decoded.
    /// @throws std::invalid_argument If `encodingErrors` is `EncodingErrors::Throw` and the data is invalid UTF-8.
    [[nodiscard]] static auto
    decodeChar(std::span<T> buffer, std::size_t &position, const EncodingErrors encodingErrors) -> char32_t {
        switch (encodingErrors) {
        case EncodingErrors::Throw:
            return decodeCharOrThrow(buffer, position);
        case EncodingErrors::Ignore:
            return decodeCharIgnoringErrors(buffer, position);
        case EncodingErrors::Replace:
            return decodeCharReplacingErrors(buffer, position);
        }
        throw std::invalid_argument{"Invalid encoding error handling mode."};
    }

    /// Decode one UTF-8 code point and throw on malformed UTF-8.
    /// @param buffer The byte buffer to decode.
    /// @param position The read position that is advanced after a successful read.
    /// @return The decoded code point.
    /// @throws std::invalid_argument If the data is invalid UTF-8.
    [[nodiscard]] static auto decodeCharOrThrow(std::span<T> buffer, std::size_t &position) -> char32_t {
        return decodeCharImpl<EncodingErrors::Throw>(buffer, position);
    }

    /// Decode one UTF-8 code point and skip malformed UTF-8 bytes.
    /// @param buffer The byte buffer to decode.
    /// @param position The read position that is advanced after a successful read.
    /// @return The decoded code point. Returns `0` with an unchanged position if no further code point can be decoded.
    [[nodiscard]] static auto decodeCharIgnoringErrors(std::span<T> buffer, std::size_t &position) noexcept
        -> char32_t {
        return decodeCharImpl<EncodingErrors::Ignore>(buffer, position);
    }

    /// Decode one UTF-8 code point and replace malformed UTF-8 bytes with `U+FFFD`.
    /// @param buffer The byte buffer to decode.
    /// @param position The read position that is advanced after a successful read.
    /// @return The decoded code point.
    [[nodiscard]] static auto decodeCharReplacingErrors(std::span<T> buffer, std::size_t &position) noexcept
        -> char32_t {
        return decodeCharImpl<EncodingErrors::Replace>(buffer, position);
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

    /// Encode a UTF-32 string as UTF-8.
    /// Invalid code points are replaced with `U+FFFD`.
    /// @param text The UTF-32 text to encode.
    /// @return The encoded UTF-8 string.
    [[nodiscard]] static auto encodeString(const std::u32string_view text)
        -> std::basic_string<std::remove_const_t<T>> {
        auto result = std::basic_string<std::remove_const_t<T>>{};
        auto byteCount = std::size_t{0};
        for (const auto character : text) {
            byteCount += encodedByteCount(character);
        }
        result.reserve(byteCount);
        for (const auto character : text) {
            encodeChar(result, character);
        }
        return result;
    }

private:
    template <EncodingErrors tEncodingErrors, typename Function>
        requires std::invocable<Function, char32_t>
    void decodeAllImpl(Function &&decodeFn) const
        noexcept(tEncodingErrors != EncodingErrors::Throw && noexcept(std::declval<Function &>()(char32_t{}))) {
        std::size_t position = 0;
        while (position < _buffer.size()) {
            const auto initialPosition = position;
            const auto codePoint = decodeCharImpl<tEncodingErrors>(_buffer, position);
            if constexpr (tEncodingErrors == EncodingErrors::Ignore) {
                if (position == initialPosition) {
                    return;
                }
            }
            decodeFn(codePoint);
        }
    }

    template <EncodingErrors tEncodingErrors>
    [[nodiscard]] auto decodeOneCharImpl() const -> char32_t {
        if constexpr (tEncodingErrors == EncodingErrors::Throw) {
            auto position = std::size_t{0};
            const auto codePoint = decodeCharImpl<EncodingErrors::Throw>(_buffer, position);
            if (position != _buffer.size()) {
                throw std::invalid_argument{"decodeOneChar() expects a single code point."};
            }
            return codePoint;
        } else {
            auto codePointCount = std::size_t{0};
            auto codePoint = char32_t{};
            decodeAllImpl<tEncodingErrors>([&](const char32_t decodedCodePoint) noexcept -> void {
                codePoint = decodedCodePoint;
                ++codePointCount;
            });
            if (codePointCount != 1) {
                throw std::invalid_argument{"decodeOneChar() expects a single code point."};
            }
            return codePoint;
        }
    }

    template <EncodingErrors tEncodingErrors>
    [[nodiscard]] static auto
    decodeCharImpl(std::span<T> buffer, std::size_t &position) noexcept(tEncodingErrors != EncodingErrors::Throw)
        -> char32_t {
        const auto initialPosition = position;
        while (position < buffer.size()) {
            auto index = position;
            auto c = static_cast<Byte>(buffer[index]);
            index += 1;
            if (c < Byte{0x80U}) {
                position = index;
                return static_cast<char32_t>(c);
            }
            const auto cSize = expectedByteCount(c);
            if (cSize == 0) {
                if constexpr (tEncodingErrors == EncodingErrors::Throw) {
                    throwEncodingError("Invalid or out-of-range start byte sequence.");
                } else if constexpr (tEncodingErrors == EncodingErrors::Replace) {
                    position += 1;
                    return cReplacementCharacter;
                } else {
                    position += 1;
                    continue;
                }
            }
            char32_t unicodeValue{};
            if (cSize == 2) {
                unicodeValue = static_cast<char32_t>(c & Byte{0b00011111U});
            } else if (cSize == 3) {
                unicodeValue = static_cast<char32_t>(c & Byte{0b00001111U});
            } else {
                unicodeValue = static_cast<char32_t>(c & Byte{0b00000111U});
            }
            auto shouldContinue = false;
            for (std::size_t i = 1; i < cSize; ++i) {
                if (index >= buffer.size()) {
                    if constexpr (tEncodingErrors == EncodingErrors::Throw) {
                        throwEncodingError("Unexpected end of the data.");
                    } else if constexpr (tEncodingErrors == EncodingErrors::Replace) {
                        position += 1;
                        return cReplacementCharacter;
                    } else {
                        position += 1;
                        shouldContinue = true;
                        break;
                    }
                }
                c = static_cast<Byte>(buffer[index]);
                index += 1;
                if ((c & Byte{0b11000000U}) != Byte{0b10000000U}) {
                    if constexpr (tEncodingErrors == EncodingErrors::Throw) {
                        throwEncodingError("Unexpected continuation byte.");
                    } else if constexpr (tEncodingErrors == EncodingErrors::Replace) {
                        position += 1;
                        return cReplacementCharacter;
                    } else {
                        position += 1;
                        shouldContinue = true;
                        break;
                    }
                }
                unicodeValue <<= 6;
                unicodeValue |= static_cast<char32_t>(c & Byte{0b00111111U});
            }
            if (shouldContinue) {
                continue;
            }
            if ((cSize == 3 && unicodeValue < 0x800) || (cSize == 4 && unicodeValue < 0x10000)) {
                if constexpr (tEncodingErrors == EncodingErrors::Throw) {
                    throwEncodingError("Overlong encoding.");
                } else if constexpr (tEncodingErrors == EncodingErrors::Replace) {
                    position += 1;
                    return cReplacementCharacter;
                } else {
                    position += 1;
                    continue;
                }
            }
            const auto result = char32_t{unicodeValue};
            if (!isValidUnicode(result)) {
                if constexpr (tEncodingErrors == EncodingErrors::Throw) {
                    throwEncodingError("Invalid Unicode character.");
                } else if constexpr (tEncodingErrors == EncodingErrors::Replace) {
                    position += 1;
                    return cReplacementCharacter;
                } else {
                    position += 1;
                    continue;
                }
            }
            position = index;
            return result;
        }
        if constexpr (tEncodingErrors == EncodingErrors::Ignore) {
            position = initialPosition;
        }
        return char32_t{};
    }

    std::span<T> _buffer;
};

U8Buffer(const std::u8string &) -> U8Buffer<const char8_t>;
U8Buffer(std::u8string_view) -> U8Buffer<const char8_t>;
U8Buffer(const std::string &) -> U8Buffer<const char>;
U8Buffer(std::string_view) -> U8Buffer<const char>;

}
