// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/cterm/ui/event/impl/EventClockAccess.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <stdexcept>
#include <thread>

namespace ui = erbsland::cterm::ui;

namespace ui_event_thread_test {

class ManualClock final {
public:
    [[nodiscard]] auto now() const noexcept -> ui::EventTime { return _now; }
    void advance(const ui::EventClock::duration duration) noexcept { _now += duration; }

private:
    ui::EventTime _now{};
};

}


TESTED_TARGETS(UiEventThread UiEventDriver UiEventScheduler)
class EventThreadTest final : public el::UnitTest {
public:
    void testInvokeRunsOnTheWorkerThread() {
        auto eventThread = ui::EventThread::create();
        auto callbackThreadId = std::thread::id{};
        const auto callerThreadId = std::this_thread::get_id();

        eventThread->invoke([&callbackThreadId]() { callbackThreadId = std::this_thread::get_id(); });
        eventThread->invoke([eventThread]() { eventThread->quit(); });

        REQUIRE(eventThread->waitForQuit(std::chrono::milliseconds{500}));
        REQUIRE_NOT_EQUAL(callbackThreadId, std::thread::id{});
        REQUIRE_NOT_EQUAL(callbackThreadId, callerThreadId);
    }

    void testInvokeDelayedRunsAfterTheConfiguredDelay() {
        auto clock = std::make_shared<ui_event_thread_test::ManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        auto eventThread = ui::EventThread::create();
        auto fireCount = std::size_t{0};

        eventThread->invokeDelayed(
            [&fireCount, eventThread]() {
                fireCount += 1;
                eventThread->quit();
            },
            std::chrono::milliseconds{20});

        std::this_thread::sleep_for(std::chrono::milliseconds{10});
        REQUIRE_EQUAL(fireCount, std::size_t{0});

        clock->advance(std::chrono::milliseconds{20});

        REQUIRE(eventThread->waitForQuit(std::chrono::milliseconds{500}));
        REQUIRE_EQUAL(fireCount, std::size_t{1});
    }

    void testQuitDrainsImmediateWorkButCancelsDelayedWork() {
        auto eventThread = ui::EventThread::create();
        auto fireCount = std::size_t{0};

        eventThread->invoke([&fireCount]() { fireCount += 1; });
        eventThread->invokeDelayed([&fireCount]() { fireCount += 100; }, std::chrono::milliseconds{100});
        eventThread->quit();

        REQUIRE(eventThread->waitForQuit(std::chrono::milliseconds{500}));
        REQUIRE_EQUAL(fireCount, std::size_t{1});
    }

    void testQuitRequestsStopOnTheRunningCallback() {
        auto eventThread = ui::EventThread::create();
        auto started = std::promise<void>{};
        auto stopObserved = std::atomic<bool>{false};

        eventThread->invoke([&started, &stopObserved](const ui::StopToken stopToken) {
            started.set_value();
            while (!stopToken.stopRequested()) {
                std::this_thread::sleep_for(std::chrono::milliseconds{1});
            }
            stopObserved.store(true, std::memory_order_release);
        });

        REQUIRE_EQUAL(started.get_future().wait_for(std::chrono::milliseconds{500}), std::future_status::ready);

        eventThread->quit();

        REQUIRE(eventThread->waitForQuit(std::chrono::milliseconds{500}));
        REQUIRE(stopObserved.load(std::memory_order_acquire));
    }

    void testAbortStopsBeforeQueuedWorkStartsAndRequestsStopOnTheRunningCallback() {
        auto eventThread = ui::EventThread::create();
        auto started = std::promise<void>{};
        auto stopObserved = std::atomic<bool>{false};
        auto queuedWorkExecuted = std::atomic<bool>{false};

        eventThread->invoke([&started, &stopObserved](const ui::StopToken stopToken) {
            started.set_value();
            while (!stopToken.stopRequested()) {
                std::this_thread::sleep_for(std::chrono::milliseconds{1});
            }
            stopObserved.store(true, std::memory_order_release);
        });
        eventThread->invoke([&queuedWorkExecuted]() { queuedWorkExecuted.store(true, std::memory_order_release); });

        REQUIRE_EQUAL(started.get_future().wait_for(std::chrono::milliseconds{500}), std::future_status::ready);

        eventThread->abort();

        REQUIRE(eventThread->waitForQuit(std::chrono::milliseconds{500}));
        REQUIRE(stopObserved.load(std::memory_order_acquire));
        REQUIRE_FALSE(queuedWorkExecuted.load(std::memory_order_acquire));
    }

    void testWaitForQuitTimesOutWhileTheThreadIsStillRunning() {
        auto eventThread = ui::EventThread::create();

        eventThread->invoke([](const ui::StopToken stopToken) {
            while (!stopToken.stopRequested()) {
                std::this_thread::sleep_for(std::chrono::milliseconds{1});
            }
        });

        REQUIRE_FALSE(eventThread->waitForQuit(std::chrono::milliseconds{10}));

        eventThread->abort();
        REQUIRE(eventThread->waitForQuit(std::chrono::milliseconds{500}));
    }

    void testWaitForQuitRethrowsCallbackFailures() {
        auto eventThread = ui::EventThread::create();

        eventThread->invoke([]() { throw std::runtime_error{"boom"}; });

        REQUIRE_THROWS_AS(std::runtime_error, eventThread->waitForQuit(std::chrono::milliseconds{500}));
        REQUIRE(eventThread->hasFailed());
    }
};
