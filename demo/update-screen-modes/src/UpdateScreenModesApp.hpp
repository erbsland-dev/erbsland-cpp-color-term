// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "UpdateScreenModesState.hpp"

#include <erbsland/cterm/all.hpp>


namespace demo::updatescreenmodes {


using namespace erbsland::cterm;


/// Demonstrate `Terminal::updateScreen()` with clear, overwrite, and smart back-buffer updates.
class UpdateScreenModesApp final {
public:
    /// Run the demo until the user quits.
    void run();

private:
    [[nodiscard]] auto canvasSize() const noexcept -> Size;
    void handleKey(const Key &key) noexcept;
    void updateAnimation() noexcept;
    void applyTerminalSettings() noexcept;
    void renderFrame();
    void drawScene(Rectangle contentRect) noexcept;
    void drawFooter(Rectangle footerRect);
    static void appendStateLine(String &text, std::string_view label, bool enabled) noexcept;
    static void appendTimingLine(String &text, std::string_view label, double milliseconds) noexcept;
    [[nodiscard]] static auto matchesCharacterKey(const Key &key, char lowerCase) noexcept -> bool;

private:
    Terminal _terminal{Size{94, 28}};
    UpdateSettings _updateSettings;
    UpdateScreenModesState _state;
    FlushSpeedTracker _flushSpeedTracker;
    Buffer _buffer;
    std::size_t _animationStep{0};
    bool _quitRequested{false};
};


}
