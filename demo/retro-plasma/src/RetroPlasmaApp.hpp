// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "PlasmaRenderer.hpp"

#include <erbsland/cterm/all.hpp>

#include <chrono>


namespace demo::retroplasma {


using namespace erbsland::cterm;


/// Run the animated plasma buffer demo.
class RetroPlasmaApp final {
public:
    /// Run the demo until the user quits.
    void run() noexcept;

private:
    [[nodiscard]] auto canvasSize() const noexcept -> Size;
    void handleKey(const Key &key) noexcept;
    void renderFrame() noexcept;
    void drawPrompt(Buffer &buffer) const;
    [[nodiscard]] auto buildPrompt() const -> String;
    static void appendText(String &target, std::string_view text, Color color = {});

private:
    Terminal _terminal{Size{90, 28}};
    PlasmaRenderer _renderer;
    double _phase{0.0};
    double _speed{1.0};
    std::size_t _paletteIndex{0};
    bool _paused{false};
    bool _quitRequested{false};
    std::chrono::steady_clock::time_point _lastFrameTime{std::chrono::steady_clock::now()};
};


}
