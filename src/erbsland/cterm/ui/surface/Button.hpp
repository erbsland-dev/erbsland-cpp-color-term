// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../ButtonAction.hpp"
#include "../Surface.hpp"

#include "../../theme/StringWithMargins.hpp"

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

public: // implement Surface
    [[nodiscard]] auto onMeasure(MeasureScope &scope, const LayoutProposal &proposal) noexcept
        -> LayoutMetrics override;
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;
    void onKeyPress(KeyPressEvent &keyPressEvent) noexcept override;

private:
    /// Initialize theme, focus state and action dispatch after construction.
    void initializeUi() override;
    /// Render the button label with themed parts.
    [[nodiscard]] auto renderButtonText(const theme::ThemeAccessor &themeAccessor) const -> theme::StringWithMargins;
    /// Trigger the action from a keyboard event.
    auto triggerFromKey(const Key &key) -> bool;

private:
    ButtonActionPtr _action; ///< The action represented by this button.
};

}
