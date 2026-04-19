// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "LogViewerApp.hpp"

#include <algorithm>
#include <array>
#include <format>

namespace demo::logviewer {

void LogViewerApp::beforeInitialize() {
    _updateSettings.setMinimumSize(Size{58, 12});
    _updateSettings.setMinimumSizeBackground(Char{" ", bg::Black});
    _updateSettings.setMinimumSizeMessage(
        String{"Resize the terminal to at least 58x12 cells for the log viewer.", Color{fg::BrightWhite, bg::Black}});
}

auto LogViewerApp::beforeRun() -> int {
    scheduleNextMessage();
    return 0;
}

auto LogViewerApp::canvasSize() const noexcept -> Size {
    if (_buffer.size().isZero()) {
        return _terminal.size().componentMax(_updateSettings.minimumSize());
    }
    return _buffer.size();
}

void LogViewerApp::onKey(const Key &key) {
    const auto viewSize = contentRectForBuffer(canvasSize()).size();
    if (key == U'q' || key == Key::Escape) {
        _quitRequested = true;
    } else if (key == U'+' || key == U'=') {
        adjustDelayPreset(-1);
    } else if (key == U'-') {
        adjustDelayPreset(1);
    } else if (key == U'f') {
        _followMode = true;
        _viewOffset =
            clampViewOffset(Position{0, _logBuffer->size().height() - viewSize.height()}, viewSize, _logBuffer->size());
    } else if (key == Key::Left) {
        _followMode = false;
        _viewOffset = clampViewOffset(_viewOffset + Position{-1, 0}, viewSize, _logBuffer->size());
    } else if (key == Key::Right) {
        _followMode = false;
        _viewOffset = clampViewOffset(_viewOffset + Position{1, 0}, viewSize, _logBuffer->size());
    } else if (key == Key::Up) {
        _followMode = false;
        _viewOffset = clampViewOffset(_viewOffset + Position{0, -1}, viewSize, _logBuffer->size());
    } else if (key == Key::Down) {
        _followMode = false;
        _viewOffset = clampViewOffset(_viewOffset + Position{0, 1}, viewSize, _logBuffer->size());
    }
}

void LogViewerApp::adjustDelayPreset(const int delta) noexcept {
    const auto presets = delayPresets();
    const auto maximumIndex = static_cast<int>(presets.size()) - 1;
    _delayPresetIndex =
        static_cast<std::size_t>(std::clamp(static_cast<int>(_delayPresetIndex) + delta, 0, maximumIndex));
    _nextMessageAt = std::chrono::steady_clock::now() + randomDelay();
}

void LogViewerApp::scheduleNextMessage() noexcept {
    if (_nextMessageAt == std::chrono::steady_clock::time_point{}) {
        _nextMessageAt = std::chrono::steady_clock::now() + randomDelay();
    } else {
        _nextMessageAt += randomDelay();
    }
}

void LogViewerApp::appendGeneratedMessage() {
    renderLogMessage(generateLogMessage());
    _messageCount += 1;
}

void LogViewerApp::onRenderToBuffer() {
    auto now = std::chrono::steady_clock::now();
    while (!_quitRequested && now >= _nextMessageAt) {
        appendGeneratedMessage();
        scheduleNextMessage();
        now = std::chrono::steady_clock::now();
    }
    _buffer.fill(Char{" ", bg::Black});
    const auto outerRect = Rectangle{0, 0, _buffer.size().width(), _buffer.size().height()};
    const auto titleRect = Rectangle{2, 1, _buffer.size().width() - 4, 1};
    const auto contentRect = contentRectForBuffer(_buffer.size());
    const auto footerRect = Rectangle{2, _buffer.size().height() - 2, _buffer.size().width() - 4, 1};
    _buffer.drawFrame(outerRect, FrameStyle::LightWithRoundedCorners);
    drawHeader(titleRect);
    drawLogView(contentRect);
    drawFooter(footerRect);
}

void LogViewerApp::drawHeader(const Rectangle rect) {
    _buffer.drawText(
        std::format(
            "Log Viewer  |  CursorBuffer {}x{} / 250x500  |  {} mode  |  delay {}",
            _logBuffer->size().width(),
            _logBuffer->size().height(),
            _followMode ? "follow" : "manual",
            delayPresets()[_delayPresetIndex].label),
        rect,
        Alignment::CenterLeft,
        Color{fg::BrightWhite, bg::Black});
    _buffer.drawText(
        std::format("entries {:04d}", static_cast<int>(_messageCount)),
        rect,
        Alignment::CenterRight,
        Color{fg::BrightCyan, bg::Black});
}

void LogViewerApp::drawFooter(const Rectangle rect) {
    _buffer.fill(rect, Char{" ", bg::BrightBlack});
    auto footer = String{};
    footer.append(
        fg::BrightCyan,
        Key{Key::Left}.toDisplayText(),
        " ",
        Key{Key::Right}.toDisplayText(),
        " ",
        Key{Key::Up}.toDisplayText(),
        " ",
        Key{Key::Down}.toDisplayText(),
        fg::BrightWhite,
        " pan  ",
        fg::BrightYellow,
        "[+][-]",
        fg::BrightWhite,
        " timing  ",
        fg::BrightYellow,
        "[F]",
        fg::BrightWhite,
        " follow newest  ",
        fg::BrightYellow,
        "[Q]",
        " ",
        Key{Key::Escape}.toDisplayText(),
        fg::BrightWhite,
        " quit  ",
        fg::BrightGreen,
        std::format("view ({}, {})", _logView.viewRect().x1(), _logView.viewRect().y1()));
    _buffer.drawText(Text{footer, rect, Alignment::CenterLeft});
}

void LogViewerApp::drawLogView(const Rectangle rect) {
    _buffer.fill(rect, Char{" ", Color{fg::Default, bg::Black}});
    updateView(rect.size());
    _buffer.drawBuffer(_logView, rect);
}

void LogViewerApp::updateView(const Size viewSize) noexcept {
    if (_followMode) {
        _viewOffset =
            clampViewOffset(Position{0, _logBuffer->size().height() - viewSize.height()}, viewSize, _logBuffer->size());
    } else {
        _viewOffset = clampViewOffset(_viewOffset, viewSize, _logBuffer->size());
    }
    _logView.setViewRect(Rectangle{_viewOffset, viewSize});
}

void LogViewerApp::renderLogMessage(const LogMessage &message) {
    const auto lineBreak = message.text.find('\n');
    if (lineBreak == std::string::npos) {
        renderInitialLine(nextTimestamp(), message.level, message.text);
        return;
    }
    renderInitialLine(nextTimestamp(), message.level, std::string_view{message.text}.substr(0, lineBreak));
    auto lineStart = lineBreak + 1;
    while (lineStart < message.text.size()) {
        const auto nextBreak = message.text.find('\n', lineStart);
        const auto lineLength =
            nextBreak == std::string::npos ? message.text.size() - lineStart : nextBreak - lineStart;
        renderContinuationLine(std::string_view{message.text}.substr(lineStart, lineLength));
        if (nextBreak == std::string::npos) {
            return;
        }
        lineStart = nextBreak + 1;
    }
    if (!message.text.empty() && message.text.back() == '\n') {
        renderContinuationLine("");
    }
}

auto LogViewerApp::shouldCopyCell(const Char &cell) noexcept -> bool {
    const auto color = cell.color();
    const auto hasDefaultColors = color.fg() == fg::Default && color.bg() == bg::Default;
    if (!hasDefaultColors) {
        return true;
    }
    return !(cell.isEmpty() || cell == U' ');
}

void LogViewerApp::renderInitialLine(
    const std::string_view timestamp, const LogLevel level, const std::string_view text) {
    _logBuffer->setColor(logLevelColor(level));
    auto line = String{timestamp, Color{fg::BrightWhite, bg::Black}};
    line += String{" ", Color{fg::White, bg::Black}};
    line += String{logTypeCode(level), logLevelColor(level)};
    line += String{" ", logLevelColor(level)};
    line += String{text, logLevelColor(level)};
    _logBuffer->printParagraph(line, initialLineOptions());
}

void LogViewerApp::renderContinuationLine(const std::string_view text) {
    _logBuffer->setColor(Color{fg::White, bg::Black});
    _logBuffer->printParagraph(String{text, Color{fg::Inherited, bg::Inherited}}, continuationLineOptions());
}

auto LogViewerApp::generateLogMessage() -> LogMessage {
    const auto level = randomLogLevel();
    const auto lengthRoll = randomInt(1, 100);
    if (lengthRoll <= 80) {
        return LogMessage{level, generateShortMessage(level)};
    }
    if (lengthRoll <= 95) {
        return LogMessage{level, generateLongMessage(level)};
    }
    return LogMessage{level, generateMultilineMessage(level)};
}

auto LogViewerApp::generateShortMessage(const LogLevel level) -> std::string {
    const auto method = sample(methodChoices());
    const auto route = sample(routeChoices());
    const auto staticRoute = sample(staticRouteChoices());
    const auto backend = sample(backendChoices());
    const auto cache = sample(cacheChoices());
    const auto ipAddress = randomIpAddress();
    auto message = std::string{};
    switch (level) {
    case LogLevel::Trace:
        message = std::format(
            "{} {} {} on {} with {} for {} in {} ms",
            method,
            staticRoute,
            sample(traceChoices()),
            backend,
            cache,
            ipAddress,
            randomInt(2, 19));
        break;
    case LogLevel::Info:
        message = std::format("{} {} completed with 200 for {} in {} ms", method, route, ipAddress, randomInt(21, 180));
        break;
    case LogLevel::Warning:
        message = std::format(
            "{} {} {} for {} after {} ms", method, route, sample(warningChoices()), ipAddress, randomInt(180, 950));
        break;
    case LogLevel::Error:
        message = std::format("{} {} {} via {}", method, route, sample(errorChoices()), backend);
        break;
    }
    if (message.size() > 80) {
        message.resize(80);
    }
    return message;
}

auto LogViewerApp::generateLongMessage(const LogLevel level) -> std::string {
    auto message = generateShortMessage(level);
    const auto targetLength = randomInt(300, 600);
    while (true) {
        const auto clause = std::format(
            "; request-id {}; upstream {}; user-agent {}; rate-limit bucket {}; payload {} bytes; cache state {}; "
            "TLS resume {}; compression {}; forwarded-for {}; origin latency {} ms",
            randomRequestId(),
            sample(backendChoices()),
            sample(userAgentChoices()),
            randomInt(1, 16),
            randomInt(820, 64'000),
            sample(cacheChoices()),
            randomInt(0, 1) == 0 ? "hit" : "miss",
            randomInt(0, 1) == 0 ? "brotli" : "gzip",
            randomIpAddress(),
            randomInt(8, 430));
        if (static_cast<int>(message.size() + clause.size()) > targetLength &&
            static_cast<int>(message.size()) >= 300) {
            break;
        }
        message += clause;
        if (static_cast<int>(message.size()) >= targetLength) {
            break;
        }
    }
    return message;
}

auto LogViewerApp::generateMultilineMessage(const LogLevel level) -> std::string {
    return std::format(
        "{}\nroute: {}\nrequest-id: {}\nclient: {} via {}\nuser-agent: {}\nextra: retry={}, cache={}, worker={}",
        generateShortMessage(level),
        sample(routeChoices()),
        randomRequestId(),
        randomIpAddress(),
        sample(backendChoices()),
        sample(userAgentChoices()),
        randomInt(0, 3),
        sample(cacheChoices()),
        randomInt(1, 24));
}

auto LogViewerApp::nextTimestamp() -> std::string {
    const auto result = std::format("{:%F %T}", _logTimestamp);
    _logTimestamp += randomTimestampStep();
    return result;
}

auto LogViewerApp::randomLogLevel() -> LogLevel {
    const auto roll = randomInt(1, 100);
    if (roll <= 80) {
        return LogLevel::Trace;
    }
    if (roll <= 90) {
        return LogLevel::Info;
    }
    if (roll <= 96) {
        return LogLevel::Warning;
    }
    return LogLevel::Error;
}

auto LogViewerApp::randomInt(const int minimum, const int maximum) -> int {
    return std::uniform_int_distribution<int>{minimum, maximum}(_rng);
}

auto LogViewerApp::randomDelay() -> std::chrono::milliseconds {
    const auto preset = delayPresets()[_delayPresetIndex];
    return std::chrono::milliseconds{randomInt(preset.minimumMs, preset.maximumMs)};
}

auto LogViewerApp::randomTimestampStep() -> std::chrono::seconds {
    return std::chrono::seconds{randomInt(3, 95)};
}

auto LogViewerApp::randomRequestId() -> std::string {
    const auto value = std::uniform_int_distribution<uint32_t>{0U, 0xffff'ffffU}(_rng);
    return std::format("req-{:08x}", value);
}

auto LogViewerApp::randomIpAddress() -> std::string {
    return std::format("203.0.113.{}", randomInt(2, 254));
}

auto LogViewerApp::sample(const std::span<const std::string_view> values) -> std::string_view {
    return values[static_cast<std::size_t>(randomInt(0, static_cast<int>(values.size()) - 1))];
}

auto LogViewerApp::initialLineOptions() -> const ParagraphOptions & {
    static const auto cOptions = [] {
        auto options = ParagraphOptions{};
        options.setWrappedLineIndent(22);
        options.setLineBreakEndMark(String{U"⤦"});
        options.setLineBreakStartMark(String{U"⤥ "});
        options.setMaximumLineWraps(2);
        options.setParagraphEllipsisMark(String{"(...)"});
        return options;
    }();
    return cOptions;
}

auto LogViewerApp::continuationLineOptions() -> const ParagraphOptions & {
    static const auto cOptions = [] {
        auto options = ParagraphOptions{};
        options.setFirstLineIndent(4);
        options.setWrappedLineIndent(4);
        options.setLineBreakEndMark(String{U"⤦"});
        options.setLineBreakStartMark(String{U"⤥ "});
        options.setMaximumLineWraps(2);
        options.setParagraphEllipsisMark(String{"(...)"});
        return options;
    }();
    return cOptions;
}

auto LogViewerApp::contentRectForBuffer(const Size bufferSize) noexcept -> Rectangle {
    return Rectangle{2, 3, std::max(1, bufferSize.width() - 4), std::max(1, bufferSize.height() - 6)};
}

auto LogViewerApp::clampViewOffset(const Position offset, const Size viewSize, const Size contentSize) noexcept
    -> Position {
    const auto maxX = std::max(0, contentSize.width() - viewSize.width());
    const auto maxY = std::max(0, contentSize.height() - viewSize.height());
    return {std::clamp(offset.x(), 0, maxX), std::clamp(offset.y(), 0, maxY)};
}

auto LogViewerApp::logLevelColor(const LogLevel level) noexcept -> Color {
    switch (level) {
    case LogLevel::Trace:
        return {fg::White, bg::Black};
    case LogLevel::Info:
        return {fg::BrightBlue, bg::Black};
    case LogLevel::Warning:
        return {fg::BrightYellow, bg::Black};
    case LogLevel::Error:
        return {fg::BrightRed, bg::Black};
    }
    return Color::reset();
}

auto LogViewerApp::logTypeCode(const LogLevel level) noexcept -> std::string_view {
    switch (level) {
    case LogLevel::Trace:
        return "TRC";
    case LogLevel::Info:
        return "INF";
    case LogLevel::Warning:
        return "WRN";
    case LogLevel::Error:
        return "ERR";
    }
    return "UNK";
}

auto LogViewerApp::delayPresets() noexcept -> std::span<const DelayPreset> {
    static constexpr auto cValues = std::array{
        DelayPreset{10, 100, "10-100 ms"},
        DelayPreset{50, 500, "50-500 ms"},
        DelayPreset{100, 1000, "100-1000 ms"},
        DelayPreset{200, 2000, "200-2000 ms"},
    };
    return cValues;
}

auto LogViewerApp::methodChoices() noexcept -> std::span<const std::string_view> {
    static constexpr auto cValues = std::array{
        std::string_view{"GET"},
        std::string_view{"POST"},
        std::string_view{"PUT"},
        std::string_view{"DELETE"},
    };
    return cValues;
}

auto LogViewerApp::routeChoices() noexcept -> std::span<const std::string_view> {
    static constexpr auto cValues = std::array{
        std::string_view{"/"},
        std::string_view{"/healthz"},
        std::string_view{"/checkout"},
        std::string_view{"/api/v1/orders"},
        std::string_view{"/api/v1/payments"},
        std::string_view{"/api/v1/profile"},
        std::string_view{"/admin/reports/daily"},
        std::string_view{"/assets/app.bundle.js"},
    };
    return cValues;
}

auto LogViewerApp::staticRouteChoices() noexcept -> std::span<const std::string_view> {
    static constexpr auto cValues = std::array{
        std::string_view{"/assets/site.css"},
        std::string_view{"/assets/app.bundle.js"},
        std::string_view{"/images/logo.svg"},
        std::string_view{"/fonts/ibm-plex.woff2"},
    };
    return cValues;
}

auto LogViewerApp::backendChoices() noexcept -> std::span<const std::string_view> {
    static constexpr auto cValues = std::array{
        std::string_view{"edge-gw-1"},
        std::string_view{"edge-gw-2"},
        std::string_view{"api-eu-1"},
        std::string_view{"api-eu-2"},
        std::string_view{"payments-us-1"},
        std::string_view{"search-eu-3"},
    };
    return cValues;
}

auto LogViewerApp::cacheChoices() noexcept -> std::span<const std::string_view> {
    static constexpr auto cValues = std::array{
        std::string_view{"warm cache"},
        std::string_view{"cold cache"},
        std::string_view{"stale-if-error"},
        std::string_view{"revalidated cache"},
    };
    return cValues;
}

auto LogViewerApp::userAgentChoices() noexcept -> std::span<const std::string_view> {
    static constexpr auto cValues = std::array{
        std::string_view{"Mozilla/5.0 Chrome/135"},
        std::string_view{"curl/8.9.1"},
        std::string_view{"Firefox/139.0"},
        std::string_view{"HealthChecker/2.4"},
    };
    return cValues;
}

auto LogViewerApp::warningChoices() noexcept -> std::span<const std::string_view> {
    static constexpr auto cValues = std::array{
        std::string_view{"needed a retry"},
        std::string_view{"hit the slow-path cache refresh"},
        std::string_view{"waited for an upstream reconnect"},
        std::string_view{"served stale content while the origin recovered"},
    };
    return cValues;
}

auto LogViewerApp::errorChoices() noexcept -> std::span<const std::string_view> {
    static constexpr auto cValues = std::array{
        std::string_view{"failed with 502 after upstream reset"},
        std::string_view{"failed with 503 because all workers were busy"},
        std::string_view{"aborted after TLS negotiation failed"},
        std::string_view{"returned 500 after the session store timed out"},
    };
    return cValues;
}

auto LogViewerApp::traceChoices() noexcept -> std::span<const std::string_view> {
    static constexpr auto cValues = std::array{
        std::string_view{"header normalization complete"},
        std::string_view{"route candidate matched"},
        std::string_view{"gzip dictionary selected"},
        std::string_view{"session cookie decoded"},
    };
    return cValues;
}

}
