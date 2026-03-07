// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <termios.h>
#include <unistd.h>

#include <array>
#include <csignal>
#include <cstddef>
#include <cstdlib>


namespace erbsland::cterm {


class PosixKeyInputSessionStorage final {
    static constexpr std::array<int, 8> cSignals = {SIGINT, SIGTERM, SIGHUP, SIGQUIT, SIGABRT, SIGSEGV, SIGILL, SIGFPE};

public:
    [[nodiscard]] auto acquire() noexcept -> bool;
    void release() noexcept;

public:
    static auto instance() noexcept -> PosixKeyInputSessionStorage &;

private:
    static void onExit() noexcept;
    static void onSignal(int signalNumber) noexcept;
    void registerAtExitHandler() noexcept;
    void installSignalHandlers() noexcept;
    void restoreSignalHandlers() noexcept;
    void restoreTerminalState() noexcept;

private:
    int _referenceCount{0};
    bool _hasActiveState{false};
    bool _hasSignalHandlers{false};
    bool _hasAtExitHandler{false};
    termios _originalState{};
    std::array<struct sigaction, cSignals.size()> _previousSignalActions{};
};


}
