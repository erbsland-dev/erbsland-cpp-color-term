// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/ColorTermIncludes.hpp"

#include <erbsland/cterm/text/impl/BufferedTokenizer.hpp>

#include <string_view>

class BufferedTokenizerProbe final : public textimpl::BufferedTokenizer {
public:
    explicit BufferedTokenizerProbe(const std::string_view text, const std::size_t initialBufferCapacity = 0) :
        BufferedTokenizer{text, initialBufferCapacity} {}

    [[nodiscard]] auto bufferText() const -> const std::u32string & { return _buffer; }
    [[nodiscard]] auto bufferCapacity() const -> std::size_t { return _buffer.capacity(); }
    [[nodiscard]] auto currentCodePoint() const noexcept -> char32_t { return current(); }

    void appendCharacter(const char32_t character) { append(character); }
    void clearBuffer() { resetBuffer(); }
    void appendCurrentCharacterAndAdvance() { appendCurrentAndAdvance(); }
    [[nodiscard]] auto consumeBufferedCharacter(const char32_t expected) noexcept -> bool {
        return consumeBufferedIf(expected);
    }
    [[nodiscard]] auto rangeFrom(const std::size_t start) const noexcept -> erbsland::cterm::IndexRange {
        return makeRange(start);
    }
    [[nodiscard]] auto trim(erbsland::cterm::IndexRange range) const noexcept -> erbsland::cterm::IndexRange {
        return trimRange(range);
    }
};
