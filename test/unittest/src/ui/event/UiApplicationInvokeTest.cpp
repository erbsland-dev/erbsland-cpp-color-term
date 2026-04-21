// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "InspectableApplication.hpp"
#include "StderrCapture.hpp"

#include "support/TerminalTestHelper.hpp"

#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <chrono>
#include <format>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

class LifecycleApplication final : public ui::Application {
public:
    LifecycleApplication(int argc, char *argv[], TerminalPtr terminal, std::vector<std::string> &log) :
        Application{argc, argv, std::move(terminal)}, _log{log} {}

protected:
    void setupUi() override {
        _log.emplace_back("setupUi");
        auto page = ui::Page::create();
        auto quitAction = ui::Action::create("quit");
        quitAction->setKeys(U'q');
        quitAction->setTriggerFn([]() -> void { ui::getApplication().quit(); });
        page->actions().add(quitAction);
        _host = ui::Panel::create();
        page->addSurface(_host);
        setMainPage(page);
    }

    void initializeTerminal() override {
        _log.emplace_back("initializeTerminal");
        Application::initializeTerminal();
    }

    auto processCommandLineArguments(const CommandLineArgs &args) -> ExitCode override {
        _log.emplace_back("processCommandLineArguments");
        _host->addSurface(ui::TextBox::create(args.size() > 1 ? args[1] : "Ready"));
        return cExitCodeContinue;
    }

    void initialize() override {
        _log.emplace_back(std::format("initialize:{}", _host->surfaces().size()));
        Application::initialize();
    }

private:
    std::vector<std::string> &_log;
    ui::PanelPtr _host;
};

class EarlyExitApplication final : public ui::Application {
public:
    EarlyExitApplication(int argc, char *argv[], TerminalPtr terminal, bool &initializeCalled) :
        Application{argc, argv, std::move(terminal)}, _initializeCalled{initializeCalled} {}

protected:
    void setupUi() override {
        auto page = ui::Page::create();
        setMainPage(page);
    }

    auto processCommandLineArguments(const CommandLineArgs &) -> ExitCode override { return 7; }

    void initialize() override {
        _initializeCalled = true;
        Application::initialize();
    }

private:
    bool &_initializeCalled;
};

TESTED_TARGETS(UiApplication UiEventThread UiInvocation)
class UiApplicationInvokeTest final : public UNITTEST_SUBCLASS(TerminalTestHelper) {
public:
    void testApplicationLifecycleProcessesArgumentsBeforeInitializingUiSystem() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{20, 4});
        auto log = std::vector<std::string>{};
        char arg0[] = "lifecycle-test";
        char arg1[] = "Loaded";
        char *argv[] = {arg0, arg1};
        auto application = LifecycleApplication{2, argv, terminal, log};

        backend->_readKeyResults.push(Key{Key::Character, U'q'});

        REQUIRE_EQUAL(application.run(), 0);
        REQUIRE_EQUAL(
            log,
            (std::vector<std::string>{"setupUi", "initializeTerminal", "processCommandLineArguments", "initialize:1"}));
    }

    void testApplicationRestoresTerminalWhenCommandLineProcessingExitsEarly() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{20, 4});
        auto initializeCalled = false;
        char arg0[] = "early-exit-test";
        char *argv[] = {arg0};
        auto application = EarlyExitApplication{1, argv, terminal, initializeCalled};

        REQUIRE_EQUAL(application.run(), 7);
        REQUIRE_FALSE(initializeCalled);
        REQUIRE_EQUAL(backend->_restorePlatformCallCount, 1);
        REQUIRE_EQUAL(backend->_readKeyCallCount, 0);
        REQUIRE_THROWS(application.run());
    }

    void testApplicationInvokeRunsQueuedCallbacksOnTheUiThread() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{20, 4});
        auto application = ui::Application{terminal};
        auto page = ui::Page::create();
        auto callbackThreadId = std::thread::id{};
        const auto uiThreadId = std::this_thread::get_id();

        page->scheduler().addSingleShot(
            [&application, &callbackThreadId]() -> void {
                auto worker = std::thread{[&application, &callbackThreadId]() -> void {
                    application.invoke([&application, &callbackThreadId]() -> void {
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
            [&application, &callbackOrder]() -> void {
                auto worker = std::thread{[&application, &callbackOrder]() -> void {
                    application.invoke([&callbackOrder]() -> void { callbackOrder.push_back('A'); });
                    application.invoke([&application, &callbackOrder]() -> void {
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
            application.invoke([]() -> void {});
        } catch (const std::logic_error &) {
            threwBeforeRun = true;
        }

        page->scheduler().addSingleShot(
            [&application, &threwAfterQuit]() -> void {
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
        auto application = InspectableApplication{terminal};
        auto page = ui::Page::create();
        auto managedThreadCount = std::size_t{999};

        page->scheduler().addSingleShot(
            [&application]() -> void {
                auto eventThread = application.createEventThread();
                eventThread->invoke([]() {});
                eventThread->quit();
            },
            std::chrono::milliseconds{10});
        page->scheduler().addSingleShot(
            [&application, &managedThreadCount]() -> void {
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
        auto application = InspectableApplication{terminal};
        auto page = ui::Page::create();
        auto stopObserved = false;
        auto managedThread = ui::EventThreadPtr{};
        auto stderrCapture = StderrCapture{};

        page->scheduler().addSingleShot(
            [&application, &managedThread, &stopObserved]() -> void {
                managedThread = application.createEventThread();
                managedThread->invoke([&stopObserved](const ui::StopToken stopToken) -> void {
                    while (!stopToken.stopRequested()) {
                        std::this_thread::sleep_for(std::chrono::milliseconds{1});
                    }
                    stopObserved = true;
                });
            },
            std::chrono::milliseconds{10});
        page->scheduler().addSingleShot(
            [&application]() -> void { application.quit(); }, std::chrono::milliseconds{20});
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
        auto application = InspectableApplication{terminal};
        auto page = ui::Page::create();
        auto managedThread = ui::EventThreadPtr{};
        auto stderrCapture = StderrCapture{};

        application.setManagedThreadShutdownTimeoutForTest(std::chrono::milliseconds{10});
        page->scheduler().addSingleShot(
            [&application, &managedThread]() -> void {
                managedThread = application.createEventThread();
                managedThread->invoke([]() { std::this_thread::sleep_for(std::chrono::milliseconds{100}); });
            },
            std::chrono::milliseconds{10});
        page->scheduler().addSingleShot(
            [&application]() -> void { application.quit(); }, std::chrono::milliseconds{20});
        application.setMainPage(page);

        REQUIRE_EQUAL(application.run(), 0);
        REQUIRE(managedThread != nullptr);
        REQUIRE(std::string::npos != stderrCapture.output().find("timed out waiting"));
        REQUIRE(managedThread->waitForQuit(std::chrono::milliseconds{500}));
    }
};
