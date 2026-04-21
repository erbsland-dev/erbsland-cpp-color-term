// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "GridLayoutApp.hpp"

#include <algorithm>
#include <format>
#include <string>

namespace demo::gridlayout {

void GridLayoutApp::beforeInitialize() {
    _updateSettings.setMinimumSize(Size{60, 20});
    _updateSettings.setMinimumSizeBackground(Char{" ", bg::Black});
    _updateSettings.setMinimumSizeMessage(
        String{
            "Resize the terminal to at least 60x20 cells for the grid layout demo.",
            Color{fg::BrightWhite, bg::Black}});
}

void GridLayoutApp::onKey(const Key &key) {
    if (key == U'c') {
        _columnCount = _columnCount % 5 + 1;
    } else if (key == U'r') {
        _rowCount = _rowCount % 3 + 1;
    } else {
        for (const auto &info : borderElements()) {
            if (key == static_cast<char32_t>(info.key)) {
                cycleBorder(info.element);
                return;
            }
        }
        TerminalApplication::onKey(key);
    }
}

void GridLayoutApp::onRenderToBuffer() {
    _buffer.fill(Char{" ", bg::Black});

    const auto titleRect = Rectangle{0, 0, _buffer.size().width(), 1};
    const auto gridArea = Rectangle{2, 2, _buffer.size().width() - 4, _buffer.size().height() - 9};
    const auto statusRect = Rectangle{2, _buffer.size().height() - 6, _buffer.size().width() - 4, 3};
    const auto footerRect = Rectangle{0, _buffer.size().height() - 2, _buffer.size().width(), 2};

    _buffer.fill(titleRect, Char{" ", bg::Blue});
    _buffer.drawText(
        std::format(
            "FrameBorder + GridLayout  |  {} columns x {} rows",
            static_cast<int>(_columnCount),
            static_cast<int>(_rowCount)),
        titleRect,
        Alignment::CenterLeft,
        Color{fg::BrightWhite, bg::Blue});

    renderGrid(gridArea);
    renderStatus(statusRect);
    renderFooter(footerRect);
}

void GridLayoutApp::cycleBorder(const FrameBorderElement element) noexcept {
    _border.set(element, nextStyle(_border.style(element)), Color{fg::BrightWhite, bg::Black});
}

void GridLayoutApp::renderGrid(const Rectangle gridArea) {
    const auto layout = createLayout(gridArea.size());
    const auto gridSize = layout.size(_border);
    const auto origin = gridArea.alignmentOffset(gridSize, Alignment::Center);

    renderCellContent(layout, origin);
    _buffer.drawGridLayout(origin, layout, _border);
}

void GridLayoutApp::renderCellContent(const GridLayout &layout, const Position origin) {
    for (std::size_t row = 0; row < _rowCount; ++row) {
        for (std::size_t column = 0; column < _columnCount; ++column) {
            const auto rect = layout.cellRect(row, column, origin, _border);
            const auto background = ((row + column) % 2 == 0) ? bg::BrightBlack : bg::Black;
            _buffer.fill(rect, Char{" ", background});
            _buffer.drawText(
                std::format("R{} C{}", static_cast<int>(row + 1), static_cast<int>(column + 1)),
                rect,
                Alignment::Center,
                Color{fg::BrightCyan, background});
        }
    }
}

void GridLayoutApp::renderStatus(const Rectangle statusRect) {
    auto status = String{};
    appendBorderStatusLine(status, 0, 2);
    status.append("\n");
    appendBorderStatusLine(status, 2, 4);
    status.append("\n");
    appendBorderStatusLine(status, 4, 6);
    _buffer.drawText(Text{status, statusRect, Alignment::CenterLeft});
}

void GridLayoutApp::appendBorderStatusLine(String &status, const std::size_t begin, const std::size_t end) const {
    const auto &elements = borderElements();
    for (auto index = begin; index < end; ++index) {
        const auto &info = elements[index];
        status.append(
            fg::BrightYellow,
            std::format("[{}]", info.key),
            fg::BrightWhite,
            std::format(" {}:{}  ", info.label, styleName(_border.style(info.element))));
    }
}

void GridLayoutApp::renderFooter(const Rectangle footerRect) {
    _buffer.fill(footerRect, Char{" ", bg::BrightBlack});
    auto prompt = String{};
    prompt.append(
        bg::BrightBlack,
        fg::BrightGreen,
        "[1-6]",
        fg::BrightWhite,
        " cycle border styles  ",
        fg::BrightCyan,
        "[C]",
        fg::BrightWhite,
        " columns  ",
        fg::BrightCyan,
        "[R]",
        fg::BrightWhite,
        " rows  ",
        fg::BrightGreen,
        "[Q]",
        fg::BrightWhite,
        " quit");
    _buffer.drawText(Text{prompt, footerRect, Alignment::CenterLeft});
}

auto GridLayoutApp::createLayout(const Size availableSize) const -> GridLayout {
    const auto separatorWidth = static_cast<Coordinate>(_columnCount - 1) * borderLineSize(FrameBorderElement::VLine);
    const auto separatorHeight = static_cast<Coordinate>(_rowCount - 1) * borderLineSize(FrameBorderElement::HLine);
    const auto lineWidth =
        borderLineSize(FrameBorderElement::Left) + borderLineSize(FrameBorderElement::Right) + separatorWidth;
    const auto lineHeight =
        borderLineSize(FrameBorderElement::Top) + borderLineSize(FrameBorderElement::Bottom) + separatorHeight;
    const auto contentWidth =
        std::max<Coordinate>(static_cast<Coordinate>(_columnCount), availableSize.width() - lineWidth);
    const auto contentHeight =
        std::max<Coordinate>(static_cast<Coordinate>(_rowCount), availableSize.height() - lineHeight);
    return GridLayout{distribute(contentWidth, _columnCount), distribute(contentHeight, _rowCount)};
}

auto GridLayoutApp::borderLineSize(const FrameBorderElement element) const noexcept -> Coordinate {
    const auto style = _border.style(element);
    return style != FrameStyle::None && FrameBorder::isLineStyle(style) ? 1 : 0;
}

auto GridLayoutApp::nextStyle(const FrameStyle style) noexcept -> FrameStyle {
    switch (style) {
    case FrameStyle::Light:
        return FrameStyle::LightDoubleDash;
    case FrameStyle::LightDoubleDash:
        return FrameStyle::LightTripleDash;
    case FrameStyle::LightTripleDash:
        return FrameStyle::LightQuadrupleDash;
    case FrameStyle::LightQuadrupleDash:
        return FrameStyle::LightWithRoundedCorners;
    case FrameStyle::LightWithRoundedCorners:
        return FrameStyle::Heavy;
    case FrameStyle::Heavy:
        return FrameStyle::HeavyDoubleDash;
    case FrameStyle::HeavyDoubleDash:
        return FrameStyle::HeavyTripleDash;
    case FrameStyle::HeavyTripleDash:
        return FrameStyle::HeavyQuadrupleDash;
    case FrameStyle::HeavyQuadrupleDash:
        return FrameStyle::Double;
    case FrameStyle::Double:
        return FrameStyle::None;
    case FrameStyle::None:
    default:
        return FrameStyle::Light;
    }
}

auto GridLayoutApp::styleName(const FrameStyle style) noexcept -> std::string_view {
    switch (style) {
    case FrameStyle::Light:
        return "Light";
    case FrameStyle::LightDoubleDash:
        return "LightDoubleDash";
    case FrameStyle::LightTripleDash:
        return "LightTripleDash";
    case FrameStyle::LightQuadrupleDash:
        return "LightQuadDash";
    case FrameStyle::LightWithRoundedCorners:
        return "Rounded";
    case FrameStyle::Heavy:
        return "Heavy";
    case FrameStyle::HeavyDoubleDash:
        return "HeavyDoubleDash";
    case FrameStyle::HeavyTripleDash:
        return "HeavyTripleDash";
    case FrameStyle::HeavyQuadrupleDash:
        return "HeavyQuadDash";
    case FrameStyle::Double:
        return "Double";
    case FrameStyle::None:
    default:
        return "None";
    }
}

auto GridLayoutApp::borderElements() noexcept -> const std::array<BorderElementInfo, 6> & {
    static constexpr auto cElements = std::array{
        BorderElementInfo{FrameBorderElement::Top, '1', "Top"},
        BorderElementInfo{FrameBorderElement::Bottom, '2', "Bottom"},
        BorderElementInfo{FrameBorderElement::Left, '3', "Left"},
        BorderElementInfo{FrameBorderElement::Right, '4', "Right"},
        BorderElementInfo{FrameBorderElement::HLine, '5', "HLine"},
        BorderElementInfo{FrameBorderElement::VLine, '6', "VLine"},
    };
    return cElements;
}

auto GridLayoutApp::distribute(const Coordinate total, const std::size_t count) -> std::vector<Coordinate> {
    auto result = std::vector<Coordinate>(count, total / static_cast<Coordinate>(count));
    const auto remainder = total % static_cast<Coordinate>(count);
    for (Coordinate index = 0; index < remainder; ++index) {
        ++result[static_cast<std::size_t>(index)];
    }
    return result;
}

}
