// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "InputWin32.hpp"

#include <conio.h>
#include <windows.h>

#include <chrono>
#include <memory>
#include <thread>


namespace erbsland::cterm {


auto InputWin32::readKey(const std::chrono::milliseconds timeout) const -> Key {
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


auto createInputForPlatform() -> std::unique_ptr<Input> {
    return std::make_unique<InputWin32>();
}


}
