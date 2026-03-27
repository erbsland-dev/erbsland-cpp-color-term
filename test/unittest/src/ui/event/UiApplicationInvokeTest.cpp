// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TerminalTestSupport.hpp"

#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace ui = erbsland::cterm::ui;

namespace ui_application_invoke_test {

class InspectableApplication : public ui::Application {
public:
    using milliseconds = std::chrono::milliseconds;

public:
    explicit InspectableApplication(TerminalPtr terminal) : ui::Application(std::move(terminal)) {}

    [[nodiscard]] auto managedThreadCountForTest() const -> std::size_t { return managedEventThreadCount(); }
    void setManagedThreadShutdownTimeoutForTest(const milliseconds timeout) { _managedThreadShutdownTimeout = timeout; }

protected:
    [[nodiscard]] auto managedEventThreadShutdownTimeout() const -> milliseconds override {
        return _managedThreadShutdownTimeout;
    }

private:
    milliseconds _managedThreadShutdownTimeout{std::chrono::milliseconds{1500}};
};


class StderrCapture final {
public:
    StderrCapture() : _previousBuffer{std::cerr.rdbuf(_buffer.rdbuf())} {}

    ~StderrCapture() { std::cerr.rdbuf(_previousBuffer); }

    [[nodiscard]] auto output() const -> std::string { return _buffer.str(); }

private:
    std::ostringstream _buffer;
    std::streambuf *_previousBuffer = nullptr;
};

}


TESTED_TARGETS(UiApplication UiEventThread UiInvocation)
class UiApplicationInvokeTest final : public UNITTEST_SUBCLASS(TerminalTestHelper) {
public:
    void testApplicationInvokeRunsQueuedCallbacksOnTheUiThread() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{20, 4});
        auto application = ui::Application{terminal};
        auto page = ui::Page::create();
        auto callbackThreadId = std::thread::id{};
        const auto uiThreadId = std::this_thread::get_id();

        page->scheduler().addSingleShot(
            [&application, &callbackThreadId]() {
                auto worker = std::thread{[&application, &callbackThreadId]() {
                    application.invoke([&application, &callbackThreadId]() {
                        callbackThreadId = std::this_thread::get_id();
                        application.quit();
                    });
                }};
                worker.join();
            },
            std::chrono::milliseconds{10});
        application.setMainPage(page);

        REQUIRE_EQUAL(application.run(), 0);
        REQUIRE_EQUAL(callbackThreadId, uiThreadId);
    }

    void testApplicationInvokePreservesFifoOrder() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{20, 4});
        auto application = ui::Application{terminal};
        auto page = ui::Page::create();
        auto callbackOrder = std::vector<char>{};

        page->scheduler().addSingleShot(
            [&application, &callbackOrder]() {
                auto worker = std::thread{[&application, &callbackOrder]() {
                    application.invoke([&callbackOrder]() { callbackOrder.push_back('A'); });
                    application.invoke([&application, &callbackOrder]() {
                        callbackOrder.push_back('B');
                        application.quit();
                    });
                }};
                worker.join();
            },
            std::chrono::milliseconds{10});
        application.setMainPage(page);

        REQUIRE_EQUAL(application.run(), 0);
        REQUIRE_EQUAL(callbackOrder, (std::vector<char>{'A', 'B'}));
    }

    void testApplicationInvokeRejectsCallsOutsideTheActiveEventLoop() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{20, 4});
        auto application = ui::Application{terminal};
        auto page = ui::Page::create();
        auto threwBeforeRun = false;
        auto threwAfterQuit = false;

        try {
            application.invoke([]() {});
        } catch (const std::logic_error &) {
            threwBeforeRun = true;
        }

        page->scheduler().addSingleShot(
            [&application, &threwAfterQuit]() {
                application.quit();
                try {
                    application.invoke([]() {});
                } catch (const std::logic_error &) {
                    threwAfterQuit = true;
                }
            },
            std::chrono::milliseconds{10});
        application.setMainPage(page);

        REQUIRE_EQUAL(application.run(), 0);
        REQUIRE(threwBeforeRun);
        REQUIRE(threwAfterQuit);
    }

    void testStoppedManagedThreadsAreRemovedFromTheManagementList() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{20, 4});
        auto application = ui_application_invoke_test::InspectableApplication{terminal};
        auto page = ui::Page::create();
        auto managedThreadCount = std::size_t{999};

        page->scheduler().addSingleShot(
            [&application]() {
                auto eventThread = application.createEventThread();
                eventThread->invoke([]() {});
                eventThread->quit();
            },
            std::chrono::milliseconds{10});
        page->scheduler().addSingleShot(
            [&application, &managedThreadCount]() {
                managedThreadCount = application.managedThreadCountForTest();
                application.quit();
            },
            std::chrono::milliseconds{40});
        application.setMainPage(page);

        REQUIRE_EQUAL(application.run(), 0);
        REQUIRE_EQUAL(managedThreadCount, std::size_t{0});
    }

    void testManagedEventThreadsQuitCooperativelyDuringShutdown() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{20, 4});
        auto application = ui_application_invoke_test::InspectableApplication{terminal};
        auto page = ui::Page::create();
        auto stopObserved = false;
        auto managedThread = ui::EventThreadPtr{};
        auto stderrCapture = ui_application_invoke_test::StderrCapture{};

        page->scheduler().addSingleShot(
            [&application, &managedThread, &stopObserved]() {
                managedThread = application.createEventThread();
                managedThread->invoke([&stopObserved](const ui::StopToken stopToken) {
                    while (!stopToken.stopRequested()) {
                        std::this_thread::sleep_for(std::chrono::milliseconds{1});
                    }
                    stopObserved = true;
                });
            },
            std::chrono::milliseconds{10});
        page->scheduler().addSingleShot([&application]() { application.quit(); }, std::chrono::milliseconds{20});
        application.setMainPage(page);

        REQUIRE_EQUAL(application.run(), 0);
        REQUIRE(managedThread != nullptr);
        REQUIRE(managedThread->waitForQuit(std::chrono::milliseconds{500}));
        REQUIRE(stopObserved);
        REQUIRE(std::string::npos == stderrCapture.output().find("timed out waiting"));
    }

    void testNonCooperativeManagedThreadsTriggerTheShutdownWarning() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{20, 4});
        auto application = ui_application_invoke_test::InspectableApplication{terminal};
        auto page = ui::Page::create();
        auto managedThread = ui::EventThreadPtr{};
        auto stderrCapture = ui_application_invoke_test::StderrCapture{};

        application.setManagedThreadShutdownTimeoutForTest(std::chrono::milliseconds{10});
        page->scheduler().addSingleShot(
            [&application, &managedThread]() {
                managedThread = application.createEventThread();
                managedThread->invoke([]() { std::this_thread::sleep_for(std::chrono::milliseconds{100}); });
            },
            std::chrono::milliseconds{10});
        page->scheduler().addSingleShot([&application]() { application.quit(); }, std::chrono::milliseconds{20});
        application.setMainPage(page);

        REQUIRE_EQUAL(application.run(), 0);
        REQUIRE(managedThread != nullptr);
        REQUIRE(std::string::npos != stderrCapture.output().find("timed out waiting"));
        REQUIRE(managedThread->waitForQuit(std::chrono::milliseconds{500}));
    }
};
