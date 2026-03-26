// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Key.hpp"


#include "impl/CombinedChar.hpp"
#include "impl/KeyDecoder.hpp"

#include <array>
#include <cctype>
#include <optional>
#include <string_view>


namespace erbsland::cterm {

namespace {


struct KeyTextDefinition final {
    Key::Type type;
    std::string_view text;
    std::string_view displayText;
};


struct KeyAliasDefinition final {
    std::string_view text;
    Key::Type type;
};

constexpr auto cKeyTextDefinitions = std::array<KeyTextDefinition, 28>{{
    {Key::Enter, "enter", "↵"},     {Key::Tab, "tab", "tab"},        {Key::BackTab, "backtab", "⇤"},
    {Key::Space, "space", "space"}, {Key::Escape, "escape", "esc"},  {Key::Backspace, "backspace", "⌫"},
    {Key::Insert, "insert", "ins"}, {Key::Delete, "delete", "del"},  {Key::Home, "home", "home"},
    {Key::End, "end", "end"},       {Key::PageUp, "pageup", "pgup"}, {Key::PageDown, "pagedown", "pgdn"},
    {Key::Left, "left", "←"},       {Key::Right, "right", "→"},      {Key::Up, "up", "↑"},
    {Key::Down, "down", "↓"},       {Key::F1, "f1", "F1"},           {Key::F2, "f2", "F2"},
    {Key::F3, "f3", "F3"},          {Key::F4, "f4", "F4"},           {Key::F5, "f5", "F5"},
    {Key::F6, "f6", "F6"},          {Key::F7, "f7", "F7"},           {Key::F8, "f8", "F8"},
    {Key::F9, "f9", "F9"},          {Key::F10, "f10", "F10"},        {Key::F11, "f11", "F11"},
    {Key::F12, "f12", "F12"},
}};


constexpr auto cKeyAliasDefinitions = std::array<KeyAliasDefinition, 39>{{
    {"enter", Key::Enter},
    {"return", Key::Enter},
    {"tab", Key::Tab},
    {"backtab", Key::BackTab},
    {"back_tab", Key::BackTab},
    {"shift_tab", Key::BackTab},
    {"space", Key::Space},
    {"escape", Key::Escape},
    {"esc", Key::Escape},
    {"backspace", Key::Backspace},
    {"insert", Key::Insert},
    {"ins", Key::Insert},
    {"delete", Key::Delete},
    {"del", Key::Delete},
    {"home", Key::Home},
    {"end", Key::End},
    {"pageup", Key::PageUp},
    {"page_up", Key::PageUp},
    {"pgup", Key::PageUp},
    {"pagedown", Key::PageDown},
    {"page_down", Key::PageDown},
    {"pgdown", Key::PageDown},
    {"pgdn", Key::PageDown},
    {"left", Key::Left},
    {"right", Key::Right},
    {"up", Key::Up},
    {"down", Key::Down},
    {"f1", Key::F1},
    {"f2", Key::F2},
    {"f3", Key::F3},
    {"f4", Key::F4},
    {"f5", Key::F5},
    {"f6", Key::F6},
    {"f7", Key::F7},
    {"f8", Key::F8},
    {"f9", Key::F9},
    {"f10", Key::F10},
    {"f11", Key::F11},
    {"f12", Key::F12},
}};


auto findKeyTextDefinition(const Key::Type type) noexcept -> const KeyTextDefinition * {
    for (const auto &definition : cKeyTextDefinitions) {
        if (definition.type == type) {
            return &definition;
        }
    }
    return nullptr;
}


auto normalizeKeyText(std::string text) noexcept -> std::string {
    for (auto &character : text) {
        character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
    }
    return text;
}


auto wrapDisplayText(const std::string_view text, const bool useBrackets) -> std::string {
    if (!useBrackets) {
        return std::string{text};
    }
    auto result = std::string{};
    result.reserve(text.size() + 2);
    result += '[';
    result.append(text);
    result += ']';
    return result;
}


auto createCharacterKey(const impl::CombinedChar &character) noexcept -> Key {
    if (character.codePointCount() <= 1) {
        return {Key::Character, character.mainCodePoint()};
    }
    return {Key::Combined, character.utf32()};
}


auto parseCharacterKeyText(const std::string_view text) -> std::optional<Key> {
    try {
        if (const auto character = impl::CombinedChar::fromTextUtf8(text); character.has_value()) {
            return createCharacterKey(*character);
        }
    } catch (...) {
        return std::nullopt;
    }
    return std::nullopt;
}


} // namespace


Key::Key(const Type type, const char32_t codePoint) noexcept : _type{type} {
    if (type == Character || type == Combined) {
        _character = impl::CombinedChar{codePoint};
    }
}

Key::Key(const Type type, const std::u32string_view character) : _type{type} {
    if (type == Character || type == Combined) {
        _character = impl::CombinedChar{character};
    }
}

auto Key::character() const noexcept -> char {
    const auto codePoint = unicode();
    if (codePoint > 0x7fU) {
        return 0;
    }
    return static_cast<char>(codePoint);
}

auto Key::unicode() const noexcept -> char32_t {
    if (_type != Character || _character.codePointCount() != 1) {
        return 0;
    }
    return _character.mainCodePoint();
}

auto Key::combined() const -> std::u32string {
    if (_type != Character && _type != Combined) {
        return {};
    }
    return _character.utf32();
}

auto Key::fromString(std::string text) noexcept -> Key {
    const auto originalText = text;
    text = normalizeKeyText(std::move(text));
    for (const auto &definition : cKeyAliasDefinitions) {
        if (definition.text == text) {
            return {definition.type};
        }
    }
    if (const auto key = parseCharacterKeyText(originalText); key.has_value()) {
        return *key;
    }
    return {None};
}

auto Key::fromConsoleInput(const std::string &text) noexcept -> Key {
    return impl::KeyDecoder{text}.decodeConsoleInput();
}

auto Key::toString() const -> std::string {
    if (_type == Character || _type == Combined) {
        return _character.utf8();
    }
    if (const auto *definition = findKeyTextDefinition(_type)) {
        return std::string{definition->text};
    }
    return {};
}

auto Key::toDisplayText(const bool useBrackets) const -> std::string {
    if (_type == Character || _type == Combined) {
        return wrapDisplayText(_character.utf8(), useBrackets);
    }
    if (const auto *definition = findKeyTextDefinition(_type)) {
        return wrapDisplayText(definition->displayText, useBrackets);
    }
    return {};
}


}
