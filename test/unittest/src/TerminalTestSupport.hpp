// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "TestHelper.hpp"

#include <chrono>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <string_view>
#include <vector>


class TerminalTestBackend final : public Backend {
public:
    struct CursorMove final {
        Position pos{};
        MoveMode mode = MoveMode::Absolute;

        auto operator==(const CursorMove &) const noexcept -> bool = default;
    };

public:
    void initializePlatform() override { _initializePlatformCallCount += 1; }

    void restorePlatform() override { _restorePlatformCallCount += 1; }

    [[nodiscard]] auto supportsColorCodes() const noexcept -> bool override { return _supportsColorCodes; }

    [[nodiscard]] auto supportsCursorCodes() const noexcept -> bool override { return _supportsCursorCodes; }

    [[nodiscard]] auto supportsCursorVisibilityCodes() const noexcept -> bool override {
        return _supportsCursorVisibilityCodes;
    }

    [[nodiscard]] auto supportsAlternateScreenBufferCodes() const noexcept -> bool override {
        return _supportsAlternateScreenBufferCodes;
    }

    [[nodiscard]] auto supportedCharAttributes() const noexcept -> CharAttributes override {
        return _supportedCharAttributes;
    }

    [[nodiscard]] auto supportedCharAttributeCodes() const noexcept -> CharAttributes override {
        return _supportedCharAttributeCodes;
    }

    [[nodiscard]] auto isInteractive() const noexcept -> bool override { return _isInteractive; }

    [[nodiscard]] auto detectScreenSize() -> std::optional<Size> override {
        _detectScreenSizeCallCount += 1;
        return _detectedScreenSize;
    }

    void emitColor(const Color color) override { _emittedColors.push_back(color); }

    void emitCharAttributes(const CharAttributes attributes) override { _emittedCharAttributes.push_back(attributes); }

    void moveCursor(const Position pos, const MoveMode mode) override { _cursorMoves.push_back(CursorMove{pos, mode}); }

    void clearScreen() override { _clearScreenCallCount += 1; }

    void setCursorVisible(const bool visible) override { _cursorVisibilityChanges.push_back(visible); }

    void setAlternateScreenBuffer(const bool enabled) override {
        _alternateScreenBufferChanges.push_back(enabled);
        _isAlternateScreenActive = enabled;
    }

    void emitText(const std::string_view text) override { _emittedText.emplace_back(text); }

    void emitFlush() override { _emitFlushCallCount += 1; }

    [[nodiscard]] auto inputMode() const noexcept -> Input::Mode override { return _inputMode; }

    void setInputMode(const Input::Mode mode) override {
        _setInputModeCallCount += 1;
        _inputMode = mode;
    }

    [[nodiscard]] auto readKey(const std::chrono::milliseconds timeout = {}) -> Key override {
        _readKeyCallCount += 1;
        _readKeyTimeouts.push_back(timeout);
        if (_readKeyResults.empty()) {
            return {};
        }
        const auto result = _readKeyResults.front();
        _readKeyResults.pop();
        return result;
    }

    [[nodiscard]] auto readLine() -> std::string override {
        _readLineCallCount += 1;
        if (_readLineResults.empty()) {
            return {};
        }
        auto result = _readLineResults.front();
        _readLineResults.pop();
        return result;
    }

    [[nodiscard]] auto output() const -> std::string {
        auto result = std::string{};
        for (const auto &segment : _emittedText) {
            result += segment;
        }
        return result;
    }

    void clearOutput() { _emittedText.clear(); }

    void clearRecordedOperations() {
        _emittedText.clear();
        _emittedColors.clear();
        _emittedCharAttributes.clear();
        _cursorMoves.clear();
        _clearScreenCallCount = 0;
        _cursorVisibilityChanges.clear();
        _alternateScreenBufferChanges.clear();
        _emitFlushCallCount = 0;
        _setInputModeCallCount = 0;
        _readKeyCallCount = 0;
        _readKeyTimeouts.clear();
        _readLineCallCount = 0;
    }

public:
    bool _supportsColorCodes = true;
    bool _supportsCursorCodes = true;
    bool _supportsCursorVisibilityCodes = true;
    bool _supportsAlternateScreenBufferCodes = true;
    CharAttributes _supportedCharAttributes = CharAttributes::all();
    CharAttributes _supportedCharAttributeCodes = CharAttributes::all();
    bool _isInteractive = true;
    bool _isAlternateScreenActive = false;
    std::optional<Size> _detectedScreenSize{};
    Input::Mode _inputMode = Input::Mode::ReadLine;
    int _initializePlatformCallCount = 0;
    int _restorePlatformCallCount = 0;
    int _detectScreenSizeCallCount = 0;
    int _emitFlushCallCount = 0;
    int _clearScreenCallCount = 0;
    int _setInputModeCallCount = 0;
    int _readKeyCallCount = 0;
    int _readLineCallCount = 0;
    std::vector<Color> _emittedColors;
    std::vector<CharAttributes> _emittedCharAttributes;
    std::vector<CursorMove> _cursorMoves;
    std::vector<bool> _cursorVisibilityChanges;
    std::vector<bool> _alternateScreenBufferChanges;
    std::vector<std::chrono::milliseconds> _readKeyTimeouts;
    std::queue<Key> _readKeyResults;
    std::queue<std::string> _readLineResults;
    std::vector<std::string> _emittedText;
};


class TerminalTestHelper : public BufferTestHelper {
public:
    auto createTerminal(const std::shared_ptr<TerminalTestBackend> &backend, const Size size = Size{80, 25})
        -> std::unique_ptr<Terminal> {
        auto terminal = std::make_unique<Terminal>(size);
        terminal->setBackend(backend);
        return terminal;
    }
};
