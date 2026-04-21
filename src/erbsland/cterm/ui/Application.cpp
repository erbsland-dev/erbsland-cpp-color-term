// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Application.hpp"

#include "event/EventDriver.hpp"
#include "event/EventScheduler.hpp"
#include "event/EventThread.hpp"
#include "event/impl/EventClockAccess.hpp"
#include "event/impl/InvocationEvent.hpp"
#include "event/impl/ScheduledActionEvent.hpp"
#include "event/KeyPressEvent.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <exception>
#include <iostream>
#include <stdexcept>

namespace erbsland::cterm::ui {

using namespace std::chrono_literals;

std::atomic<Application *> Application::_instance{nullptr};

auto getApplication() -> Application & {
    const auto instance = Application::_instance.load(std::memory_order_acquire);
    if (instance == nullptr) {
        throw std::logic_error("Accessing ui::Application without an active instance.");
    }
    return *instance;
}

auto Application::instanceIfAvailable() noexcept -> Application * {
    return _instance.load(std::memory_order_acquire);
}

Application::Application() : Application(std::make_shared<Terminal>()) {
}

Application::Application(int argc, char *argv[]) : Application(argc, argv, std::make_shared<Terminal>()) {
}

Application::Application(TerminalPtr terminal) : Application(0, nullptr, std::move(terminal)) {
}

Application::Application(int argc, char *argv[], TerminalPtr terminal) : _terminal{std::move(terminal)} {
    Application *expected = nullptr;
    if (!_instance.compare_exchange_strong(expected, this)) {
        throw std::runtime_error("Only one Application instance is allowed.");
    }
    if (_terminal == nullptr) {
        throw std::invalid_argument{"terminal must not be null"};
    }
    if (argc > 0 && argv != nullptr) {
        _commandLineArgs.reserve(static_cast<std::size_t>(argc));
        for (int i = 0; i < argc; i++) {
            _commandLineArgs.emplace_back(argv[i]);
        }
    }
}

Application::~Application() {
    _managedEventThreadsCleanupToken.reset();
    _instance.store(nullptr, std::memory_order_release);
}

void Application::setMainPage(PagePtr page) {
    if (page == nullptr) {
        throw std::invalid_argument{"page must not be null"};
    }
    _mainPage = std::move(page);
}

void Application::setTheme(const theme::ThemeConstPtr &theme) {
    _theme = theme == nullptr ? theme::Theme::dark() : theme;
    if (_display != nullptr) {
        _display->setTheme(_theme);
    }
    if (_mainPage != nullptr) {
        _mainPage->flags().setThemeOutdated();
    }
}

auto Application::theme() const noexcept -> const theme::ThemeConstPtr & {
    return _theme;
}

auto Application::commandLineArgs() const -> const CommandLineArgs & {
    return _commandLineArgs;
}

auto Application::terminal() const -> Terminal & {
    return *_terminal;
}

auto Application::display() const -> Display & {
    if (_display == nullptr) {
        throw std::logic_error{"Application::display() must only be called from application events."};
    }
    return *_display;
}

auto Application::run() -> ExitCode {
    if (_state.load(std::memory_order_acquire) != State::Constructing) {
        throw std::logic_error{"Application::run() can only be called once."};
    }
    try {
        if (const auto exitCode = initializeImpl(); exitCode != cExitCodeContinue) {
            restoreTerminal();
            setState(State::Destroyed);
            return exitCode;
        }
        _appMethodWasCalled = false;
        const auto exitCode = runEventLoop();
        if (!_appMethodWasCalled) {
            throw std::logic_error{"You must call Application::runEventLoop() when subclassing Application."};
        }
        shutdownImpl();
        return exitCode;
    } catch (...) {
        restoreTerminal();
        throw;
    }
}

auto Application::isShuttingDown() const -> bool {
    const auto state = _state.load(std::memory_order_acquire);
    return state == State::Stopping || state == State::Destroyed;
}

void Application::addEvent(const EventType eventType, EventDataUniquePtr &&eventData) {
    if (_quitRequested.load(std::memory_order_acquire) || isShuttingDown()) {
        return;
    }
    _eventDriver->addEvent(eventType, std::move(eventData));
}

void Application::invoke(std::function<void()> invokedFn) {
    verifyEventSystemIsActive();
    pruneManagedEventThreads();
    _eventDriver->addEvent(
        EventType::Invocation,
        std::make_unique<impl::InvocationEvent>([invokedFn = std::move(invokedFn)](const StopToken) -> void {
            if (invokedFn) {
                invokedFn();
            }
        }));
}

auto Application::createEventThread() -> EventThreadPtr {
    verifyEventSystemIsActive();
    auto eventThread = EventThread::create();
    eventThread->setFinishedCallback(
        [cleanupToken = std::weak_ptr<int>{_managedEventThreadsCleanupToken}, this]() -> void {
            if (!cleanupToken.expired()) {
                pruneManagedEventThreads();
            }
        });
    const auto lock = std::scoped_lock{_managedEventThreadsMutex};
    pruneManagedEventThreadsLocked();
    _managedEventThreads.emplace_back(eventThread);
    return eventThread;
}

void Application::quit(const ExitCode exitCode) {
    _quitRequested.store(true, std::memory_order_release);
    if (isShuttingDown()) {
        return;
    }
    _eventDriver->quit(exitCode);
}

void Application::abort() {
    _quitRequested.store(true, std::memory_order_release);
    _abortRequested.store(true, std::memory_order_release);
}

auto Application::manualInitialize() -> ExitCode {
    if (_state.load(std::memory_order_acquire) != State::Constructing) {
        throw std::logic_error{"Application::manualInitialize() can only be called once."};
    }
    return initializeImpl();
}

auto Application::manualProcessEvents() -> ExitCode {
    return processEventsImpl();
}

void Application::manualShutdown() {
    shutdownImpl();
}

void Application::setupUi() {
    // do nothing.
}

void Application::initialize() {
    if (_mainPage == nullptr) {
        throw std::logic_error{"Application::initialize() must be called after setting the main page."};
    }
    _abortRequested.store(false, std::memory_order_release);
    _quitRequested.store(false, std::memory_order_release);
    _eventDriver = std::make_unique<EventDriver>();
    _eventDriver->setEventHandler([this](const Event &event) -> void { handleEvent(event); });
    _eventScheduler = std::make_unique<EventScheduler>(*_eventDriver.get());
    _display = std::make_unique<Display>(_terminal, _mainPage, _theme);
    armSurfaceSchedulers(_mainPage);
    _appMethodWasCalled = true;
}

void Application::initializeTerminal() {
    _terminal->setSafeMarginEnabled(false);
    _terminal->initializeScreen();
    _terminal->input().setMode(Input::Mode::Key);
}

auto Application::processCommandLineArguments([[maybe_unused]] const std::vector<std::string> &args) -> int {
    return cExitCodeContinue;
}

auto Application::runEventLoop() -> int {
    _appMethodWasCalled = true;
    while (!_abortRequested.load(std::memory_order_acquire)) {
        if (const auto exitCode = processEventsImpl(); exitCode != cExitCodeContinue) {
            return exitCode;
        }
    }
    return 1;
}

void Application::restoreTerminal() {
    _terminal->restoreScreen();
}

void Application::shutdown() {
    shutdownManagedEventThreads();
    _eventScheduler.reset();
    _eventDriver.reset();
    _appMethodWasCalled = true;
}

auto Application::managedEventThreadShutdownTimeout() const -> std::chrono::milliseconds {
    return 1500ms;
}

auto Application::managedEventThreadCount() const -> std::size_t {
    const auto lock = std::scoped_lock{_managedEventThreadsMutex};
    auto count = std::size_t{0};
    for (const auto &eventThread : _managedEventThreads) {
        if (const auto sharedEventThread = eventThread.lock();
            sharedEventThread != nullptr && !sharedEventThread->isFinished()) {
            count += 1;
        }
    }
    return count;
}

auto Application::initializeImpl() -> ExitCode {
    setupUi();
    initializeTerminal();
    setState(State::Running);
    if (const auto exitCode = processCommandLineArguments(_commandLineArgs); exitCode != cExitCodeContinue) {
        return exitCode;
    }
    _appMethodWasCalled = false;
    initialize();
    if (!_appMethodWasCalled) {
        throw std::logic_error{"You must call Application::initialize() when subclassing Application."};
    }
    return cExitCodeContinue;
}

auto Application::processEventsImpl() -> ExitCode {
    _display->pollTerminalResize();
    _eventScheduler->poll();
    const auto result = _eventDriver->processEvents(0ms);
    if (result.status() == EventDriver::ProcessResult::Quit) {
        if (result.exitCode() == cExitCodeContinue) {
            return 1; // prevent unexpected behavior if the wrong exit code is used
        }
        return result.exitCode();
    }
    _display->pollRender();
    const auto inputTimeout = calculateInputTimeout();
    if (const auto key = _terminal->input().readKey(inputTimeout); key.valid()) {
        addEvent(EventType::KeyPress, std::make_unique<KeyPressEvent>(key));
    }
    return cExitCodeContinue;
}

void Application::shutdownImpl() {
    setState(State::Stopping);
    restoreTerminal();
    _appMethodWasCalled = false;
    shutdown();
    if (!_appMethodWasCalled) {
        throw std::logic_error{"You must call Application::shutdown() when subclassing Application."};
    }
    setState(State::Destroyed);
}

void Application::scheduleAction(
    const SurfaceWeakPtr &surface,
    const ScheduledActionRef actionRef,
    const uint64_t generation,
    EventDataUniquePtr &&eventData,
    const EventTime eventTime) {
    if (surface.expired() || _eventScheduler == nullptr || isShuttingDown()) {
        return;
    }
    if (eventData == nullptr) {
        eventData = std::make_unique<impl::ScheduledActionEvent>(surface, actionRef, generation);
    }
    _eventScheduler->schedule(EventType::ScheduledAction, std::move(eventData), eventTime);
}

void Application::armSurfaceSchedulers(const SurfacePtr &surface) {
    if (surface == nullptr) {
        return;
    }
    if (auto &scheduler = surface->schedulerPtr(); scheduler != nullptr) {
        scheduler->armAllActions();
    }
    for (const auto &child : surface->surfaces()) {
        armSurfaceSchedulers(child);
    }
}

auto Application::calculateInputTimeout() const -> std::chrono::milliseconds {
    if (_eventDriver->hasPendingEvents()) {
        return 0ms;
    }
    auto nextWakeTime = _display->nextWakeTime();
    if (const auto nextEventTime = _eventScheduler->nextEventTime(); nextEventTime.has_value()) {
        nextWakeTime = std::min(nextWakeTime, *nextEventTime);
    }
    const auto now = impl::EventClockAccess::now();
    if (nextWakeTime <= now) {
        return 0ms;
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(nextWakeTime - now);
}

void Application::handleEvent(const Event &event) {
    switch (event.type()) {
    case EventType::KeyPress:
        if (auto *keyPressEvent = dynamic_cast<KeyPressEvent *>(event.data().get()); keyPressEvent != nullptr) {
            handleKeyPress(*keyPressEvent);
        }
        break;
    case EventType::ScheduledAction:
        if (auto *scheduledActionEvent = dynamic_cast<impl::ScheduledActionEvent *>(event.data().get());
            scheduledActionEvent != nullptr) {
            if (const auto surface = scheduledActionEvent->surface().lock(); surface != nullptr) {
                if (auto &scheduler = surface->schedulerPtr(); scheduler != nullptr) {
                    scheduler->executeAction(scheduledActionEvent->actionRef(), scheduledActionEvent->generation());
                }
            }
        }
        break;
    case EventType::Invocation:
        if (auto *invocationEvent = dynamic_cast<impl::InvocationEvent *>(event.data().get());
            invocationEvent != nullptr) {
            invocationEvent->invoke({});
        }
        break;
    default:
        break;
    }
}

void Application::handleKeyPress(KeyPressEvent &keyPressEvent) const noexcept {
    if (_display == nullptr) {
        return;
    }
    _display->onKeyPress(keyPressEvent);
}

void Application::setState(const State state) {
    _state.store(state, std::memory_order_release);
}

void Application::verifyEventSystemIsActive() const {
    if (_state.load(std::memory_order_acquire) != State::Running || _quitRequested.load(std::memory_order_acquire) ||
        isShuttingDown() || _eventDriver == nullptr || _eventScheduler == nullptr) {
        throw std::logic_error{"The application event system is not active."};
    }
}

void Application::shutdownManagedEventThreads() noexcept {
    pruneManagedEventThreads();
    auto managedEventThreads = std::vector<EventThreadPtr>{};
    {
        const auto lock = std::scoped_lock{_managedEventThreadsMutex};
        managedEventThreads.reserve(_managedEventThreads.size());
        for (const auto &managedEventThread : _managedEventThreads) {
            if (const auto eventThread = managedEventThread.lock();
                eventThread != nullptr && !eventThread->isFinished()) {
                managedEventThreads.emplace_back(eventThread);
            }
        }
    }
    for (const auto &eventThread : managedEventThreads) {
        eventThread->quit();
    }
    const auto deadline = std::chrono::steady_clock::now() + managedEventThreadShutdownTimeout();
    auto runningThreads = std::size_t{0};
    for (const auto &eventThread : managedEventThreads) {
        auto remaining =
            std::chrono::duration_cast<std::chrono::milliseconds>(deadline - std::chrono::steady_clock::now());
        if (remaining < 0ms) {
            remaining = 0ms;
        }
        try {
            if (!eventThread->waitForQuit(remaining)) {
                runningThreads += 1;
            }
        } catch (const std::exception &exception) {
            std::cerr << "Warning: Managed event thread failed during application shutdown: " << exception.what()
                      << "\n";
        } catch (...) {
            std::cerr
                << "Warning: Managed event thread failed during application shutdown with an unknown exception.\n";
        }
    }
    if (runningThreads > 0) {
        std::cerr << "Warning: Application shutdown timed out waiting for " << runningThreads
                  << " managed event thread(s) to stop.\n";
    }
    pruneManagedEventThreads();
}

void Application::pruneManagedEventThreads() noexcept {
    const auto lock = std::scoped_lock{_managedEventThreadsMutex};
    pruneManagedEventThreadsLocked();
}

void Application::pruneManagedEventThreadsLocked() noexcept {
    std::erase_if(_managedEventThreads, [](const auto &managedEventThread) -> bool {
        if (const auto eventThread = managedEventThread.lock(); eventThread != nullptr) {
            return eventThread->isFinished();
        }
        return true;
    });
}

}
