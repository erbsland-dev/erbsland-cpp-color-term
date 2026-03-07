// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <erbsland/cterm/all.hpp>


namespace demo {


using namespace erbsland::cterm;


/// Manage terminal initialization and restoration for one interactive demo.
class ScopedTerminalSession final {
public:
    /// Configure and initialize the terminal for a demo run.
    /// @param terminal The terminal to manage.
    /// @param refreshMode The refresh mode used during the demo.
    /// @param inputMode The input mode used during the demo.
    /// @param sizeDetectionEnabled Enable or disable dynamic terminal size detection.
    explicit ScopedTerminalSession(
        Terminal &terminal,
        Terminal::RefreshMode refreshMode = Terminal::RefreshMode::Overwrite,
        Input::Mode inputMode = Input::Mode::Key,
        bool sizeDetectionEnabled = true) noexcept;

    /// Restore the previous terminal configuration.
    ~ScopedTerminalSession();

    ScopedTerminalSession(const ScopedTerminalSession &) = delete;
    auto operator=(const ScopedTerminalSession &) -> ScopedTerminalSession & = delete;
    ScopedTerminalSession(ScopedTerminalSession &&) = delete;
    auto operator=(ScopedTerminalSession &&) -> ScopedTerminalSession & = delete;

private:
    Terminal &_terminal;
    Terminal::RefreshMode _previousRefreshMode{};
    Input::Mode _previousInputMode{};
    bool _previousSizeDetectionEnabled{true};
    bool _initialized{false};
};


}
