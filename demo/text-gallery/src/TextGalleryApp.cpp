// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TextGalleryApp.hpp"

#include "ScopedTerminalSession.hpp"

#include <algorithm>
#include <format>


namespace demo::textgallery {


void TextGalleryApp::run() {
    _font = Font::defaultAscii();
    auto session = ScopedTerminalSession{_terminal, Terminal::RefreshMode::Overwrite, Input::Mode::Key};
    while (!_quitRequested) {
        const auto key = _terminal.input().read(std::chrono::milliseconds{90});
        if (key.valid()) {
            handleKey(key);
        }
        renderFrame();
        ++_animationCycle;
    }
}


auto TextGalleryApp::canvasSize() const noexcept -> Size {
    return _terminal.size() - Size{1, 1};
}


void TextGalleryApp::handleKey(const Key &key) noexcept {
    if (key == Key{Key::Character, 'q'}) {
        _quitRequested = true;
    } else if (key == Key{Key::Left}) {
        _pageIndex = (_pageIndex + 2) % 3;
    } else if (key == Key{Key::Right}) {
        _pageIndex = (_pageIndex + 1) % 3;
    }
}


void TextGalleryApp::renderFrame() {
    _terminal.testScreenSize();
    auto buffer = Buffer{canvasSize()};
    buffer.fill(Char{" ", bg::Black});
    if (buffer.size().width() < 38 || buffer.size().height() < 14) {
        buffer.drawText(
            "Resize the terminal to at least 38x14 cells for the text gallery.",
            Rectangle{0, 0, buffer.size().width(), buffer.size().height()},
            Alignment::Center,
            Color{fg::BrightWhite, bg::Black});
    } else {
        const auto outerRect = Rectangle{0, 0, buffer.size().width(), buffer.size().height()};
        buffer.drawFrame(outerRect, FrameStyle::LightWithRoundedCorners);
        const auto titleRect = Rectangle{2, 1, buffer.size().width() - 4, 1};
        const auto contentRect = Rectangle{2, 3, buffer.size().width() - 4, buffer.size().height() - 7};
        const auto footerRect = Rectangle{2, buffer.size().height() - 3, buffer.size().width() - 4, 1};
        buffer.drawText(
            "Text Gallery  |  alignment, wrapping, wide characters, and bitmap fonts",
            titleRect,
            Alignment::Center,
            Color{fg::BrightWhite, bg::Black});
        switch (_pageIndex) {
        case 0:
            drawOverviewPage(buffer, contentRect);
            break;
        case 1:
            drawMixedWidthPage(buffer, contentRect);
            break;
        case 2:
        default:
            drawBitmapFontPage(buffer, contentRect);
            break;
        }
        drawFooter(buffer, footerRect);
    }
    _terminal.updateScreen(buffer);
    _terminal.flush();
}


void TextGalleryApp::drawOverviewPage(Buffer &buffer, const Rectangle contentRect) {
    const auto gap = 1;
    const auto columnWidth = std::max(12, (contentRect.width() - gap) / 2);
    const auto rowHeight = std::max(5, (contentRect.height() - gap) / 2);
    drawPanel(
        buffer,
        Rectangle{contentRect.x1(), contentRect.y1(), columnWidth, rowHeight},
        "Top Left",
        "Small panels are an easy way to compare alignment and wrapping side by side.",
        Alignment::TopLeft,
        FrameStyle::Light,
        Color{fg::White, bg::BrightBlack},
        Color{fg::White, bg::BrightBlack});
    drawPanel(
        buffer,
        Rectangle{contentRect.x1() + columnWidth + gap, contentRect.y1(), columnWidth, rowHeight},
        "Center",
        "The same paragraph can be centered without a custom layout engine.",
        Alignment::Center,
        FrameStyle::Heavy,
        Color{fg::BrightBlue, bg::Blue},
        Color{fg::BrightBlue, bg::Blue});
    drawPanel(
        buffer,
        Rectangle{contentRect.x1(), contentRect.y1() + rowHeight + gap, columnWidth, rowHeight},
        "Bottom Right",
        "Right and bottom alignment stay readable even inside narrow frames.",
        Alignment::BottomRight,
        FrameStyle::Double,
        Color{fg::BrightMagenta, bg::Magenta},
        Color{fg::BrightMagenta, bg::Magenta});
    drawPanel(
        buffer,
        Rectangle{contentRect.x1() + columnWidth + gap, contentRect.y1() + rowHeight + gap, columnWidth, rowHeight},
        "Top Center",
        "Wrapping respects the box width, while alignment still decides where each line starts.",
        Alignment::TopCenter,
        FrameStyle::LightWithRoundedCorners,
        Color{fg::BrightCyan, bg::Cyan},
        Color{fg::BrightCyan, bg::Cyan});
}


void TextGalleryApp::drawMixedWidthPage(Buffer &buffer, const Rectangle contentRect) {
    const auto topHeight = std::max(5, contentRect.height() / 2 - 1);
    drawPanel(
        buffer,
        Rectangle{contentRect.x1(), contentRect.y1(), contentRect.width(), topHeight},
        "Mixed Width Layout",
        "English meets 日本語 and 漢字 in the same wrapped paragraph. The frame and alignment stay stable because "
        "the renderer measures each character width instead of counting bytes.",
        Alignment::Center,
        FrameStyle::Light,
        bg::BrightBlack,
        Color{fg::BrightWhite, bg::BrightBlack});

    const auto lowerY = contentRect.y1() + topHeight + 1;
    const auto lowerHeight = contentRect.y2() - lowerY;
    const auto leftWidth = std::max(12, (contentRect.width() - 1) / 2);
    drawPanel(
        buffer,
        Rectangle{contentRect.x1(), lowerY, leftWidth, lowerHeight},
        "Center",
        "A界B  C東京D  E文字F\nCentered text keeps the wide glyphs balanced.",
        Alignment::Center,
        FrameStyle::Heavy,
        bg::Blue,
        Color{fg::BrightWhite, bg::Blue});
    drawPanel(
        buffer,
        Rectangle{contentRect.x1() + leftWidth + 1, lowerY, leftWidth, lowerHeight},
        "Right",
        "右寄せ with ASCII, kana, and kanji.\nZürich, 東京, Kyoto, and 大阪 all line up cleanly.",
        Alignment::BottomRight,
        FrameStyle::Double,
        bg::Magenta,
        Color{fg::BrightWhite, bg::Magenta});
}


void TextGalleryApp::drawBitmapFontPage(Buffer &buffer, const Rectangle contentRect) {
    const auto titleHeight = std::min(6, contentRect.height());
    auto title = Text{
        String{titleForWidth(contentRect.width())},
        Rectangle{contentRect.x1(), contentRect.y1(), contentRect.width(), titleHeight},
        Alignment::Center};
    title.setFont(_font);
    title.setColorSequence(titleColors());
    title.setAnimation(TextAnimation::ColorDiagonal);
    buffer.drawText(title, _animationCycle);

    drawPanel(
        buffer,
        Rectangle{
            contentRect.x1(), contentRect.y1() + titleHeight, contentRect.width(), contentRect.height() - titleHeight},
        "Bitmap Font",
        "The large title above uses the built-in default ASCII bitmap font. Regular text still fits naturally "
        "around it, so a page can mix dramatic headlines with practical terminal UI copy.",
        Alignment::TopCenter,
        FrameStyle::LightWithRoundedCorners,
        bg::BrightBlack,
        Color{fg::BrightWhite, bg::BrightBlack});
}


void TextGalleryApp::drawPanel(
    Buffer &buffer,
    const Rectangle rect,
    const std::string_view title,
    const std::string_view text,
    const Alignment alignment,
    const FrameStyle frameStyle,
    const Color fillColor,
    const Color textColor) {

    if (rect.width() <= 2 || rect.height() <= 2) {
        return;
    }
    buffer.drawFilledFrame(rect, frameStyle, Char{" ", fillColor});
    buffer.drawText(title, Rectangle{rect.x1() + 2, rect.y1(), rect.width() - 4, 1}, Alignment::Center, textColor);
    buffer.drawText(
        text, Rectangle{rect.x1() + 1, rect.y1() + 1, rect.width() - 2, rect.height() - 2}, alignment, textColor);
}


void TextGalleryApp::drawFooter(Buffer &buffer, const Rectangle rect) const {
    buffer.fill(rect, Char{" ", bg::BrightBlack});
    auto footer = Text{buildFooterText(), rect, Alignment::CenterLeft};
    buffer.drawText(footer);
}


auto TextGalleryApp::buildFooterText() const -> String {
    auto result = String{};
    result.append(
        fg::BrightCyan,
        "[←][→]",
        fg::BrightWhite,
        " switch pages  ",
        fg::BrightYellow,
        "[Q]",
        fg::BrightWhite,
        " quit  ",
        fg::BrightGreen,
        std::format("page {}/3", _pageIndex + 1));
    return result;
}


auto TextGalleryApp::titleColors() -> ColorSequence {
    return ColorSequence{
        {Color{fg::BrightBlue, bg::Black}, 10},
        {Color{fg::BrightCyan, bg::Black}, 5},
        {Color{fg::BrightMagenta, bg::Black}, 3},
        {Color{fg::BrightYellow, bg::Black}, 2},
        {Color{fg::BrightGreen, bg::Black}, 5},
    };
}


auto TextGalleryApp::titleForWidth(const int width) -> std::string_view {
    if (width >= 60) {
        return "-+[ COLOR TERM ]+-";
    }
    if (width >= 36) {
        return "COLOR TERM";
    }
    if (width >= 22) {
        return "COLOR";
    }
    return "TERM";
}


}
