// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "ActionHelp.hpp"
#include "DynamicText.hpp"
#include "Label.hpp"
#include "Panel.hpp"

#include "../ExclusiveSurfaceManager.hpp"

#include <chrono>
#include <cstdint>
#include <deque>
#include <memory>
#include <string_view>

namespace erbsland::cterm::ui::surface {

class FooterLine;
using FooterLinePtr = std::shared_ptr<FooterLine>;

/// A themed footer line with dynamic text, action help, and queued overlay messages.
class FooterLine final : public Panel, protected ExclusiveSurfaceManager {
public:
    using milliseconds = std::chrono::milliseconds;

private:
    struct Message final {
        String text;            ///< The message text.
        CharStyle style;        ///< The message style.
        milliseconds timeout{}; ///< How long to display the message, or zero for manual hide.
    };

public:
    /// Create a footer line with fixed height `1`.
    explicit FooterLine(ProtectedTag) noexcept;
    ~FooterLine() override = default;

public:
    /// Create a footer line.
    /// @return The new footer line.
    [[nodiscard]] static auto create() -> FooterLinePtr;

public:
    /// Access the dynamic text surface on the left side.
    [[nodiscard]] auto leftText() const noexcept -> const DynamicTextPtr & { return _leftText; }
    /// Access the action-help surface on the right side.
    [[nodiscard]] auto actionHelp() const noexcept -> const ActionHelpPtr & { return _actionHelp; }
    /// Replace the static/dynamic left-side text.
    /// @param text The new text.
    void setText(String text);
    /// Replace the static/dynamic left-side text from UTF-8.
    /// @param text The new text.
    void setText(std::string_view text);
    /// Display a queued overlay message.
    /// A timeout of `0ms` keeps the message visible until `hideMessage()` is called.
    /// @param message The message text.
    /// @param style The message style.
    /// @param timeout The display timeout.
    void displayMessage(String message, CharStyle style = {}, milliseconds timeout = {});
    /// Display a queued overlay message from UTF-8 text.
    /// @param message The message text.
    /// @param style The message style.
    /// @param timeout The display timeout.
    void displayMessage(std::string_view message, CharStyle style = {}, milliseconds timeout = {});
    /// Hide the current overlay message and show the next queued message if present.
    void hideMessage();

public: // implement Surface
    void onLayout(LayoutScope &scope) noexcept override;

protected: // implement ExclusiveSurfaceManager
    [[nodiscard]] auto isManagedChild(const SurfacePtr &surface) const noexcept -> bool override;

private:
    /// Create and attach the owned child surfaces.
    void initializeUi() override;
    /// Show the next queued message.
    void showNextMessage();

private:
    DynamicTextPtr _leftText;          ///< The left-side dynamic text.
    ActionHelpPtr _actionHelp;         ///< The right-side action help.
    LabelPtr _messageText;             ///< The centered message overlay.
    std::deque<Message> _messageQueue; ///< Queued overlay messages.
    bool _hasMessage{false};           ///< Whether a message is currently visible.
    uint64_t _messageGeneration{0};    ///< Increments whenever the visible message changes.
};

}
