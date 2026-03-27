// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <chrono>

namespace demo::updatescreenmodes {

/// Tracks the average duration of repeated `Terminal::updateScreen()` calls.
class FlushSpeedTracker final {
public:
    /// Add one measured flush duration to the running average.
    /// @param duration The measured duration for one `updateScreen()` call.
    void addSample(std::chrono::nanoseconds duration) noexcept {
        _lastDuration = duration;
        _totalDuration += duration;
        _sampleCount += 1;
    }
    /// Get the number of recorded flush measurements.
    [[nodiscard]] auto sampleCount() const noexcept -> std::size_t { return _sampleCount; }
    /// Get the most recently recorded flush duration.
    /// @return The last duration, or zero if no samples were recorded yet.
    [[nodiscard]] auto lastDuration() const noexcept -> std::chrono::nanoseconds { return _lastDuration; }
    /// Get the average recorded flush duration.
    /// @return The average duration, or zero if no samples were recorded yet.
    [[nodiscard]] auto averageDuration() const noexcept -> std::chrono::nanoseconds {
        if (_sampleCount == 0) {
            return {};
        }
        return _totalDuration / static_cast<int64_t>(_sampleCount);
    }
    /// Get the most recently recorded flush duration in milliseconds.
    /// @return The last duration as milliseconds.
    [[nodiscard]] auto lastMilliseconds() const noexcept -> double {
        return std::chrono::duration<double, std::milli>{lastDuration()}.count();
    }
    /// Get the average recorded flush duration in milliseconds.
    /// @return The average duration as milliseconds.
    [[nodiscard]] auto averageMilliseconds() const noexcept -> double {
        return std::chrono::duration<double, std::milli>{averageDuration()}.count();
    }

private:
    std::chrono::nanoseconds _lastDuration{};
    std::chrono::nanoseconds _totalDuration{};
    std::size_t _sampleCount{0};
};

}
