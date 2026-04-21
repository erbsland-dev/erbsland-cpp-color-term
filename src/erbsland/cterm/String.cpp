// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "String.hpp"

#include "StringView.hpp"

#include "impl/StringData.hpp"
#include "impl/StringWrapper.hpp"

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <utility>

namespace erbsland::cterm {

String::String() noexcept : _data{impl::StringData::sharedEmpty()}, _range{} {
}

String::String(const std::string_view str, const EncodingErrors encodingErrors) :
    String{str, CharStyle{}, encodingErrors} {
}

String::String(const std::string_view str, const CharStyle style, const EncodingErrors encodingErrors) :
    String{splitCharacters(str, style.color(), style.attributes(), encodingErrors)} {
}

String::String(const std::u32string_view str) : String{str, CharStyle{}} {
}

String::String(const std::u32string_view str, const CharStyle style) :
    String{splitCharacters(str, style.color(), style.attributes())} {
}

String::String(const std::size_t count, const Char character) noexcept :
    String{Storage{std::min(count, static_cast<std::size_t>(10'000'000U)), character}} {
}

String::String(const StringView &view) : String{view._data->copyChars(view._range)} {
}

String::String(std::shared_ptr<impl::StringData> data, const IndexRange range) noexcept :
    _data{std::move(data)}, _range{range.clampedTo(_data->size())} {
}

String::String(Storage chars) noexcept :
    _data{chars.empty() ? impl::StringData::sharedEmpty() : std::make_shared<impl::StringData>(std::move(chars))},
    _range{0, _data->size()} {
}

auto String::operator[](const std::size_t index) const noexcept -> Char {
    return characterAt(index);
}

auto String::operator[](const std::size_t index) noexcept -> Char & {
    detach();
    _data->invalidateDisplayWidth();
    return _data->chars()[index];
}

auto String::operator+=(const String &other) noexcept -> String & {
    appendRange(other, 0, other.size());
    return *this;
}

auto String::operator+=(const StringView &other) noexcept -> String & {
    appendRange(other, 0, other.size());
    return *this;
}

auto String::operator+(const String &other) const noexcept -> String {
    auto copy = *this;
    copy += other;
    return copy;
}

auto String::operator+(const StringView &other) const noexcept -> String {
    auto copy = *this;
    copy += other;
    return copy;
}

auto String::displayWidth() const noexcept -> int {
    if (_range.startIndex() == 0 && _range.length() == _data->size() && _data->hasDisplayWidthCache()) {
        return _data->displayWidth();
    }
    auto result = 0;
    for (auto it = begin(); it != end(); ++it) {
        result += it->displayWidth();
    }
    return result;
}

auto String::at(const std::size_t index) const -> Char {
    if (index >= size()) {
        throw std::out_of_range{"String index out of range."};
    }
    return characterAt(index);
}

auto String::begin() noexcept -> iterator {
    detach();
    _data->invalidateDisplayWidth();
    return _data->chars().begin();
}

auto String::end() noexcept -> iterator {
    detach();
    _data->invalidateDisplayWidth();
    return _data->chars().end();
}

auto String::begin() const noexcept -> const_iterator {
    return _data->chars().cbegin() + static_cast<difference_type>(_range.startIndex());
}

auto String::end() const noexcept -> const_iterator {
    return begin() + static_cast<difference_type>(_range.length());
}

auto String::cbegin() const noexcept -> const_iterator {
    return begin();
}

auto String::cend() const noexcept -> const_iterator {
    return end();
}

auto String::rbegin() noexcept -> reverse_iterator {
    return reverse_iterator{end()};
}

auto String::rend() noexcept -> reverse_iterator {
    return reverse_iterator{begin()};
}

auto String::rbegin() const noexcept -> const_reverse_iterator {
    return const_reverse_iterator{end()};
}

auto String::rend() const noexcept -> const_reverse_iterator {
    return const_reverse_iterator{begin()};
}

auto String::crbegin() const noexcept -> const_reverse_iterator {
    return rbegin();
}

auto String::crend() const noexcept -> const_reverse_iterator {
    return rend();
}

auto String::count(const Char &character) const noexcept -> std::size_t {
    return static_cast<std::size_t>(
        std::ranges::count_if(*this, [&](const Char &candidate) -> bool { return candidate == character; }));
}

auto String::count(const char32_t character) const noexcept -> std::size_t {
    return static_cast<std::size_t>(std::ranges::count_if(
        *this, [&](const Char &candidate) -> bool { return candidate.singleCodePoint() == character; }));
}

auto String::indexOf(const Char &character, const std::size_t startIndex) const noexcept -> std::size_t {
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

auto String::indexOf(const char32_t character, const std::size_t startIndex) const noexcept -> std::size_t {
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

auto String::substr(const std::size_t startIndex, std::size_t length) const noexcept -> String {
    if (startIndex >= size() || length == 0) {
        return {};
    }
    if (length == npos || length > size() - startIndex) {
        length = size() - startIndex;
    }
    return String{_data, IndexRange{_range.startIndex() + startIndex, length}};
}

auto String::trimmed(const std::u32string_view characters) const noexcept -> String {
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

auto String::containsControlCharacters() const noexcept -> bool {
    return std::ranges::any_of(*this, [](const Char &character) -> bool { return character.isControl(); });
}

void String::reserve(const std::size_t size) noexcept {
    detach();
    _data->chars().reserve(size);
}

void String::clear() noexcept {
    _data = impl::StringData::sharedEmpty();
    _range = {};
}

void String::insertWithBaseStyle(std::size_t pos, const StringView &other, const CharStyle style) noexcept {
    if (other.empty()) {
        return;
    }
    detach();
    pos = std::min(pos, size());
    _data->chars().insert(_data->chars().begin() + static_cast<std::ptrdiff_t>(pos), other.size(), Char{});
    for (std::size_t i = 0; i < other.size(); ++i) {
        _data->chars()[pos + i] = other[i].withBase(style);
    }
    syncRangeWithStorage();
}

void String::appendStyled(std::string_view text, const CharStyle style) noexcept {
    detach();
    _data->appendCharacters(text, style.color(), style.attributes(), EncodingErrors::Replace);
    syncRangeWithStorage();
}

void String::appendStyled(const std::u32string_view text, const CharStyle style) noexcept {
    detach();
    _data->appendCharacters(text, style.color(), style.attributes());
    syncRangeWithStorage();
}

void String::appendWithBaseStyle(const StringView &other, CharStyle style) noexcept {
    appendViewWithBaseStyle(other, style);
}

void String::append(const std::size_t count, const Char character) noexcept {
    const auto limitedCount = std::min(count, static_cast<std::size_t>(10'000'000U));
    if (limitedCount == 0) {
        return;
    }
    detach();
    _data->chars().insert(_data->chars().end(), limitedCount, character);
    _data->invalidateDisplayWidth();
    syncRangeWithStorage();
}

void String::append(const std::size_t count, const char32_t character, const CharStyle style) noexcept {
    append(count, Char{character, style});
}

void String::appendRange(const StringView &other, const std::size_t startIndex, const std::size_t length) noexcept {
    appendView(other.substr(startIndex, length), CharStyle{});
}

void String::appendRangeWithBaseStyle(
    const StringView &other, const std::size_t startIndex, const std::size_t length, const CharStyle style) noexcept {
    appendViewWithBaseStyle(other.substr(startIndex, length), style);
}

auto String::splitWords() const noexcept -> std::vector<String> {
    auto words = std::vector<String>{};
    for (const auto &word : StringView{*this}.splitWords()) {
        words.push_back(String{word._data, word._range});
    }
    return words;
}

auto String::wrapIntoLines(const int width, const ParagraphSpacing paragraphSpacing) const noexcept
    -> std::vector<String> {
    return impl::StringWrapper{*this}.wrapIntoLines(width, paragraphSpacing);
}

auto String::terminalLines(const int width) const noexcept -> int {
    return StringView{*this}.terminalLines(width);
}

auto String::naturalTextSize() const noexcept -> Size {
    return StringView{*this}.naturalTextSize();
}

auto String::wrappedTextHeight(const Coordinate width, const TextOptions &options) const noexcept -> Coordinate {
    return StringView{*this}.wrappedTextHeight(width, options);
}

auto String::splitLines() const noexcept -> std::vector<String> {
    auto result = std::vector<String>{};
    for (const auto &line : StringView{*this}.splitLines()) {
        result.push_back(String{line._data, line._range});
    }
    return result;
}

auto String::fromLines(
    const std::initializer_list<std::string_view> lines, const Color color, const CharAttributes attributes) noexcept
    -> String {
    if (lines.size() == 0) {
        return {};
    }
    auto data = impl::StringData{};
    auto reservedCapacity = lines.size() - 1U; // count newlines
    for (auto &line : lines) {
        reservedCapacity +=
            static_cast<std::size_t>(impl::StringData::measureDisplayWidth(line, EncodingErrors::Replace));
    }
    data.reserve(reservedCapacity);
    bool isFirstLine = true;
    for (const auto &line : lines) {
        if (isFirstLine) {
            isFirstLine = false;
        } else {
            data.append(Char{U'\n', color, attributes});
        }
        data.appendCharacters(line, color, attributes, EncodingErrors::Replace);
    }
    return fromStorageWithDisplayWidth(std::move(data.chars()), data.displayWidth());
}

auto String::fromLines(const std::initializer_list<std::string_view> lines, const CharStyle style) noexcept -> String {
    return fromLines(lines, style.color(), style.attributes());
}

auto String::fromLines(
    const std::initializer_list<std::u32string_view> lines, const Color color, const CharAttributes attributes) noexcept
    -> String {
    if (lines.size() == 0) {
        return {};
    }
    auto data = impl::StringData{};
    auto reservedCapacity = lines.size() - 1U; // count newlines
    for (auto &line : lines) {
        reservedCapacity += static_cast<std::size_t>(impl::StringData::measureDisplayWidth(line));
    }
    data.reserve(reservedCapacity);
    bool isFirstLine = true;
    for (const auto &line : lines) {
        if (isFirstLine) {
            isFirstLine = false;
        } else {
            data.append(Char{U'\n', color, attributes});
        }
        data.appendCharacters(line, color, attributes);
    }
    return fromStorageWithDisplayWidth(std::move(data.chars()), data.displayWidth());
}

auto String::fromLines(const std::initializer_list<std::u32string_view> lines, const CharStyle style) noexcept
    -> String {
    return fromLines(lines, style.color(), style.attributes());
}

auto String::splitCharacters(
    const std::string_view str, const Color color, const CharAttributes attributes, const EncodingErrors encodingErrors)
    -> Storage {
    auto data = impl::StringData{};
    data.reserve(static_cast<std::size_t>(impl::StringData::measureDisplayWidth(str, encodingErrors)));
    data.appendCharacters(str, color, attributes, encodingErrors);
    return std::move(data.chars());
}

auto String::splitCharacters(const std::u32string_view str, const Color color, const CharAttributes attributes)
    -> Storage {
    auto data = impl::StringData{};
    data.reserve(str.size());
    data.appendCharacters(str, color, attributes);
    return std::move(data.chars());
}

void String::appendElement(const Color color, CharStyle &style) noexcept {
    style.setColor(color);
}

void String::appendElement(const Foreground::Hue foreground, CharStyle &style) noexcept {
    style.setFg(foreground);
}

void String::appendElement(const Foreground foreground, CharStyle &style) noexcept {
    style.setFg(foreground);
}

void String::appendElement(const Background::Hue background, CharStyle &style) noexcept {
    style.setBg(background);
}

void String::appendElement(const Background background, CharStyle &style) noexcept {
    style.setBg(background);
}

void String::appendElement(const CharStyle overlayStyle, CharStyle &style) noexcept {
    style = style.withOverlay(overlayStyle);
}

void String::appendElement(const CharAttributes attributes, CharStyle &style) noexcept {
    style.setAttributes(attributes.withBase(style.attributes()));
}

void String::appendElement(const Char &character, CharStyle &style) noexcept {
    detach();
    _data->append(character.withBase(style));
    syncRangeWithStorage();
}

void String::appendElement(const String &other, CharStyle &style) noexcept {
    appendViewWithBaseStyle(other, style);
}

void String::appendElement(const StringView &other, CharStyle &style) noexcept {
    appendViewWithBaseStyle(other, style);
}

void String::appendElement(const std::u32string_view str, CharStyle &style) noexcept {
    appendStyled(str, style);
}

void String::appendElement(const std::string_view str, CharStyle &style) noexcept {
    appendStyled(str, style);
}

void String::appendView(const StringView &view, const CharStyle style) noexcept {
    static_cast<void>(style);
    if (view.empty()) {
        return;
    }
    detach();
    auto &chars = _data->chars();
    chars.insert(chars.end(), view.begin(), view.end());
    _data->addDisplayWidth(view.displayWidth());
    syncRangeWithStorage();
}

void String::appendViewWithBaseStyle(const StringView &view, const CharStyle style) noexcept {
    if (view.empty()) {
        return;
    }
    detach();
    for (const auto &character : view) {
        const auto resolved = character.withBase(style);
        _data->append(resolved);
    }
    syncRangeWithStorage();
}

void String::detach() {
    if (_range.startIndex() == 0 && _range.length() == _data->size() && _data.use_count() == 1) {
        return;
    }
    _data = std::make_shared<impl::StringData>(_data->copyChars(_range));
    _range = IndexRange{0, _data->size()};
}

auto String::fromStorageWithDisplayWidth(Storage chars, const int displayWidth) noexcept -> String {
    if (chars.empty()) {
        return {};
    }
    const auto size = chars.size();
    auto data = std::make_shared<impl::StringData>(std::move(chars), displayWidth);
    return String{std::move(data), IndexRange{0, size}};
}

void String::syncRangeWithStorage() noexcept {
    _range = IndexRange{0, _data->size()};
}

auto String::characterAt(const std::size_t index) const noexcept -> const Char & {
    return _data->chars()[_range.startIndex() + index];
}

}
