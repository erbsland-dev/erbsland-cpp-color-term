// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>


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


/// Stores the interactive state for the `update-screen-modes` demo.
class UpdateScreenModesState final {
public:
    /// Check whether overwrite refresh mode is enabled.
    [[nodiscard]] auto overwriteModeEnabled() const noexcept -> bool { return _overwriteModeEnabled; }

    /// Check whether line buffering is enabled.
    [[nodiscard]] auto lineBufferEnabled() const noexcept -> bool { return _lineBufferEnabled; }

    /// Check whether the terminal safe margin is enabled.
    [[nodiscard]] auto safeMarginEnabled() const noexcept -> bool { return _safeMarginEnabled; }

    /// Check whether back-buffer rendering is enabled for the demo.
    [[nodiscard]] auto backBufferEnabled() const noexcept -> bool { return _backBufferEnabled; }

    /// Check whether the selected mode can actively use the back buffer.
    [[nodiscard]] auto backBufferActive() const noexcept -> bool { return _backBufferEnabled && _overwriteModeEnabled; }

    /// Toggle overwrite refresh mode on or off.
    void toggleOverwriteMode() noexcept { _overwriteModeEnabled = !_overwriteModeEnabled; }

    /// Toggle line buffering on or off.
    void toggleLineBuffer() noexcept { _lineBufferEnabled = !_lineBufferEnabled; }

    /// Toggle the terminal safe margin on or off.
    void toggleSafeMargin() noexcept { _safeMarginEnabled = !_safeMarginEnabled; }

    /// Toggle the back-buffer feature on or off.
    void toggleBackBuffer() noexcept { _backBufferEnabled = !_backBufferEnabled; }

    /// Get the title for the current screen refresh mode.
    [[nodiscard]] auto modeTitle() const noexcept -> std::string_view {
        return _overwriteModeEnabled ? "Overwrite" : "Clear";
    }

    /// Get the description for the current screen refresh mode.
    [[nodiscard]] auto modeDescription() const noexcept -> std::string_view {
        if (_overwriteModeEnabled) {
            return "Each frame moves the cursor back to the top-left corner and redraws the visible content. This "
                   "avoids the explicit clear and allows back-buffer patches.";
        }
        return "Each frame clears the full screen before rendering the scene again. This is the simplest and "
               "most robust refresh path.";
    }

private:
    bool _overwriteModeEnabled{true};
    bool _lineBufferEnabled{true};
    bool _safeMarginEnabled{true};
    bool _backBufferEnabled{true};
};


}
