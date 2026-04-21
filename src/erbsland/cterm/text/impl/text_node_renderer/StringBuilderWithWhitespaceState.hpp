// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../../../impl/StringBuilder.hpp"
#include "../../TextNode.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

/// A string builder that handles whitespace state and trimming.
class StringBuilderWithWhitespaceState : public cterm::impl::StringBuilder {
public:
    StringBuilderWithWhitespaceState() = default;
    ~StringBuilderWithWhitespaceState() = default;

public:
    void appendDecoration(const std::optional<String> &decoration, const CharStyle &style) {
        if (!decoration.has_value()) {
            return;
        }
        appendWithBaseStyle(*decoration, style);
    }
    void appendDecorationTrimmed(const std::optional<String> &decoration, const CharStyle &style) {
        if (!decoration.has_value()) {
            return;
        }
        appendTrimmedView(*decoration, style);
    }
    void appendTrimmed(std::u32string_view textValue, const CharStyle &style) noexcept;
    void appendTrimmedView(const StringView &view, const CharStyle &style) noexcept;

public: // Whitespace state methods
    [[nodiscard]] auto atLineStart() const noexcept -> bool { return _atLineStart; }
    [[nodiscard]] auto hasPendingWhitespace() const noexcept -> bool { return !_pendingWhitespace.empty(); }
    void resetLineStart() noexcept {
        _pendingWhitespace.clear();
        _atLineStart = true;
    }
    void appendPendingWhitespace(const StringView &view, const CharStyle &style) noexcept {
        for (auto character : view) {
            _pendingWhitespace.push_back(character.withBase(style));
        }
    }
    void appendPendingWhitespace(const std::u32string_view text, const CharStyle &style) noexcept {
        for (auto character : String{text, style}) {
            _pendingWhitespace.push_back(character);
        }
    }
    void flushPendingWhitespace() noexcept {
        if (_pendingWhitespace.empty()) {
            return;
        }
        for (auto character : _pendingWhitespace) {
            append(character);
        }
        _pendingWhitespace.clear();
    }
    void markContentWritten() noexcept { _atLineStart = false; }

private:
    std::vector<Char> _pendingWhitespace; ///< Collapsible whitespace waiting for following content.
    bool _atLineStart{true};              ///< `true` at the start of the block and after explicit line breaks.
};

}
