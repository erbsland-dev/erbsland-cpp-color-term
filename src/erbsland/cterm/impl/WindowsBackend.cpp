// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "WindowsBackend.hpp"


#include "WindowsSignalDispatcher.hpp"

#include <conio.h>
#include <fcntl.h>
#include <windows.h>

#include <chrono>
#include <iostream>


namespace erbsland::cterm {

auto Backend::createPlatformDefault(const TerminalFlags terminalFlags) -> BackendPtr {
    return impl::WindowsBackend::getOrCreate(terminalFlags);
}

}

namespace erbsland::cterm::impl {


std::mutex WindowsBackend::_instanceMutex;
WindowsBackend *WindowsBackend::_instance = nullptr;

WindowsBackend::WindowsBackend(TerminalFlags terminalFlags) : _terminalFlags{terminalFlags} {
    // called once per application.
    _instance = this;
    if (!_terminalFlags.has(TerminalFlag::NoSignalHandling)) {
        _signalHandler = std::make_unique<WindowsSignalDispatcher>(
            [this](const int signalNumber) -> void { handleProcessSignal(signalNumber); });
    }
}

WindowsBackend::~WindowsBackend() {
    _signalHandler.reset();
    std::scoped_lock lock{_instanceMutex};
    if (_instance != nullptr) {
        _instance->restorePlatform();
        _instance = nullptr;
    }
}

void WindowsBackend::initializePlatform() {
    enableUtf8Mode();
    enableAnsiMode();
    _cursorVisible = changeCursorVisibility(false);
    _cursorStateSaved = true;
    _initialized = true;
}

void WindowsBackend::restorePlatform() {
    if (!_initialized) {
        return;
    }
    if (_cursorStateSaved) {
        _cursorStateSaved = false;
        changeCursorVisibility(_cursorVisible);
    }
    if (_isAlternateScreenActive) {
        std::cout << "\x1b[?1049l"; // disable alternative screen buffer
    }
    std::cout << "\x1b[0m";         // restore to default color
    std::cout << "\x1b[?25h";       // make the cursor visible.
    std::cout << "\n";
    std::cout.flush();
}

auto WindowsBackend::supportsColorCodes() const noexcept -> bool {
    return true;
}

auto WindowsBackend::supportsCursorCodes() const noexcept -> bool {
    return true;
}

bool WindowsBackend::supportsCursorVisibilityCodes() const noexcept {
    return false;
}

bool WindowsBackend::supportsAlternateScreenBufferCodes() const noexcept {
    return true;
}

auto WindowsBackend::detectScreenSize() -> std::optional<Size> {
    const auto outputHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
    if (outputHandle == nullptr || outputHandle == INVALID_HANDLE_VALUE) {
        return std::nullopt;
    }
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (::GetConsoleScreenBufferInfo(outputHandle, &info) == 0) {
        return std::nullopt;
    }
    const auto width = static_cast<int>(info.srWindow.Right - info.srWindow.Left + 1);
    const auto height = static_cast<int>(info.srWindow.Bottom - info.srWindow.Top + 1);
    if (width <= 0 || height <= 0) {
        return std::nullopt;
    }
    return Size{width, height};
}

void WindowsBackend::setCursorVisible(bool visible) {
    changeCursorVisibility(visible);
}

void WindowsBackend::emitText(std::string_view text) {
    std::cout.write(text.data(), static_cast<std::streamsize>(text.size()));
}

void WindowsBackend::emitFlush() {
    std::cout.flush();
}

void WindowsBackend::setAlternateScreenBuffer(const bool enabled) {
    _isAlternateScreenActive = enabled;
}

auto WindowsBackend::inputMode() const noexcept -> Input::Mode {
    return _inputMode;
}

void WindowsBackend::setInputMode(Input::Mode mode) {
    _inputMode = mode;
}

auto WindowsBackend::readKey(std::chrono::milliseconds timeout) -> Key {
    if (_inputMode == Input::Mode::ReadLine) {
        return Key::fromConsoleInput(readLine());
    }
    using namespace std::chrono;
    const auto inputHandle = GetStdHandle(STD_INPUT_HANDLE);
    const auto timeoutMilliseconds = (timeout.count() == 0) ? INFINITE : static_cast<DWORD>(timeout.count() / 2);
    if (WaitForSingleObject(inputHandle, timeoutMilliseconds) != WAIT_OBJECT_0) {
        return {};
    }
    Key lastKey{};
    for (;;) {
        DWORD available = 0;
        if ((GetNumberOfConsoleInputEvents(inputHandle, &available) == 0) || available == 0) {
            break;
        }

        INPUT_RECORD inputRecord;
        DWORD read = 0;
        if ((ReadConsoleInputW(inputHandle, &inputRecord, 1, &read) == 0) || read != 1) {
            break;
        }

        if (inputRecord.EventType != KEY_EVENT) {
            continue;
        }

        const auto &keyEvent = inputRecord.Event.KeyEvent;
        if (keyEvent.bKeyDown == 0) {
            continue; // consume, ignore
        }

        switch (keyEvent.wVirtualKeyCode) {
        case VK_UP:
            lastKey = {Key::Up};
            break;
        case VK_DOWN:
            lastKey = {Key::Down};
            break;
        case VK_LEFT:
            lastKey = {Key::Left};
            break;
        case VK_RIGHT:
            lastKey = {Key::Right};
            break;
        case VK_RETURN:
            lastKey = {Key::Enter};
            break;
        case VK_TAB:
            lastKey = {Key::Tab};
            break;
        case VK_SPACE:
            lastKey = {Key::Space};
            break;
        case VK_ESCAPE:
            lastKey = {Key::Escape};
            break;
        case VK_BACK:
            lastKey = {Key::Backspace};
            break;
        case VK_INSERT:
            lastKey = {Key::Insert};
            break;
        case VK_DELETE:
            lastKey = {Key::Delete};
            break;
        case VK_HOME:
            lastKey = {Key::Home};
            break;
        case VK_END:
            lastKey = {Key::End};
            break;
        case VK_PRIOR:
            lastKey = {Key::PageUp};
            break;
        case VK_NEXT:
            lastKey = {Key::PageDown};
            break;
        case VK_F1:
            lastKey = {Key::F1};
            break;
        case VK_F2:
            lastKey = {Key::F2};
            break;
        case VK_F3:
            lastKey = {Key::F3};
            break;
        case VK_F4:
            lastKey = {Key::F4};
            break;
        case VK_F5:
            lastKey = {Key::F5};
            break;
        case VK_F6:
            lastKey = {Key::F6};
            break;
        case VK_F7:
            lastKey = {Key::F7};
            break;
        case VK_F8:
            lastKey = {Key::F8};
            break;
        case VK_F9:
            lastKey = {Key::F9};
            break;
        case VK_F10:
            lastKey = {Key::F10};
            break;
        case VK_F11:
            lastKey = {Key::F11};
            break;
        case VK_F12:
            lastKey = {Key::F12};
            break;
        default:
            if (keyEvent.uChar.AsciiChar != 0) {
                lastKey = {Key::Character, static_cast<char>(keyEvent.uChar.AsciiChar)};
            }
            break;
        }
    }

    return lastKey;
}

auto WindowsBackend::readLine() -> std::string {
    std::string input;
    std::getline(std::cin, input);
    return input;
}

auto WindowsBackend::getOrCreate(const TerminalFlags terminalFlags) noexcept -> BackendPtr {
    std::scoped_lock lock{_instanceMutex};
    if (_instance == nullptr) {
        return std::make_shared<WindowsBackend>(terminalFlags);
    }
    return _instance->shared_from_this();
}

auto WindowsBackend::instance() noexcept -> WindowsBackend * {
    std::scoped_lock lock(_instanceMutex);
    return _instance;
}

void WindowsBackend::restoreGlobalPlatform() noexcept {
    std::scoped_lock lock(_instanceMutex);
    if (_instance == nullptr) {
        return;
    }
    _instance->restorePlatform();
}

void WindowsBackend::enableUtf8Mode() {
    constexpr UINT cUtf8CodePage = 65001;
    ::SetConsoleCP(cUtf8CodePage);
    ::SetConsoleOutputCP(cUtf8CodePage);
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
}

void WindowsBackend::enableAnsiMode() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, mode);
}

auto WindowsBackend::changeCursorVisibility(bool visible) -> bool {
    const auto outputHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
    if (outputHandle == nullptr || outputHandle == INVALID_HANDLE_VALUE) {
        return true;
    }
    CONSOLE_CURSOR_INFO cursorInfo{};
    if (::GetConsoleCursorInfo(outputHandle, &cursorInfo) == 0) {
        return true;
    }
    bool previousState = cursorInfo.bVisible != FALSE;
    cursorInfo.bVisible = visible ? TRUE : FALSE;
    ::SetConsoleCursorInfo(outputHandle, &cursorInfo);
    return previousState;
}

void WindowsBackend::handleProcessSignal(const int exitCode) noexcept {
    restoreGlobalPlatform();
    std::_Exit(exitCode);
}


}
