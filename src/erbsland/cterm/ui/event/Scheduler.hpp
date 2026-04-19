// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Event.hpp"
#include "ScheduledActionRef.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace erbsland::cterm::ui {

class Scheduler;
using SchedulerPtr = std::unique_ptr<Scheduler>;
class Surface;
using SurfaceWeakPtr = std::weak_ptr<Surface>;

/// A scheduler for UI actions.
/// Scheduler management methods are thread-safe.
/// Scheduled callbacks are always executed on the application UI thread.
class Scheduler {
public:
    /// Duration type used for delays and repeat intervals.
    using milliseconds = std::chrono::milliseconds;
    /// Callback type executed on the application UI thread.
    using ActionFn = std::function<void()>;

private:
    friend class Application;

    /// The execution mode of a scheduled action.
    enum class Mode : uint8_t { SingleShot, Repeating };

    /// The stored state of a scheduled action.
    struct ActionEntry {
        ActionFn action;                ///< The callback to execute on the UI thread.
        Mode mode = Mode::SingleShot;   ///< The scheduling mode.
        milliseconds delayOrInterval{}; ///< The current delay or interval.
        EventTime nextExecutionTime{};  ///< The next scheduled execution time.
        uint64_t generation = 0;        ///< The current action generation.
    };

public:
    /// Maximum number of scheduled actions tracked at once.
    static constexpr auto cMaximumScheduledActions = std::size_t{1000};
    /// Minimum supported delay or repeat interval.
    static constexpr auto cMinimumInterval = milliseconds{10};
    /// Maximum supported delay or repeat interval.
    static constexpr auto cMaximumInterval = std::chrono::duration_cast<milliseconds>(std::chrono::hours{12});

public:
    /// Create a scheduler for one surface.
    /// @param surface The owning surface.
    explicit Scheduler(SurfaceWeakPtr surface);
    ~Scheduler() = default;

public:
    /// Schedules a single action with a delay.
    /// @param action The action to schedule.
    /// @param delay The delay before the action is executed.
    /// @return The reference of the scheduled action.
    auto addSingleShot(ActionFn &&action, milliseconds delay) -> ScheduledActionRef;
    /// Schedules a single action with a delay.
    /// This is either schedule a new action or move the execution time of an existing action.
    /// @param action The action to schedule.
    /// @param actionRef The reference of the action to reschedule if it exists.
    /// @param delay The delay before the action is executed.
    /// @return The reference of the scheduled action.
    auto addSingleShot(ActionFn &&action, ScheduledActionRef actionRef, milliseconds delay) -> ScheduledActionRef;
    /// Add a repeating action to the scheduler.
    /// @param action The action to schedule.
    /// @param interval The interval between executions.
    /// @return The reference of the scheduled action.
    auto addRepeated(ActionFn &&action, milliseconds interval) -> ScheduledActionRef;
    /// Change the interval or delay of a scheduled action.
    /// This resets the due time relative to the current time.
    /// @param actionRef The reference to the action. If the action does not exist, this call is ignored.
    /// @param delayOrInterval The new delay or interval for the action.
    void setDelayOrInterval(ScheduledActionRef actionRef, milliseconds delayOrInterval);
    /// Stop/Remove a scheduled action.
    /// If the given reference does not exist, this call is ignored.
    /// @param actionRef The reference of the action to remove.
    void remove(ScheduledActionRef actionRef);
    /// Remove all scheduled actions.
    void removeAll();

private:
    /// Execute one scheduled action from a scheduled action event.
    /// @param actionRef The action reference.
    /// @param generation The action generation carried by the scheduled action event.
    void executeAction(ScheduledActionRef actionRef, uint64_t generation);
    /// Arm all currently scheduled actions.
    void armAllActions();
    /// Test if the scheduler contains the given action generation.
    /// @param actionRef The action reference.
    /// @param generation The generation to check.
    /// @return `true` if the action still exists with the given generation.
    [[nodiscard]] auto hasGeneration(ScheduledActionRef actionRef, uint64_t generation) const -> bool;
    /// Create or replace an action entry.
    /// @param action The callback to schedule.
    /// @param actionRef The action reference to reuse, or an invalid reference to allocate a new one.
    /// @param mode The scheduling mode.
    /// @param delayOrInterval The delay or interval.
    /// @return The final action reference.
    auto scheduleAction(ActionFn &&action, ScheduledActionRef actionRef, Mode mode, milliseconds delayOrInterval)
        -> ScheduledActionRef;
    /// Schedule the given action in the global event scheduler.
    /// @param actionRef The action reference.
    /// @param generation The current action generation.
    /// @param eventTime The due time for the scheduled action event.
    void armAction(ScheduledActionRef actionRef, uint64_t generation, EventTime eventTime);
    /// Clamp a delay or interval into the supported range.
    /// @param delayOrInterval The requested delay or interval.
    /// @return The clamped delay or interval.
    [[nodiscard]] static auto clampDelayOrInterval(milliseconds delayOrInterval) -> milliseconds;
    /// Increment the generation counter.
    /// @param generation The current generation.
    /// @return The next generation.
    [[nodiscard]] static auto nextGeneration(uint64_t generation) -> uint64_t;
    /// Allocate the next action reference.
    /// @return The next action reference.
    [[nodiscard]] auto allocateActionRef() -> ScheduledActionRef;

private:
    mutable std::mutex _mutex;                                    ///< Protects the scheduled actions.
    SurfaceWeakPtr _surface;                                      ///< The owning surface.
    std::unordered_map<ScheduledActionRef, ActionEntry> _actions; ///< The scheduled actions.
    uint64_t _nextActionId = 1;                                   ///< The next action identifier.
};

}
