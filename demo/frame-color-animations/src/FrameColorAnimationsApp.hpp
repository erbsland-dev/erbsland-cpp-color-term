// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <array>
#include <string_view>

#include <erbsland/cterm/all.hpp>


namespace demo::framecoloranimations {


using namespace erbsland::cterm;


/// Interactive demo showing the animated frame color modes.
class FrameColorAnimationsApp final {
public:
    /// Run the demo until the user quits.
    void run();

private:
    struct PanelSpec final {
        std::string_view title;
        FrameStyle style;
        FrameColorMode mode;
        std::size_t sequenceIndex;
    };

private:
    [[nodiscard]] auto canvasSize() const noexcept -> Size;
    void handleKey(const Key &key) noexcept;
    void renderFrame();
    void drawPanel(Buffer &buffer, Rectangle rect, const PanelSpec &panel) const;
    void drawHeader(Buffer &buffer, Rectangle rect) const;
    void drawFooter(Buffer &buffer, Rectangle rect) const;

private:
    [[nodiscard]] static auto panelSpecs() -> const std::array<PanelSpec, 7> &;
    [[nodiscard]] static auto colorSequence(std::size_t index) -> const ColorSequence &;
    [[nodiscard]] static auto outerFrameColors() -> const ColorSequence &;
    [[nodiscard]] static auto fillColors() -> const ColorSequence &;

private:
    Terminal _terminal{Size{112, 34}};
    std::size_t _animationCycle{0};
    bool _quitRequested{false};
};


}
