// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TerminalTestSupport.hpp"

#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/cterm/ui/event/impl/EventClockAccess.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <memory>
#include <ranges>
#include <stdexcept>
#include <thread>
#include <vector>

namespace ui = erbsland::cterm::ui;

namespace ui_scheduler_test {

class ManualClock final {
public:
    using milliseconds = std::chrono::milliseconds;

public:
    [[nodiscard]] auto now() const noexcept -> ui::EventTime { return _now; }
    void advance(const ui::EventClock::duration delay) noexcept { _now += delay; }
    [[nodiscard]] auto elapsed() const noexcept -> milliseconds {
        return std::chrono::duration_cast<milliseconds>(_now.time_since_epoch());
    }

private:
    ui::EventTime _now{};
};


class ControlledTimeBackend final : public Backend {
public:
    explicit ControlledTimeBackend(std::shared_ptr<ManualClock> clock) : _clock{std::move(clock)} {}

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
    void moveCursor(const Position pos, const MoveMode mode) override { _cursorMoves.push_back({pos, mode}); }
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
        if (_failOnZeroTimeoutRead && timeout == std::chrono::milliseconds{0}) {
            throw std::runtime_error{"Unexpected zero-timeout polling read in this scheduler regression test."};
        }
        auto advanceBy = ui::EventClock::duration{timeout} + _extraAdvanceOnNextRead;
        if (timeout == std::chrono::milliseconds{0}) {
            advanceBy += _extraAdvanceOnZeroTimeoutRead;
        }
        _clock->advance(advanceBy);
        _extraAdvanceOnNextRead = std::chrono::milliseconds{0};
        if (_readKeyResults.empty()) {
            return {};
        }
        const auto result = _readKeyResults.front();
        _readKeyResults.pop();
        return result;
    }
    [[nodiscard]] auto waitForKey() -> Key override { return readKey(std::chrono::milliseconds{1}); }
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

public:
    std::shared_ptr<ManualClock> _clock; ///< The deterministic test clock.
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
    std::vector<TerminalTestBackend::CursorMove> _cursorMoves;
    std::vector<bool> _cursorVisibilityChanges;
    std::vector<bool> _alternateScreenBufferChanges;
    std::vector<std::chrono::milliseconds> _readKeyTimeouts;
    std::queue<Key> _readKeyResults;
    std::queue<std::string> _readLineResults;
    std::vector<std::string> _emittedText;
    ui::EventClock::duration _extraAdvanceOnNextRead{};        ///< Extra time added to the next input wait.
    ui::EventClock::duration _extraAdvanceOnZeroTimeoutRead{}; ///< Extra time added to each zero-time input poll.
    bool _failOnZeroTimeoutRead = false; ///< Fail the test if the loop passes a zero-timeout key read.
};

}


TESTED_TARGETS(UiScheduler UiApplication UiDisplay)
class UiSchedulerTest final : public UNITTEST_SUBCLASS(TerminalTestHelper) {
public:
    void testSingleShotRunsOnceOnTheUiThread() {
        auto clock = std::make_shared<ui_scheduler_test::ManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ui_scheduler_test::ControlledTimeBackend>(clock);
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto application = ui::Application{terminal};
        auto page = ui::Page::create();
        auto fireCount = std::size_t{0};
        auto callbackThreadId = std::thread::id{};
        const auto uiThreadId = std::this_thread::get_id();

        page->setBackground(Char{U'A'});
        page->scheduler().addSingleShot(
            [&]() {
                callbackThreadId = std::this_thread::get_id();
                fireCount += 1;
                ui::getApplication().quit();
            },
            std::chrono::milliseconds{10});
        application.setMainPage(page);

        const auto exitCode = application.run();

        REQUIRE_EQUAL(exitCode, 0);
        REQUIRE_EQUAL(fireCount, std::size_t{1});
        REQUIRE_EQUAL(callbackThreadId, uiThreadId);
    }

    void testReschedulingSuppressesOlderQueuedScheduledActionEvents() {
        auto clock = std::make_shared<ui_scheduler_test::ManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ui_scheduler_test::ControlledTimeBackend>(clock);
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        auto application = ui::Application{terminal};
        auto page = ui::Page::create();
        auto fireCount = std::size_t{0};

        auto actionRef = page->scheduler().addSingleShot([&]() { fireCount += 100; }, std::chrono::milliseconds{30});
        page->scheduler().addSingleShot(
            [&]() {
                page->scheduler().addSingleShot([&]() { fireCount += 1; }, actionRef, std::chrono::milliseconds{10});
            },
            std::chrono::milliseconds{10});
        page->scheduler().addSingleShot([&]() { ui::getApplication().quit(); }, std::chrono::milliseconds{40});
        application.setMainPage(page);

        application.run();

        REQUIRE_EQUAL(fireCount, std::size_t{1});
    }

    void testRepeatedActionUsesFixedRateCatchUpAndStopsAfterRemove() {
        auto clock = std::make_shared<ui_scheduler_test::ManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ui_scheduler_test::ControlledTimeBackend>(clock);
        backend->_extraAdvanceOnNextRead = std::chrono::milliseconds{25};
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        auto application = ui::Application{terminal};
        auto page = ui::Page::create();
        auto fireTimes = std::vector<std::chrono::milliseconds>{};
        auto repeatingRef = ui::ScheduledActionRef{};

        repeatingRef = page->scheduler().addRepeated(
            [&]() {
                fireTimes.push_back(clock->elapsed());
                if (fireTimes.size() == 3) {
                    page->scheduler().remove(repeatingRef);
                    ui::getApplication().quit();
                }
            },
            std::chrono::milliseconds{10});
        application.setMainPage(page);

        application.run();

        REQUIRE_EQUAL(fireTimes.size(), std::size_t{3});
        REQUIRE_EQUAL(fireTimes[0], std::chrono::milliseconds{35});
        REQUIRE_EQUAL(fireTimes[1], std::chrono::milliseconds{35});
        REQUIRE_EQUAL(fireTimes[2], std::chrono::milliseconds{35});
    }

    void testSubMillisecondScheduledActionWakeUsesZeroTimeoutInputPolling() {
        auto clock = std::make_shared<ui_scheduler_test::ManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ui_scheduler_test::ControlledTimeBackend>(clock);
        backend->_extraAdvanceOnNextRead = std::chrono::milliseconds{9} + std::chrono::microseconds{500};
        backend->_extraAdvanceOnZeroTimeoutRead = std::chrono::microseconds{500};
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        auto application = ui::Application{terminal};
        auto page = ui::Page::create();
        auto fireCount = std::size_t{0};

        page->scheduler().addRepeated(
            [&]() {
                fireCount += 1;
                if (fireCount == 2) {
                    ui::getApplication().quit();
                }
            },
            std::chrono::milliseconds{10});
        application.setMainPage(page);

        const auto exitCode = application.run();

        REQUIRE_EQUAL(exitCode, 0);
        REQUIRE_EQUAL(fireCount, std::size_t{2});
        REQUIRE(
            std::ranges::find(backend->_readKeyTimeouts, std::chrono::milliseconds{0}) !=
            backend->_readKeyTimeouts.end());
    }

    void testRemoveAllCancelsQueuedActions() {
        auto clock = std::make_shared<ui_scheduler_test::ManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ui_scheduler_test::ControlledTimeBackend>(clock);
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        auto application = ui::Application{terminal};
        auto page = ui::Page::create();
        auto child = ui::Panel::create();
        auto fireCount = std::size_t{0};

        page->addChild(child);
        child->scheduler().addSingleShot([&]() { fireCount += 1; }, std::chrono::milliseconds{30});
        child->scheduler().addSingleShot([&]() { child->scheduler().removeAll(); }, std::chrono::milliseconds{10});
        page->scheduler().addSingleShot([&]() { ui::getApplication().quit(); }, std::chrono::milliseconds{40});
        application.setMainPage(page);

        application.run();

        REQUIRE_EQUAL(fireCount, std::size_t{0});
    }

    void testDestroyingASurfaceCancelsQueuedActions() {
        auto clock = std::make_shared<ui_scheduler_test::ManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ui_scheduler_test::ControlledTimeBackend>(clock);
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        auto application = ui::Application{terminal};
        auto page = ui::Page::create();
        auto child = ui::Panel::create();
        auto childWeak = ui::SurfaceWeakPtr{child};
        auto fireCount = std::size_t{0};

        child->scheduler().addSingleShot([&]() { fireCount += 1; }, std::chrono::milliseconds{20});
        page->addChild(child);
        page->scheduler().addSingleShot(
            [&]() {
                page->removeChild(child);
                child.reset();
            },
            std::chrono::milliseconds{10});
        page->scheduler().addSingleShot([&]() { ui::getApplication().quit(); }, std::chrono::milliseconds{30});
        application.setMainPage(page);

        application.run();

        REQUIRE_EQUAL(fireCount, std::size_t{0});
        REQUIRE(childWeak.expired());
    }

    void testSchedulingFromABackgroundThreadStillRunsOnTheUiThread() {
        auto clock = std::make_shared<ui_scheduler_test::ManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ui_scheduler_test::ControlledTimeBackend>(clock);
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        auto application = ui::Application{terminal};
        auto page = ui::Page::create();
        auto callbackThreadId = std::thread::id{};
        const auto uiThreadId = std::this_thread::get_id();

        auto worker = std::thread{[page, &callbackThreadId]() {
            page->scheduler().addSingleShot(
                [&callbackThreadId]() {
                    callbackThreadId = std::this_thread::get_id();
                    ui::getApplication().quit();
                },
                std::chrono::milliseconds{10});
        }};
        worker.join();
        application.setMainPage(page);

        application.run();

        REQUIRE_EQUAL(callbackThreadId, uiThreadId);
    }

    void testVisibleScheduledActionDrivenChangesTriggerPromptRedraw() {
        auto clock = std::make_shared<ui_scheduler_test::ManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ui_scheduler_test::ControlledTimeBackend>(clock);
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto application = ui::Application{terminal};
        auto page = ui::Page::create();

        page->setBackground(Char{U'A'});
        page->scheduler().addSingleShot([page]() { page->setBackground(Char{U'B'}); }, std::chrono::milliseconds{10});
        page->scheduler().addSingleShot([&]() { ui::getApplication().quit(); }, std::chrono::milliseconds{60});
        application.setMainPage(page);

        application.run();

        REQUIRE_EQUAL(backend->output(), std::string{"A\nB\n"});
    }

    void testHiddenPageScheduledActionsDoNotTriggerOutputUntilThePageIsVisibleAgain() {
        auto clock = std::make_shared<ui_scheduler_test::ManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ui_scheduler_test::ControlledTimeBackend>(clock);
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto application = ui::Application{terminal};
        auto mainPage = ui::Page::create();
        auto overlayPage = ui::Page::create();

        mainPage->setBackground(Char{U'A'});
        overlayPage->setBackground(Char{U'#'});
        mainPage->scheduler().addSingleShot(
            [overlayPage]() { ui::getApplication().display().pushPage(overlayPage); }, std::chrono::milliseconds{10});
        mainPage->scheduler().addSingleShot(
            [mainPage]() { mainPage->setBackground(Char{U'B'}); }, std::chrono::milliseconds{20});
        mainPage->scheduler().addSingleShot(
            [&]() { ui::getApplication().display().popPage(); }, std::chrono::milliseconds{60});
        mainPage->scheduler().addSingleShot([&]() { ui::getApplication().quit(); }, std::chrono::milliseconds{110});
        application.setMainPage(mainPage);

        application.run();

        REQUIRE_EQUAL(backend->output(), std::string{"A\n#\nB\n"});
    }
};
