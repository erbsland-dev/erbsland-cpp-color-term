// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Input.hpp"

#include <iostream>


namespace erbsland::cterm {


void Input::setMode(const Mode mode) {
    if (_mode == mode) {
        return;
    }
    handleModeChange(_mode, mode);
    _mode = mode;
}


auto Input::mode() const noexcept -> Mode {
    return _mode;
}


auto Input::read(const std::chrono::milliseconds timeout) const -> Key {
    if (mode() == Mode::ReadLine) {
        return Key::fromConsoleInput(readLine());
    }
    return readKey(timeout);
}


void Input::handleModeChange(const Mode /*previousMode*/, const Mode /*newMode*/) noexcept {
}


auto Input::readLine() -> std::string {
    std::string input;
    std::getline(std::cin, input);
    return input;
}


}
