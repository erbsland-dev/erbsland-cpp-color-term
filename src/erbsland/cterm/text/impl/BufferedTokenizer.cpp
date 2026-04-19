// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "BufferedTokenizer.hpp"

#include "../../impl/TextUtil.hpp"

namespace erbsland::cterm::text::impl {

BufferedTokenizer::BufferedTokenizer(const std::string_view text, const std::size_t initialBufferCapacity) :
    Tokenizer{text} {
    if (initialBufferCapacity > 0) {
        _buffer.reserve(initialBufferCapacity);
    }
}

auto BufferedTokenizer::consumeBufferedIf(const char32_t expected) noexcept -> bool {
    if (current() != expected) {
        return false;
    }
    appendCurrentAndAdvance();
    return true;
}

void BufferedTokenizer::resetBuffer() {
    _buffer.clear();
}

void BufferedTokenizer::append(const char32_t character) {
    _buffer.push_back(character);
}

void BufferedTokenizer::appendCurrentAndAdvance() {
    append(current());
    next();
}

auto BufferedTokenizer::makeRange(const std::size_t start) const noexcept -> IndexRange {
    return IndexRange{start, _buffer.size() - start};
}

auto BufferedTokenizer::trimRange(IndexRange range) const noexcept -> IndexRange {
    while (range.length() > 0 && cterm::impl::isAsciiWhitespace(_buffer[range.startIndex()])) {
        range = IndexRange{range.startIndex() + 1, range.length() - 1};
    }
    while (range.length() > 0 && cterm::impl::isAsciiWhitespace(_buffer[(range.startIndex() + range.length()) - 1])) {
        range = IndexRange{range.startIndex(), range.length() - 1};
    }
    return range;
}

}
