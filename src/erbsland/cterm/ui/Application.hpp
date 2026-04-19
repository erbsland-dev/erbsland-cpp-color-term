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

public:
    /// Create a new UI application using the default Terminal backend.
    Application();

    /// Create a new UI application using a custom Terminal backend.
    /// @param terminal The terminal backend to use. Must not be null.
    explicit Application(TerminalPtr terminal);

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

    /// Access the terminal instance.
    /// @warning Only access and manipulate the terminal from the main thread that owns this application.
    [[nodiscard]] auto terminal() const -> Terminal &;

    /// Access the display instance.
    /// @warning Only access and manipulate the display from events!
    [[nodiscard]] auto display() const -> Display &;

    /// Run the application event loop.
    /// This will initialize the terminal and start the event loop.
    /// If the event loop exits, the screen is restored.
    /// Any exception from the event loop will be propagated.
    /// @return The exit code of the application.
    auto run() -> int;

    /// Test if this application is shutting down.
    [[nodiscard]] auto isShuttingDown() const -> bool;

    /// Add an event to the event loop.
    /// If the application is shutting down, this call is ignored.
    /// @param eventType The event type.
    /// @param eventData The event data.
    void addEvent(EventType eventType, EventDataUniquePtr &&eventData);
    /// Queue one callback on the application UI thread.
    /// @param fn The callback to execute on the UI thread.
    /// @throw std::logic_error If the application event system is not active anymore.
    void invoke(std::function<void()> fn);
    /// Create one managed background event thread.
    /// The application automatically stops managed event threads during shutdown.
    /// @return The started event thread.
    /// @throw std::logic_error If the application event system is not active anymore.
    [[nodiscard]] auto createEventThread() -> EventThreadPtr;

    /// Quit the application.
    /// This will schedule a quit event and exit the event loop as soon as all scheduled events up to this quit event
    /// are processed.
    void quit(int exitCode = 0);

    /// Abort the application.
    /// This aborts the application as soon as possible. Events in the event loop are not processed.
    /// @note Use `quit()` to exit the application gracefully.
    void abort();

protected:
    /// Initializes the application.
    /// You can *extend* this method to perform additional initialization steps.
    virtual void initialize();
    /// Initialize the terminal.
    /// You can extend/overwrite this method to perform additional terminal initialization steps or
    /// change the terminal initialization behavior.
    /// You **must make sure** calling `initializeTerminal` once, either by calling it yourself or by extending
    /// `initialize`.
    /// By default, this method only calls `initializeTerminal()`.
    virtual void initializeTerminal();
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
    static std::atomic<Application *> _instance;    ///< The global application instance.
    std::atomic<State> _state{State::Constructing}; ///< The application state.
    TerminalPtr _terminal;                          ///< The global terminal instance
    EventDriverPtr _eventDriver;                    ///< The global event driver.
    EventSchedulerPtr _eventScheduler;              ///< The event scheduler
    std::atomic<bool> _abortRequested{false};       ///< The abort flag
    std::atomic<bool> _quitRequested{false};        ///< A graceful shutdown was requested.
    bool _appMethodWasCalled = false;               ///< Sanity flag, to check if init/... methods were called
    PagePtr _mainPage;                              ///< The main page for this application.
    DisplayPtr _display;                            ///< The display instance.
    mutable std::mutex _managedEventThreadsMutex;   ///< Protects the managed event threads.
    std::vector<std::weak_ptr<EventThread>> _managedEventThreads;                    ///< The managed event threads.
    std::shared_ptr<int> _managedEventThreadsCleanupToken{std::make_shared<int>(0)}; ///< Safe cleanup token.
};

/// Access the global application instance.
/// Calling this method is thread-safe and can be used to post events from other threads.
/// @warning If you make calls from other threads, ensure that the application has been constructed and is not shutting
/// down. You **must make sure** that your threads are terminated, *before* the application instance is destroyed.
/// Do this by adding `wait` or `join` at the end of `main` or similar functions.
/// @throw std::logic_error if no application instance exists.
[[nodiscard]] auto getApplication() -> Application &;

}
