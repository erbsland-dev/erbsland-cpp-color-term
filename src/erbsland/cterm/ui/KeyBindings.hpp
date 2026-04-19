// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "event/KeyPressEvent.hpp"

#include <functional>
#include <initializer_list>
#include <memory>
#include <string_view>
#include <unordered_map>

namespace erbsland::cterm::ui {

class KeyBindings;
using KeyBindingsPtr = std::unique_ptr<KeyBindings>;

/// Key-to-action bindings for surfaces and pages.
class KeyBindings {
public:
    KeyBindings() = default;
    ~KeyBindings() = default;

public:
    /// Handle the key press event.
    void onKeyPress(KeyPressEvent &keyPressEvent) noexcept;

    /// Bind a key to a function.
    /// Rebinding the same key replaces the previous action.
    /// Passing an empty action removes the existing binding.
    /// @param key The key to bind.
    /// @param action The action to invoke.
    void bind(Key key, std::function<void()> action);
    /// Bind one special key type to a function.
    /// Rebinding the same key replaces the previous action.
    /// Passing an empty action removes the existing binding.
    /// @param keyType The special key type to bind.
    /// @param action The action to invoke.
    /// @throws std::invalid_argument if `keyType` is `Key::None`, `Key::Character`, or `Key::Combined`.
    void bind(Key::Type keyType, std::function<void()> action);
    /// Bind one Unicode character key to a function.
    /// Rebinding the same key replaces the previous action.
    /// Passing an empty action removes the existing binding.
    /// @param character The Unicode character to bind.
    /// @param action The action to invoke.
    void bind(char32_t character, std::function<void()> action);
    /// Bind one Unicode text input to a function.
    /// A one-code-point input is stored as `Key::Character`; longer input uses `Key::Combined`.
    /// Rebinding the same key replaces the previous action.
    /// Passing an empty action removes the existing binding.
    /// @param text The Unicode text input to bind.
    /// @param action The action to invoke.
    /// @throws std::invalid_argument if `text` is empty.
    void bind(std::u32string_view text, std::function<void()> action);
    /// Bind multiple keys to one function.
    /// Every supplied key is updated independently.
    /// Passing an empty action removes all supplied bindings.
    /// @param keys The keys to bind.
    /// @param action The action to invoke.
    void bind(std::initializer_list<Key> keys, std::function<void()> action);
    /// Bind multiple special key types to one function.
    /// Every supplied key is updated independently.
    /// Passing an empty action removes all supplied bindings.
    /// @param keyTypes The special key types to bind.
    /// @param action The action to invoke.
    /// @throws std::invalid_argument if one key type is `Key::None`, `Key::Character`, or `Key::Combined`.
    void bind(std::initializer_list<Key::Type> keyTypes, std::function<void()> action);
    /// Bind multiple Unicode character keys to one function.
    /// Every supplied key is updated independently.
    /// Passing an empty action removes all supplied bindings.
    /// @param characters The Unicode characters to bind.
    /// @param action The action to invoke.
    void bind(std::initializer_list<char32_t> characters, std::function<void()> action);

private:
    /// Validate and convert one special key type into a bindable key.
    /// @param keyType The special key type to validate.
    /// @return The converted key.
    /// @throws std::invalid_argument if `keyType` is `Key::None`, `Key::Character`, or `Key::Combined`.
    [[nodiscard]] static auto keyFromType(Key::Type keyType) -> Key;
    /// Validate and convert one Unicode text input into a bindable key.
    /// @param text The Unicode text input.
    /// @return The converted key.
    /// @throws std::invalid_argument if `text` is empty.
    [[nodiscard]] static auto keyFromText(std::u32string_view text) -> Key;

    std::unordered_map<Key, std::function<void()>> _bindings; ///< The actions for individual keys.
};

}
