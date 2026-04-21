// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Display.hpp"

#include "event/Event.hpp"
#include "event/EventData.hpp"
#include "event/EventType.hpp"
#include "event/KeyPressEvent.hpp"
#include "event/ScheduledActionRef.hpp"

#include "../Terminal.hpp"
#include "../theme/Theme.hpp"

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace erbsland::cterm::ui {

class Scheduler;
class EventDriver;
using EventDriverPtr = std::unique_ptr<EventDriver>;
class EventScheduler;
using EventSchedulerPtr = std::unique_ptr<EventScheduler>;
class EventThread;
using EventThreadPtr = std::shared_ptr<EventThread>;

/// UI Application instance for event-driven UI applications.
class Application {
    friend auto getApplication() -> Application &;
    friend class Scheduler;
    enum class State : uint8_t { Constructing, Running, Stopping, Destroyed };
    using ManagedEventThreads = std::vector<std::weak_ptr<EventThread>>;
    using CleanUpToken = std::shared_ptr<int>;

public:
    /// The type for the exit code.
    using ExitCode = int;
    /// Special value to indicate that the event loop shall continue.
    constexpr static auto cExitCodeContinue = std::numeric_limits<ExitCode>::min() + 2;
    /// A list of command line arguments.
    using CommandLineArgs = std::vector<std::string>;

public:
    /// Create a new UI application using the default Terminal backend.
    Application();
    /// Create a new UI application using the default Terminal backend.
    /// Stores the command line arguments.
    /// @param argc The number of command line arguments
    /// @param argv The command line arguments
    Application(int argc, char *argv[]);
    /// Create a new UI application using a custom Terminal backend.
    /// @param terminal The terminal backend to use. Must not be null.
    explicit Application(TerminalPtr terminal);
    /// Create a new UI application using a custom Terminal backend.
    /// Stores the command line arguments.
    /// @param argc The number of command line arguments
    /// @param argv The command line arguments
    /// @param terminal The terminal backend to use. Must not be null.
    explicit Application(int argc, char *argv[], TerminalPtr terminal);

    /// dtor
    virtual ~Application();

    // defaults
    Application(const Application &) = delete;
    Application(Application &&) = delete;
    auto operator=(const Application &) -> Application & = delete;
    auto operator=(Application &&) -> Application & = delete;

public:
    /// Set the main page for this application.
    /// The main page is automatically registered as the initial page in the display, just before the event loop starts.
    void setMainPage(PagePtr page);
    /// Replace the application theme.
    /// @param theme The new theme. A null pointer restores the default dark theme.
    void setTheme(const theme::ThemeConstPtr &theme);
    /// Access the application theme.
    /// @return The active application theme.
    [[nodiscard]] auto theme() const noexcept -> const theme::ThemeConstPtr &;
    /// Access the command line arguments
    [[nodiscard]] auto commandLineArgs() const -> const CommandLineArgs &;
    /// Access the terminal instance.
    /// @warning Only access and manipulate the terminal from the main thread that owns this application.
    [[nodiscard]] auto terminal() const -> Terminal &;
    /// Access the display instance.
    /// @warning Only access and manipulate the display from events!
    [[nodiscard]] auto display() const -> Display &;
    /// Run the application event loop.
    ///
    /// This will initialize the terminal and start the event loop.
    /// If the event loop exits, the screen is restored.
    /// Any exception from the event loop will be propagated.
    ///
    /// @return The exit code of the application.
    auto run() -> ExitCode;
    /// Test if this application is shutting down.
    [[nodiscard]] auto isShuttingDown() const -> bool;
    /// Add an event to the event loop.
    /// If the application is shutting down, this call is ignored.
    /// @param eventType The event type.
    /// @param eventData The event data.
    void addEvent(EventType eventType, EventDataUniquePtr &&eventData);
    /// Queue one callback on the application UI thread.
    /// @param invokedFn The callback to execute on the UI thread.
    /// @throw std::logic_error If the application event system is not active anymore.
    void invoke(std::function<void()> invokedFn);
    /// Create one managed background event thread.
    /// The application automatically stops managed event threads during shutdown.
    /// @return The started event thread.
    /// @throw std::logic_error If the application event system is not active anymore.
    [[nodiscard]] auto createEventThread() -> EventThreadPtr;
    /// Quit the application.
    /// This will schedule a quit event and exit the event loop as soon as all scheduled events up to this quit event
    /// are processed.
    void quit(ExitCode exitCode = 0);
    /// Abort the application.
    /// This aborts the application as soon as possible. Events in the event loop are not processed.
    /// @note Use `quit()` to exit the application gracefully.
    void abort();

public: // manually drive the application and event loop.
    /// Initialize the application.
    /// @note Only use this method if you need to drive the event loop manually.
    /// @return `cExitCodeContinue` if everything is ok and the application shall go into the event loop.
    ///     Any other value is a return value to use when exiting the application.
    ///     Before exiting the application, `manualShutdown()` must be called.
    /// @throws Any exception that may occur during initialization.
    auto manualInitialize() -> ExitCode;
    /// Refresh the Display, read input and process events.
    /// @note Only use this method if you need to drive the event loop manually.
    /// @return `cExitCodeContinue` if everything is ok and the loop shall continue.
    ///     Any other value is a return value to use when exiting the application.
    ///     Before exiting the application, `manualShutdown()` must be called.
    /// @throws std::exception Any exception that may occur during run.
    auto manualProcessEvents() -> ExitCode;
    /// Shutdown the application.
    /// @note Only use this method if you need to drive the event loop manually.
    /// This method **must** be called before exiting the application in **every** case.
    void manualShutdown();

protected: // Methods for derived classes
    /// Set up the main page of your user interface,
    /// Overwrite this class to set up your UI before the application is initialized.
    /// At this point, nothing is initialized.
    /// Your implementation must call `setMainPage()` to set the main page of the application.
    virtual void setupUi();
    /// Initializes the UI system.
    /// *Extend* this method to perform additional initialization steps after command line arguments are processed.
    virtual void initialize();
    /// Initialize the terminal.
    /// You can extend/overwrite this method to perform additional terminal initialization steps or
    /// change the terminal initialization behavior.
    /// You **must make sure** the base implementation is called once.
    /// By default, this method initializes the screen and configures key input mode.
    virtual void initializeTerminal();
    /// Process command line arguments.
    /// Overwrite this method to parse the command line arguments at the start of the application.
    /// Return `cExitCodeContinue` if everything is fine and the application should continue to start.
    /// Any other value returned will cause the application to exit with that value.
    /// At this point, `setupUi()` has built the surface tree and the terminal is initialized, but the UI event system
    /// is not active yet.
    /// @param args The command line arguments.
    /// @return The exit code to use when exiting the application or `cExitCodeContinue` if everything is fine.
    virtual auto processCommandLineArguments(const std::vector<std::string> &args) -> int;
    /// Running the event loop.
    /// Only extend this method if you need to change the event loop behavior or have to perform work just
    /// before or after running the event loop.
    virtual auto runEventLoop() -> int;
    /// Restore the terminal to its original state.
    /// By default, this method only calls `restoreTerminal()`.
    virtual void restoreTerminal();
    /// Shutdown the application.
    /// You can *extend* this method to perform additional shutdown steps.
    virtual void shutdown();
    /// Get the shutdown timeout for managed event threads.
    /// @return The total shutdown timeout.
    [[nodiscard]] virtual auto managedEventThreadShutdownTimeout() const -> std::chrono::milliseconds;
    /// Get the current number of managed event threads.
    /// Primarily useful for tests and diagnostics in derived classes.
    /// @return The number of currently tracked managed event threads.
    [[nodiscard]] auto managedEventThreadCount() const -> std::size_t;

private:
    /// Initialize the UI application
    /// @return `cExitCodeContinue` if initialization was successful, otherwise an error code.
    auto initializeImpl() -> ExitCode;
    /// Process events.
    /// @return `cExitCodeContinue` if events were processed successfully, otherwise an error code.
    auto processEventsImpl() -> ExitCode;
    /// Shutdown the application.
    void shutdownImpl();
    /// Handle one queued non-control event.
    /// @param event The event to process.
    void handleEvent(const Event &event);
    /// Forward a key press event to the display.
    /// @param keyPressEvent The key press event to forward.
    void handleKeyPress(KeyPressEvent &keyPressEvent) const noexcept;
    /// Set this application into a new state.
    void setState(State state);
    /// Schedule one scheduled action event.
    /// @param surface The target surface.
    /// @param actionRef The action reference.
    /// @param generation The current action generation.
    /// @param eventData The scheduled action event payload.
    /// @param eventTime The due time for the scheduled action event.
    void scheduleAction(
        const SurfaceWeakPtr &surface,
        ScheduledActionRef actionRef,
        uint64_t generation,
        EventDataUniquePtr &&eventData,
        EventTime eventTime);
    /// Arm all existing surface schedulers in the given subtree.
    /// @param surface The root surface.
    void armSurfaceSchedulers(const SurfacePtr &surface);
    /// Calculate the next timeout for terminal input polling.
    /// A return value of zero causes `Input::readKey()` to perform a non-blocking poll.
    /// @return The timeout for `Input::readKey()`.
    [[nodiscard]] auto calculateInputTimeout() const -> std::chrono::milliseconds;
    /// Verify that the event system is active and still accepts new work.
    void verifyEventSystemIsActive() const;
    /// Stop all managed event threads and wait for them to end.
    void shutdownManagedEventThreads() noexcept;
    /// Remove finished or expired managed event threads from the tracking list.
    void pruneManagedEventThreads() noexcept;
    /// Remove finished or expired managed event threads from the tracking list.
    void pruneManagedEventThreadsLocked() noexcept;

private:
    /// Try to access the active application instance.
    /// @return The active application instance, or `nullptr` if no application exists.
    [[nodiscard]] static auto instanceIfAvailable() noexcept -> Application *;

private:
    static std::atomic<Application *> _instance;       ///< The global application instance.
    std::atomic<State> _state{State::Constructing};    ///< The application state.
    CommandLineArgs _commandLineArgs;                  ///< A snapshot of the command line arguments
    TerminalPtr _terminal;                             ///< The global terminal instance
    EventDriverPtr _eventDriver;                       ///< The global event driver.
    EventSchedulerPtr _eventScheduler;                 ///< The event scheduler
    std::atomic<bool> _abortRequested{false};          ///< The abort flag
    std::atomic<bool> _quitRequested{false};           ///< A graceful shutdown was requested.
    bool _appMethodWasCalled = false;                  ///< Sanity flag, to check if init/... methods were called
    PagePtr _mainPage;                                 ///< The main page for this application.
    DisplayPtr _display;                               ///< The display instance.
    theme::ThemeConstPtr _theme{theme::Theme::dark()}; ///< The application theme.
    mutable std::mutex _managedEventThreadsMutex;      ///< Protects the managed event threads.
    ManagedEventThreads _managedEventThreads;          ///< The managed event threads.
    CleanUpToken _managedEventThreadsCleanupToken{std::make_shared<int>(0)}; ///< Safe cleanup token.
};

/// Access the global application instance.
/// Calling this method is thread-safe and can be used to post events from other threads.
/// @warning If you make calls from other threads, ensure that the application has been constructed and is not shutting
/// down. You **must make sure** that your threads are terminated, *before* the application instance is destroyed.
/// Do this by adding `wait` or `join` at the end of `main` or similar functions.
/// @throw std::logic_error if no application instance exists.
[[nodiscard]] auto getApplication() -> Application &;

}
