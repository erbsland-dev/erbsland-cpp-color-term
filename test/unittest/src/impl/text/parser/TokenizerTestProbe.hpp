// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/ColorTermIncludes.hpp"

#include <erbsland/cterm/text/impl/Tokenizer.hpp>

#include <array>
#include <string_view>

class TokenizerProbe final : public textimpl::Tokenizer {
public:
    explicit TokenizerProbe(const std::string_view text) : Tokenizer{text} {}

    [[nodiscard]] auto currentCodePoint() const noexcept -> char32_t { return current(); }
    [[nodiscard]] auto peekCodePoint() const noexcept -> char32_t { return peek(); }
    [[nodiscard]] auto atEnd() const noexcept -> bool { return isAtEnd(); }

    auto advance() noexcept -> void { next(); }
    auto consume(const char32_t expected) noexcept -> bool { return consumeIf(expected); }

    [[nodiscard]] auto advanceTwiceAndRestore() noexcept -> std::array<char32_t, 4> {
        const auto state = save();
        next();
        next();
        const auto currentAfterAdvance = current();
        const auto peekAfterAdvance = peek();
        load(state);
        return {currentAfterAdvance, peekAfterAdvance, current(), peek()};
    }
};
