// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "String.hpp"

#include "StringView.hpp"

#include "impl/StringData.hpp"
#include "impl/StringRangeView.hpp"
#include "impl/StringWrapper.hpp"

#include <algorithm>
#include <span>
#include <utility>

namespace erbsland::cterm {

String::String() noexcept : _data{impl::StringData::sharedEmpty()} {
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

auto String::operator==(const String &other) const noexcept -> bool {
    const auto view = impl::StringRangeView{*_data, _range};
    const auto otherView = impl::StringRangeView{*other._data, other._range};
    if (view.size() != otherView.size()) {
        return false;
    }
    for (std::size_t i = 0; i < view.size(); ++i) {
        if (view.characterAt(i) != otherView.characterAt(i)) {
            return false;
        }
    }
    return true;
}

auto String::operator[](const std::size_t index) const noexcept -> Char {
    return impl::StringRangeView{*_data, _range}[index];
}

auto String::operator[](const std::size_t index) noexcept -> Char & {
    if (index >= size()) {
        return ignoredMutableCharacter();
    }
    detach();
    _data->invalidateDisplayWidth();
    return _data->chars()[index];
}

auto String::operator+=(const String &other) noexcept -> String & {
    append(other);
    return *this;
}

auto String::operator+=(const StringView &other) noexcept -> String & {
    append(other);
    return *this;
}

auto String::operator+(const String &other) const noexcept -> String {
    auto copy = *this;
    copy.append(other);
    return copy;
}

auto String::operator+(const StringView &other) const noexcept -> String {
    auto copy = *this;
    copy.append(other);
    return copy;
}

auto String::displayWidth() const noexcept -> int {
    return impl::StringRangeView{*_data, _range}.displayWidth();
}

auto String::at(const std::size_t index) const -> Char {
    return impl::StringRangeView{*_data, _range}.at(index, "String");
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
    return impl::StringRangeView{*_data, _range}.begin();
}

auto String::end() const noexcept -> const_iterator {
    return impl::StringRangeView{*_data, _range}.end();
}

auto String::cbegin() const noexcept -> const_iterator {
    return impl::StringRangeView{*_data, _range}.cbegin();
}

auto String::cend() const noexcept -> const_iterator {
    return impl::StringRangeView{*_data, _range}.cend();
}

auto String::rbegin() noexcept -> reverse_iterator {
    return reverse_iterator{end()};
}

auto String::rend() noexcept -> reverse_iterator {
    return reverse_iterator{begin()};
}

auto String::rbegin() const noexcept -> const_reverse_iterator {
    return impl::StringRangeView{*_data, _range}.rbegin();
}

auto String::rend() const noexcept -> const_reverse_iterator {
    return impl::StringRangeView{*_data, _range}.rend();
}

auto String::crbegin() const noexcept -> const_reverse_iterator {
    return impl::StringRangeView{*_data, _range}.crbegin();
}

auto String::crend() const noexcept -> const_reverse_iterator {
    return impl::StringRangeView{*_data, _range}.crend();
}

auto String::count(const Char &character) const noexcept -> std::size_t {
    return impl::StringRangeView{*_data, _range}.count(character);
}

auto String::count(const char32_t character) const noexcept -> std::size_t {
    return impl::StringRangeView{*_data, _range}.count(character);
}

auto String::indexOf(const Char &character, const std::size_t startIndex) const noexcept -> std::size_t {
    return impl::StringRangeView{*_data, _range}.indexOf(character, startIndex);
}

auto String::indexOf(const char32_t character, const std::size_t startIndex) const noexcept -> std::size_t {
    return impl::StringRangeView{*_data, _range}.indexOf(character, startIndex);
}

auto String::indexOf(const std::u32string_view characterSet, std::size_t startIndex) const noexcept -> std::size_t {
    return impl::StringRangeView{*_data, _range}.indexOf(characterSet, startIndex);
}

auto String::indexNotOf(const std::u32string_view characterSet, std::size_t startIndex) const noexcept -> std::size_t {
    return impl::StringRangeView{*_data, _range}.indexNotOf(characterSet, startIndex);
}

auto String::substr(const std::size_t startIndex, std::size_t length) const noexcept -> String {
    const auto range = impl::StringRangeView{*_data, _range}.subRange(startIndex, length);
    if (range.empty()) {
        return {};
    }
    return String{_data, range};
}

auto String::croppedToDisplayWidth(const Coordinate displayWidth, const Alignment alignment) const noexcept -> String {
    const auto range = impl::StringRangeView{*_data, _range}.croppedRange(displayWidth, alignment);
    if (range.empty()) {
        return {};
    }
    return String{_data, range};
}

auto String::trimmed(std::u32string_view characters) const noexcept -> String {
    if (empty()) {
        return {};
    }
    auto copy = *this;
    copy.trim(characters);
    return copy;
}

auto String::normalized(const std::u32string_view characters, const Char separator) const noexcept -> String {
    if (empty()) {
        return {};
    }
    auto copy = *this;
    copy.normalize(characters, separator);
    return copy;
}

auto String::containsControlCharacters() const noexcept -> bool {
    return impl::StringRangeView{*_data, _range}.containsControlCharacters();
}

void String::reserve(const std::size_t size) noexcept {
    detach();
    _data->chars().reserve(size);
}

void String::clear() noexcept {
    _data = impl::StringData::sharedEmpty();
    _range = {};
}

void String::trim(std::u32string_view characters) noexcept {
    if (empty()) {
        return;
    }
    const auto range = impl::StringRangeView{*_data, _range}.trimmedRange(characters);
    if (range == _range) {
        return;
    }
    if (range.empty()) {
        clear();
        return;
    }
    _range = range;
}

void String::normalize(std::u32string_view characters, const Char separator) noexcept {
    if (empty()) {
        return;
    }
    if (characters.empty()) {
        characters = U" \n\t";
    }
    trim(characters);
    auto startPos = indexOf(characters);
    while (startPos != npos) {
        auto endPos = indexNotOf(characters, startPos + 1);
        if (endPos == npos) {
            endPos = size();
        }
        auto newSeparator = separator.withBase(at(startPos).style());
        if (endPos - startPos != 1 || at(startPos) != newSeparator) {
            replace(IndexRange{startPos, endPos - startPos}, newSeparator);
        }
        // Replacing the characters shortened the string.
        // Start the next search after the "not of" character previously found.
        startPos = indexOf(characters, startPos + 2);
    }
}

void String::replace(IndexRange range, const Char replacement) noexcept {
    if (empty() || range.empty() || range.startIndex() >= size()) {
        return;
    }
    if (range.endIndex() >= size()) {
        range = IndexRange{range.startIndex(), size() - range.startIndex()};
    }
    if (range.empty()) {
        return;
    }
    if (range.length() == 1 && !replacement.isEmpty()) { // fast path.
        if (this->at(range.startIndex()) != replacement) {
            detach();
            (*this)[range.startIndex()] = replacement;
        }
        return;
    }
    if (replacement.isEmpty()) { // empty char is like a remove operation.
        remove(range);
        return;
    }
    remove(IndexRange{range.startIndex() + 1, range.length() - 1});
    if (at(range.startIndex()) != replacement) {
        detach();
        _data->invalidateDisplayWidth();
        _data->chars()[range.startIndex()] = replacement;
    }
}

void String::replace(IndexRange range, const StringView &replacement) noexcept {
    if (empty() || range.empty() || range.startIndex() >= size()) {
        return;
    }
    if (range.endIndex() >= size()) {
        range = IndexRange{range.startIndex(), size() - range.startIndex()};
    }
    if (range.empty()) {
        return;
    }
    if (replacement.empty()) {
        remove(range);
        return;
    }
    auto replacementChars = std::span(replacement.cbegin(), replacement.cend());
    if (range.length() == replacementChars.size()) { // fast path.
        auto firstMismatch = range.length();
        for (std::size_t i = 0; i < range.length(); ++i) {
            if (at(range.startIndex() + i) != replacementChars[i]) {
                firstMismatch = i;
                break;
            }
        }
        if (firstMismatch == range.length()) {
            return;
        }
        detach();
        auto &chars = _data->chars();
        for (std::size_t i = firstMismatch; i < replacementChars.size(); ++i) {
            chars[range.startIndex() + i] = replacementChars[i];
        }
        _data->invalidateDisplayWidth();
        return;
    }
    detach();
    using Diff = impl::StringData::Storage::difference_type;
    auto &chars = _data->chars();
    if (range.length() > replacementChars.size()) {
        chars.erase(
            chars.begin() + static_cast<Diff>(range.startIndex() + replacementChars.size()),
            chars.begin() + static_cast<Diff>(range.endIndex()));
        for (std::size_t i = 0; i < replacementChars.size(); ++i) {
            chars[range.startIndex() + i] = replacementChars[i];
        }
    } else {
        const auto missingCharCount = replacementChars.size() - range.length();
        chars.insert(
            chars.begin() + static_cast<Diff>(range.startIndex()),
            replacementChars.begin(),
            replacementChars.begin() + static_cast<Diff>(missingCharCount));
        for (std::size_t i = missingCharCount; i < replacementChars.size(); ++i) {
            chars[range.startIndex() + i] = replacementChars[i];
        }
    }
    _data->invalidateDisplayWidth();
    syncRangeWithStorage();
}

void String::remove(IndexRange range) noexcept {
    if (empty() || range.empty() || range.startIndex() >= size()) {
        return;
    }
    if (range.endIndex() >= size()) { // fast path, string only needs trimming at the end.
        _range.setLength(range.startIndex());
        return;
    }
    detach();
    using Diff = impl::StringData::Storage::difference_type;
    _data->chars().erase(
        _data->chars().begin() + static_cast<Diff>(range.startIndex()),
        _data->chars().begin() + static_cast<Diff>(range.endIndex()));
    _data->invalidateDisplayWidth();
    syncRangeWithStorage();
}

void String::set(const std::size_t index, const Char character) noexcept {
    if (index >= size()) {
        return;
    }
    if (const auto oldChar = at(index); oldChar != character) {
        detach();
        _data->chars()[index] = character;
        if (oldChar.displayWidth() != character.displayWidth()) {
            _data->invalidateDisplayWidth();
        }
    }
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

void String::appendStyled(const StringView &other, CharStyle style) noexcept {
    appendViewWithBaseStyle(other, style);
}

void String::append(const std::size_t count, const Char character) noexcept {
    if (count == 0) {
        return;
    }
    const auto limitedCount = std::min(count, static_cast<std::size_t>(10'000'000U));
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

auto String::withBase(const CharStyle style) const noexcept -> String {
    // This method does not detach, if the base style keeps the original string intact.
    if (style == CharStyle{}) { // fast-path
        return *this;
    }
    auto result = *this;
    // here we first compare and only assign actual changed characters.
    for (std::size_t i = 0; i < result.size(); ++i) {
        const auto character = result.at(i);
        const auto styledCharacter = character.withBase(style);
        if (character != styledCharacter) {
            result[i] = styledCharacter;
        }
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
    return impl::StringRangeView{*_data, _range}.characterAt(index);
}

auto String::ignoredMutableCharacter() noexcept -> Char & {
    static thread_local auto ignored = Char{};
    ignored = Char{};
    return ignored;
}

}
