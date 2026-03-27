// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Tokenizer.hpp"

#include <cstddef>
#include <string>
#include <string_view>


namespace erbsland::cterm::text::impl {

/// A tokenizer base class with a reusable UTF-32 buffer for decoded content.
class BufferedTokenizer : public Tokenizer {
protected:
    /// A range inside `_buffer`.
    struct IndexRange final {
        std::size_t start{0};  ///< The start index in `_buffer`.
        std::size_t length{0}; ///< The number of code points in the range.
    };

protected:
    /// Create a buffered tokenizer for the given text.
    /// @param text The UTF-8 encoded source text.
    /// @param initialBufferCapacity The optional initial capacity reserved for `_buffer`.
    explicit BufferedTokenizer(std::string_view text, std::size_t initialBufferCapacity = 0);

public:
    // defaults
    ~BufferedTokenizer() override = default;
    BufferedTokenizer(const BufferedTokenizer &) = delete;
    BufferedTokenizer(BufferedTokenizer &&) = delete;
    auto operator=(const BufferedTokenizer &) -> BufferedTokenizer & = delete;
    auto operator=(BufferedTokenizer &&) -> BufferedTokenizer & = delete;

protected:
    /// Consume the current character if it matches and append it to `_buffer`.
    /// @param expected The expected character.
    /// @return `true` if the character matched and was consumed.
    [[nodiscard]] auto consumeBufferedIf(char32_t expected) noexcept -> bool;
    /// Clear `_buffer` so it can be reused.
    void resetBuffer();
    /// Append one character to `_buffer`.
    /// @param character The character to append.
    void append(char32_t character);
    /// Append the current character to `_buffer` and advance the tokenizer.
    void appendCurrentAndAdvance();
    /// Create a range from `start` to the current end of `_buffer`.
    /// @param start The range start inside `_buffer`.
    /// @return The created range.
    [[nodiscard]] auto makeRange(std::size_t start) const noexcept -> IndexRange;
    /// Trim ASCII whitespace at both ends of a `_buffer` range.
    /// @param range The range to trim.
    /// @return The trimmed range.
    [[nodiscard]] auto trimRange(IndexRange range) const noexcept -> IndexRange;

protected:
    std::u32string _buffer; ///< Reusable decode buffer for buffered tokenizers.
};

}
