// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "LayoutLineToken.hpp"

#include "../../StringView.hpp"

namespace erbsland::cterm::impl::paragraph {

auto LayoutLineToken::remainingWidth(const StringView &text, const std::size_t offset) const noexcept -> int {
    assert(isWord());
    if (offset == 0) {
        return _displayWidth;
    }
    auto width = 0;
    for (auto index = offset; index < _length; ++index) {
        width += text[sourceIndex(index)].displayWidth();
    }
    return width;
}

auto LayoutLineToken::split(
    const StringView &text,
    const std::size_t offset,
    const int availableWidth,
    const int trailingMarkerWidth) const noexcept -> std::optional<SplitResult> {
    assert(isWord());
    if (availableWidth - trailingMarkerWidth <= 0) {
        return std::nullopt;
    }
    auto result = SplitResult{};
    auto usedWidth = 0;
    for (auto index = offset; index < _length; ++index) {
        const auto characterWidth = text[sourceIndex(index)].displayWidth();
        if (characterWidth <= 0) {
            continue;
        }
        if (usedWidth + characterWidth > availableWidth - trailingMarkerWidth) {
            break;
        }
        usedWidth += characterWidth;
        result.sourceCharacterCount = (index - offset) + 1;
    }
    if (result.sourceCharacterCount == 0) {
        return std::nullopt;
    }
    result.sourceWidth = usedWidth;
    result.width = usedWidth;
    if (offset + result.sourceCharacterCount < _length) {
        result.width += trailingMarkerWidth;
    }
    return result;
}

}
