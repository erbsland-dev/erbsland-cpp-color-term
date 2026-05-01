// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StringRangeView.hpp"

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <string>

namespace erbsland::cterm::impl {

auto StringRangeView::storageIndex(const std::size_t localIndex) const noexcept -> std::size_t {
    return _range.startIndex() + localIndex;
}

auto StringRangeView::characterAt(const std::size_t localIndex) const noexcept -> const Char & {
    return _data.chars()[storageIndex(localIndex)];
}

auto StringRangeView::begin() const noexcept -> const_iterator {
    return _data.chars().cbegin() + static_cast<difference_type>(_range.startIndex());
}

auto StringRangeView::end() const noexcept -> const_iterator {
    return begin() + static_cast<difference_type>(_range.length());
}

auto StringRangeView::cbegin() const noexcept -> const_iterator {
    return begin();
}

auto StringRangeView::cend() const noexcept -> const_iterator {
    return end();
}

auto StringRangeView::rbegin() const noexcept -> const_reverse_iterator {
    return const_reverse_iterator{end()};
}

auto StringRangeView::rend() const noexcept -> const_reverse_iterator {
    return const_reverse_iterator{begin()};
}

auto StringRangeView::crbegin() const noexcept -> const_reverse_iterator {
    return rbegin();
}

auto StringRangeView::crend() const noexcept -> const_reverse_iterator {
    return rend();
}

auto StringRangeView::displayWidth() const noexcept -> int {
    if (_range.startIndex() == 0 && _range.length() == _data.size() && _data.hasDisplayWidthCache()) {
        return _data.displayWidth();
    }
    auto result = 0;
    for (auto it = begin(); it != end(); ++it) {
        result += it->displayWidth();
    }
    return result;
}

auto StringRangeView::operator[](const std::size_t index) const noexcept -> Char {
    if (index >= size()) {
        return {};
    }
    return characterAt(index);
}

auto StringRangeView::at(const std::size_t index, const std::string_view typeName) const -> Char {
    if (index >= size()) {
        throw std::out_of_range{std::string{typeName} + " index out of range."};
    }
    return characterAt(index);
}

auto StringRangeView::count(const Char &character) const noexcept -> std::size_t {
    return static_cast<std::size_t>(
        std::ranges::count_if(*this, [&](const Char &candidate) -> bool { return candidate == character; }));
}

auto StringRangeView::count(const char32_t character) const noexcept -> std::size_t {
    return static_cast<std::size_t>(std::ranges::count_if(
        *this, [&](const Char &candidate) -> bool { return candidate.singleCodePoint() == character; }));
}

auto StringRangeView::indexOf(const Char &character, const std::size_t startIndex) const noexcept -> std::size_t {
    if (startIndex >= size()) {
        return IndexRange::npos;
    }
    for (auto index = startIndex; index < size(); ++index) {
        if (characterAt(index) == character) {
            return index;
        }
    }
    return IndexRange::npos;
}

auto StringRangeView::indexOf(const char32_t character, const std::size_t startIndex) const noexcept -> std::size_t {
    if (startIndex >= size()) {
        return IndexRange::npos;
    }
    for (auto index = startIndex; index < size(); ++index) {
        if (characterAt(index).singleCodePoint() == character) {
            return index;
        }
    }
    return IndexRange::npos;
}

auto StringRangeView::indexOf(const std::u32string_view characterSet, const std::size_t startIndex) const noexcept
    -> std::size_t {
    if (startIndex >= size()) {
        return IndexRange::npos;
    }
    for (auto index = startIndex; index < size(); ++index) {
        if (characterAt(index).isOneOf(characterSet)) {
            return index;
        }
    }
    return IndexRange::npos;
}

auto StringRangeView::indexNotOf(const std::u32string_view characterSet, const std::size_t startIndex) const noexcept
    -> std::size_t {
    if (startIndex >= size()) {
        return IndexRange::npos;
    }
    for (auto index = startIndex; index < size(); ++index) {
        if (!characterAt(index).isOneOf(characterSet)) {
            return index;
        }
    }
    return IndexRange::npos;
}

auto StringRangeView::subRange(const std::size_t startIndex, std::size_t length) const noexcept -> IndexRange {
    if (startIndex >= size() || length == 0) {
        return {};
    }
    if (length == IndexRange::npos || length > size() - startIndex) {
        length = size() - startIndex;
    }
    return IndexRange{storageIndex(startIndex), length};
}

auto StringRangeView::croppedRange(const Coordinate displayWidth, const Alignment alignment) const noexcept
    -> IndexRange {
    if (displayWidth <= 0) {
        return {};
    }
    if (this->displayWidth() < displayWidth) {
        return _range;
    }
    auto currentDisplayWidth = Coordinate{0};
    auto index = std::size_t{0};
    auto newIndexRange = IndexRange{};
    if (alignment.isRight()) {
        index = size();
        while (currentDisplayWidth < displayWidth && index > 0) {
            const auto characterDisplayWidth = characterAt(index - 1).displayWidth();
            if (currentDisplayWidth + characterDisplayWidth > displayWidth) {
                break;
            }
            currentDisplayWidth += characterDisplayWidth;
            index -= 1;
        }
        newIndexRange = IndexRange{storageIndex(index), size() - index};
    } else {
        while (currentDisplayWidth < displayWidth && index < size()) {
            const auto characterDisplayWidth = characterAt(index).displayWidth();
            if (currentDisplayWidth + characterDisplayWidth > displayWidth) {
                break;
            }
            currentDisplayWidth += characterDisplayWidth;
            index += 1;
        }
        newIndexRange = IndexRange{_range.startIndex(), index};
    }
    if (newIndexRange.empty()) {
        return {};
    }
    return newIndexRange;
}

auto StringRangeView::trimmedRange(std::u32string_view characters) const noexcept -> IndexRange {
    if (empty()) {
        return {};
    }
    if (characters.empty()) {
        characters = U" \n\t";
    }
    auto startIndex = std::size_t{0};
    auto endIndex = size();
    while (startIndex < endIndex && characterAt(startIndex).isOneOf(characters)) {
        startIndex += 1;
    }
    while (endIndex > startIndex && characterAt(endIndex - 1).isOneOf(characters)) {
        endIndex -= 1;
    }
    if (startIndex == endIndex) {
        return {};
    }
    return subRange(startIndex, endIndex - startIndex);
}

auto StringRangeView::containsControlCharacters() const noexcept -> bool {
    return std::ranges::any_of(*this, [](const Char &character) -> bool { return character.isControl(); });
}

auto StringRangeView::splitWordRanges() const noexcept -> std::vector<IndexRange> {
    auto words = std::vector<IndexRange>{};
    auto wordStart = IndexRange::npos;
    for (auto index = std::size_t{0}; index < size(); ++index) {
        if (characterAt(index).isSpacing()) {
            if (wordStart != IndexRange::npos) {
                words.emplace_back(subRange(wordStart, index - wordStart));
                wordStart = IndexRange::npos;
            }
            continue;
        }
        if (wordStart == IndexRange::npos) {
            wordStart = index;
        }
    }
    if (wordStart != IndexRange::npos) {
        words.emplace_back(subRange(wordStart));
    }
    return words;
}

auto StringRangeView::splitLineRanges() const noexcept -> std::vector<IndexRange> {
    if (empty()) {
        return {};
    }
    auto result = std::vector<IndexRange>{};
    result.reserve(count(U'\n') + 1);
    auto lineStartIndex = std::size_t{0};
    while (lineStartIndex < size()) {
        const auto lineEndIndex = indexOf(U'\n', lineStartIndex);
        if (lineEndIndex == IndexRange::npos) {
            result.emplace_back(subRange(lineStartIndex));
            break;
        }
        result.emplace_back(subRange(lineStartIndex, lineEndIndex - lineStartIndex));
        lineStartIndex = lineEndIndex + 1;
    }
    return result;
}

auto StringRangeView::terminalLines(const int width) const noexcept -> int {
    if (width <= 0) {
        return 0;
    }
    if (width == 1) {
        return static_cast<int>(size());
    }
    auto renderedLines = 0;
    auto currentWidth = 0;
    for (const auto &character : *this) {
        if (character == U'\n') {
            renderedLines += 1;
            currentWidth = 0;
            continue;
        }
        const auto characterWidth = character.displayWidth();
        currentWidth += characterWidth;
        if (currentWidth >= width) {
            renderedLines += 1;
            currentWidth = currentWidth > width ? characterWidth : 0;
        }
    }
    if (currentWidth > 0) {
        renderedLines += 1;
    }
    return renderedLines;
}

auto StringRangeView::naturalTextSize() const noexcept -> Size {
    auto preferredWidth = Coordinate{1};
    auto preferredHeight = Coordinate{1};
    auto currentLineWidth = Coordinate{0};
    for (auto index = std::size_t{0}; index < size(); ++index) {
        const auto character = characterAt(index);
        if (character == U'\n') {
            preferredWidth = std::max(preferredWidth, currentLineWidth);
            currentLineWidth = 0;
            if (index + 1 < size()) {
                preferredHeight += 1;
            }
            continue;
        }
        currentLineWidth += character.displayWidth();
    }
    preferredWidth = std::max(preferredWidth, currentLineWidth);
    return {preferredWidth, preferredHeight};
}

}
