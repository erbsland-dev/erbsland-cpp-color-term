// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StringView.hpp"

#include "impl/StringData.hpp"
#include "impl/StringWrapper.hpp"

#include <algorithm>
#include <ranges>
#include <stdexcept>

namespace erbsland::cterm {

StringView::StringView() noexcept : _data{impl::StringData::sharedEmpty()}, _range{} {
}

StringView::StringView(const String &string) noexcept : _data{string._data}, _range{string._range} {
}

StringView::StringView(std::shared_ptr<impl::StringData> data, const IndexRange range) noexcept :
    _data{std::move(data)}, _range{range.clampedTo(_data->size())} {
}

auto StringView::operator[](const std::size_t index) const noexcept -> Char {
    return characterAt(index);
}

auto StringView::displayWidth() const noexcept -> int {
    if (_range.startIndex() == 0 && _range.length() == _data->size() && _data->hasDisplayWidthCache()) {
        return _data->displayWidth();
    }
    auto result = 0;
    for (auto it = begin(); it != end(); ++it) {
        result += it->displayWidth();
    }
    return result;
}

auto StringView::at(const std::size_t index) const -> Char {
    if (index >= size()) {
        throw std::out_of_range{"StringView index out of range."};
    }
    return characterAt(index);
}

auto StringView::begin() const noexcept -> const_iterator {
    return _data->chars().cbegin() + static_cast<difference_type>(_range.startIndex());
}

auto StringView::end() const noexcept -> const_iterator {
    return begin() + static_cast<difference_type>(_range.length());
}

auto StringView::cbegin() const noexcept -> const_iterator {
    return begin();
}

auto StringView::cend() const noexcept -> const_iterator {
    return end();
}

auto StringView::rbegin() const noexcept -> const_reverse_iterator {
    return const_reverse_iterator{end()};
}

auto StringView::rend() const noexcept -> const_reverse_iterator {
    return const_reverse_iterator{begin()};
}

auto StringView::crbegin() const noexcept -> const_reverse_iterator {
    return rbegin();
}

auto StringView::crend() const noexcept -> const_reverse_iterator {
    return rend();
}

auto StringView::count(const Char &character) const noexcept -> std::size_t {
    return static_cast<std::size_t>(
        std::ranges::count_if(*this, [&](const Char &candidate) -> bool { return candidate == character; }));
}

auto StringView::count(const char32_t character) const noexcept -> std::size_t {
    return static_cast<std::size_t>(std::ranges::count_if(
        *this, [&](const Char &candidate) -> bool { return candidate.singleCodePoint() == character; }));
}

auto StringView::indexOf(const Char &character, const std::size_t startIndex) const noexcept -> std::size_t {
    if (startIndex >= size()) {
        return npos;
    }
    for (auto index = startIndex; index < size(); ++index) {
        if (characterAt(index) == character) {
            return index;
        }
    }
    return npos;
}

auto StringView::indexOf(const char32_t character, const std::size_t startIndex) const noexcept -> std::size_t {
    if (startIndex >= size()) {
        return npos;
    }
    for (auto index = startIndex; index < size(); ++index) {
        if (characterAt(index).singleCodePoint() == character) {
            return index;
        }
    }
    return npos;
}

auto StringView::substr(const std::size_t startIndex, std::size_t length) const noexcept -> StringView {
    if (startIndex >= size() || length == 0) {
        return {};
    }
    if (length == npos || length > size() - startIndex) {
        length = size() - startIndex;
    }
    return StringView{_data, IndexRange{storageIndex(startIndex), length}};
}

auto StringView::trimmed(const std::u32string_view characters) const noexcept -> StringView {
    auto start = std::size_t{0};
    auto endIndex = size();
    while (start < endIndex && characterAt(start).isOneOf(characters)) {
        start += 1;
    }
    while (endIndex > start && characterAt(endIndex - 1).isOneOf(characters)) {
        endIndex -= 1;
    }
    return substr(start, endIndex - start);
}

auto StringView::containsControlCharacters() const noexcept -> bool {
    return std::ranges::any_of(*this, [](const Char &character) -> bool { return character.isControl(); });
}

auto StringView::splitWords() const noexcept -> std::vector<StringView> {
    auto words = std::vector<StringView>{};
    auto wordStart = npos;
    for (auto index = std::size_t{0}; index < size(); ++index) {
        if (characterAt(index).isSpacing()) {
            if (wordStart != npos) {
                words.emplace_back(substr(wordStart, index - wordStart));
                wordStart = npos;
            }
            continue;
        }
        if (wordStart == npos) {
            wordStart = index;
        }
    }
    if (wordStart != npos) {
        words.emplace_back(substr(wordStart));
    }
    return words;
}

auto StringView::wrapIntoLines(const int width, const ParagraphSpacing paragraphSpacing) const noexcept -> StringLines {
    return impl::StringWrapper{*this}.wrapIntoLines(width, paragraphSpacing);
}

auto StringView::terminalLines(const int width) const noexcept -> int {
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

auto StringView::splitLines() const noexcept -> std::vector<StringView> {
    if (empty()) {
        return {};
    }
    auto result = std::vector<StringView>{};
    result.reserve(count(U'\n') + 1);
    auto lineStartIndex = std::size_t{0};
    while (lineStartIndex < size()) {
        const auto lineEndIndex = indexOf(U'\n', lineStartIndex);
        if (lineEndIndex == npos) {
            result.emplace_back(substr(lineStartIndex));
            break;
        }
        result.emplace_back(substr(lineStartIndex, lineEndIndex - lineStartIndex));
        lineStartIndex = lineEndIndex + 1;
    }
    return result;
}

auto StringView::characterAt(const std::size_t localIndex) const noexcept -> const Char & {
    return _data->chars()[storageIndex(localIndex)];
}

}
