// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "TerminalApplication.hpp"

#include <chrono>


namespace demo::textgallery {


using namespace erbsland::cterm;


/// Browse several text rendering demonstrations with left and right cursor keys.
class TextGalleryApp final : public TerminalApplication {
public:
    /// Prepare the shared terminal update settings before the terminal is initialized.
    void beforeInitialize() override;
    /// Load the shared bitmap font after the terminal is ready.
    auto beforeRun() -> int override;
    /// Handle gallery page navigation keys.
    void onKey(const Key &key) override;
    /// Render the current gallery page into the shared demo buffer.
    void onRenderToBuffer() override;

private:
    void drawOverviewPage(Rectangle contentRect);
    void drawMixedWidthPage(Rectangle contentRect);
    void drawBitmapFontPage(Rectangle contentRect);
    void drawPanel(
        Rectangle rect,
        std::string_view title,
        std::string_view text,
        Alignment alignment,
        FrameStyle frameStyle,
        Color fillColor,
        Color textColor);
    void drawFooter(Rectangle rect);
    [[nodiscard]] auto buildFooterText() const -> String;
    [[nodiscard]] static auto titleColors() -> ColorSequence;
    [[nodiscard]] static auto titleForWidth(int width) -> std::string_view;

private:
    FontPtr _font;
    std::size_t _pageIndex{0};
};


}
