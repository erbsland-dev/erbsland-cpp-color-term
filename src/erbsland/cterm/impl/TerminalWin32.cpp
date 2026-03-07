// Copyright © 2026 by Tobias Erbsland / Erbsland DEV.
// SPDX-License-Identifier: CC-BY-4.0
#include "Terminal.hpp"

#include <fcntl.h>
#include <windows.h>

#include <iostream>


namespace erbsland::cterm {


void Terminal::initializePlatform() noexcept {
    constexpr UINT cUtf8CodePage = 65001;
    ::SetConsoleCP(cUtf8CodePage);
    ::SetConsoleOutputCP(cUtf8CodePage);
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const auto outputHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
    if (outputHandle == nullptr || outputHandle == INVALID_HANDLE_VALUE) {
        return;
    }
    CONSOLE_CURSOR_INFO cursorInfo{};
    if (!::GetConsoleCursorInfo(outputHandle, &cursorInfo)) {
        return;
    }
    _cursorVisible = (cursorInfo.bVisible != FALSE);
    _cursorStateSaved = true;
    cursorInfo.bVisible = FALSE;
    ::SetConsoleCursorInfo(outputHandle, &cursorInfo);
}


void Terminal::restorePlatform() noexcept {
    if (!_cursorStateSaved) {
        return;
    }
    const auto outputHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
    if (outputHandle == nullptr || outputHandle == INVALID_HANDLE_VALUE) {
        return;
    }
    CONSOLE_CURSOR_INFO cursorInfo{};
    if (!::GetConsoleCursorInfo(outputHandle, &cursorInfo)) {
        return;
    }
    cursorInfo.bVisible = _cursorVisible ? TRUE : FALSE;
    ::SetConsoleCursorInfo(outputHandle, &cursorInfo);
}


auto Terminal::detectScreenSize() const noexcept -> std::optional<Size> {
    const auto outputHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
    if (outputHandle == nullptr || outputHandle == INVALID_HANDLE_VALUE) {
        return std::nullopt;
    }
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (!::GetConsoleScreenBufferInfo(outputHandle, &info)) {
        return std::nullopt;
    }
    const auto width = static_cast<int>(info.srWindow.Right - info.srWindow.Left + 1);
    const auto height = static_cast<int>(info.srWindow.Bottom - info.srWindow.Top + 1);
    if (width <= 0 || height <= 0) {
        return std::nullopt;
    }
    return Size{width, height};
}


}
