// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../Backend.hpp"

#include <signal.h>
#include <termios.h>
#include <unistd.h>

#include <chrono>
#include <cstdint>


namespace erbsland::cterm::impl {


class PosixBackend final : public Backend {
    using clock = std::chrono::steady_clock;
    constexpr static auto cMinimumDelayBetweenScreenSizeDetection = std::chrono::milliseconds{100};
    static constexpr std::array<int, 8> cSignals = {SIGINT, SIGTERM, SIGHUP, SIGQUIT, SIGABRT, SIGSEGV, SIGILL, SIGFPE};

public:
    enum class SizeDetectionResult : uint8_t { NoTerminalAttached, NoTerminalSize, Success };

public:
    PosixBackend(); // must never be called directly
    ~PosixBackend() override;

public:
    void initializePlatform() override;
    void restorePlatform() override;
    [[nodiscard]] auto supportsColorCodes() const noexcept -> bool override;
    [[nodiscard]] auto supportsCursorCodes() const noexcept -> bool override;
    [[nodiscard]] auto detectScreenSize() -> std::optional<Size> override;
    void emitText(std::string_view text) override;
    void emitFlush() override;
    void setAlternateScreenBuffer(bool enabled) override;

    // input
    [[nodiscard]] auto inputMode() const noexcept -> Input::Mode override;
    void setInputMode(Input::Mode mode) override;
    [[nodiscard]] auto readKey(std::chrono::milliseconds timeout) -> Key override;
    [[nodiscard]] auto readLine() -> std::string override;

public:
    /// Create or access the global instance.
    static auto getOrCreate() noexcept -> BackendPtr;
    /// Access the global instance.
    static auto instance() noexcept -> PosixBackend *;
    /// Restore the platform using the global instance.
    static void restoreGlobalPlatform() noexcept;

private:
    /// Detect the screen size.
    [[nodiscard]] auto getScreenSize() -> std::pair<SizeDetectionResult, Size>;
    /// Try to detect the size for a file descriptor.
    [[nodiscard]] static auto getScreenSizeForFd(int fd) -> std::pair<SizeDetectionResult, Size>;
    /// Close the tty
    void closeTty();

    /// Initialize a key interactive session.
    void initializeKeyInputSession();
    /// Restore a key interactive session.
    void restoreKeyInputSession();

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
    static PosixBackend *_instance;   ///< The global instance of the PosixBackend.
    static std::array<struct sigaction, cSignals.size()> _previousSignalActions; ///< Previous signal actions.

    bool _isInitialized = false;                                                 ///< If the platform was initialized.
    bool _keyInputSessionActive = false;             ///< If we have an input session that needs to be restored.
    clock::time_point _lastScreenSizeDetection = {}; ///< Time of last screen size detection.
    std::optional<Size> _lastScreenSize;             ///< The last cached screen size.
    bool _firstScreenSizeDetection = true;           ///< A flag to mark the first detection, for extra effort.
    bool _hasNoTerminalAttached = false;             ///< If we definitely know that screen detection will never work.
    int _ttyFdForDetection = -1;                     ///< The open /dev/tty we use for screen size detection
    std::size_t _noSizeFailureCount = 0; ///< A failure count to escalate if we repeatedly cannot detect screen size.
    Input::Mode _inputMode{Input::Mode::ReadLine}; ///< The current input mode.
    bool _isAlternateScreenActive = false;         ///< remember if we are in alternate screen mode.
    termios _originalState{};                      ///< state backup.
};


}
