// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Tokenizer.hpp"

#include "../../impl/U8Buffer.hpp"

namespace erbsland::cterm::text::impl {

Tokenizer::Tokenizer(std::string_view text) : _text{text} {
    next();
    next();
}

auto Tokenizer::save() const noexcept -> State {
    return _state;
}

void Tokenizer::load(const State &state) noexcept {
    _state = state;
}

auto Tokenizer::consumeIf(const char32_t expected) noexcept -> bool {
    if (current() != expected) {
        return false;
    }
    next();
    return true;
}

void Tokenizer::next() noexcept {
    _state.current = _state.next;
    if (_state.readPos >= _text.size()) {
        _state.next = {};
        return;
    }
    _state.next.position = _state.readPos;
    _state.next.codePoint = cterm::impl::U8Buffer<const char>::decodeCharReplacingErrors(
        std::span(_text.data(), _text.size()), _state.readPos);
}

}
