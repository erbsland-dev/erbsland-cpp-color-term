// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <erbsland/cterm/all.hpp>

#include <chrono>


namespace demo::updatescreenmodes {


using namespace erbsland::cterm;


/// Demonstrate `Terminal::updateScreen()` with clear, overwrite, and smart back-buffer updates.
class UpdateScreenModesApp final {
public:
    /// Run the demo until the user quits.
    void run();

private:
    enum class Mode : uint8_t {
        Clear,
        Overwrite,
        OverwriteWithBackBuffer,
    };

private:
    [[nodiscard]] auto canvasSize() const noexcept -> Size;
    void handleKey(const Key &key) noexcept;
    void updateAnimation(std::chrono::milliseconds elapsed) noexcept;
    void advanceMode(int delta) noexcept;
    void applyMode() noexcept;
    void renderFrame();
    void drawScene(Buffer &buffer, Rectangle contentRect) const noexcept;
    void drawFooter(Buffer &buffer, Rectangle footerRect) const;
    [[nodiscard]] auto modeTitle() const noexcept -> std::string_view;
    [[nodiscard]] auto modeDescription() const noexcept -> std::string_view;

private:
    Terminal _terminal{Size{94, 28}};
    std::chrono::steady_clock::time_point _lastTick{std::chrono::steady_clock::now()};
    std::chrono::milliseconds _modeCycleAccumulator{};
    std::size_t _animationStep{0};
    Mode _mode{Mode::Clear};
    bool _quitRequested{false};
};


}
