// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "FlushSpeedTracker.hpp"
#include "TerminalApplication.hpp"
#include "UpdateScreenModesState.hpp"


namespace demo::updatescreenmodes {


using namespace erbsland::cterm;


/// Demonstrate `Terminal::updateScreen()` with clear, overwrite, and smart back-buffer updates.
class UpdateScreenModesApp final : public TerminalApplication {
public:
    /// Handle interactive refresh-mode toggle keys.
    void onKey(const Key &key) override;
    /// Render the current update-screen diagnostics into the shared demo buffer.
    void onRenderToBuffer() override;
    /// Record the measured `updateScreen()` duration from the shared render loop.
    void onAfterUpdateScreen(std::chrono::nanoseconds duration) override;
    /// Match the original refresh cadence of the diagnostics demo.
    [[nodiscard]] auto loopInterval() const noexcept -> std::chrono::milliseconds override {
        return std::chrono::milliseconds{70};
    }

private:
    void applyTerminalSettings() noexcept;
    void drawScene(Rectangle contentRect) noexcept;
    void drawFooter(Rectangle footerRect);
    static void appendStateLine(String &text, std::string_view label, bool enabled) noexcept;
    static void appendTimingLine(String &text, std::string_view label, double milliseconds) noexcept;

private:
    UpdateScreenModesState _state;
    FlushSpeedTracker _flushSpeedTracker;
};


}
