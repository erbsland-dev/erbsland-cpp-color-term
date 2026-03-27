// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "BitmapShowcaseApp.hpp"

#include <algorithm>
#include <array>
#include <format>
#include <string>
#include <vector>


namespace demo::bitmapshowcase {


void BitmapShowcaseApp::beforeInitialize() {
    _updateSettings.setMinimumSize(Size{68, 20});
    _updateSettings.setMinimumSizeBackground(Char{" ", bg::Black});
    _updateSettings.setMinimumSizeMessage(
        String{
            "Resize the terminal to at least 68x20 cells for the bitmap showcase.", Color{fg::BrightWhite, bg::Black}});
}


void BitmapShowcaseApp::onKey(const Key &key) {
    if (key == Key::Left) {
        _pageIndex = (_pageIndex + 3) % 4;
    } else if (key == Key::Right) {
        _pageIndex = (_pageIndex + 1) % 4;
    } else if (key == Key::Up) {
        selectVariantDelta(-1);
    } else if (key == Key::Down) {
        selectVariantDelta(1);
    } else {
        TerminalApplication::onKey(key);
    }
}


void BitmapShowcaseApp::onRenderToBuffer() {
    _buffer.fill(Char{" ", bg::Black});
    const auto outerRect = Rectangle{0, 0, _buffer.size().width(), _buffer.size().height()};
    const auto titleRect = Rectangle{2, 1, _buffer.size().width() - 4, 1};
    const auto contentRect = Rectangle{2, 3, _buffer.size().width() - 4, _buffer.size().height() - 7};
    const auto footerRect = Rectangle{2, _buffer.size().height() - 3, _buffer.size().width() - 4, 1};
    auto selectorWidth = std::clamp(contentRect.width() / 3, 22, 30);
    if (contentRect.width() - selectorWidth - 2 < 36) {
        selectorWidth = std::max(18, contentRect.width() / 4);
    }
    const auto selectorRect = Rectangle{contentRect.x1(), contentRect.y1(), selectorWidth, contentRect.height()};
    const auto previewRect = Rectangle{
        selectorRect.x2() + 2, contentRect.y1(), contentRect.x2() - selectorRect.x2() - 2, contentRect.height()};

    _buffer.drawFrame(outerRect, FrameStyle::LightWithRoundedCorners);
    _buffer.drawText(
        std::format("Bitmap Showcase  |  {}", pageTitle()),
        titleRect,
        Alignment::Center,
        Color{fg::BrightWhite, bg::Black});
    drawSelector(selectorRect);
    drawPreview(previewRect);
    drawFooter(footerRect);
}


void BitmapShowcaseApp::drawSelector(const Rectangle rect) {
    const auto count = static_cast<int>(variantCount(_pageIndex));
    if (count <= 0 || rect.height() <= 2) {
        return;
    }
    const auto selected = static_cast<int>(selectedVariantIndex());
    const auto rowStride = rect.height() >= count * 2 + 2 ? 2 : 1;
    const auto visibleCount = std::max(1, (rect.height() - 1) / rowStride);
    auto firstVisible = std::max(0, selected - visibleCount / 2);
    firstVisible = std::min(firstVisible, std::max(0, count - visibleCount));

    for (auto visibleIndex = 0; visibleIndex < visibleCount; ++visibleIndex) {
        const auto itemIndex = firstVisible + visibleIndex;
        if (itemIndex >= count) {
            break;
        }
        const auto itemIndexAsSize = static_cast<std::size_t>(itemIndex);
        const auto y = rect.y1() + visibleIndex * rowStride;
        const auto lineRect = Rectangle{rect.x1(), y, rect.width(), 1};
        const auto isSelected = itemIndex == selected;
        if (isSelected) {
            _buffer.fill(lineRect, Char{" ", bg::BrightBlack});
        }
        auto label = String{};
        if (isSelected) {
            label.append(fg::BrightYellow, "▶ ", fg::BrightWhite, variantTitle(_pageIndex, itemIndexAsSize));
        } else {
            label.append(fg::BrightBlack, "  ", fg::White, variantTitle(_pageIndex, itemIndexAsSize));
        }
        _buffer.drawText(Text{label, lineRect, Alignment::CenterLeft});
    }
}


void BitmapShowcaseApp::drawPreview(const Rectangle rect) {
    switch (_pageIndex) {
    case 0:
        drawScaleModeVariant(rect, selectedVariantIndex());
        break;
    case 1:
        drawColorModeVariant(rect, selectedVariantIndex());
        break;
    case 2:
        drawLayoutVariant(rect, selectedVariantIndex());
        break;
    case 3:
    default:
        drawStyleVariant(rect, selectedVariantIndex());
        break;
    }
}


void BitmapShowcaseApp::drawScaleModeVariant(const Rectangle rect, const std::size_t variantIndex) {

    static constexpr auto cPanelColors = std::array<Color, 4>{
        Color{fg::BrightWhite, bg::BrightBlack},
        Color{fg::BrightWhite, bg::Blue},
        Color{fg::BrightWhite, bg::Magenta},
        Color{fg::BrightWhite, bg::Green},
    };
    static constexpr auto cDescriptions = std::array<std::string_view, 4>{
        "2x2 pixels are packed into one terminal cell. Even the empty table entry is drawn, so the bitmap can paint a "
        "background.",
        "One full block is emitted for each set pixel. Unset pixels are left untouched, which keeps the panel fill "
        "visible below.",
        "Each set pixel is stretched to two terminal cells, which usually looks more natural on regular terminal "
        "character grids.",
        "A Char16Style ignores the scale mode and derives a block from each set pixel plus its four direct neighbors.",
    };

    const auto panelColor = cPanelColors[std::min(variantIndex, cPanelColors.size() - 1)];
    drawPreviewPanel(rect, variantTitle(_pageIndex, variantIndex), panelColor);
    const auto demoRect = Rectangle{rect.x1() + 3, rect.y1() + 2, rect.width() - 6, rect.height() - 7};
    const auto descriptionRect = Rectangle{rect.x1() + 2, rect.y2() - 3, rect.width() - 4, 2};

    switch (variantIndex) {
    case 0: {
        auto options = BitmapDrawOptions{Color{fg::BrightCyan, bg::Blue}};
        _buffer.drawBitmap(ringBitmap(), demoRect, Alignment::Center, options, _animationCycle / 2);
        break;
    }
    case 1: {
        auto options = BitmapDrawOptions{Color{fg::BrightYellow, bg::Blue}};
        options.setScaleMode(BitmapScaleMode::FullBlock);
        _buffer.drawBitmap(rocketBitmap(), demoRect, Alignment::Center, options, _animationCycle / 2);
        break;
    }
    case 2: {
        auto options = BitmapDrawOptions{Color{fg::BrightWhite, bg::Magenta}};
        options.setScaleMode(BitmapScaleMode::DoubleBlock);
        _buffer.drawBitmap(waveBitmap(), demoRect, Alignment::Center, options, _animationCycle / 2);
        break;
    }
    case 3:
    default: {
        auto options = BitmapDrawOptions{Color{fg::BrightWhite, bg::Green}};
        options.setChar16Style(Char16Style::lightRoundedFrame());
        _buffer.drawBitmap(circuitBitmap(), demoRect, Alignment::Center, options, _animationCycle / 2);
        break;
    }
    }
    _buffer.drawText(
        cDescriptions[std::min(variantIndex, cDescriptions.size() - 1)], descriptionRect, Alignment::Center);
}


void BitmapShowcaseApp::drawColorModeVariant(const Rectangle rect, const std::size_t variantIndex) {

    static constexpr auto cModes = std::array<BitmapColorMode, 5>{
        BitmapColorMode::OneColor,
        BitmapColorMode::VerticalStripes,
        BitmapColorMode::HorizontalStripes,
        BitmapColorMode::ForwardDiagonalStripes,
        BitmapColorMode::BackwardDiagonalStripes,
    };
    static constexpr auto cDescriptions = std::array<std::string_view, 5>{
        "OneColor locks the whole bitmap to the same entry in the ColorSequence.",
        "Vertical stripes advance the sequence with the rendered x position.",
        "Horizontal stripes advance the sequence with the rendered y position.",
        "Forward diagonals use x + y, which makes the palette drift from top-left to bottom-right.",
        "Backward diagonals use -x + y, which makes the palette drift in the opposite direction.",
    };

    drawPreviewPanel(rect, variantTitle(_pageIndex, variantIndex), Color{fg::BrightWhite, bg::BrightBlack});
    const auto demoRect = Rectangle{rect.x1() + 3, rect.y1() + 2, rect.width() - 6, rect.height() - 7};
    const auto descriptionRect = Rectangle{rect.x1() + 2, rect.y2() - 3, rect.width() - 4, 2};

    auto options = BitmapDrawOptions{};
    options.setColorSequence(rainbowColors(), cModes[std::min(variantIndex, cModes.size() - 1)]);
    options.setScaleMode(BitmapScaleMode::DoubleBlock);
    options.setColorAnimationOffset(variantIndex * 2);
    _buffer.drawBitmap(waveBitmap(), demoRect, Alignment::Center, options, _animationCycle / 2);
    _buffer.drawText(
        cDescriptions[std::min(variantIndex, cDescriptions.size() - 1)], descriptionRect, Alignment::Center);
}


void BitmapShowcaseApp::drawLayoutVariant(const Rectangle rect, const std::size_t variantIndex) {

    static constexpr auto cAlignments = std::array<Alignment, 5>{
        Alignment::TopLeft,
        Alignment::Center,
        Alignment::BottomRight,
        Alignment::Center,
        Alignment::Center,
    };
    static constexpr auto cDescriptions = std::array<std::string_view, 5>{
        "The viewport keeps the top-left part of the rendered bitmap when it must crop.",
        "Center alignment crops evenly around the preview whenever the rendered bitmap is larger than the viewport.",
        "Bottom-right alignment keeps the last visible part of the rendered bitmap.",
        "HalfBlock layout crops the rendered cells, not the original pixels.",
        "A centered viewport keeps the bitmap readable on narrower screens.",
    };

    drawPreviewPanel(rect, variantTitle(_pageIndex, variantIndex), Color{fg::BrightWhite, bg::BrightBlack});
    const auto frameRect = Rectangle{rect.x1() + 6, rect.y1() + 3, rect.width() - 12, rect.height() - 10};
    const auto viewport = frameRect.insetBy(Margins{1});
    const auto descriptionRect = Rectangle{rect.x1() + 2, rect.y2() - 3, rect.width() - 4, 2};
    _buffer.drawFrame(frameRect, FrameStyle::Double, Color{fg::BrightCyan, bg::BrightBlack});

    auto options = BitmapDrawOptions{};
    options.setColorSequence(rainbowColors(), BitmapColorMode::ForwardDiagonalStripes);
    options.setColorAnimationOffset(variantIndex);
    switch (variantIndex) {
    case 0:
    case 1:
    case 2:
        options.setScaleMode(BitmapScaleMode::FullBlock);
        _buffer.drawBitmap(rocketBitmap(), viewport, cAlignments[variantIndex], options, _animationCycle / 2);
        break;
    case 3:
        _buffer.drawBitmap(ringBitmap(), viewport, cAlignments[variantIndex], options, _animationCycle / 2);
        break;
    case 4:
    default:
        options.setScaleMode(BitmapScaleMode::DoubleBlock);
        _buffer.drawBitmap(waveBitmap(), viewport, cAlignments[variantIndex], options, _animationCycle / 2);
        break;
    }
    _buffer.drawText(
        cDescriptions[std::min(variantIndex, cDescriptions.size() - 1)], descriptionRect, Alignment::Center);
}


void BitmapShowcaseApp::drawStyleVariant(const Rectangle rect, const std::size_t variantIndex) {

    static constexpr auto cPanelColors = std::array<Color, 5>{
        Color{fg::BrightWhite, bg::Blue},
        Color{fg::BrightWhite, bg::Magenta},
        Color{fg::BrightWhite, bg::Green},
        Color{fg::BrightWhite, bg::BrightBlack},
        Color{fg::BrightWhite, bg::BrightBlack},
    };
    static constexpr auto cDescriptions = std::array<std::string_view, 5>{
        "fullBlock() can be replaced with any single-width character. Its own colors are still overlaid on the base "
        "bitmap color.",
        "doubleBlocks() picks one character for the left cell and one for the right cell of every rendered bitmap "
        "pixel.",
        "halfBlocks() accepts any table of sixteen single-width characters. Entry 0 stays the empty 2x2 bitmap cell.",
        "A custom Char16Style is useful when the bitmap should turn into lines, traces, or a schematic instead of "
        "solid blocks.",
        "When a combination style is set, the rendered bitmap can merge into an existing frame grid instead of "
        "overwriting it.",
    };

    const auto panelColor = cPanelColors[std::min(variantIndex, cPanelColors.size() - 1)];
    drawPreviewPanel(rect, variantTitle(_pageIndex, variantIndex), panelColor);
    const auto demoRect = Rectangle{rect.x1() + 3, rect.y1() + 2, rect.width() - 6, rect.height() - 7};
    const auto descriptionRect = Rectangle{rect.x1() + 2, rect.y2() - 3, rect.width() - 4, 2};

    switch (variantIndex) {
    case 0: {
        auto options = BitmapDrawOptions{Color{fg::BrightWhite, bg::Blue}};
        options.setScaleMode(BitmapScaleMode::FullBlock);
        options.setFullBlock(Char{U'●', fg::BrightYellow});
        _buffer.drawBitmap(ringBitmap(), demoRect, Alignment::Center, options, _animationCycle / 2);
        break;
    }
    case 1: {
        auto options = BitmapDrawOptions{Color{fg::BrightWhite, bg::Magenta}};
        options.setScaleMode(BitmapScaleMode::DoubleBlock);
        options.setDoubleBlocks(String{"<>"});
        _buffer.drawBitmap(waveBitmap(), demoRect, Alignment::Center, options, _animationCycle / 2);
        break;
    }
    case 2: {
        auto options = BitmapDrawOptions{Color{fg::BrightWhite, bg::Green}};
        options.setHalfBlocks(String{" 123456789ABCDEF"});
        _buffer.drawBitmap(ringBitmap(), demoRect, Alignment::Center, options, _animationCycle / 2);
        break;
    }
    case 3: {
        auto options = BitmapDrawOptions{Color{fg::BrightCyan, bg::BrightBlack}};
        options.setChar16Style(Char16Style::lightRoundedFrame());
        _buffer.drawBitmap(circuitBitmap(), demoRect, Alignment::Center, options, _animationCycle / 2);
        break;
    }
    case 4:
    default: {
        const auto viewport = demoRect.insetBy(Margins{2, 1});
        _buffer.drawFrame(viewport, FrameStyle::Light, Color{fg::BrightBlack, bg::BrightBlack});
        for (auto x = viewport.x1() + 4; x < viewport.x2() - 4; x += 5) {
            _buffer.drawFrame(Rectangle{x, viewport.y1() + 1, 4, viewport.height() - 2}, FrameStyle::Light);
        }
        auto options = BitmapDrawOptions{Color{fg::BrightCyan, bg::BrightBlack}};
        options.setChar16Style(Char16Style::lightFrame());
        options.setCombinationStyle(CharCombinationStyle::commonBoxFrame());
        _buffer.drawBitmap(circuitBitmap(), viewport, Alignment::Center, options, _animationCycle / 2);
        break;
    }
    }
    _buffer.drawText(
        cDescriptions[std::min(variantIndex, cDescriptions.size() - 1)], descriptionRect, Alignment::Center);
}


void BitmapShowcaseApp::drawPreviewPanel(const Rectangle rect, const std::string_view title, const Color fillColor) {

    if (rect.width() <= 4 || rect.height() <= 4) {
        return;
    }
    _buffer.drawFilledFrame(rect, FrameStyle::LightWithRoundedCorners, Char{" ", fillColor});
    _buffer.drawText(
        title,
        Rectangle{rect.x1() + 2, rect.y1(), rect.width() - 4, 1},
        Alignment::Center,
        Color{fg::BrightWhite, fillColor.bg()});
}


void BitmapShowcaseApp::drawFooter(const Rectangle rect) {
    _buffer.fill(rect, Char{" ", bg::BrightBlack});
    auto footer = Text{footerText(), rect, Alignment::CenterLeft};
    _buffer.drawText(footer);
}


auto BitmapShowcaseApp::footerText() const -> String {
    auto result = String{};
    result.append(
        fg::BrightCyan,
        "[←][→]",
        fg::BrightWhite,
        " switch pages  ",
        fg::BrightYellow,
        "[↑][↓]",
        fg::BrightWhite,
        " switch modes  ",
        fg::BrightGreen,
        "[Q]",
        fg::BrightWhite,
        " quit  ",
        fg::BrightGreen,
        std::format("page {}/4", _pageIndex + 1));
    return result;
}


auto BitmapShowcaseApp::pageTitle() const -> std::string_view {
    switch (_pageIndex) {
    case 0:
        return "scale modes";
    case 1:
        return "color modes";
    case 2:
        return "alignment and cropping";
    case 3:
    default:
        return "custom blocks and combinations";
    }
}


auto BitmapShowcaseApp::variantCount(const std::size_t pageIndex) const noexcept -> std::size_t {
    switch (pageIndex) {
    case 0:
        return 4;
    case 1:
        return 5;
    case 2:
        return 5;
    case 3:
    default:
        return 5;
    }
}


auto BitmapShowcaseApp::variantTitle(const std::size_t pageIndex, const std::size_t variantIndex) const
    -> std::string_view {

    switch (pageIndex) {
    case 0:
        switch (variantIndex) {
        case 0:
            return "HalfBlock";
        case 1:
            return "FullBlock";
        case 2:
            return "DoubleBlock";
        case 3:
        default:
            return "Char16Style";
        }
    case 1:
        switch (variantIndex) {
        case 0:
            return "OneColor";
        case 1:
            return "VerticalStripes";
        case 2:
            return "HorizontalStripes";
        case 3:
            return "ForwardDiagonal";
        case 4:
        default:
            return "BackwardDiagonal";
        }
    case 2:
        switch (variantIndex) {
        case 0:
            return "TopLeft Crop";
        case 1:
            return "Center Crop";
        case 2:
            return "BottomRight Crop";
        case 3:
            return "HalfBlock Crop";
        case 4:
        default:
            return "DoubleBlock Crop";
        }
    case 3:
    default:
        switch (variantIndex) {
        case 0:
            return "Custom Full Block";
        case 1:
            return "Custom Double Blocks";
        case 2:
            return "Custom Half Blocks";
        case 3:
            return "Rounded Char16";
        case 4:
        default:
            return "Combination Style";
        }
    }
}


auto BitmapShowcaseApp::selectedVariantIndex() const noexcept -> std::size_t {
    return _selectedVariantByPage[_pageIndex];
}


void BitmapShowcaseApp::selectVariantDelta(const int delta) noexcept {
    const auto count = static_cast<int>(variantCount(_pageIndex));
    if (count <= 0) {
        return;
    }
    const auto current = static_cast<int>(_selectedVariantByPage[_pageIndex]);
    const auto next = (current + delta % count + count) % count;
    _selectedVariantByPage[_pageIndex] = static_cast<std::size_t>(next);
}


auto BitmapShowcaseApp::ringBitmap() -> const Bitmap & {
    static const auto bitmap = Bitmap::fromPattern({
        "..########..",
        ".##......##.",
        "##........##",
        "##........##",
        "##........##",
        "##........##",
        ".##......##.",
        "..########..",
    });
    return bitmap;
}


auto BitmapShowcaseApp::rocketBitmap() -> const Bitmap & {
    static const auto bitmap = Bitmap::fromPattern({
        "....##.....",
        "...#.#.....",
        "..#..#.....",
        ".#...#.....",
        "#....#.....",
        "######.....",
        ".....#.....",
        "###########",
        "#.#.#.#.#.#",
        ".#.#.#.#.#.",
        "..#######..",
    });
    return bitmap;
}


auto BitmapShowcaseApp::waveBitmap() -> const Bitmap & {
    static const auto bitmap = Bitmap::fromPattern({
        "......####.....",
        "#...##....##...",
        ".#.#...#.#..##.",
        "..#...........#",
        ".#.#....#######",
        "#...#......#...",
        ".....######....",
    });
    return bitmap;
}


auto BitmapShowcaseApp::circuitBitmap() -> const Bitmap & {
    static const auto bitmap = Bitmap::fromPattern({
        "#...#####....",
        "#...#...#....",
        "#############",
        "...#.....#..#",
        "...#######..#.",
        "....#...#....",
        "..###...###..",
    });
    return bitmap;
}


auto BitmapShowcaseApp::rainbowColors() -> const ColorSequence & {
    static const auto colors = ColorSequence{
        {Color{fg::BrightBlue, bg::Black}, 3},
        {Color{fg::BrightCyan, bg::Black}, 3},
        {Color{fg::BrightGreen, bg::Black}, 3},
        {Color{fg::BrightYellow, bg::Black}, 3},
        {Color{fg::BrightRed, bg::Black}, 3},
        {Color{fg::BrightMagenta, bg::Black}, 3},
    };
    return colors;
}


}
