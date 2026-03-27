..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: implementation notes; invocation
    single: implementation notes; UI invocation
    single: implementation notes; event thread
    single: implementation notes; managed event threads

****************************
Invocation and Event Threads
****************************

This page explains the invocation system in the UI layer and the
:cpp:any:`EventThread <erbsland::cterm::ui::EventThread>` helper.

It is written for contributors who need to extend, debug, or reason about
cross-thread work delivery in the UI system without reverse-engineering the
code from scratch.

After reading this page, you should be able to answer:

* how :cpp:any:`Application::invoke() <erbsland::cterm::ui::Application::invoke>` reaches the UI thread
* how :cpp:any:`EventThread <erbsland::cterm::ui::EventThread>` reuses the same event transport
* what the difference is between ``quit()`` and ``abort()``
* how delayed invocations are stored and cancelled
* how cooperative stop tokens are installed and requested
* how :cpp:any:`Application::createEventThread() <erbsland::cterm::ui::Application::createEventThread>`
  tracks, prunes, and shuts down managed worker threads

This page complements :doc:`action-scheduling`.
That page explains surface-local scheduled UI callbacks.
This page explains generic invocation delivery and reusable background event loops.

Relevant Source Files
=====================

You will find the implementation primarily in:

* :file:`src/erbsland/cterm/ui/Application.hpp`
* :file:`src/erbsland/cterm/ui/Application.cpp`
* :file:`src/erbsland/cterm/ui/event/EventThread.hpp`
* :file:`src/erbsland/cterm/ui/event/EventThread.cpp`
* :file:`src/erbsland/cterm/ui/event/EventDriver.hpp`
* :file:`src/erbsland/cterm/ui/event/EventDriver.cpp`
* :file:`src/erbsland/cterm/ui/event/EventScheduler.hpp`
* :file:`src/erbsland/cterm/ui/event/EventScheduler.cpp`
* :file:`src/erbsland/cterm/ui/event/EventType.hpp`
* :file:`src/erbsland/cterm/ui/event/impl/InvocationEvent.hpp`
* :file:`test/unittest/src/ui/event/UiApplicationInvokeTest.cpp`
* :file:`test/unittest/src/ui/event/EventThreadTest.cpp`
* :file:`test/unittest/src/ui/surface/UiSchedulerTest.cpp`

Why This System Exists
======================

The UI layer already had one important rule:

All UI work must be serialized on the application thread.

That rule is easy to keep for:

* key input
* surface-local scheduled actions
* display updates

but it becomes harder once background work enters the picture.

Typical needs are:

* a worker thread wants to hand a finished result back to the UI
* a helper thread wants its own tiny event loop without exposing raw threading mechanics
* application shutdown must stop these background workers in a predictable way
* long-running worker callbacks should be able to stop cooperatively

The design goal was therefore:

Do not invent a second callback-delivery model.
Reuse the existing event infrastructure and make background work feel like a natural extension of it.

High-Level Picture
==================

There are two public entry points into the invocation system:

* ``Application::invoke(fn)``
* ``EventThread::invoke(fn)`` and ``EventThread::invokeDelayed(fn, delay)``

Both use the same event type and the same internal payload,
but they do **not** share one physical queue.
Each owner feeds its own event loop.

.. mermaid::

    flowchart TB
        subgraph applicationPath["Application path"]
            direction TB
            appInvoke["Application::invoke(fn)"] --> appDriver["Application EventDriver"]
            appDriver --> appEvent["EventType::Invocation + InvocationEvent"]
            appEvent --> appHandle["Application::handleEvent()"]
            appHandle --> appFn["fn() on the UI thread"]
        end
        subgraph workerPath["Worker path"]
            direction TB
            workerInvoke["EventThread::invoke(fn)"] --> workerDriver["EventThread EventDriver"]
            workerDriver --> workerEvent["EventType::Invocation + InvocationEvent"]
            workerEvent --> workerHandle["EventThread::handleEvent()"]
            workerHandle --> workerFn["fn(stop_token) on the worker thread"]
        end
        subgraph delayedPath["Delayed worker path"]
            direction TB
            delayedInvoke["EventThread::invokeDelayed(fn, delay)"] --> delayedScheduler["EventThread EventScheduler"]
            delayedScheduler --> delayedDriver["EventThread EventDriver"]
            delayedDriver --> delayedEvent["EventType::Invocation + InvocationEvent"]
            delayedEvent --> delayedHandle["EventThread::handleEvent()"]
            delayedHandle --> delayedFn["fn(stop_token) on the worker thread"]
        end

The shared transport matters because it gives all three entry paths the same core behavior:

* work is queued as an event, not executed directly by the caller
* delivery is serialized by one event loop
* delayed work is handled by :cpp:any:`EventScheduler <erbsland::cterm::ui::EventScheduler>`
* shutdown can be expressed as event-loop control rather than as ad-hoc thread interruption

Shared Building Blocks
======================

``EventType::Invocation``
-------------------------

This is the common transport label for invoked callbacks.

There is intentionally only one generic invocation event type.
The code does not distinguish between:

* UI-thread invocation
* background-thread invocation
* immediate invocation
* delayed invocation

Those differences are handled by the owner of the event loop, not by the event type itself.

``impl::InvocationEvent``
-------------------------

The payload stores one callable shape:

.. code-block:: cpp

    std::function<void(std::stop_token)>

That choice solves two problems at once:

* ``EventThread`` can pass a real cooperative stop token to worker callbacks
* ``Application`` can reuse the same payload by simply invoking it with a default, never-requested token

Plain ``void()`` callbacks are wrapped into a token-ignoring adapter.

``EventDriver``
---------------

``EventDriver`` is the immediate event queue.
For this system, the relevant operation is ``processOneEvent()``,
which lets ``EventThread`` stop between individual queued callbacks.

That is important for ``abort()``.
If the worker loop drained the entire queue in one call, an abort request would have to wait
until every queued callback had already run.

``EventScheduler``
------------------

``EventScheduler`` stores future events and releases them into an ``EventDriver`` once they are due.

For the invocation system, the important behavior is ``clear()``.
It lets ``EventThread::quit()`` and ``EventThread::abort()`` cancel delayed-not-yet-due work immediately.

Think of the two layers like this:

.. code-block:: text

    EventScheduler = "hold this until later"
    EventDriver    = "deliver this as soon as possible"

Application::invoke()
=====================

What It Promises
----------------

``Application::invoke(fn)`` is the generic way to marshal arbitrary work back onto the UI thread.

It is valid only while the application event system is active:

* the application must already be running
* shutdown must not have been requested
* the event driver and scheduler must still exist

Outside that window it throws ``std::logic_error``.

Why That Restriction Exists
---------------------------

The method is intentionally strict because a queued callback is only meaningful while the UI event loop still exists.

If the application already requested shutdown:

* a newly queued callback might land behind the queued quit event
* it would then never be executed
* silently accepting it would create confusing race-dependent behavior

The implementation therefore treats this as programmer error instead of best-effort work submission.

Execution Path
--------------

The full delivery path is:

.. mermaid::

    flowchart TD
        source["background thread or helper code"] --> invoke["Application::invoke(fn)"]
        invoke --> addEvent["EventDriver::addEvent(EventType::Invocation, InvocationEvent)"]
        addEvent --> loop["Application::runEventLoop()"]
        loop --> process["EventDriver::processEvents(0ms)"]
        process --> handle["Application::handleEvent()"]
        handle --> eventInvoke["InvocationEvent::invoke({})"]
        eventInvoke --> fn["fn() runs on the UI thread"]

There is no extra dispatcher layer beyond the normal event loop.

Application Event Loop Integration
----------------------------------

The application loop follows this shape:

.. code-block:: text

    while not abort requested:
        poll terminal resize
        poll scheduled UI events
        drain queued events
        poll render work
        timeout = calculateInputTimeout()
        key = readKey(timeout)
        if key is valid:
            queue KeyPress event

Two details matter here:

* ``calculateInputTimeout()`` returns ``0ms`` when work is already due
* ``readKey(0ms)`` is used directly as the non-blocking poll

Visual Timeline
---------------

.. code-block:: text

    t0: background thread queues Application::invoke(...)
    t1: application loop reaches EventDriver::processEvents(0ms)
    t2: invocation event is dispatched
    t3: callback runs on the UI thread

The key point is:

The callback is not run "from the posting thread".
It is run at the point where the application loop reaches queued event dispatch.

.. note::

    ``readKey(0ms)`` made the loop logic simpler and more explicit.
    It did **not** add an operating-system level wake-up primitive for a ``readKey(timeout)``
    call that is already waiting with a non-zero timeout.
    In practice, a newly posted application invocation is still observed on the next loop cycle.

EventThread
===========

What It Is
----------

``EventThread`` is a small convenience wrapper around:

* one :cpp:any:`EventDriver <erbsland::cterm::ui::EventDriver>`
* one :cpp:any:`EventScheduler <erbsland::cterm::ui::EventScheduler>`
* one detached worker thread
* one stop token for the currently running callback

The intention is:

Give library users a ready-to-use worker event loop without making them build queueing,
delayed delivery, stop-token handling, and shutdown coordination themselves.

What It Is Not
--------------

``EventThread`` is not:

* a general-purpose thread pool
* a preemptive interruption mechanism
* a timer wheel with arbitrary cancellation handles
* a second UI thread

It is a single-threaded event loop with one queue and a small delayed-event scheduler.

Construction Model
------------------

Instances are created with ``EventThread::create()``.

Internally, the object is started like this:

.. mermaid::

    flowchart TD
        createPtr["make shared_ptr<EventThread>"] --> startThread["start detached std::thread"]
        startThread --> capture["detached lambda captures the shared_ptr"]

That last step is important.
It means the worker object stays alive even if the caller drops its last external ``shared_ptr``
while the worker loop is still running.

Worker Loop
-----------

The worker loop is intentionally small:

.. code-block:: text

    poll delayed events into EventDriver
    process queued events one by one
    if abort requested:
        stop immediately
    otherwise:
        wait for wake-up or next delayed event

In slightly more detail:

.. mermaid::

    flowchart TD
        poll["EventScheduler::poll()"] --> queued{"queued events exist?"}
        queued -->|yes| process["EventDriver::processOneEvent()"]
        process --> quit{"quit event was processed?"}
        quit -->|yes| stop["stop loop"]
        quit -->|no| abort{"abort requested?"}
        abort -->|yes| stop
        abort -->|no| queued
        queued -->|no| wait["compute next wait duration"]
        wait --> sleep["sleep on condition variable"]

Why It Processes One Event at a Time
------------------------------------

This is one of the most important design choices in the whole class.

``EventThread`` does **not** drain the entire queue in one go.
It processes one queued event, then checks again whether it should continue.

That is what makes this possible:

.. code-block:: text

    callback A is running
    callback B is already queued
    abort() is called
    callback A returns
    callback B is skipped

Without single-event stepping, ``abort()`` would only be able to stop between large queue-drain batches.

Delayed Invocations
-------------------

``invokeDelayed(...)`` stores an ``Invocation`` event in ``EventScheduler``.

The event does not enter the immediate queue until its due time arrives:

.. mermaid::

    flowchart TD
        delayed["invokeDelayed(fn, 50ms)"] --> scheduled["EventScheduler stores:<br/>time -> InvocationEvent"]
        scheduled --> poll["worker loop polls scheduler"]
        poll --> due["due event is moved into EventDriver"]
        due --> normal["normal invocation handling"]

This is deliberately the same model used by surface action scheduling.

Wait Slicing
------------

The worker loop does not sleep for the full remaining delayed interval in one go.
It clamps each wait slice to a short maximum interval.

That helps in two ways:

* a newly queued earlier event is noticed promptly
* tests that override the internal event clock can still advance time deterministically

The slice limit is ``5ms``.

Stop Tokens
===========

Per-Invocation Stop Source
--------------------------

Before each callback is executed, ``EventThread`` creates a fresh ``std::stop_source``:

.. mermaid::

    flowchart TD
        callback["callback about to run"] --> source["create fresh stop_source"]
        source --> publish["publish it as the active stop source"]
        publish --> invoke["invoke callback with stop_source.get_token()"]
        invoke --> clear["clear the active stop source afterwards"]

That means stop requests are scoped to the currently executing callback.

They are **not** a permanent thread-global cancellation state.

What ``quit()`` Does
--------------------

``quit()`` requests graceful shutdown.

Its steps are:

.. code-block:: text

    mark quit requested
    cancel all delayed future work
    request stop on the active callback
    enqueue a Quit event at the tail of the immediate queue
    wake the worker

The result is:

* the current callback may finish cooperatively
* already queued immediate callbacks before the quit event are still processed
* delayed callbacks that were still waiting in ``EventScheduler`` are discarded

This is the key graceful-shutdown guarantee:

Drain work that is already in the immediate queue.
Do not keep waiting for work that has not become due yet.

What ``abort()`` Does
---------------------

``abort()`` requests immediate shutdown.

Its steps are:

.. code-block:: text

    mark abort requested
    cancel all delayed future work
    request stop on the active callback
    wake the worker

The important difference is:

There is no queued quit event.

Instead, the loop stops as soon as it notices the abort flag.
That means queued callbacks that have not started yet are skipped.

Comparison Table
----------------

.. list-table::
    :header-rows: 1

    * - Operation
      - Active callback
      - Immediate queued callbacks
      - Delayed not-yet-due callbacks
      - Exit style
    * - ``quit()``
      - Asked to stop cooperatively
      - Drained until queued quit event is reached
      - Cancelled
      - graceful
    * - ``abort()``
      - Asked to stop cooperatively
      - skipped after the current callback returns
      - Cancelled
      - immediate-best-effort

.. important::

    Neither ``quit()`` nor ``abort()`` can forcibly terminate arbitrary C++ code in the middle of execution.
    A long-running callback must check its stop token if it wants to react quickly.

Failure Handling
================

If a worker callback throws:

* the exception escapes normal event dispatch
* the worker loop catches it
* the first ``std::exception_ptr`` is stored
* the thread stops processing further work
* ``waitForQuit()`` and ``quitAndWait()`` rethrow that stored exception once the worker has finished

That gives contributors and users one clear rule:

If ``waitForQuit()`` throws, the worker callback failed.

This is intentionally stricter than printing and continuing.
Once a callback threw, the worker thread is considered failed.

Application-Managed Event Threads
=================================

What ``createEventThread()`` Adds
---------------------------------

``Application::createEventThread()`` is more than a convenience factory.
It adds lifecycle management on top of ``EventThread::create()``.

The application:

* creates the thread
* registers a finished callback that prunes the management list
* stores a weak reference in the managed-thread registry

Why Weak References Are Enough
------------------------------

The registry stores ``weak_ptr<EventThread>`` entries, not owning ``shared_ptr`` entries.

That works because the detached worker thread already keeps the object alive through the startup ``shared_ptr`` capture.

This gives the registry two nice properties:

* it can still lock and stop live workers during shutdown
* finished workers naturally disappear once no strong reference remains

The finished callback then removes stale entries promptly so short-lived workers do not accumulate in the registry.

Visual Lifecycle
----------------

.. mermaid::

    flowchart TD
        create["Application::createEventThread()"] --> worker["EventThread::create()"]
        worker --> callback["install finished callback:<br/>prune managed thread list"]
        callback --> registry["store weak_ptr in registry"]
        registry --> result["caller receives EventThreadPtr"]

Managed Shutdown
----------------

During :cpp:any:`Application::shutdown() <erbsland::cterm::ui::Application::shutdown>`,
the application performs this sequence:

.. mermaid::

    flowchart TD
        prune["prune finished entries"] --> collect["collect all still-live managed threads"]
        collect --> quit["call quit() on all of them"]
        quit --> wait["wait until one shared shutdown deadline expires"]
        wait --> warn["print warning for threads that did not stop in time"]
        warn --> pruneAgain["prune the registry again"]

The shutdown timeout is intentionally shared across the whole group.
It is not "timeout per thread".

That keeps shutdown bounded even if many managed threads exist.

Warnings
--------

Two situations generate developer-facing warnings on ``std::cerr``:

* a managed thread does not finish before the deadline
* a managed thread already failed and rethrows during ``waitForQuit()``

This is not meant to recover silently.
It is a signal to the developer that the worker callback ignored cooperative shutdown
or that worker code failed outright.

How This Relates to Surface Scheduling
======================================

It is useful to compare the two systems side by side:

.. list-table::
    :header-rows: 1

    * - Topic
      - Surface scheduling
      - Invocation / EventThread
    * - Public owner
      - ``Surface::scheduler()``
      - ``Application`` or ``EventThread``
    * - Typical use
      - UI-local delayed or repeated work
      - generic cross-thread work delivery
    * - Execution thread
      - application UI thread
      - UI thread or dedicated worker thread
    * - Cancellation model
      - action refs + generations
      - thread-level ``quit()`` / ``abort()``
    * - Delayed transport
      - ``EventScheduler``
      - ``EventScheduler``

This is why both pages belong in the same chapter:

They are two related applications of the same core event infrastructure.

Debugging Checklist
===================

If you need to debug this system, start with these questions:

Callback never runs
-------------------

Check:

* was the application or event thread already stopping when the work was queued?
* for delayed work: was it cleared by ``quit()`` or ``abort()`` before it became due?
* for application work: is the UI event loop still running?

Callback runs on the wrong thread
---------------------------------

Check:

* did the code call the callback directly instead of using ``invoke(...)``?
* is the observed path ``Application::invoke()`` or ``EventThread::invoke()``?

``waitForQuit()`` throws
------------------------

Check:

* which callback threw?
* did the exception happen in the currently running invocation rather than in shutdown logic?

Shutdown warning appears
------------------------

Check:

* did the callback ignore ``stop_token.stop_requested()``?
* did it block in external code without any cooperative exit path?
* did it enqueue delayed work and expect that work to survive ``quit()``?

Worker seems idle but delayed work does not fire
------------------------------------------------

Check:

* was the event scheduled beyond the scheduler limits?
* is the internal event clock overridden in tests?
* if you are debugging tests, remember that the worker loop waits in short slices rather than one long blocking wait

Summary
=======

The main design idea is simple:

Use one event transport model for both UI-thread invocation and background worker loops.

Everything else follows from that choice:

* ``Application::invoke()`` marshals work back onto the UI thread
* ``EventThread`` provides a reusable worker event loop
* ``EventScheduler`` handles delayed delivery in both systems
* stop tokens provide cooperative early exit for running worker callbacks
* managed worker threads integrate into application shutdown instead of floating outside it

If you keep that picture in mind, the code becomes much easier to navigate:

events carry work,
owners decide where the work runs,
and shutdown is expressed as event-loop control instead of ad-hoc thread tricks.
