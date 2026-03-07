// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "PosixKeyInputSessionStorage.hpp"


namespace erbsland::cterm {


auto PosixKeyInputSessionStorage::acquire() noexcept -> bool {
    ++_referenceCount;
    if (_referenceCount > 1) {
        return true;
    }
    if (tcgetattr(STDIN_FILENO, &_originalState) != 0) {
        _referenceCount = 0;
        return false;
    }
    auto rawState = _originalState;
    rawState.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
    rawState.c_cc[VMIN] = 0;
    rawState.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &rawState) != 0) {
        _referenceCount = 0;
        return false;
    }
    _hasActiveState = true;
    installSignalHandlers();
    registerAtExitHandler();
    return true;
}


void PosixKeyInputSessionStorage::release() noexcept {
    if (_referenceCount == 0) {
        return;
    }
    --_referenceCount;
    if (_referenceCount == 0) {
        restoreTerminalState();
        restoreSignalHandlers();
    }
}


auto PosixKeyInputSessionStorage::instance() noexcept -> PosixKeyInputSessionStorage & {
    static auto result = PosixKeyInputSessionStorage{};
    return result;
}


void PosixKeyInputSessionStorage::onExit() noexcept {
    auto &self = PosixKeyInputSessionStorage::instance();
    self.restoreTerminalState();
    self.restoreSignalHandlers();
}


void PosixKeyInputSessionStorage::onSignal(const int signalNumber) noexcept {
    auto &self = PosixKeyInputSessionStorage::instance();
    self.restoreTerminalState();
    self.restoreSignalHandlers();
    std::signal(signalNumber, SIG_DFL);
    std::raise(signalNumber);
    std::_Exit(128 + signalNumber);
}


void PosixKeyInputSessionStorage::registerAtExitHandler() noexcept {
    if (_hasAtExitHandler) {
        return;
    }
    std::atexit(&PosixKeyInputSessionStorage::onExit);
    _hasAtExitHandler = true;
}


void PosixKeyInputSessionStorage::installSignalHandlers() noexcept {
    if (_hasSignalHandlers) {
        return;
    }
    struct sigaction action{};
    action.sa_handler = &PosixKeyInputSessionStorage::onSignal;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    for (std::size_t index = 0; index < cSignals.size(); ++index) {
        sigaction(cSignals[index], &action, &_previousSignalActions[index]);
    }
    _hasSignalHandlers = true;
}


void PosixKeyInputSessionStorage::restoreSignalHandlers() noexcept {
    if (!_hasSignalHandlers) {
        return;
    }
    for (std::size_t index = 0; index < cSignals.size(); ++index) {
        sigaction(cSignals[index], &_previousSignalActions[index], nullptr);
    }
    _hasSignalHandlers = false;
}


void PosixKeyInputSessionStorage::restoreTerminalState() noexcept {
    if (_hasActiveState) {
        tcsetattr(STDIN_FILENO, TCSANOW, &_originalState);
        _hasActiveState = false;
    }
}


}
