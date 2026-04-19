// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace demo::updatescreenmodes {

/// Stores the interactive state for the `update-screen-modes` demo.
class UpdateScreenModesState final {
public:
    /// Check whether overwrite refresh mode is enabled.
    [[nodiscard]] auto overwriteModeEnabled() const noexcept -> bool { return _overwriteModeEnabled; }
    /// Check whether line buffering is enabled.
    [[nodiscard]] auto lineBufferEnabled() const noexcept -> bool { return _lineBufferEnabled; }
    /// Check whether back-buffer rendering is enabled for the demo.
    [[nodiscard]] auto backBufferEnabled() const noexcept -> bool { return _backBufferEnabled; }
    /// Check whether the selected mode can actively use the back buffer.
    [[nodiscard]] auto backBufferActive() const noexcept -> bool { return _backBufferEnabled && _overwriteModeEnabled; }
    /// Check whether safe-margin rendering is enabled for the demo.
    [[nodiscard]] auto safeMarginEnabled() const noexcept -> bool { return _safeMarginEnabled; }
    /// Toggle overwrite refresh mode on or off.
    void toggleOverwriteMode() noexcept { _overwriteModeEnabled = !_overwriteModeEnabled; }
    /// Toggle line buffering on or off.
    void toggleLineBuffer() noexcept { _lineBufferEnabled = !_lineBufferEnabled; }
    /// Toggle the back-buffer feature on or off.
    void toggleBackBuffer() noexcept { _backBufferEnabled = !_backBufferEnabled; }
    /// Toggle the safe-margin feature on or off.
    void toggleSafeMargin() noexcept { _safeMarginEnabled = !_safeMarginEnabled; }
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
    bool _backBufferEnabled{true};
    bool _safeMarginEnabled{false};
};

}
