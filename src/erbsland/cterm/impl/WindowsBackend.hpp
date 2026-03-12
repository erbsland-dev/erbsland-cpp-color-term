// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../Backend.hpp"

#include <signal.h>

#include <csignal>
#include <mutex>


namespace erbsland::cterm::impl {


class WindowsBackend : public Backend {
public:
    static constexpr std::array<int, 8> cSignals = {SIGINT, SIGTERM, SIGABRT, SIGSEGV, SIGILL, SIGFPE};
    using SignalHandler = void (*)(int);

public:
    WindowsBackend();
    ~WindowsBackend() override;

public: // implement backend
    void initializePlatform() override;
    void restorePlatform() override;
    [[nodiscard]] auto supportsColorCodes() const noexcept -> bool override;
    [[nodiscard]] auto supportsCursorCodes() const noexcept -> bool override;
    [[nodiscard]] auto supportsCursorVisibilityCodes() const noexcept -> bool override;
    [[nodiscard]] auto supportsAlternateScreenBufferCodes() const noexcept -> bool override;
    [[nodiscard]] auto detectScreenSize() -> std::optional<Size> override;
    void setCursorVisible(bool visible) override;
    void emitText(std::string_view text) override;
    void emitFlush() override;
    void setAlternateScreenBuffer(bool enabled) override;

public: // input
    [[nodiscard]] auto inputMode() const noexcept -> Input::Mode override;
    void setInputMode(Input::Mode mode) override;
    [[nodiscard]] auto readKey(std::chrono::milliseconds timeout) -> Key override;
    [[nodiscard]] auto readLine() -> std::string override;

public:
    /// Create or access the global instance.
    static auto getOrCreate() noexcept -> BackendPtr;
    /// Access the global instance.
    static auto instance() noexcept -> WindowsBackend *;
    /// Restore the platform using the global instance.
    static void restoreGlobalPlatform() noexcept;

private:
    /// Enabled UTF-8 in the Windows terminal.
    void enableUtf8Mode();
    /// Enables ANSI mode in the Windows terminal.
    void enableAnsiMode();
    /// Change the cursor visibility.
    /// @param visible true to show the cursor, false to hide it
    /// @return The previous cursor visibility state
    auto changeCursorVisibility(bool visible) -> bool;

    /// Register all exit handlers
    void registerExitHandlers();
    /// Unregister all exit handlers
    void unregisterExitHandlers();

    /// Registered exit handler.
    static void onExit() noexcept;
    /// Registered signal handler.
    static void onSignal(int signalNumber) noexcept;

private:
    static std::mutex _instanceMutex; ///< The mutex to protect the instance.
    static WindowsBackend *_instance;   ///< The global instance of the PosixBackend.
    static std::array<SignalHandler, cSignals.size()> _previousSignalHandlers; ///< Previous signal handlers.

    bool _initialized{false}; ///< If the platform was initialized.
    bool _cursorStateSaved{false}; ///< If the backend saved the cursor state.
    bool _cursorVisible{true}; ///< The current cursor visibility state.
    Input::Mode _inputMode{Input::Mode::ReadLine}; ///< The current input mode.
    bool _isAlternateScreenActive{false}; ///< Flag if the alternate screen is active.
};

}
