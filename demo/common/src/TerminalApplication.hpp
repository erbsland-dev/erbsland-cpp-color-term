// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <erbsland/cterm/all.hpp>

#include <chrono>

namespace demo {

using namespace erbsland::cterm;

/// The base class for all demo applications
class TerminalApplication {
public:
    TerminalApplication() = default;
    virtual ~TerminalApplication() = default;

public:
    /// Initialized the demo application.
    /// This calls `onCommandLine()`, `beforeInitialize()`, `Terminal::initializeScreen()`, and `beforeRun()`.
    auto initialize(int argc, char **argv) -> int;
    /// The main loop of the application.
    /// This calls `run()` when the application continues after initialization.
    void main();
    /// Finalize the application.
    /// This calls `onShutdown()`, then `Terminal::restoreScreen()`, if the screen was initialized.
    void finalize();

public:
    /// Overwrite this method if you want to handle command line arguments.
    /// @return 0 on success to continue, or !=0 to stop the application with this exit code.
    virtual auto onCommandLine(const std::vector<std::string_view> &args) -> int;
    /// Overwrite this method to initialize the `_terminal` or `_updateSettings` before it is initialized.
    /// Does nothing by default.
    virtual void beforeInitialize();
    /// Overwrite this method to initialize application data, *after* the terminal is initialized.
    /// This method also allows an early exit, e.g. based on the command line arguments.
    /// Does nothing by default.
    /// @return 0 on success to continue, or !=0 to stop the application with this exit code,
    ///   or -1 to exit the application early with exit code 0.
    virtual auto beforeRun() -> int;
    /// Overwrite this method if you like to implement a custom main loop.
    /// The default implementation is a simple loop that waits for key input and renders the frame.
    /// If a key is pressed, `onKey()` is called. For each loop, `onRenderToBuffer()` is called, the buffer is
    /// updated on the terminal, `onAfterUpdateScreen()` is called, and `_animationCycle` is incremented.
    virtual void run();
    /// Overwrite this method to customize the shutdown behavior.
    /// Does nothing by default.
    virtual void onShutdown();
    /// Overwrite this method if you need to prepare something only when the `_buffer` size changed.
    virtual void onResize();
    /// Overwrite this method to implement the render logic to `_buffer`.
    virtual void onRenderToBuffer();
    /// Overwrite this method if you need the measured `updateScreen()` duration for diagnostics.
    /// @param duration The time spent in `Terminal::updateScreen()` for the most recent frame.
    virtual void onAfterUpdateScreen(std::chrono::nanoseconds duration);
    /// Overwrite/extend this method to implement the key handling.
    /// By default, the `q` key will quit the application.
    virtual void onKey(const Key &key);
    /// Get the loop interval in milliseconds.
    virtual auto loopInterval() const noexcept -> std::chrono::milliseconds { return std::chrono::milliseconds{90}; }

protected:
    Terminal _terminal{Size{80, 25}};
    UpdateSettings _updateSettings;
    Buffer _buffer;
    bool _quitRequested{false};
    std::size_t _animationCycle{0};
    bool _screenInitialized{false};
};

}
