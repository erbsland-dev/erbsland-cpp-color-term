// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StringView.hpp"

#include "TextOptions.hpp"

#include "impl/paragraph/Layout.hpp"
#include "impl/paragraph/LayoutNewlineMode.hpp"
#include "impl/StringData.hpp"
#include "impl/StringRangeView.hpp"
#include "impl/StringWrapper.hpp"

#include <algorithm>
#include <ranges>

namespace erbsland::cterm {

StringView::StringView() noexcept : _data{impl::StringData::sharedEmpty()} {
}

StringView::StringView(const String &string) noexcept : _data{string._data}, _range{string._range} {
}

StringView::StringView(std::shared_ptr<impl::StringData> data, const IndexRange range) noexcept :
    _data{std::move(data)}, _range{range.clampedTo(_data->size())} {
}

auto StringView::operator[](const std::size_t index) const noexcept -> Char {
    return impl::StringRangeView{*_data, _range}[index];
}

auto StringView::displayWidth() const noexcept -> int {
    return impl::StringRangeView{*_data, _range}.displayWidth();
}

auto StringView::at(const std::size_t index) const -> Char {
    return impl::StringRangeView{*_data, _range}.at(index, "StringView");
}

auto StringView::begin() const noexcept -> const_iterator {
    return impl::StringRangeView{*_data, _range}.begin();
}

auto StringView::end() const noexcept -> const_iterator {
    return impl::StringRangeView{*_data, _range}.end();
}

auto StringView::cbegin() const noexcept -> const_iterator {
    return impl::StringRangeView{*_data, _range}.cbegin();
}

auto StringView::cend() const noexcept -> const_iterator {
    return impl::StringRangeView{*_data, _range}.cend();
}

auto StringView::rbegin() const noexcept -> const_reverse_iterator {
    return impl::StringRangeView{*_data, _range}.rbegin();
}

auto StringView::rend() const noexcept -> const_reverse_iterator {
    return impl::StringRangeView{*_data, _range}.rend();
}

auto StringView::crbegin() const noexcept -> const_reverse_iterator {
    return impl::StringRangeView{*_data, _range}.crbegin();
}

auto StringView::crend() const noexcept -> const_reverse_iterator {
    return impl::StringRangeView{*_data, _range}.crend();
}

auto StringView::count(const Char &character) const noexcept -> std::size_t {
    return impl::StringRangeView{*_data, _range}.count(character);
}

auto StringView::count(const char32_t character) const noexcept -> std::size_t {
    return impl::StringRangeView{*_data, _range}.count(character);
}

auto StringView::indexOf(const Char &character, const std::size_t startIndex) const noexcept -> std::size_t {
    return impl::StringRangeView{*_data, _range}.indexOf(character, startIndex);
}

auto StringView::indexOf(const char32_t character, const std::size_t startIndex) const noexcept -> std::size_t {
    return impl::StringRangeView{*_data, _range}.indexOf(character, startIndex);
}

auto StringView::indexOf(std::u32string_view characterSet, std::size_t startIndex) const noexcept -> std::size_t {
    return impl::StringRangeView{*_data, _range}.indexOf(characterSet, startIndex);
}

auto StringView::indexNotOf(std::u32string_view characterSet, std::size_t startIndex) const noexcept -> std::size_t {
    return impl::StringRangeView{*_data, _range}.indexNotOf(characterSet, startIndex);
}

auto StringView::substr(const std::size_t startIndex, std::size_t length) const noexcept -> StringView {
    const auto range = impl::StringRangeView{*_data, _range}.subRange(startIndex, length);
    if (range.empty()) {
        return {};
    }
    return StringView{_data, range};
}

auto StringView::croppedToDisplayWidth(const Coordinate displayWidth, const Alignment alignment) const noexcept
    -> StringView {
    const auto range = impl::StringRangeView{*_data, _range}.croppedRange(displayWidth, alignment);
    if (range.empty()) {
        return {};
    }
    return StringView{_data, range};
}

auto StringView::trimmed(std::u32string_view characters) const noexcept -> StringView {
    const auto range = impl::StringRangeView{*_data, _range}.trimmedRange(characters);
    if (range.empty()) {
        return {};
    }
    return StringView{_data, range};
}

auto StringView::containsControlCharacters() const noexcept -> bool {
    return impl::StringRangeView{*_data, _range}.containsControlCharacters();
}

auto StringView::splitWords() const noexcept -> std::vector<StringView> {
    auto words = std::vector<StringView>{};
    for (const auto &range : impl::StringRangeView{*_data, _range}.splitWordRanges()) {
        words.emplace_back(StringView{_data, range});
    }
    return words;
}

auto StringView::wrapIntoLines(const int width, const ParagraphSpacing paragraphSpacing) const noexcept -> StringLines {
    return impl::StringWrapper{*this}.wrapIntoLines(width, paragraphSpacing);
}

auto StringView::terminalLines(const int width) const noexcept -> int {
    return impl::StringRangeView{*_data, _range}.terminalLines(width);
}

auto StringView::naturalTextSize() const noexcept -> Size {
    return impl::StringRangeView{*_data, _range}.naturalTextSize();
}

auto StringView::wrappedTextHeight(const Coordinate width, const TextOptions &options) const noexcept -> Coordinate {
    const auto margins = options.margins();
    const auto verticalMargins = margins.verticalExtent();
    if (options.font() != nullptr) {
        auto lineCount = Coordinate{0};
        const auto lineHeight = std::max(Coordinate{1}, (options.font()->height() + 1) / 2);
        for (const auto &line : splitLines()) {
            if (std::ranges::any_of(line, [&](const Char &character) -> bool {
                    return options.font()->glyph(character.charStr()) != nullptr;
                })) {
                lineCount += lineHeight;
            }
        }
        return std::max(lineCount, Coordinate{1}) + verticalMargins;
    }
    const auto contentWidth = std::max(width - margins.horizontalExtent(), Coordinate{1});
    const auto layout =
        impl::paragraph::Layout{
            *this, contentWidth, options.paragraphOptions(), impl::paragraph::LayoutNewlineMode::ParagraphBreak}
            .build();
    if (layout.valid()) {
        return std::max(static_cast<Coordinate>(layout.size()), Coordinate{1}) + verticalMargins;
    }
    if (options.onError() == ParagraphOnError::Empty) {
        return verticalMargins;
    }
    const auto fallbackLines = impl::StringWrapper{*this}.wrapIntoLines(contentWidth, options.paragraphSpacing());
    return std::max(static_cast<Coordinate>(fallbackLines.size()), Coordinate{1}) + verticalMargins;
}

auto StringView::splitLines() const noexcept -> std::vector<StringView> {
    auto result = std::vector<StringView>{};
    for (const auto &range : impl::StringRangeView{*_data, _range}.splitLineRanges()) {
        if (range.empty()) {
            result.emplace_back();
        } else {
            result.emplace_back(StringView{_data, range});
        }
    }
    return result;
}

auto StringView::withBase(const CharStyle style) const noexcept -> String {
    return String{*this}.withBase(style);
}

auto StringView::characterAt(const std::size_t localIndex) const noexcept -> const Char & {
    return impl::StringRangeView{*_data, _range}.characterAt(localIndex);
}

}
