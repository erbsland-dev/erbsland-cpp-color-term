// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "EventData.hpp"
#include "EventType.hpp"

#include <chrono>
#include <memory>

namespace erbsland::cterm::ui {

using EventClock = std::chrono::steady_clock;
using EventTime = EventClock::time_point;

/// A single event.
class Event {
public:
    /// Create an event of a given type.
    explicit Event(const EventType type) noexcept : Event{type, {}} {}
    /// Create an event of a given type with optional data.
    Event(const EventType type, EventDataUniquePtr data) noexcept : _type{type}, _data{std::move(data)} {}
    /// Move constructor.
    Event(Event &&other) noexcept : _time{other._time}, _type{other._type}, _data{std::move(other._data)} {
        if (this == &other) {
            return;
        }
        other._type = EventType::NoEvent;
    }
    /// Move assignment operator.
    auto operator=(Event &&other) noexcept -> Event & {
        if (this == &other) {
            return *this;
        }
        _time = other._time;
        _type = other._type;
        _data = std::move(other._data);
        other._type = EventType::NoEvent;
        return *this;
    }

    // defaults
    ~Event() = default;
    Event(const Event &) = delete;
    auto operator=(const Event &) -> Event & = delete;

public:
    /// The event creation time.
    [[nodiscard]] auto time() const noexcept -> EventTime { return _time; }
    /// The event type.
    [[nodiscard]] auto type() const noexcept -> EventType { return _type; }
    /// Optional event data.
    [[nodiscard]] auto data() const noexcept -> const EventDataUniquePtr & { return _data; }

private:
    EventTime _time{EventClock::now()};  ///< Creation time for the event.
    EventType _type{EventType::NoEvent}; ///< The event type.
    EventDataUniquePtr _data{nullptr};   ///< Optional event data.
};

}
