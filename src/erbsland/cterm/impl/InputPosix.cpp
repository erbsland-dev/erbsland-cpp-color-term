// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "InputPosix.hpp"


#include "PosixKeyInputSession.hpp"

#include <sys/select.h>
#include <unistd.h>

#include <memory>


namespace erbsland::cterm {


InputPosix::~InputPosix() {
    if (_keyInputSessionActive) {
        PosixKeyInputSession::release();
        _keyInputSessionActive = false;
    }
}


void InputPosix::handleModeChange(const Mode previousMode, const Mode newMode) noexcept {
    if (previousMode == Mode::Key && newMode != Mode::Key && _keyInputSessionActive) {
        PosixKeyInputSession::release();
        _keyInputSessionActive = false;
    }
    if (previousMode != Mode::Key && newMode == Mode::Key && !_keyInputSessionActive) {
        _keyInputSessionActive = PosixKeyInputSession::acquire();
    }
}


auto InputPosix::readKey(const std::chrono::milliseconds timeout) const -> Key {
    fd_set set;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    timeval tv{};
    timeval *ptv = nullptr;
    if (timeout.count() > 0) {
        tv.tv_sec = timeout.count() / 1000;
        tv.tv_usec = (static_cast<int>(timeout.count()) % 1000) * 1000;
        ptv = &tv;
    }
    std::string consoleInput;
    if (select(STDIN_FILENO + 1, &set, nullptr, nullptr, ptv) > 0) {
        char buf[32];
        ssize_t len = ::read(STDIN_FILENO, buf, sizeof(buf));
        if (len > 0) {
            consoleInput.assign(buf, static_cast<std::size_t>(len));
        }
        if (!consoleInput.empty() && consoleInput[0] == '\x1b') {
            while (true) {
                FD_ZERO(&set);
                FD_SET(STDIN_FILENO, &set);
                tv.tv_sec = 0;
                tv.tv_usec = 1000;
                if (select(STDIN_FILENO + 1, &set, nullptr, nullptr, &tv) > 0) {
                    len = ::read(STDIN_FILENO, buf, sizeof(buf));
                    if (len > 0) {
                        consoleInput.append(buf, static_cast<std::size_t>(len));
                    }
                } else {
                    break;
                }
            }
        }
    }
    return Key::fromConsoleInput(consoleInput);
}


auto createInputForPlatform() -> std::unique_ptr<Input> {
    return std::make_unique<InputPosix>();
}


}
