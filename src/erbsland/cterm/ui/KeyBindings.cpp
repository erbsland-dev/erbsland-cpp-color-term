// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "KeyBindings.hpp"

#include <stdexcept>

namespace erbsland::cterm::ui {

void KeyBindings::onKeyPress(KeyPressEvent &keyPressEvent) noexcept {
    if (keyPressEvent.isHandled()) {
        return;
    }
    const auto it = _bindings.find(keyPressEvent.key());
    if (it == _bindings.end()) {
        return;
    }
    it->second();
    keyPressEvent.setHandled();
}

void KeyBindings::bind(Key key, std::function<void()> action) {
    if (!action) {
        _bindings.erase(key);
        return;
    }
    _bindings[std::move(key)] = std::move(action);
}

void KeyBindings::bind(const Key::Type keyType, std::function<void()> action) {
    bind(keyFromType(keyType), std::move(action));
}

void KeyBindings::bind(const char32_t character, std::function<void()> action) {
    bind(Key{Key::Character, character}, std::move(action));
}

void KeyBindings::bind(const std::u32string_view text, std::function<void()> action) {
    bind(keyFromText(text), std::move(action));
}

void KeyBindings::bind(const std::initializer_list<Key> keys, std::function<void()> action) {
    for (const auto &key : keys) {
        bind(key, action);
    }
}

void KeyBindings::bind(const std::initializer_list<Key::Type> keyTypes, std::function<void()> action) {
    for (const auto keyType : keyTypes) {
        bind(keyFromType(keyType), action);
    }
}

void KeyBindings::bind(const std::initializer_list<char32_t> characters, std::function<void()> action) {
    for (const auto character : characters) {
        bind(Key{Key::Character, character}, action);
    }
}

auto KeyBindings::keyFromType(const Key::Type keyType) -> Key {
    switch (keyType) {
    case Key::None:
    case Key::Character:
    case Key::Combined:
        throw std::invalid_argument{"bind(Key::Type, ...) requires one special key type."};
    default:
        return Key{keyType};
    }
}

auto KeyBindings::keyFromText(const std::u32string_view text) -> Key {
    if (text.empty()) {
        throw std::invalid_argument{"bind(std::u32string_view, ...) requires non-empty text."};
    }
    if (text.size() == 1) {
        return Key{Key::Character, text.front()};
    }
    return Key{Key::Combined, text};
}

}
