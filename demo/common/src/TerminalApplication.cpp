// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TerminalApplication.hpp"

namespace demo {

auto TerminalApplication::initialize(const int argc, char **argv) -> int {
    _screenInitialized = false;
    if (const auto exitCode = onCommandLine(std::vector<std::string_view>{argv, argv + argc}); exitCode != 0) {
        return exitCode;
    }
    _updateSettings.setMinimumSize(Size{60, 20});
    _updateSettings.setMinimumSizeBackground(Char{" ", bg::Black});
    _updateSettings.setMinimumSizeMessage(
        String{"Resize the terminal to at least 60x20 cells for this demo.", Color{fg::BrightRed, bg::Black}});
    _terminal.setSafeMarginEnabled(false);
    _terminal.input().setMode(Input::Mode::Key);
    beforeInitialize();
    _terminal.initializeScreen();
    _screenInitialized = true;
    if (const auto exitCode = beforeRun(); exitCode != 0) {
        return exitCode;
    }
    return 0;
}

void TerminalApplication::main() {
    run();
}

void TerminalApplication::finalize() {
    if (!_screenInitialized) {
        return;
    }
    onShutdown();
    _terminal.restoreScreen();
    _screenInitialized = false;
}

auto TerminalApplication::onCommandLine([[maybe_unused]] const std::vector<std::string_view> &args) -> int {
    return 0;
}

void TerminalApplication::beforeInitialize() {
    // empty
}

auto TerminalApplication::beforeRun() -> int {
    return 0; // empty
}

void TerminalApplication::run() {
    while (!_quitRequested) {
        const auto key = _terminal.input().readKey(loopInterval());
        if (key.valid()) {
            onKey(key);
        }
        _terminal.testScreenSize();
        if (_buffer.size() != _terminal.size()) {
            _buffer.resize(_terminal.size());
            onResize();
        }
        onRenderToBuffer();
        const auto updateStarted = std::chrono::steady_clock::now();
        _terminal.updateScreen(_buffer, _updateSettings);
        onAfterUpdateScreen(std::chrono::steady_clock::now() - updateStarted);
        ++_animationCycle;
    }
}

void TerminalApplication::onShutdown() {
    // empty
}

void TerminalApplication::onResize() {
    // empty
}

void TerminalApplication::onRenderToBuffer() {
    // empty
}

void TerminalApplication::onAfterUpdateScreen([[maybe_unused]] const std::chrono::nanoseconds duration) {
    // empty
}

void TerminalApplication::onKey(const Key &key) {
    if (key == U'q') {
        _quitRequested = true;
    }
}

}
