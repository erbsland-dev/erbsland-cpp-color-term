// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "ControlledTimeBackend.hpp"
#include "UiSchedulerManualClock.hpp"

#include <erbsland/cterm/theme/Theme.hpp>
#include <erbsland/cterm/theme/ThemeBuilder.hpp>
#include <erbsland/cterm/ui/event/impl/EventClockAccess.hpp>
#include <erbsland/cterm/ui/surface/Panel.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <ranges>
#include <thread>
#include <vector>

TESTED_TARGETS(UiScheduler UiApplication UiDisplay)
class UiSchedulerTest final : public UNITTEST_SUBCLASS(TerminalTestHelper) {
public:
    void testSingleShotRunsOnceOnTheUiThread() {
        auto clock = std::make_shared<UiSchedulerManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ControlledTimeBackend>(clock);
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto application = ExactSizeApplication{terminal};
        auto page = ui::Page::create();
        auto fireCount = std::size_t{0};
        auto callbackThreadId = std::thread::id{};
        const auto uiThreadId = std::this_thread::get_id();

        page->setTheme(pageTheme(U'A'));
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

    void testHiddenSurfaceScheduledActionsStillRun() {
        auto clock = std::make_shared<UiSchedulerManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ControlledTimeBackend>(clock);
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        auto application = ExactSizeApplication{terminal};
        auto page = ui::Page::create();
        auto hiddenSurface = ui::Panel::create();
        auto fireCount = std::size_t{0};

        hiddenSurface->flags().setVisible(false);
        hiddenSurface->scheduler().addSingleShot(
            [&]() {
                fireCount += 1;
                ui::getApplication().quit();
            },
            std::chrono::milliseconds{10});
        page->addSurface(hiddenSurface);
        application.setMainPage(page);

        application.run();

        REQUIRE_EQUAL(fireCount, std::size_t{1});
    }

    void testReschedulingSuppressesOlderQueuedScheduledActionEvents() {
        auto clock = std::make_shared<UiSchedulerManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ControlledTimeBackend>(clock);
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        auto application = ExactSizeApplication{terminal};
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
        auto clock = std::make_shared<UiSchedulerManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ControlledTimeBackend>(clock);
        backend->_extraAdvanceOnNextRead = std::chrono::milliseconds{25};
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        auto application = ExactSizeApplication{terminal};
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
        auto clock = std::make_shared<UiSchedulerManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ControlledTimeBackend>(clock);
        backend->_extraAdvanceOnNextRead = std::chrono::milliseconds{9} + std::chrono::microseconds{500};
        backend->_extraAdvanceOnZeroTimeoutRead = std::chrono::microseconds{500};
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        auto application = ExactSizeApplication{terminal};
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
        auto clock = std::make_shared<UiSchedulerManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ControlledTimeBackend>(clock);
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        auto application = ExactSizeApplication{terminal};
        auto page = ui::Page::create();
        auto child = ui::Panel::create();
        auto fireCount = std::size_t{0};

        page->addSurface(child);
        child->scheduler().addSingleShot([&]() { fireCount += 1; }, std::chrono::milliseconds{30});
        child->scheduler().addSingleShot([&]() { child->scheduler().removeAll(); }, std::chrono::milliseconds{10});
        page->scheduler().addSingleShot([&]() { ui::getApplication().quit(); }, std::chrono::milliseconds{40});
        application.setMainPage(page);

        application.run();

        REQUIRE_EQUAL(fireCount, std::size_t{0});
    }

    void testDestroyingASurfaceCancelsQueuedActions() {
        auto clock = std::make_shared<UiSchedulerManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ControlledTimeBackend>(clock);
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        auto application = ExactSizeApplication{terminal};
        auto page = ui::Page::create();
        auto child = ui::Panel::create();
        auto childWeak = ui::SurfaceWeakPtr{child};
        auto fireCount = std::size_t{0};

        child->scheduler().addSingleShot([&]() { fireCount += 1; }, std::chrono::milliseconds{20});
        page->addSurface(child);
        page->scheduler().addSingleShot(
            [&]() {
                page->surfaces().remove(child);
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
        auto clock = std::make_shared<UiSchedulerManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ControlledTimeBackend>(clock);
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        auto application = ExactSizeApplication{terminal};
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
        auto clock = std::make_shared<UiSchedulerManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ControlledTimeBackend>(clock);
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto application = ExactSizeApplication{terminal};
        auto page = ui::Page::create();

        page->setTheme(pageTheme(U'A'));
        page->scheduler().addSingleShot([page]() { page->setTheme(pageTheme(U'B')); }, std::chrono::milliseconds{10});
        page->scheduler().addSingleShot([&]() { ui::getApplication().quit(); }, std::chrono::milliseconds{60});
        application.setMainPage(page);

        application.run();

        REQUIRE_EQUAL(backend->output(), std::string{"A\nB\n"});
    }

    void testHiddenPageScheduledActionsDoNotTriggerOutputUntilThePageIsVisibleAgain() {
        auto clock = std::make_shared<UiSchedulerManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        const auto backend = std::make_shared<ControlledTimeBackend>(clock);
        auto terminal = std::make_shared<Terminal>(backend, Size{1, 1});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto application = ExactSizeApplication{terminal};
        auto mainPage = ui::Page::create();
        auto overlayPage = ui::Page::create();

        mainPage->setTheme(pageTheme(U'A'));
        overlayPage->setTheme(pageTheme(U'#'));
        mainPage->scheduler().addSingleShot(
            [overlayPage]() { ui::getApplication().display().pushPage(overlayPage); }, std::chrono::milliseconds{10});
        mainPage->scheduler().addSingleShot(
            [mainPage]() { mainPage->setTheme(pageTheme(U'B')); }, std::chrono::milliseconds{20});
        mainPage->scheduler().addSingleShot(
            [&]() { ui::getApplication().display().popPage(); }, std::chrono::milliseconds{60});
        mainPage->scheduler().addSingleShot([&]() { ui::getApplication().quit(); }, std::chrono::milliseconds{110});
        application.setMainPage(mainPage);

        application.run();

        REQUIRE_EQUAL(backend->output(), std::string{"A\n#\nB\n"});
    }

private:
    class ExactSizeApplication final : public ui::Application {
    public:
        using ui::Application::Application;

    protected:
        void initialize() override {
            ui::Application::initialize();
            display().setHardMinimumDisplaySize({});
            display().setMinimumDisplaySize({});
        }
    };

    [[nodiscard]] static auto pageTheme(const char32_t block) -> theme::ThemeConstPtr {
        auto builder = theme::ThemeBuilder::from(theme::Theme::dark());
        builder.edit(theme::Selector{theme::Element::Page, theme::Part::Background})
            .setBlock(theme::BlockRole::Background, block);
        return builder.build();
    }
};
