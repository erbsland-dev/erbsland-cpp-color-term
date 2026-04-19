// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StringBuilder.hpp"

#include "../StringView.hpp"

namespace erbsland::cterm::impl {

void StringBuilder::reserve(const std::size_t size) {
    _data.reserve(size);
}

void StringBuilder::clear() noexcept {
    _data.clear();
}

auto StringBuilder::empty() const noexcept -> bool {
    return _data.size() == 0;
}

auto StringBuilder::size() const noexcept -> std::size_t {
    return _data.size();
}

auto StringBuilder::displayWidth() const noexcept -> int {
    return _data.displayWidth();
}

void StringBuilder::append(const Char &character) noexcept {
    _data.append(character);
}

void StringBuilder::append(const StringView &view) noexcept {
    if (view.empty()) {
        return;
    }
    auto &chars = _data.chars();
    chars.reserve(chars.size() + view.size());
    chars.insert(chars.end(), view.begin(), view.end());
    _data.addDisplayWidth(view.displayWidth());
}

void StringBuilder::appendWithBaseStyle(const StringView &view, const CharStyle style) noexcept {
    if (view.empty()) {
        return;
    }
    auto &chars = _data.chars();
    chars.reserve(chars.size() + view.size());
    const auto color = style.color();
    const auto attributes = style.attributes();
    for (const auto &character : view) {
        const auto resolved = character.withBase(color, attributes);
        _data.append(resolved);
    }
}

void StringBuilder::appendStyled(const std::u32string_view text, const CharStyle style) noexcept {
    _data.appendCharacters(text, style.color(), style.attributes());
}

void StringBuilder::appendStyled(
    const std::string_view text, const CharStyle style, const EncodingErrors encodingErrors) noexcept {
    _data.appendCharacters(text, style.color(), style.attributes(), encodingErrors);
}

auto StringBuilder::toString() const -> String {
    return String::fromStorageWithDisplayWidth(_data.chars(), _data.displayWidth());
}

auto StringBuilder::takeString() -> String {
    const auto previousCapacity = _data.chars().capacity();
    const auto displayWidth = _data.displayWidth();
    auto chars = std::move(_data.chars());
    _data = StringData{};
    _data.reserve(previousCapacity);
    return String::fromStorageWithDisplayWidth(std::move(chars), displayWidth);
}

}
