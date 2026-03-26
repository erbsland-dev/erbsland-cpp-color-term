// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <erbsland/cterm/all.hpp>

#include <chrono>
#include <memory>
#include <random>
#include <span>
#include <string>


namespace demo::logviewer {


using namespace erbsland::cterm;


/// Demonstrate a live log viewport backed by a growing cursor buffer.
class LogViewerApp final {
public:
    /// Run the demo until the user quits.
    void run();

private:
    enum class LogLevel : uint8_t {
        Trace,
        Info,
        Warning,
        Error,
    };

    struct LogMessage final {
        LogLevel level{};
        std::string text;
    };

    struct DelayPreset final {
        int minimumMs{};
        int maximumMs{};
        std::string_view label;
    };

private:
    [[nodiscard]] auto canvasSize() const noexcept -> Size;
    void handleKey(const Key &key) noexcept;
    void adjustDelayPreset(int delta) noexcept;
    void scheduleNextMessage() noexcept;
    void appendGeneratedMessage();
    void renderFrame();
    void drawHeader(Rectangle rect);
    void drawFooter(Rectangle rect);
    void drawLogView(Rectangle rect);
    void updateView(Size viewSize) noexcept;
    void renderLogMessage(const LogMessage &message);
    [[nodiscard]] static auto shouldCopyCell(const Char &cell) noexcept -> bool;
    void renderInitialLine(std::string_view timestamp, LogLevel level, std::string_view text);
    void renderContinuationLine(std::string_view text);
    [[nodiscard]] auto generateLogMessage() -> LogMessage;
    [[nodiscard]] auto generateShortMessage(LogLevel level) -> std::string;
    [[nodiscard]] auto generateLongMessage(LogLevel level) -> std::string;
    [[nodiscard]] auto generateMultilineMessage(LogLevel level) -> std::string;
    [[nodiscard]] auto nextTimestamp() -> std::string;
    [[nodiscard]] auto randomLogLevel() -> LogLevel;
    [[nodiscard]] auto randomInt(int minimum, int maximum) -> int;
    [[nodiscard]] auto randomDelay() -> std::chrono::milliseconds;
    [[nodiscard]] auto randomTimestampStep() -> std::chrono::seconds;
    [[nodiscard]] auto randomRequestId() -> std::string;
    [[nodiscard]] auto randomIpAddress() -> std::string;
    [[nodiscard]] auto sample(std::span<const std::string_view> values) -> std::string_view;
    [[nodiscard]] static auto initialLineOptions() -> const ParagraphOptions &;
    [[nodiscard]] static auto continuationLineOptions() -> const ParagraphOptions &;
    [[nodiscard]] static auto contentRectForBuffer(Size bufferSize) noexcept -> Rectangle;
    [[nodiscard]] static auto clampViewOffset(Position offset, Size viewSize, Size contentSize) noexcept -> Position;
    [[nodiscard]] static auto logLevelColor(LogLevel level) noexcept -> Color;
    [[nodiscard]] static auto logTypeCode(LogLevel level) noexcept -> std::string_view;
    [[nodiscard]] static auto delayPresets() noexcept -> std::span<const DelayPreset>;
    [[nodiscard]] static auto methodChoices() noexcept -> std::span<const std::string_view>;
    [[nodiscard]] static auto routeChoices() noexcept -> std::span<const std::string_view>;
    [[nodiscard]] static auto staticRouteChoices() noexcept -> std::span<const std::string_view>;
    [[nodiscard]] static auto backendChoices() noexcept -> std::span<const std::string_view>;
    [[nodiscard]] static auto cacheChoices() noexcept -> std::span<const std::string_view>;
    [[nodiscard]] static auto userAgentChoices() noexcept -> std::span<const std::string_view>;
    [[nodiscard]] static auto warningChoices() noexcept -> std::span<const std::string_view>;
    [[nodiscard]] static auto errorChoices() noexcept -> std::span<const std::string_view>;
    [[nodiscard]] static auto traceChoices() noexcept -> std::span<const std::string_view>;

private:
    Terminal _terminal{Size{96, 28}};
    UpdateSettings _updateSettings;
    Buffer _buffer;
    std::shared_ptr<CursorBuffer> _logBuffer = std::make_shared<CursorBuffer>(
        Size{250, 10}, CursorBuffer::OverflowMode::ExpandThenShift, Size{250, 500}, Char{U' ', fg::Default, bg::Black});
    BufferView _logView{_logBuffer, Rectangle{0, 0, 1, 1}};
    std::mt19937 _rng{std::random_device{}()};
    std::chrono::steady_clock::time_point _nextMessageAt{};
    std::chrono::sys_seconds _logTimestamp{
        std::chrono::sys_days{std::chrono::year{2026} / std::chrono::March / 26} + std::chrono::hours{9}};
    Position _viewOffset{0, 0};
    std::size_t _messageCount{0};
    std::size_t _delayPresetIndex{2};
    bool _followMode{true};
    bool _quitRequested{false};
};


}
