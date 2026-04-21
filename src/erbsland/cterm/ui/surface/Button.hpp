// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../ButtonAction.hpp"
#include "../Surface.hpp"

namespace erbsland::cterm::ui::surface {

class Button;
using ButtonPtr = std::shared_ptr<Button>;

/// A focusable action view rendered as a compact terminal button.
class Button final : public Surface {
public:
    /// Create a button for an action.
    /// @param action The action represented by the button.
    explicit Button(ButtonActionPtr action, ProtectedTag);
    ~Button() override = default;

public:
    /// Create a button for an action.
    /// @param action The action represented by the button.
    /// @return The new button.
    [[nodiscard]] static auto create(ButtonActionPtr action) -> ButtonPtr;

public:
    /// Access the represented action.
    [[nodiscard]] auto action() const noexcept -> const ButtonActionPtr &;
    /// Test if the represented action is enabled.
    [[nodiscard]] auto isEnabled() const noexcept -> bool;
    /// Calculate the button's current preferred size from its action metadata.
    [[nodiscard]] auto preferredSize() const -> Size;
    /// Refresh the preferred layout size from the current action metadata.
    void refreshLayoutSize();

public: // implement Surface
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;
    void onKeyPress(KeyPressEvent &keyPressEvent) noexcept override;

private:
    /// Render the button label with themed parts.
    [[nodiscard]] auto renderButton(const ThemeContext &themeContext) const -> String;
    /// Get the main display key, or an empty string if no key exists.
    [[nodiscard]] auto keyText() const -> String;
    /// Trigger the action from a keyboard event.
    auto triggerFromKey(const Key &key) -> bool;

private:
    static constexpr auto cHorizontalPadding = Coordinate{2};
    static constexpr auto cGapBeforeKey = Coordinate{2};

    ButtonActionPtr _action; ///< The action represented by this button.
};

}
