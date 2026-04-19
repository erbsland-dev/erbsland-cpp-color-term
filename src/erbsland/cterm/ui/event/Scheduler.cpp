// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Scheduler.hpp"

#include "impl/EventClockAccess.hpp"
#include "impl/ScheduledActionEvent.hpp"

#include "../Application.hpp"

#include <algorithm>
#include <tuple>
#include <utility>
#include <vector>

namespace erbsland::cterm::ui {

Scheduler::Scheduler(SurfaceWeakPtr surface) : _surface{std::move(surface)} {
}

auto Scheduler::addSingleShot(ActionFn &&action, const milliseconds delay) -> ScheduledActionRef {
    return scheduleAction(std::move(action), {}, Mode::SingleShot, delay);
}

auto Scheduler::addSingleShot(ActionFn &&action, ScheduledActionRef actionRef, const milliseconds delay)
    -> ScheduledActionRef {
    return scheduleAction(std::move(action), actionRef, Mode::SingleShot, delay);
}

auto Scheduler::addRepeated(ActionFn &&action, const milliseconds interval) -> ScheduledActionRef {
    return scheduleAction(std::move(action), {}, Mode::Repeating, interval);
}

void Scheduler::setDelayOrInterval(const ScheduledActionRef actionRef, const milliseconds delayOrInterval) {
    auto generationToArm = uint64_t{0};
    auto nextExecutionTime = EventTime{};
    {
        std::scoped_lock lock{_mutex};
        const auto it = _actions.find(actionRef);
        if (it == _actions.end()) {
            return;
        }
        it->second.delayOrInterval = clampDelayOrInterval(delayOrInterval);
        it->second.nextExecutionTime = impl::EventClockAccess::now() + it->second.delayOrInterval;
        it->second.generation = nextGeneration(it->second.generation);
        generationToArm = it->second.generation;
        nextExecutionTime = it->second.nextExecutionTime;
    }
    armAction(actionRef, generationToArm, nextExecutionTime);
}

void Scheduler::remove(const ScheduledActionRef actionRef) {
    std::scoped_lock lock{_mutex};
    _actions.erase(actionRef);
}

void Scheduler::removeAll() {
    std::scoped_lock lock{_mutex};
    _actions.clear();
}

void Scheduler::executeAction(const ScheduledActionRef actionRef, const uint64_t generation) {
    auto action = ActionFn{};
    auto mode = Mode::SingleShot;
    auto delayOrInterval = milliseconds{};
    auto nextExecutionTime = EventTime{};
    {
        std::scoped_lock lock{_mutex};
        const auto it = _actions.find(actionRef);
        if (it == _actions.end() || it->second.generation != generation) {
            return;
        }
        action = it->second.action;
        mode = it->second.mode;
        delayOrInterval = it->second.delayOrInterval;
        nextExecutionTime = it->second.nextExecutionTime;
    }
    action();
    if (mode == Mode::SingleShot) {
        std::scoped_lock lock{_mutex};
        if (const auto it = _actions.find(actionRef); it != _actions.end() && it->second.generation == generation) {
            _actions.erase(it);
        }
        return;
    }
    auto nextGenerationToArm = uint64_t{0};
    auto nextExecutionTimeToArm = EventTime{};
    {
        std::scoped_lock lock{_mutex};
        const auto it = _actions.find(actionRef);
        if (it == _actions.end() || it->second.generation != generation) {
            return;
        }
        it->second.nextExecutionTime = nextExecutionTime + delayOrInterval;
        it->second.generation = nextGeneration(it->second.generation);
        nextGenerationToArm = it->second.generation;
        nextExecutionTimeToArm = it->second.nextExecutionTime;
    }
    armAction(actionRef, nextGenerationToArm, nextExecutionTimeToArm);
}

void Scheduler::armAllActions() {
    auto actionsToArm = std::vector<std::tuple<ScheduledActionRef, uint64_t, EventTime>>{};
    {
        std::scoped_lock lock{_mutex};
        actionsToArm.reserve(_actions.size());
        for (const auto &[actionRef, action] : _actions) {
            actionsToArm.emplace_back(actionRef, action.generation, action.nextExecutionTime);
        }
    }
    for (const auto &[actionRef, generation, eventTime] : actionsToArm) {
        armAction(actionRef, generation, eventTime);
    }
}

auto Scheduler::hasGeneration(const ScheduledActionRef actionRef, const uint64_t generation) const -> bool {
    std::scoped_lock lock{_mutex};
    if (const auto it = _actions.find(actionRef); it != _actions.end()) {
        return it->second.generation == generation;
    }
    return false;
}

auto Scheduler::scheduleAction(
    ActionFn &&action, ScheduledActionRef actionRef, const Mode mode, const milliseconds delayOrInterval)
    -> ScheduledActionRef {
    auto generationToArm = uint64_t{0};
    auto nextExecutionTime = EventTime{};
    {
        std::scoped_lock lock{_mutex};
        if (!actionRef.isValid() || !_actions.contains(actionRef)) {
            if (_actions.size() >= cMaximumScheduledActions) {
                return {};
            }
            actionRef = allocateActionRef();
            _actions.emplace(actionRef, ActionEntry{});
        }
        auto &entry = _actions.at(actionRef);
        entry.action = std::move(action);
        entry.mode = mode;
        entry.delayOrInterval = clampDelayOrInterval(delayOrInterval);
        entry.nextExecutionTime = impl::EventClockAccess::now() + entry.delayOrInterval;
        entry.generation = nextGeneration(entry.generation);
        generationToArm = entry.generation;
        nextExecutionTime = entry.nextExecutionTime;
    }
    armAction(actionRef, generationToArm, nextExecutionTime);
    return actionRef;
}

void Scheduler::armAction(const ScheduledActionRef actionRef, const uint64_t generation, const EventTime eventTime) {
    auto *application = Application::instanceIfAvailable();
    if (application == nullptr || !hasGeneration(actionRef, generation)) {
        return;
    }
    application->scheduleAction(
        _surface,
        actionRef,
        generation,
        std::make_unique<impl::ScheduledActionEvent>(_surface, actionRef, generation),
        eventTime);
}

auto Scheduler::clampDelayOrInterval(milliseconds delayOrInterval) -> milliseconds {
    return std::clamp(delayOrInterval, cMinimumInterval, cMaximumInterval);
}

auto Scheduler::nextGeneration(const uint64_t generation) -> uint64_t {
    return generation + 1;
}

auto Scheduler::allocateActionRef() -> ScheduledActionRef {
    return ScheduledActionRef{_nextActionId++};
}

}
