// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/ColorTermIncludes.hpp"

#include <chrono>

class UiSchedulerManualClock final {
public:
    using milliseconds = std::chrono::milliseconds;

public:
    [[nodiscard]] auto now() const noexcept -> ui::EventTime { return _now; }

    void advance(const ui::EventClock::duration delay) noexcept { _now += delay; }

    [[nodiscard]] auto elapsed() const noexcept -> milliseconds {
        return std::chrono::duration_cast<milliseconds>(_now.time_since_epoch());
    }

private:
    ui::EventTime _now{};
};
