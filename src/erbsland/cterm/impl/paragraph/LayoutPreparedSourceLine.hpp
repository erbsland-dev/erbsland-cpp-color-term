// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "LayoutLine.hpp"
#include "LayoutLineToken.hpp"
#include "LayoutSourceLineRange.hpp"

#include <vector>


namespace erbsland::cterm::impl::paragraph {

/// A pre-tokenized source line ready for physical line layout.
class LayoutPreparedSourceLine final {
public:
    /// Create a prepared source line for the given source range.
    /// Storage is reserved from the source range length to reduce allocation churn while tokenizing.
    /// @param sourceRange The original source-line range in the input text.
    explicit LayoutPreparedSourceLine(LayoutSourceLineRange sourceRange) : sourceLine{sourceRange} {
        tokens.reserve(sourceRange.length);
        _pendingTokens.reserve(4);
    }

    // defaults
    ~LayoutPreparedSourceLine() = default;
    LayoutPreparedSourceLine(const LayoutPreparedSourceLine &) = delete;
    LayoutPreparedSourceLine(LayoutPreparedSourceLine &&) noexcept = default;
    auto operator=(const LayoutPreparedSourceLine &) -> LayoutPreparedSourceLine & = delete;
    auto operator=(LayoutPreparedSourceLine &&) noexcept -> LayoutPreparedSourceLine & = default;

public:
    /// Test whether this prepared source line contains any tokens.
    /// @return `true` if this prepared source line contains no tokens.
    [[nodiscard]] auto empty() const noexcept -> bool { return tokens.empty(); }
    /// Append one word token and flush pending spacing tokens in front of it.
    /// @param startIndex The first source character index of the word.
    /// @param length The number of source characters in the word.
    /// @param displayWidth The cached display width of the word.
    void appendWord(std::size_t startIndex, std::size_t length, int displayWidth) noexcept {
        if (length == 0) {
            return;
        }
        flushPendingTokens();
        tokens.emplace_back(LayoutLineToken::Type::Word, startIndex, length, displayWidth);
        _hasWordToken = true;
    }
    /// Append one pending tab token that may be flushed later.
    /// @param index The source character index of the tab.
    void appendPendingTab(std::size_t index) noexcept {
        _pendingTokens.emplace_back(LayoutLineToken::Type::Tab, index, 1, 0);
    }
    /// Append one pending separator-space token, merging adjacent pending separator runs.
    /// @param index The source character index of the separator.
    void appendPendingSeparatorSpace(std::size_t index) noexcept {
        if (!_pendingTokens.empty() && _pendingTokens.back().type() == LayoutLineToken::Type::SeparatorSpace) {
            const auto &separator = _pendingTokens.back();
            _pendingTokens.back() = LayoutLineToken{
                LayoutLineToken::Type::SeparatorSpace, separator.startIndex(), separator.length() + 1, 1};
            return;
        }
        _pendingTokens.emplace_back(LayoutLineToken::Type::SeparatorSpace, index, 1, 1);
    }
    /// Flush the remaining pending spacing tokens into the prepared token list.
    void finish() noexcept {
        flushPendingTokens();
        _pendingTokens.clear();
    }

public:
    LayoutSourceLineRange sourceLine;    ///< The original source line range.
    std::vector<LayoutLineToken> tokens; ///< The extracted tokens of the source line.

private:
    void flushPendingTokens() noexcept {
        if (!_hasWordToken) {
            for (const auto &token : _pendingTokens) {
                if (token.type() == LayoutLineToken::Type::Tab) {
                    tokens.push_back(token);
                }
            }
            _pendingTokens.clear();
            return;
        }
        tokens.reserve(tokens.size() + _pendingTokens.size());
        for (const auto &token : _pendingTokens) {
            tokens.push_back(token);
        }
        _pendingTokens.clear();
    }

private:
    std::vector<LayoutLineToken> _pendingTokens; ///< Spacing tokens waiting for the next emitted word.
    bool _hasWordToken{false};                   ///< Tracks whether a word token was already emitted.
};


}
