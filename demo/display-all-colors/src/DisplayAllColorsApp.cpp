// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "DisplayAllColorsApp.hpp"

#include "ScopedTerminalSession.hpp"

#include <string_view>


namespace demo::display_all_colors {


void DisplayAllColorsApp::run() noexcept {
    auto terminal = Terminal{Size{80, 25}};
    auto session = ScopedTerminalSession{terminal, Terminal::RefreshMode::Clear, Input::Mode::ReadLine, false};
    _buffer = Buffer{Size{80, 9 + 18 + 18}};
    _buffer.fill(Char{" ", bg::Default});
    renderTable();
    renderMatrix();
    renderRainbow();
    terminal.write(_buffer);
}


void DisplayAllColorsApp::renderTable() {
    auto headerCells = _buffer.rect().subRectangle(Anchor::Top, Size{0, 1}, {}).gridCells(1, 2);
    _buffer.drawText("Foregrounds", headerCells.at(0), Alignment::Center, {fg::BrightWhite, bg::Inherited});
    _buffer.drawText("Backgrounds", headerCells.at(1), Alignment::Center, {fg::BrightWhite});
    auto cells = _buffer.rect().subRectangle(Anchor::Top, Size{0, 8}, {1, 0, 0, 0}).gridCells(8, 4);
    for (int i = 0; i < 8; ++i) {
        Foreground fgColor = Foreground::fromIndex16(i);
        Background bgColor = fgColor != fg::Black ? bg::Black : bg::BrightBlack;
        auto rect = cells.at(static_cast<std::size_t>(i) * 4U);
        _buffer.fill(rect, Char{" ", fgColor, bgColor});
        _buffer.drawText(fgColor.toString(), rect.insetBy(Margins{1, 0}), Alignment::Left, {fgColor, bgColor});
        fgColor = Foreground::fromIndex16(i + 8);
        bgColor = bg::Black;
        rect = cells.at(static_cast<std::size_t>(i) * 4U + 1U);
        _buffer.fill(rect, Char{" ", fgColor, bgColor});
        _buffer.drawText(fgColor.toString(), rect.insetBy(Margins{1, 0}), Alignment::Left, {fgColor, bgColor});
        bgColor = Background::fromIndex16(i);
        fgColor = fg::BrightWhite;
        rect = cells.at(static_cast<std::size_t>(i) * 4U + 2U);
        _buffer.fill(rect, Char{" ", fgColor, bgColor});
        _buffer.drawText(bgColor.toString(), rect.insetBy(Margins{1, 0}), Alignment::Left, {fgColor, bgColor});
        bgColor = Background::fromIndex16(i + 8);
        fgColor = bgColor != bg::BrightWhite ? fg::BrightWhite : fg::BrightBlack;
        rect = cells.at(static_cast<std::size_t>(i) * 4U + 3U);
        _buffer.fill(rect, Char{" ", fgColor, bgColor});
        _buffer.drawText(bgColor.toString(), rect.insetBy(Margins{1, 0}), Alignment::Left, {fgColor, bgColor});
    }
}

void DisplayAllColorsApp::renderMatrix() {
    auto matrixRect = Rectangle{0, 10, 16 * 5, 17};
    auto cells = matrixRect.gridCells(17, 16);
    for (int x = 0; x < 16; ++x) {
        for (int y = 0; y < 17; ++y) {
            auto cell = cells.at(static_cast<std::size_t>(x + y * 16));
            if (y == 0) {
                auto color = Background::fromIndex16(x);
                _buffer.fill(cell, Char(" ", fg::BrightWhite, color));
            } else {
                const auto fgColor = Foreground::fromIndex16(x);
                const auto bgColor = Background::fromIndex16(y - 1);
                _buffer.fill(cell, Char(" ", fgColor, bgColor));
                _buffer.drawText(std::to_string(x + (y - 1) * 16), cell, Alignment::Center);
            }
        }
    }
}

void DisplayAllColorsApp::renderRainbow() {
    auto fgColors = std::array<Foreground, 16>{
        fg::Yellow,
        fg::Red,
        fg::BrightRed,
        fg::BrightMagenta,
        fg::Magenta,
        fg::Blue,
        fg::BrightBlue,
        fg::BrightCyan,
        fg::Cyan,
        fg::Green,
        fg::BrightGreen,
        fg::BrightYellow,
        fg::BrightWhite,
        fg::White,
        fg::BrightBlack,
        fg::Black};
    auto bgColors = std::array<Background, 16>{
        bg::Yellow,
        bg::Red,
        bg::BrightRed,
        bg::BrightMagenta,
        bg::Magenta,
        bg::Blue,
        bg::BrightBlue,
        bg::BrightCyan,
        bg::Cyan,
        bg::Green,
        bg::BrightGreen,
        bg::BrightYellow,
        bg::BrightWhite,
        bg::White,
        bg::BrightBlack,
        bg::Black};
    auto matrixRect = Rectangle{0, 28, 16 * 5, 17};
    auto cells = matrixRect.gridCells(17, 16);
    for (int x = 0; x < 16; ++x) {
        for (int y = 0; y < 17; ++y) {
            auto cell = cells.at(static_cast<std::size_t>(x + y * 16));
            if (y == 0) {
                const auto fgColor = fgColors.at(static_cast<std::size_t>(std::min(x + 1, 15)));
                const auto bgColor = bgColors.at(static_cast<std::size_t>(x));
                _buffer.fill(cell, Char(" ", fgColor, bgColor));
                _buffer.drawText("░▒▓█", cell, Alignment::Right);
            } else {
                const auto fgColor = fgColors.at(static_cast<std::size_t>(x));
                const auto bgColor = bgColors.at(static_cast<std::size_t>(y - 1));
                _buffer.fill(cell, Char(" ", fgColor, bgColor));
                _buffer.drawText("░▒▓█", cell, Alignment::Right);
            }
        }
    }
}


}
