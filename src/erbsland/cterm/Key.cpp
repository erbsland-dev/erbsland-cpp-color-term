// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Key.hpp"

#include <array>
#include <cctype>
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


struct ConsoleSequenceDefinition final {
    std::string_view sequence;
    Key::Type type;
};


constexpr auto cKeyTextDefinitions = std::array<KeyTextDefinition, 27>{{
    {Key::Enter, "enter", "↵"},
    {Key::Tab, "tab", "tab"},
    {Key::Space, "space", "space"},
    {Key::Escape, "escape", "esc"},
    {Key::Backspace, "backspace", "⌫"},
    {Key::Insert, "insert", "ins"},
    {Key::Delete, "delete", "del"},
    {Key::Home, "home", "home"},
    {Key::End, "end", "end"},
    {Key::PageUp, "pageup", "pgup"},
    {Key::PageDown, "pagedown", "pgdn"},
    {Key::Left, "left", "←"},
    {Key::Right, "right", "→"},
    {Key::Up, "up", "↑"},
    {Key::Down, "down", "↓"},
    {Key::F1, "f1", "F1"},
    {Key::F2, "f2", "F2"},
    {Key::F3, "f3", "F3"},
    {Key::F4, "f4", "F4"},
    {Key::F5, "f5", "F5"},
    {Key::F6, "f6", "F6"},
    {Key::F7, "f7", "F7"},
    {Key::F8, "f8", "F8"},
    {Key::F9, "f9", "F9"},
    {Key::F10, "f10", "F10"},
    {Key::F11, "f11", "F11"},
    {Key::F12, "f12", "F12"},
}};


constexpr auto cKeyAliasDefinitions = std::array<KeyAliasDefinition, 36>{{
    {"enter", Key::Enter},
    {"return", Key::Enter},
    {"tab", Key::Tab},
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


constexpr auto cConsoleSequenceDefinitions = std::array<ConsoleSequenceDefinition, 31>{{
    {"\n", Key::Enter},
    {"\r", Key::Enter},
    {"\t", Key::Tab},
    {" ", Key::Space},
    {"\x1b", Key::Escape},
    {"\x08", Key::Backspace},
    {"\x7f", Key::Backspace},
    {"\x1b[A", Key::Up},
    {"\x1b[B", Key::Down},
    {"\x1b[C", Key::Right},
    {"\x1b[D", Key::Left},
    {"\x1b[H", Key::Home},
    {"\x1b[F", Key::End},
    {"\x1bOH", Key::Home},
    {"\x1bOF", Key::End},
    {"\x1b[2~", Key::Insert},
    {"\x1b[3~", Key::Delete},
    {"\x1b[5~", Key::PageUp},
    {"\x1b[6~", Key::PageDown},
    {"\x1bOP", Key::F1},
    {"\x1bOQ", Key::F2},
    {"\x1bOR", Key::F3},
    {"\x1bOS", Key::F4},
    {"\x1b[15~", Key::F5},
    {"\x1b[17~", Key::F6},
    {"\x1b[18~", Key::F7},
    {"\x1b[19~", Key::F8},
    {"\x1b[20~", Key::F9},
    {"\x1b[21~", Key::F10},
    {"\x1b[23~", Key::F11},
    {"\x1b[24~", Key::F12},
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


} // namespace


Key::Key(const Type type, const char ch) noexcept : _type{type}, _ch{ch} {
}

auto Key::fromString(std::string text) noexcept -> Key {
    text = normalizeKeyText(std::move(text));
    if (text.size() == 1 && std::isalnum(static_cast<unsigned char>(text[0]))) {
        return {Character, text[0]};
    }
    for (const auto &definition : cKeyAliasDefinitions) {
        if (definition.text == text) {
            return {definition.type};
        }
    }
    return {None};
}

auto Key::fromConsoleInput(const std::string &text) noexcept -> Key {
    for (const auto &definition : cConsoleSequenceDefinitions) {
        if (definition.sequence == text) {
            return {definition.type};
        }
    }
    if (text.size() == 1 && std::isalnum(static_cast<unsigned char>(text[0]))) {
        return {Character, text[0]};
    }
    return {None};
}

auto Key::toString() const -> std::string {
    if (_type == Character) {
        return std::string(1, _ch);
    }
    if (const auto *definition = findKeyTextDefinition(_type)) {
        return std::string{definition->text};
    }
    return {};
}

auto Key::toDisplayText(const bool useBrackets) const -> std::string {
    if (_type == Character) {
        return wrapDisplayText(std::string_view{&_ch, 1}, useBrackets);
    }
    if (const auto *definition = findKeyTextDefinition(_type)) {
        return wrapDisplayText(definition->displayText, useBrackets);
    }
    return {};
}


}
