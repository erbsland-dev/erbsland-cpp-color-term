// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Key.hpp"

#include "impl/CombinedChar.hpp"
#include "impl/KeyDecoder.hpp"

#include <array>
#include <cctype>
#include <optional>
#include <string>
#include <string_view>

namespace erbsland::cterm {

auto Key::keyTextDefinitions() noexcept -> const std::array<KeyTextDefinition, 28> & {
    static constexpr auto cKeyTextDefinitions = std::array<KeyTextDefinition, 28>{{
        {Enter, "enter", "↵"},     {Tab, "tab", "tab"},        {BackTab, "backtab", "⇤"},
        {Space, "space", "space"}, {Escape, "escape", "esc"},  {Backspace, "backspace", "⌫"},
        {Insert, "insert", "ins"}, {Delete, "delete", "del"},  {Home, "home", "home"},
        {End, "end", "end"},       {PageUp, "pageup", "pgup"}, {PageDown, "pagedown", "pgdn"},
        {Left, "left", "←"},       {Right, "right", "→"},      {Up, "up", "↑"},
        {Down, "down", "↓"},       {F1, "f1", "F1"},           {F2, "f2", "F2"},
        {F3, "f3", "F3"},          {F4, "f4", "F4"},           {F5, "f5", "F5"},
        {F6, "f6", "F6"},          {F7, "f7", "F7"},           {F8, "f8", "F8"},
        {F9, "f9", "F9"},          {F10, "f10", "F10"},        {F11, "f11", "F11"},
        {F12, "f12", "F12"},
    }};
    return cKeyTextDefinitions;
}

auto Key::keyAliasDefinitions() noexcept -> const std::array<KeyAliasDefinition, 39> & {
    static constexpr auto cKeyAliasDefinitions = std::array<KeyAliasDefinition, 39>{{
        {"enter", Enter},
        {"return", Enter},
        {"tab", Tab},
        {"backtab", BackTab},
        {"back_tab", BackTab},
        {"shift_tab", BackTab},
        {"space", Space},
        {"escape", Escape},
        {"esc", Escape},
        {"backspace", Backspace},
        {"insert", Insert},
        {"ins", Insert},
        {"delete", Delete},
        {"del", Delete},
        {"home", Home},
        {"end", End},
        {"pageup", PageUp},
        {"page_up", PageUp},
        {"pgup", PageUp},
        {"pagedown", PageDown},
        {"page_down", PageDown},
        {"pgdown", PageDown},
        {"pgdn", PageDown},
        {"left", Left},
        {"right", Right},
        {"up", Up},
        {"down", Down},
        {"f1", F1},
        {"f2", F2},
        {"f3", F3},
        {"f4", F4},
        {"f5", F5},
        {"f6", F6},
        {"f7", F7},
        {"f8", F8},
        {"f9", F9},
        {"f10", F10},
        {"f11", F11},
        {"f12", F12},
    }};
    return cKeyAliasDefinitions;
}

auto Key::findKeyTextDefinition(const Type type) noexcept -> const KeyTextDefinition * {
    for (const auto &definition : keyTextDefinitions()) {
        if (definition.type == type) {
            return &definition;
        }
    }
    return nullptr;
}

auto Key::normalizeKeyText(std::string text) noexcept -> std::string {
    for (auto &character : text) {
        character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
    }
    return text;
}

auto Key::parseModifierText(const std::string_view text) noexcept -> std::optional<KeyModifier> {
    if (text == "shift") {
        return KeyModifier::Shift;
    }
    if (text == "ctrl" || text == "control") {
        return KeyModifier::Control;
    }
    if (text == "alt") {
        return KeyModifier::Alt;
    }
    return std::nullopt;
}

auto Key::parseModifiers(std::string &text) noexcept -> KeyModifiers {
    auto modifiers = KeyModifiers{};
    while (true) {
        const auto separator = text.find('+');
        if (separator == std::string::npos) {
            return modifiers;
        }
        const auto modifierText = normalizeKeyText(text.substr(0, separator));
        const auto modifier = parseModifierText(modifierText);
        if (!modifier.has_value()) {
            return modifiers;
        }
        modifiers.set(*modifier);
        text.erase(0, separator + 1);
    }
}

void Key::appendModifierString(std::string &text, const KeyModifiers modifiers) {
    if (modifiers.has(KeyModifier::Shift)) {
        text += "shift+";
    }
    if (modifiers.has(KeyModifier::Control)) {
        text += "ctrl+";
    }
    if (modifiers.has(KeyModifier::Alt)) {
        text += "alt+";
    }
}

void Key::appendModifierDisplayText(std::string &text, const KeyModifiers modifiers) {
    if (modifiers.has(KeyModifier::Shift)) {
        text += "Shift+";
    }
    if (modifiers.has(KeyModifier::Control)) {
        text += "Ctrl+";
    }
    if (modifiers.has(KeyModifier::Alt)) {
        text += "Alt+";
    }
}

auto Key::wrapDisplayText(const std::string_view text, const bool useBrackets) -> std::string {
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

auto Key::createCharacterKey(const impl::CombinedChar &character) noexcept -> Key {
    if (character.codePointCount() <= 1) {
        return Key{Character, character.mainCodePoint()};
    }
    return Key{Combined, character.utf32()};
}

auto Key::parseCharacterKeyText(const std::string_view text) -> std::optional<Key> {
    if (text.empty()) {
        return std::nullopt;
    }
    if (const auto character = impl::CombinedChar::fromTextUtf8(text); character.has_value()) {
        return createCharacterKey(*character);
    }
    return std::nullopt;
}

Key::Key(const Type type, const char32_t codePoint, const KeyModifiers modifiers) noexcept :
    _type{type}, _modifiers{modifiers} {
    if (type == Character || type == Combined) {
        _character = impl::CombinedChar{codePoint};
    }
}

Key::Key(const Type type, const KeyModifiers modifiers) noexcept : Key{type, char32_t{0}, modifiers} {
}

Key::Key(const char32_t codePoint, const KeyModifiers modifiers) noexcept : Key{Character, codePoint, modifiers} {
}

Key::Key(const Type type, const std::u32string_view character, const KeyModifiers modifiers) :
    _type{type}, _modifiers{modifiers} {
    if (type == Character || type == Combined) {
        _character = impl::CombinedChar{character};
    }
}

auto Key::operator==(const char32_t other) const noexcept -> bool {
    return _type == Character && _modifiers.empty() && _character.mainCodePoint() == other;
}

auto Key::operator!=(const char32_t other) const noexcept -> bool {
    return !operator==(other);
}

auto Key::operator==(const std::u32string_view other) const noexcept -> bool {
    return _type == Combined && _modifiers.empty() && combined() == other;
}

auto Key::operator!=(const std::u32string_view other) const noexcept -> bool {
    return !operator==(other);
}

auto Key::operator==(const Type type) const noexcept -> bool {
    if (type == Character || type == Combined) {
        return false;
    }
    return _type == type && _modifiers.empty();
}

auto Key::operator!=(const Type type) const noexcept -> bool {
    return !operator==(type);
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

auto Key::withoutModifiers() const noexcept -> Key {
    auto result = *this;
    result._modifiers = {};
    return result;
}

auto Key::fromString(std::string text) noexcept -> Key {
    auto originalText = text;
    const auto modifiers = parseModifiers(text);
    originalText = text;
    text = normalizeKeyText(std::move(text));
    for (const auto &definition : keyAliasDefinitions()) {
        if (definition.text == text) {
            return Key{definition.type, modifiers};
        }
    }
    if (!modifiers.empty()) {
        return Key{None};
    }
    if (const auto key = parseCharacterKeyText(originalText); key.has_value()) {
        return *key;
    }
    return Key{None};
}

auto Key::fromConsoleInput(const std::string &text) noexcept -> Key {
    return impl::KeyDecoder{text}.decodeConsoleInput();
}

auto Key::toString() const -> std::string {
    auto result = std::string{};
    appendModifierString(result, _modifiers);
    if (_type == Character || _type == Combined) {
        result += _character.utf8();
        return result;
    }
    if (const auto *definition = findKeyTextDefinition(_type)) {
        result += definition->text;
        return result;
    }
    return {};
}

auto Key::toDisplayText(const bool useBrackets) const -> std::string {
    auto result = std::string{};
    appendModifierDisplayText(result, _modifiers);
    if (_type == Character || _type == Combined) {
        result += _character.utf8();
        return wrapDisplayText(result, useBrackets);
    }
    if (const auto *definition = findKeyTextDefinition(_type)) {
        result += definition->displayText;
        return wrapDisplayText(result, useBrackets);
    }
    return {};
}

}
