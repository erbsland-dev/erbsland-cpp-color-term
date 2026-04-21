// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Action.hpp"

#include "event/impl/EventClockAccess.hpp"

#include <utility>

namespace erbsland::cterm::ui {

using namespace std::chrono_literals;

Action::Action(std::string name) : _help{std::move(name)} {
}

auto Action::create(std::string name) -> ActionPtr {
    return std::make_shared<Action>(std::move(name));
}

void Action::setEnabled(const bool enabled) noexcept {
    setEnabledState(enabled);
}

void Action::setEnabledFn(EnabledFn enabledFn, const EnabledUpdateMode updateMode) {
    _enabledFn = std::move(enabledFn);
    _enabledUpdateMode = updateMode;
    _lastPollingRefresh = std::nullopt;
}

void Action::clearEnabledFn() noexcept {
    _enabledFn = {};
    _enabledUpdateMode = EnabledUpdateMode::Manual;
    _lastPollingRefresh = std::nullopt;
}

void Action::setEnabledUpdateMode(const EnabledUpdateMode updateMode) noexcept {
    _enabledUpdateMode = updateMode;
    _lastPollingRefresh = std::nullopt;
}

void Action::setKeys(Keys keys) {
    _keys = std::move(keys);
}

void Action::setTriggerFn(TriggerFn triggerFn) {
    _triggerFn = std::move(triggerFn);
}

void Action::setTriggerFn(SimpleTriggerFn triggerFn) {
    if (!triggerFn) {
        _triggerFn = {};
        return;
    }
    _triggerFn = [triggerFn = std::move(triggerFn)]([[maybe_unused]] const ActionTriggerContext &context) -> void {
        triggerFn();
    };
}

void Action::clearTriggerFn() noexcept {
    _triggerFn = {};
}

auto Action::matchesKey(const Key &key) const noexcept -> bool {
    return _keys.matches(key);
}

auto Action::refreshEnabled(const EnabledRefreshReason reason) -> bool {
    if (!_enabledFn) {
        return false;
    }
    auto shouldRefresh = false;
    switch (reason) {
    case EnabledRefreshReason::Manual:
        shouldRefresh = true;
        break;
    case EnabledRefreshReason::BeforeTriggered:
        shouldRefresh = true;
        break;
    case EnabledRefreshReason::BeforeRepaint:
        shouldRefresh = _enabledUpdateMode == EnabledUpdateMode::BeforeRepaint;
        break;
    case EnabledRefreshReason::Polled100ms:
        shouldRefresh = _enabledUpdateMode == EnabledUpdateMode::Polled100ms && isPollingRefreshDue();
        break;
    }
    if (!shouldRefresh) {
        return false;
    }
    setEnabledState(_enabledFn());
    if (reason == EnabledRefreshReason::Polled100ms) {
        _lastPollingRefresh = impl::EventClockAccess::now();
    }
    return true;
}

auto Action::trigger(const ActionTriggerContext &context) -> bool {
    refreshEnabled(EnabledRefreshReason::BeforeTriggered);
    if (!_enabled) {
        return false;
    }
    if (_triggerFn) {
        _triggerFn(context);
    }
    return true;
}

void Action::onEnabledChanged([[maybe_unused]] const bool enabled) noexcept {
}

void Action::setEnabledState(const bool enabled) noexcept {
    if (_enabled == enabled) {
        return;
    }
    _enabled = enabled;
    onEnabledChanged(enabled);
}

auto Action::isPollingRefreshDue() const -> bool {
    if (!_lastPollingRefresh.has_value()) {
        return true;
    }
    return impl::EventClockAccess::now() - *_lastPollingRefresh >= 100ms;
}

}
