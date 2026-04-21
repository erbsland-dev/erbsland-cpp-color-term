// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StringBuilderWithWhitespaceState.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

void StringBuilderWithWhitespaceState::appendTrimmed(
    const std::u32string_view textValue, const CharStyle &style) noexcept {
    if (textValue.empty()) {
        return;
    }
    auto contentStart = std::size_t{0};
    if (atLineStart()) {
        while (contentStart < textValue.size() && textValue[contentStart] == U' ') {
            contentStart += 1;
        }
    }
    if (contentStart == textValue.size()) {
        if (!atLineStart()) {
            appendPendingWhitespace(textValue, style);
        }
        return;
    }
    auto contentEnd = textValue.size();
    while (contentEnd > contentStart && textValue[contentEnd - 1] == U' ') {
        contentEnd -= 1;
    }
    flushPendingWhitespace();
    appendStyled(textValue.substr(contentStart, contentEnd - contentStart), style);
    markContentWritten();
    if (contentEnd < textValue.size()) {
        appendPendingWhitespace(textValue.substr(contentEnd), style);
    }
}

void StringBuilderWithWhitespaceState::appendTrimmedView(const StringView &view, const CharStyle &style) noexcept {
    if (view.empty()) {
        return;
    }
    auto contentStart = std::size_t{0};
    if (atLineStart()) {
        while (contentStart < view.size() && view[contentStart].isOneOf(U" \t\r\n\f")) {
            contentStart += 1;
        }
    }
    if (contentStart == view.size()) {
        if (!atLineStart()) {
            appendPendingWhitespace(view, style);
        }
        return;
    }
    auto contentEnd = view.size();
    while (contentEnd > contentStart && view[contentEnd - 1].isOneOf(U" \t\r\n\f")) {
        contentEnd -= 1;
    }
    flushPendingWhitespace();
    appendWithBaseStyle(view.substr(contentStart, contentEnd - contentStart), style);
    markContentWritten();
    if (contentEnd < view.size()) {
        appendPendingWhitespace(view.substr(contentEnd), style);
    }
}

}
