// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <string_view>


namespace erbsland::cterm::text::impl {

class Tokenizer {
protected:
    /// A snapshot of the parser state used for speculative parsing and rollback.
    struct State final {
        std::size_t readPos{0};      ///< The byte position used to decode the next lookahead character.
        struct DecodedChar {
            std::size_t position{0}; ///< The start position of the character.
            char32_t codePoint{0};   ///< The decoded code point.
        };
        DecodedChar current;         ///< The character at the current position.
        DecodedChar next;            ///< The lookahead character.
    };

    explicit Tokenizer(std::string_view text);

public:
    // defaults
    virtual ~Tokenizer() = default;
    Tokenizer(const Tokenizer &) = delete;
    Tokenizer(Tokenizer &&) = delete;
    auto operator=(const Tokenizer &) -> Tokenizer & = delete;
    auto operator=(Tokenizer &&) -> Tokenizer & = delete;

protected:
    /// Test whether the parser reached the end of the input.
    /// @return `true` if there is no current code point, otherwise `false`.
    [[nodiscard]] auto isAtEnd() const noexcept -> bool { return current() == char32_t{0}; }
    /// Get the current code point being processed.
    /// @return The current code point, or zero if the end of the text is reached.
    [[nodiscard]] auto current() const noexcept -> char32_t { return _state.current.codePoint; }
    /// Peek at the next code point.
    /// @return The next code point, or zero if the end of the text is reached.
    [[nodiscard]] auto peek() const noexcept -> char32_t { return _state.next.codePoint; }
    /// Save the current parser state for later rollback.
    /// @return A state snapshot that can be restored with `load()`.
    [[nodiscard]] auto save() const noexcept -> State;
    /// Restore a previously saved parser state.
    /// @param state The state snapshot to restore.
    void load(const State &state) noexcept;
    /// Consume the current character if it matches the expected value.
    /// @param expected The expected character.
    /// @return `true` if the character matched and was consumed, otherwise `false`.
    auto consumeIf(char32_t expected) noexcept -> bool;
    /// Read the next code point.
    void next() noexcept;

private:
    std::string_view _text; ///< A reference to the parsed text.
    State _state;           ///< The complete parser state.
};

}
