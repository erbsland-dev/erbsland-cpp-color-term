// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Action.hpp"
#include "ui_namespace.hpp"

#include <memory>
#include <string>

namespace erbsland::cterm::ui::surface {
class Button;
}

namespace erbsland::cterm::ui {

class ButtonAction;
using ButtonActionPtr = std::shared_ptr<ButtonAction>;

/// An action that is owned by one button surface and synchronizes its enabled state.
class ButtonAction : public Action {
    friend class surface::Button;

public:
    /// Create a button action with a short help name.
    /// @param name The short action name used in generated help.
    explicit ButtonAction(std::string name);
    ~ButtonAction() override = default;

    // defaults
    ButtonAction(const ButtonAction &) = delete;
    ButtonAction(ButtonAction &&) = delete;
    auto operator=(const ButtonAction &) -> ButtonAction & = delete;
    auto operator=(ButtonAction &&) -> ButtonAction & = delete;

public:
    /// Create a button action with a short help name.
    /// @param name The short action name used in generated help.
    /// @return The new button action.
    [[nodiscard]] static auto create(std::string name) -> ButtonActionPtr;

protected: // implement Action
    void onEnabledChanged(bool enabled) noexcept override;

private:
    /// Bind this action to its owning button.
    /// @param button The button using this action.
    void bindButton(const std::shared_ptr<surface::Button> &button);
    /// Synchronize the current enabled state to the bound button.
    void synchronizeButtonEnabled() noexcept;

private:
    std::weak_ptr<surface::Button> _button; ///< The button that owns this action.
};

}
