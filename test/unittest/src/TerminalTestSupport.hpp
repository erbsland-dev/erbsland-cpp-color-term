// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <chrono>
#include <format>
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

    [[nodiscard]] auto detectScreenSize() -> std::optional<Size> override {
        _detectScreenSizeCallCount += 1;
        return _detectedScreenSize;
    }

    void emitColor(const Color color) override { _emittedColors.push_back(color); }

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
    std::vector<CursorMove> _cursorMoves;
    std::vector<bool> _cursorVisibilityChanges;
    std::vector<bool> _alternateScreenBufferChanges;
    std::vector<std::chrono::milliseconds> _readKeyTimeouts;
    std::queue<Key> _readKeyResults;
    std::queue<std::string> _readLineResults;
    std::vector<std::string> _emittedText;
};


class TerminalTestHelper : public el::UnitTest {
public:
    auto createTerminal(const std::shared_ptr<TerminalTestBackend> &backend, const Size size = Size{80, 25})
        -> std::unique_ptr<Terminal> {
        auto terminal = std::make_unique<Terminal>(size);
        terminal->setBackend(backend);
        return terminal;
    }

    auto createBuffer(const std::initializer_list<std::string_view> rows) -> Buffer {
        REQUIRE_FALSE(rows.size() == 0);
        auto width = 0;
        auto rowIndex = 0;
        for (const auto row : rows) {
            runWithContext(
                SOURCE_LOCATION(),
                [&]() {
                    REQUIRE_FALSE(row.empty());
                    if (width == 0) {
                        width = static_cast<int>(row.size());
                    } else {
                        REQUIRE_EQUAL(static_cast<int>(row.size()), width);
                    }
                },
                [&]() -> std::string {
                    return std::format(
                        "rowIndex = {} / row = \"{}\" / rowSize = {} / expectedWidth = {}",
                        rowIndex,
                        row,
                        row.size(),
                        width);
                });
            rowIndex += 1;
        }
        auto buffer = Buffer{Size{width, static_cast<int>(rows.size())}};
        auto y = 0;
        for (const auto row : rows) {
            for (auto x = 0; x < width; ++x) {
                buffer.set(Position{x, y}, Char{static_cast<char32_t>(static_cast<unsigned char>(row[x]))});
            }
            y += 1;
        }
        return buffer;
    }
};
