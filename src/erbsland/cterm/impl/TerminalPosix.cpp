// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Terminal.hpp"

#include <sys/ioctl.h>
#include <unistd.h>

#include <array>


namespace erbsland::cterm {


void Terminal::initializePlatform() noexcept {
}


void Terminal::restorePlatform() noexcept {
}


auto Terminal::detectScreenSize() const noexcept -> std::optional<Size> {
    constexpr auto cFileDescriptors = std::array{STDOUT_FILENO, STDERR_FILENO, STDIN_FILENO};
    for (const auto fileDescriptor : cFileDescriptors) {
        winsize windowSize{};
        if (::ioctl(fileDescriptor, TIOCGWINSZ, &windowSize) != 0) {
            continue;
        }
        if (windowSize.ws_col <= 0 || windowSize.ws_row <= 0) {
            continue;
        }
        return Size{windowSize.ws_col, windowSize.ws_row};
    }
    return std::nullopt;
}


}
