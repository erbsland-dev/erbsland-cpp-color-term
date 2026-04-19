// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TextGalleryApp.hpp"

#include <algorithm>
#include <format>

namespace demo::textgallery {

void TextGalleryApp::beforeInitialize() {
    _updateSettings.setMinimumSize(Size{38, 14});
    _updateSettings.setMinimumSizeBackground(Char{" ", bg::Black});
    _updateSettings.setMinimumSizeMessage(
        String{"Resize the terminal to at least 38x14 cells for the text gallery.", Color{fg::BrightWhite, bg::Black}});
}

auto TextGalleryApp::beforeRun() -> int {
    _font = Font::defaultAscii();
    return 0;
}

void TextGalleryApp::onKey(const Key &key) {
    if (key == Key::Left) {
        _pageIndex = (_pageIndex + 2) % 3;
    } else if (key == Key::Right) {
        _pageIndex = (_pageIndex + 1) % 3;
    } else {
        TerminalApplication::onKey(key);
    }
}

void TextGalleryApp::onRenderToBuffer() {
    _buffer.fill(Char{" ", bg::Black});
    const auto outerRect = Rectangle{0, 0, _buffer.size().width(), _buffer.size().height()};
    _buffer.drawFrame(outerRect, FrameStyle::LightWithRoundedCorners);
    const auto titleRect = Rectangle{2, 1, _buffer.size().width() - 4, 1};
    const auto contentRect = Rectangle{2, 3, _buffer.size().width() - 4, _buffer.size().height() - 7};
    const auto footerRect = Rectangle{2, _buffer.size().height() - 3, _buffer.size().width() - 4, 1};
    _buffer.drawText(
        "Text Gallery  |  alignment, wrapping, wide characters, and bitmap fonts",
        titleRect,
        Alignment::Center,
        Color{fg::BrightWhite, bg::Black});
    switch (_pageIndex) {
    case 0:
        drawOverviewPage(contentRect);
        break;
    case 1:
        drawMixedWidthPage(contentRect);
        break;
    case 2:
    default:
        drawBitmapFontPage(contentRect);
        break;
    }
    drawFooter(footerRect);
}

void TextGalleryApp::drawOverviewPage(const Rectangle contentRect) {
    const auto gap = 1;
    const auto columnWidth = std::max(12, (contentRect.width() - gap) / 2);
    const auto rowHeight = std::max(5, (contentRect.height() - gap) / 2);
    drawPanel(
        Rectangle{contentRect.x1(), contentRect.y1(), columnWidth, rowHeight},
        "Top Left",
        "Small panels are an easy way to compare alignment and wrapping side by side.",
        Alignment::TopLeft,
        FrameStyle::Light,
        Color{fg::White, bg::BrightBlack},
        Color{fg::White, bg::BrightBlack});
    drawPanel(
        Rectangle{contentRect.x1() + columnWidth + gap, contentRect.y1(), columnWidth, rowHeight},
        "Center",
        "The same paragraph can be centered without a custom layout engine.",
        Alignment::Center,
        FrameStyle::Heavy,
        Color{fg::BrightBlue, bg::Blue},
        Color{fg::BrightBlue, bg::Blue});
    drawPanel(
        Rectangle{contentRect.x1(), contentRect.y1() + rowHeight + gap, columnWidth, rowHeight},
        "Bottom Right",
        "Right and bottom alignment stay readable even inside narrow frames.",
        Alignment::BottomRight,
        FrameStyle::Double,
        Color{fg::BrightMagenta, bg::Magenta},
        Color{fg::BrightMagenta, bg::Magenta});
    drawPanel(
        Rectangle{contentRect.x1() + columnWidth + gap, contentRect.y1() + rowHeight + gap, columnWidth, rowHeight},
        "Top Center",
        "Wrapping respects the box width, while alignment still decides where each line starts.",
        Alignment::TopCenter,
        FrameStyle::LightWithRoundedCorners,
        Color{fg::BrightCyan, bg::Cyan},
        Color{fg::BrightCyan, bg::Cyan});
}

void TextGalleryApp::drawMixedWidthPage(const Rectangle contentRect) {
    const auto topHeight = std::max(5, contentRect.height() / 2 - 1);
    drawPanel(
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
        Rectangle{contentRect.x1(), lowerY, leftWidth, lowerHeight},
        "Center",
        "A界B  C東京D  E文字F\nCentered text keeps the wide glyphs balanced.",
        Alignment::Center,
        FrameStyle::Heavy,
        bg::Blue,
        Color{fg::BrightWhite, bg::Blue});
    drawPanel(
        Rectangle{contentRect.x1() + leftWidth + 1, lowerY, leftWidth, lowerHeight},
        "Right",
        "右寄せ with ASCII, kana, and kanji.\nZürich, 東京, Kyoto, and 大阪 all line up cleanly.",
        Alignment::BottomRight,
        FrameStyle::Double,
        bg::Magenta,
        Color{fg::BrightWhite, bg::Magenta});
}

void TextGalleryApp::drawBitmapFontPage(const Rectangle contentRect) {
    const auto titleHeight = std::min(6, contentRect.height());
    auto title = Text{
        String{titleForWidth(contentRect.width())},
        Rectangle{contentRect.x1(), contentRect.y1(), contentRect.width(), titleHeight},
        Alignment::Center};
    title.setFont(_font);
    title.setColorSequence(titleColors());
    title.setAnimation(TextAnimation::ColorDiagonal);
    _buffer.drawText(title, _animationCycle);

    drawPanel(
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
    _buffer.drawFilledFrame(rect, frameStyle, Char{" ", fillColor});
    _buffer.drawText(title, Rectangle{rect.x1() + 2, rect.y1(), rect.width() - 4, 1}, Alignment::Center, textColor);
    _buffer.drawText(
        text, Rectangle{rect.x1() + 1, rect.y1() + 1, rect.width() - 2, rect.height() - 2}, alignment, textColor);
}

void TextGalleryApp::drawFooter(const Rectangle rect) {
    _buffer.fill(rect, Char{" ", bg::BrightBlack});
    auto footer = Text{buildFooterText(), rect, Alignment::CenterLeft};
    _buffer.drawText(footer);
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
