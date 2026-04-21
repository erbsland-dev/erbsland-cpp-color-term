// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "CombinedChar.hpp"
#include "U8Buffer.hpp"

#include "../Key.hpp"

#include <array>
#include <cstddef>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace erbsland::cterm::impl {

/// Decode one key from a UTF-8 console input buffer.
class KeyDecoder final {
public:
    /// Result of parsing one key from the beginning of a byte stream.
    class ParseResult final {
    public:
        /// Create one parse result without a decoded key.
        /// @param status The parsing status.
        /// @param consumedByteCount The number of bytes consumed.
        ParseResult(
            const U8ParseStatus status = U8ParseStatus::Invalid, const std::size_t consumedByteCount = 0) noexcept :
            _status{status}, _consumedByteCount{consumedByteCount} {}

        /// Create one parse result with a decoded key.
        /// @param status The parsing status.
        /// @param key The decoded key.
        /// @param consumedByteCount The number of bytes consumed.
        ParseResult(const U8ParseStatus status, Key key, const std::size_t consumedByteCount) noexcept :
            _status{status}, _key{key}, _consumedByteCount{consumedByteCount} {}

    public:
        /// Access the parsing status.
        [[nodiscard]] auto status() const noexcept -> U8ParseStatus { return _status; }
        /// Access the parsed key for `Parsed`.
        [[nodiscard]] auto key() const noexcept -> const Key & { return _key; }
        /// Access the number of bytes consumed.
        [[nodiscard]] auto consumedByteCount() const noexcept -> std::size_t { return _consumedByteCount; }

    private:
        U8ParseStatus _status{U8ParseStatus::Invalid}; ///< The parsing status.
        Key _key;                                      ///< The parsed key for `Parsed`.
        std::size_t _consumedByteCount{0};             ///< Number of bytes consumed.
    };

public:
    /// Create a decoder for the given input bytes.
    /// @param text The input bytes to decode.
    explicit KeyDecoder(const std::string_view text) noexcept : _text{text} {}

    KeyDecoder(const KeyDecoder &) = delete;
    KeyDecoder(KeyDecoder &&) = delete;
    auto operator=(const KeyDecoder &) -> KeyDecoder & = delete;
    auto operator=(KeyDecoder &&) -> KeyDecoder & = delete;

public:
    /// Parse one key from the beginning of the configured input bytes.
    /// @return The parsing result for the leading bytes.
    [[nodiscard]] auto parseConsoleInputPrefix() const noexcept -> ParseResult;
    /// Decode exactly one full console input item.
    /// @return The decoded key, or an invalid key if the text is unsupported or incomplete.
    [[nodiscard]] auto decodeConsoleInput() const noexcept -> Key;

private:
    struct SimpleSequenceDefinition final {
        std::string_view sequence;
        Key::Type type;
    };

private:
    [[nodiscard]] static auto createCharacterKey(const CombinedChar &character) noexcept -> Key;
    [[nodiscard]] static auto simpleSequenceDefinitions() noexcept -> const std::array<SimpleSequenceDefinition, 6> &;
    [[nodiscard]] static auto parseModifierParameter(int value) noexcept -> std::optional<KeyModifiers>;
    [[nodiscard]] static auto parseCsiParameters(std::string_view text) noexcept -> std::optional<std::vector<int>>;
    [[nodiscard]] static auto findCsiFinalByte(std::string_view text) noexcept -> std::optional<std::size_t>;
    [[nodiscard]] static auto keyFromCsiFinal(char finalByte) noexcept -> Key::Type;
    [[nodiscard]] static auto keyFromCsiTildeParameter(int parameter) noexcept -> Key::Type;
    [[nodiscard]] static auto decodeCsi(std::string_view text) noexcept -> ParseResult;
    [[nodiscard]] static auto decodeSs3(std::string_view text) noexcept -> ParseResult;
    [[nodiscard]] auto parseUtf8CodePointPrefix(std::size_t offset) const noexcept -> U8ParseResult;

private:
    std::string_view _text;
};

}
