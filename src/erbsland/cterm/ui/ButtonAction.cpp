// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ButtonAction.hpp"

#include "surface/Button.hpp"

#include <stdexcept>
#include <utility>

namespace erbsland::cterm::ui {

ButtonAction::ButtonAction(std::string name) : Action{std::move(name)} {
}

auto ButtonAction::create(std::string name) -> ButtonActionPtr {
    return std::make_shared<ButtonAction>(std::move(name));
}

void ButtonAction::onEnabledChanged(const bool enabled) noexcept {
    Action::onEnabledChanged(enabled);
    synchronizeButtonEnabled();
}

void ButtonAction::bindButton(const std::shared_ptr<surface::Button> &button) {
    if (button == nullptr) {
        throw std::invalid_argument{"Cannot bind a button action to a null button."};
    }
    if (const auto existingButton = _button.lock(); existingButton != nullptr && existingButton != button) {
        throw std::invalid_argument{"ButtonAction instances can only be bound to one live button."};
    }
    _button = button;
    synchronizeButtonEnabled();
}

void ButtonAction::synchronizeButtonEnabled() noexcept {
    if (const auto button = _button.lock(); button != nullptr) {
        button->flags().setEnabled(isEnabled());
    }
}

}
