// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "ActionTriggerContext.hpp"
#include "HelpData.hpp"

#include "event/Event.hpp"

#include "../Keys.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace erbsland::cterm::ui {

class Action;
using ActionPtr = std::shared_ptr<Action>;
using ActionConstPtr = std::shared_ptr<const Action>;

/// A shared user-interface action with key triggers, help metadata, and enablement state.
class Action {
public:
    /// When a callback-backed enabled state is refreshed automatically.
    enum class EnabledUpdateMode : uint8_t {
        Manual,          ///< Only update when explicitly requested or before triggering.
        BeforeTriggered, ///< Update before key/manual triggering.
        BeforeRepaint,   ///< Update before repaint-driven action help collection.
        Polled100ms,     ///< Update at most every 100ms when action help is refreshed.
    };

    /// Why the enabled callback is being refreshed.
    enum class EnabledRefreshReason : uint8_t {
        Manual,          ///< Explicit refresh.
        BeforeTriggered, ///< A trigger is about to be attempted.
        BeforeRepaint,   ///< A repaint is about to display the action.
        Polled100ms,     ///< The 100ms polling interval elapsed.
    };

    /// Callback used to update the enabled state.
    using EnabledFn = std::function<bool()>;
    /// Callback invoked when the action is triggered.
    using TriggerFn = std::function<void(const ActionTriggerContext &)>;
    /// Simple callback invoked when the action is triggered.
    using SimpleTriggerFn = std::function<void()>;

public:
    /// Create an action with a short help name.
    /// @param name The short action name used in generated help.
    explicit Action(std::string name);
    virtual ~Action() = default;

    // defaults
    Action(const Action &) = delete;
    Action(Action &&) = delete;
    auto operator=(const Action &) -> Action & = delete;
    auto operator=(Action &&) -> Action & = delete;

public:
    /// Create an action with a short help name.
    /// @param name The short action name used in generated help.
    /// @return The new action.
    [[nodiscard]] static auto create(std::string name) -> ActionPtr;

public:
    /// Get mutable help metadata.
    [[nodiscard]] auto help() noexcept -> HelpData & { return _help; }
    /// Get immutable help metadata.
    [[nodiscard]] auto help() const noexcept -> const HelpData & { return _help; }
    /// Test if this action is currently enabled.
    [[nodiscard]] auto isEnabled() const noexcept -> bool { return _enabled; }
    /// Manually set the enabled state.
    /// @param enabled The new enabled state.
    void setEnabled(bool enabled) noexcept;
    /// Replace the enabled callback and update mode.
    /// @param enabledFn The callback that returns the current enabled state.
    /// @param updateMode The automatic update mode for the callback.
    void setEnabledFn(EnabledFn enabledFn, EnabledUpdateMode updateMode = EnabledUpdateMode::BeforeTriggered);
    /// Remove the enabled callback and return to manual enabled state.
    void clearEnabledFn() noexcept;
    /// Get the enabled update mode.
    [[nodiscard]] auto enabledUpdateMode() const noexcept -> EnabledUpdateMode { return _enabledUpdateMode; }
    /// Replace the enabled update mode.
    /// @param updateMode The new update mode.
    void setEnabledUpdateMode(EnabledUpdateMode updateMode) noexcept;
    /// Get the trigger keys.
    [[nodiscard]] auto keys() const noexcept -> const Keys & { return _keys; }
    /// Replace all trigger keys.
    /// @param keys The new trigger keys.
    void setKeys(Keys keys);
    /// Replace the trigger callback.
    /// @param triggerFn The new callback.
    void setTriggerFn(TriggerFn triggerFn);
    /// Replace the trigger callback with a simple no-argument callback.
    /// @param triggerFn The new callback.
    void setTriggerFn(SimpleTriggerFn triggerFn);
    /// Remove the trigger callback.
    void clearTriggerFn() noexcept;

public:
    /// Test if the action is triggered by a key.
    /// @param key The key to test.
    /// @return `true` if the key is assigned to the action.
    [[nodiscard]] auto matchesKey(const Key &key) const noexcept -> bool;
    /// Refresh the enabled state if the callback and update mode require it.
    /// @param reason The reason for the refresh.
    /// @return `true` if the enabled callback was evaluated.
    auto refreshEnabled(EnabledRefreshReason reason) -> bool;
    /// Trigger this action manually.
    /// The enabled callback is always evaluated before triggering when present.
    /// @param context The trigger context.
    /// @return `true` if the action was enabled and was accepted as triggered.
    auto trigger(const ActionTriggerContext &context) -> bool;

protected:
    /// Called after the enabled state changed.
    /// @param enabled The new enabled state.
    virtual void onEnabledChanged(bool enabled) noexcept;

private:
    /// Replace the enabled state and notify subclasses when it changes.
    /// @param enabled The new enabled state.
    void setEnabledState(bool enabled) noexcept;
    /// Test if a 100ms polling refresh is due.
    [[nodiscard]] auto isPollingRefreshDue() const -> bool;

private:
    Keys _keys;                                                      ///< The trigger keys.
    HelpData _help;                                                  ///< Display help metadata.
    bool _enabled{true};                                             ///< The current enabled state.
    EnabledFn _enabledFn;                                            ///< Optional enabled callback.
    EnabledUpdateMode _enabledUpdateMode{EnabledUpdateMode::Manual}; ///< Enabled refresh mode.
    TriggerFn _triggerFn;                                            ///< Trigger callback.
    std::optional<EventTime> _lastPollingRefresh;                    ///< Last 100ms polling refresh.
};

}
