// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <erbsland/cterm/all.hpp>

#include <chrono>


namespace demo::textgallery {


using namespace erbsland::cterm;


/// Browse several text rendering demonstrations with left and right cursor keys.
class TextGalleryApp final {
public:
    /// Run the demo until the user quits.
    void run();

private:
    [[nodiscard]] auto canvasSize() const noexcept -> Size;
    void handleKey(const Key &key) noexcept;
    void renderFrame();
    void drawOverviewPage(Buffer &buffer, Rectangle contentRect);
    void drawMixedWidthPage(Buffer &buffer, Rectangle contentRect);
    void drawBitmapFontPage(Buffer &buffer, Rectangle contentRect);
    void drawPanel(
        Buffer &buffer,
        Rectangle rect,
        std::string_view title,
        std::string_view text,
        Alignment alignment,
        FrameStyle frameStyle,
        Color fillColor,
        Color textColor);
    void drawFooter(Buffer &buffer, Rectangle rect) const;
    [[nodiscard]] auto buildFooterText() const -> String;
    [[nodiscard]] static auto titleColors() -> ColorSequence;
    [[nodiscard]] static auto titleForWidth(int width) -> std::string_view;

private:
    Terminal _terminal{Size{96, 30}};
    FontPtr _font;
    std::size_t _pageIndex{0};
    std::size_t _animationCycle{0};
    bool _quitRequested{false};
};


}
