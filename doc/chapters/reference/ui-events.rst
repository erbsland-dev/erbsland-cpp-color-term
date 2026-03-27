..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

************************
UI Events and Scheduling
************************

The UI runtime is built on a small, flexible event system that is also
available as a public API. Most applications interact with it indirectly
through :cpp:any:`Application <erbsland::cterm::ui::Application>`, but
you can use it directly when you need fine-grained control over event
flow, timers, or background work.

This layer becomes especially useful when you:

* integrate the UI into an existing event loop
* schedule delayed or repeated actions
* run background tasks without blocking the UI thread

.. important::

    The UI Framework is currently in beta, and parts of the API may
    change in future releases.

Usage
=====

Driving an Event Queue Manually
-------------------------------

:cpp:any:`EventDriver <erbsland::cterm::ui::EventDriver>` manages the
event queue and dispatches events to a single handler. Combine it with
:cpp:any:`EventScheduler <erbsland::cterm::ui::EventScheduler>` if you
need time-based events.

.. code-block:: cpp

    using namespace erbsland::cterm::ui;

    auto driver = EventDriver{};
    driver.setEventHandler([](const Event &event) {
        if (event.type() == EventType::Resize) {
            // Recalculate layout here.
        }
    });

    auto scheduler = EventScheduler{driver};
    scheduler.schedule(
        EventType::Resize,
        {},
        EventClock::now() + std::chrono::milliseconds{50});

    while (driver.processEvents(std::chrono::milliseconds{10}).status
           != EventDriver::ProcessResult::Quit) {
        scheduler.poll();
    }

This level of control is mainly useful for testing, embedding, or custom
runtime integration. In typical applications,
:cpp:any:`Application <erbsland::cterm::ui::Application>` manages both
the driver and the scheduler for you.

Scheduling Surface-Local UI Actions
-----------------------------------

Each :cpp:any:`Surface <erbsland::cterm::ui::Surface>` can lazily create
its own :cpp:any:`Scheduler <erbsland::cterm::ui::Scheduler>`. All
scheduled callbacks run on the UI thread, which makes them safe for
modifying surface state.

.. code-block:: cpp

    auto notice = TextBox::create("Saving...", Alignment::Center);
    auto action = notice->scheduler().addSingleShot(
        [weakNotice = SurfaceWeakPtr{notice}]() {
            if (const auto locked = weakNotice.lock()) {
                locked->setPaintOutdated();
            }
        },
        std::chrono::milliseconds{750});

    if (action.isValid()) {
        // The UI thread will execute the callback once the delay expires.
    }

Use this mechanism for UI-related delays such as animations, status
updates, or deferred rendering changes.

:cpp:any:`ScheduledActionRef <erbsland::cterm::ui::ScheduledActionRef>`
acts as a handle for the scheduled task. You can use it to reschedule or
cancel the action later without introducing your own identifier system.

Running Background Work with Cooperative Stop
---------------------------------------------

:cpp:any:`EventThread <erbsland::cterm::ui::EventThread>` runs callbacks
on a dedicated worker thread. This allows you to perform longer-running
tasks without blocking the UI.

Stoppable callbacks receive a
:cpp:any:`StopToken <erbsland::cterm::ui::StopToken>`, which lets them
exit cooperatively when the application shuts down.

.. code-block:: cpp

    auto worker = getApplication().createEventThread();
    worker->invoke([](StopToken stopToken) {
        while (!stopToken.stopRequested()) {
            // Perform one unit of work here.
        }
    });

    worker->quitAndWait(std::chrono::seconds{1});

Important: do not update UI surfaces directly from a worker thread.
Instead, post results back to the UI thread using:

* :cpp:any:`Application::invoke() <erbsland::cterm::ui::Application::invoke()>`
* or a surface-local :cpp:any:`Scheduler <erbsland::cterm::ui::Scheduler>`

This keeps your UI consistent and avoids subtle race conditions.

Key and Quit Events
-------------------

The framework currently exposes two primary event payload types:

* :cpp:any:`KeyPressEvent <erbsland::cterm::ui::KeyPressEvent>` for user
  input
* :cpp:any:`QuitEvent <erbsland::cterm::ui::QuitEvent>` for terminating
  the application

Key events are routed through the focused surface chain, allowing each
surface to handle or forward input.

Quit events stop the event loop gracefully and can carry the final exit
code of your application.

Interface
=========

.. doxygenenum:: erbsland::cterm::ui::EventType

.. doxygenclass:: erbsland::cterm::ui::EventData
    :members:

.. doxygenclass:: erbsland::cterm::ui::Event
    :members:

.. doxygenclass:: erbsland::cterm::ui::EventDriver
    :members:

.. doxygenclass:: erbsland::cterm::ui::EventScheduler
    :members:

.. doxygenclass:: erbsland::cterm::ui::Scheduler
    :members:

.. doxygenclass:: erbsland::cterm::ui::ScheduledActionRef
    :members:

.. doxygenclass:: erbsland::cterm::ui::StopToken
    :members:

.. doxygenclass:: erbsland::cterm::ui::StopSource
    :members:

.. doxygenclass:: erbsland::cterm::ui::EventThread
    :members:

.. doxygenclass:: erbsland::cterm::ui::KeyPressEvent
    :members:

.. doxygenclass:: erbsland::cterm::ui::QuitEvent
    :members:
