// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "CombinedChar.hpp"
#include "U8Buffer.hpp"

#include "../Key.hpp"

#include <array>
#include <cstddef>
#include <string_view>


namespace erbsland::cterm::impl {


/// Decode one key from a UTF-8 console input buffer.
class KeyDecoder final {
public:
    /// Result of parsing one key from the beginning of a byte stream.
    struct ParseResult final {
        U8ParseStatus status{U8ParseStatus::Invalid}; ///< The parsing status.
        Key key{};                                    ///< The parsed key for `Parsed`.
        std::size_t consumedByteCount{0};             ///< Number of bytes consumed.
    };

public:
    /// Create a decoder for the given input bytes.
    /// @param text The input bytes to decode.
    explicit KeyDecoder(std::string_view text) noexcept : _text{text} {}

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
    struct ConsoleSequenceDefinition final {
        std::string_view sequence;
        Key::Type type;
    };

private:
    [[nodiscard]] static auto createCharacterKey(const CombinedChar &character) noexcept -> Key;
    [[nodiscard]] static auto consoleSequenceDefinitions() noexcept
        -> const std::array<ConsoleSequenceDefinition, 32> &;
    [[nodiscard]] auto parseUtf8CodePointPrefix(std::size_t offset) const noexcept -> U8ParseResult;

private:
    std::string_view _text;
};


}
