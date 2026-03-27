// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "event/KeyPressEvent.hpp"

#include <functional>
#include <memory>
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

private:
    std::unordered_map<Key, std::function<void()>> _bindings; ///< The actions for individual keys.
};

}
