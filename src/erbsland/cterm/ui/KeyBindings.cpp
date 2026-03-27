// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "KeyBindings.hpp"

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

}
