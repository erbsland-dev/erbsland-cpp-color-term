// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "ScopedTerminalSession.hpp"


namespace demo {


ScopedTerminalSession::ScopedTerminalSession(
    Terminal &terminal,
    const Terminal::RefreshMode refreshMode,
    const Input::Mode inputMode,
    const bool sizeDetectionEnabled) noexcept :
    _terminal{terminal},
    _previousRefreshMode{terminal.refreshMode()},
    _previousInputMode{terminal.input().mode()},
    _previousSizeDetectionEnabled{terminal.sizeDetectionEnabled()} {

    _terminal.setRefreshMode(refreshMode);
    _terminal.setSizeDetectionEnabled(sizeDetectionEnabled);
    _terminal.initializeScreen();
    _terminal.input().setMode(inputMode);
    _initialized = true;
}


ScopedTerminalSession::~ScopedTerminalSession() {
    if (!_initialized) {
        return;
    }
    _terminal.input().setMode(_previousInputMode);
    _terminal.setRefreshMode(_previousRefreshMode);
    _terminal.setSizeDetectionEnabled(_previousSizeDetectionEnabled);
    _terminal.flush();
    _terminal.restoreScreen();
}


}
