// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Actions.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace erbsland::cterm::ui {

Actions::Actions(SurfaceWeakPtr owner) noexcept : _owner{std::move(owner)} {
}

void Actions::add(ActionPtr action) {
    if (action == nullptr) {
        throw std::invalid_argument{"Cannot add a null action."};
    }
    if (contains(action)) {
        throw std::invalid_argument{"Cannot add the same action twice to one action container."};
    }
    _actions.emplace_back(std::move(action));
}

void Actions::remove(const ActionPtr &action) noexcept {
    std::erase(_actions, action);
}

void Actions::clear() noexcept {
    _actions.clear();
}

auto Actions::contains(const ActionPtr &action) const noexcept -> bool {
    return std::ranges::find(_actions, action) != _actions.end();
}

void Actions::onKeyPress(KeyPressEvent &keyPressEvent) {
    if (keyPressEvent.isHandled()) {
        return;
    }
    for (const auto &action : _actions) {
        if (action == nullptr || !action->matchesKey(keyPressEvent.key())) {
            continue;
        }
        const auto context = ActionTriggerContext{keyPressEvent.key(), _owner};
        if (action->trigger(context)) {
            keyPressEvent.setHandled();
            return;
        }
    }
}

void Actions::refreshForRepaint() {
    for (const auto &action : _actions) {
        if (action == nullptr) {
            continue;
        }
        action->refreshEnabled(Action::EnabledRefreshReason::BeforeRepaint);
        action->refreshEnabled(Action::EnabledRefreshReason::Polled100ms);
    }
}

}
