..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: implementation notes; action scheduling
    single: implementation notes; UI scheduler
    single: implementation notes; scheduled actions

*****************
Action Scheduling
*****************

This page explains the full action scheduling system used by the UI layer.
It covers the public surface-level API, the internal event transport,
the application loop integration, and the render wake-up behavior.

The goal of this note is practical understanding.
After reading it, a contributor should be able to answer:

* what ``Surface::scheduler()`` really owns
* how a scheduled action travels from API call to callback execution
* why the system uses both an action reference and an action generation
* how stale scheduled action events are ignored safely
* why hidden pages can keep scheduled actions without forcing redraws
* where to start when debugging or extending the scheduling system

Relevant Source Files
=====================

You will find the implementation primarily in:

* :file:`src/erbsland/cterm/ui/Surface.hpp`
* :file:`src/erbsland/cterm/ui/Surface.cpp`
* :file:`src/erbsland/cterm/ui/Application.hpp`
* :file:`src/erbsland/cterm/ui/Application.cpp`
* :file:`src/erbsland/cterm/ui/Display.hpp`
* :file:`src/erbsland/cterm/ui/Display.cpp`
* :file:`src/erbsland/cterm/ui/event/Scheduler.hpp`
* :file:`src/erbsland/cterm/ui/event/Scheduler.cpp`
* :file:`src/erbsland/cterm/ui/event/ScheduledActionRef.hpp`
* :file:`src/erbsland/cterm/ui/event/EventScheduler.hpp`
* :file:`src/erbsland/cterm/ui/event/EventScheduler.cpp`
* :file:`src/erbsland/cterm/ui/event/EventDriver.hpp`
* :file:`src/erbsland/cterm/ui/event/EventDriver.cpp`
* :file:`src/erbsland/cterm/ui/event/EventType.hpp`
* :file:`src/erbsland/cterm/ui/event/impl/ScheduledActionEvent.hpp`
* :file:`src/erbsland/cterm/ui/event/impl/EventClockAccess.hpp`
* :file:`src/erbsland/cterm/ui/event/impl/EventClockAccess.cpp`
* :file:`test/unittest/src/ui/surface/UiSchedulerTest.cpp`
* :file:`demo/ui-hello-world/src/UiHelloWorldApp.cpp`

Why the System Exists
=====================

At the UI level, surfaces often need delayed or repeated work:

* animations
* blinking or ticking status elements
* deferred state updates
* polling-style refreshes that must still run on the UI thread

The implementation deliberately does **not** run these callbacks directly from a background timer.
Instead, scheduled work is funneled back into the normal UI event loop.

That gives the system several important properties:

* scheduled callbacks always run on the application UI thread
* scheduled work is serialized with key handling and other UI events
* surfaces do not need their own threads
* stale or cancelled actions can be ignored at event-delivery time
* the rendering system can stay dirty-driven instead of repainting on a blind interval

In short:

The scheduler is not a standalone timer subsystem.
It is a UI-thread callback delivery system that happens to use time-based wakeups.

High-Level Architecture
=======================

The scheduling system is split into four layers:

.. code-block:: text

    Surface::scheduler()
        |
        v
    ui::Scheduler
        |
        v
    ui::EventScheduler
        |
        v
    EventType::ScheduledAction in Application::handleEvent()
        |
        v
    callback executes on the UI thread

Each layer has one clear responsibility.

Layer 1: ``Surface::scheduler()``
---------------------------------

This is the public entry point.

From the outside, users see a surface-local API:

* ``addSingleShot(...)``
* ``addSingleShot(..., existingRef, ...)``
* ``addRepeated(...)``
* ``setDelayOrInterval(...)``
* ``remove(...)``
* ``removeAll()``

This API is intentionally bound to a surface, because scheduled work is usually part of that surface's state.

Layer 2: ``ui::Scheduler``
--------------------------

``Scheduler`` is the surface-owned action registry.
It stores:

* the callback
* the scheduling mode
* the delay or repeat interval
* the next execution time
* the current generation

It does **not** execute callbacks itself.
Its job is to manage action state and turn that state into scheduled UI events.

Layer 3: ``ui::EventScheduler``
-------------------------------

``EventScheduler`` is the global time-based holding area for future events.
It does not know anything about surfaces or callbacks.
It simply stores ``Event`` objects until their due time arrives, then forwards them into the event queue.

Layer 4: ``Application`` and the event loop
-------------------------------------------

``Application::handleEvent()`` receives ``EventType::ScheduledAction`` events,
resolves the target surface, validates the action generation,
and finally executes the callback on the UI thread.

This keeps scheduled actions aligned with the rest of the UI system:

* key presses
* display updates
* resize polling
* graceful shutdown

Responsibility Split
====================

The most important design rule is:

``Scheduler`` owns action state.
``EventScheduler`` owns time-based event release.
``Application`` owns callback execution.

This split avoids a long list of problems:

* no callback execution while holding the scheduler mutex
* no direct callback execution from a background thread
* no duplicate timing logic inside both scheduler layers
* no need for ``EventScheduler`` to understand surface lifetime
* no need for ``Display`` to understand scheduler internals

Data Model
==========

``ScheduledActionRef``
----------------------

``ScheduledActionRef`` is the stable identity of one logical action slot.
It is returned to the caller and reused for:

* explicit removal
* delay changes
* rescheduling an existing single-shot action

It is allocated from an incrementing counter inside ``Scheduler``.

Think of it as:

Which scheduled action are we talking about?

``ActionEntry``
---------------

Each action stored inside ``Scheduler`` is represented by one ``ActionEntry``.

It stores:

* ``action`` – the callback to run
* ``mode`` – ``SingleShot`` or ``Repeating``
* ``delayOrInterval`` – the active timing configuration
* ``nextExecutionTime`` – the current due time
* ``generation`` – the current version of this action slot

``generation``
--------------

The generation is the most important safety mechanism in the system.

It changes whenever an action is re-armed:

* first schedule
* reschedule of an existing action reference
* explicit delay change
* re-arm of a repeating action after execution

Think of it as:

Which *version* of this scheduled action is still current?

Why the System Needs Both IDs
-----------------------------

The action reference and generation answer different questions.

.. list-table::
    :header-rows: 1

    * - Concept
      - Meaning
      - Why it exists
    * - ``ScheduledActionRef``
      - Stable logical identity
      - Lets callers remove or update a specific action
    * - ``generation``
      - Version of that logical action
      - Lets the system ignore stale scheduled action events

Example:

.. code-block:: text

    actionRef = 17

    generation 1:
        due at 100 ms

    reschedule same actionRef:
        generation 2
        due at 300 ms

    old event arrives:
        (actionRef = 17, generation = 1)
        -> ignored

Without generations, the old ``100 ms`` event could still execute after the action had been moved.

Lifecycle: From API Call to Callback
====================================

The simplest way to understand the system is to trace one action from creation to delivery.

Step 1: A surface creates or updates an action
----------------------------------------------

Example public call:

.. code-block:: cpp

    auto ref = surface->scheduler().addRepeated(
        [surface]() { surface->flags().setPaintOutdated(); },
        std::chrono::seconds{1});

Inside ``Scheduler``:

#. the delay or interval is clamped to the supported range
#. a new or existing action slot is selected
#. the callback and timing fields are updated
#. the generation is incremented
#. the next execution time is computed
#. a scheduled action event is armed in the application

Step 2: ``Scheduler`` arms a future scheduled action event
----------------------------------------------------------

``Scheduler::armAction()`` does not enqueue a callback directly.
Instead it asks the application to schedule an internal event payload:

* ``SurfaceWeakPtr``
* ``ScheduledActionRef``
* ``generation``

This payload is stored as ``impl::ScheduledActionEvent``.

Why a weak surface pointer is used:

* the scheduler must not keep destroyed surfaces alive accidentally
* already queued events must become harmless if the surface disappears

Step 3: ``EventScheduler`` stores the future event
--------------------------------------------------

``EventScheduler`` stores the event in a time-ordered multimap.

It knows:

* when the event should become active
* which ``EventType`` it belongs to
* the event payload object

It does **not** know:

* whether the surface still exists later
* whether the action has been removed or rescheduled
* whether the event is already stale

That validation is intentionally deferred until delivery.

Step 4: The application loop wakes up
-------------------------------------

The application loop combines three wake-up sources:

* keyboard input
* the next due scheduled event in ``EventScheduler``
* the next display wake-up required by resize polling or dirty rendering

The important consequence is:

The UI loop sleeps until the next relevant reason to wake up.

Step 5: Due scheduled action events enter the normal event queue
----------------------------------------------------------------

``EventScheduler::poll()`` moves all due events into ``EventDriver``.

From this point onward, scheduled actions are normal queued UI events.

This is the key architectural boundary:

Time-based release happens in ``EventScheduler``.
Actual UI execution happens in ``Application::handleEvent()``.

Step 6: ``Application::handleEvent()`` validates and executes
--------------------------------------------------------------

When the event type is ``EventType::ScheduledAction``, the application:

#. extracts the ``ScheduledActionEvent``
#. tries to lock the ``SurfaceWeakPtr``
#. checks whether the surface still has a scheduler
#. asks the scheduler to execute the action using ``actionRef`` and ``generation``

Then ``Scheduler::executeAction()`` performs the final safety check:

* if the action ref no longer exists, ignore the event
* if the generation no longer matches, ignore the event
* otherwise copy the callback out and execute it on the UI thread

End-to-End Flow Diagram
-----------------------

.. code-block:: text

    caller
      |
      | addSingleShot / addRepeated / reschedule
      v
    Surface::scheduler()
      |
      v
    Scheduler
      |
      | create or update ActionEntry
      | increment generation
      | compute nextExecutionTime
      v
    Application::scheduleScheduledAction()
      |
      v
    EventScheduler
      |
      | wait until due time
      v
    EventDriver queue
      |
      v
    Application::handleEvent()
      |
      | resolve surface
      | validate ref + generation
      v
    Scheduler::executeAction()
      |
      v
    callback on UI thread

Cancellation and Stale Event Handling
=====================================

This system assumes that scheduled action events may already be in flight when the logical action changes.

That is not a corner case.
It is the normal design.

When an event becomes stale
---------------------------

A scheduled action event is considered stale if any of the following is true:

* the target surface no longer exists
* the surface no longer has a scheduler
* the action reference no longer exists in the scheduler
* the generation stored in the event no longer matches the current action generation

The system does not try to remove these events from every queue retroactively.
Instead it makes them cheap and safe to ignore.

Why ignoring stale events is better than trying to retract them
---------------------------------------------------------------

Once an event has been handed to ``EventScheduler`` or ``EventDriver``,
it may already be queued in multiple internal structures.
Trying to walk back through all of those structures would make the design
much more complex and much more fragile.

Generation-based invalidation is simpler:

* rescheduling only updates current action state
* removal only erases current action state
* old events can still arrive
* old events fail validation and become no-ops

This is one of the core design decisions of the implementation.

Single-Shot Actions
===================

Single-shot actions execute at most once.

Delivery behavior:

#. validate action reference and generation
#. copy out the callback
#. execute it
#. remove the action if it still has the same generation

That final condition matters.
If the callback itself reschedules the same action reference,
the generation will have changed and the old post-callback cleanup must not erase the new version.

Repeating Actions
=================

Repeating actions use **fixed-rate** semantics.

That means:

The next due time is based on the previous target time, not on callback completion time.

If an action with a ``1 s`` interval was due at:

* 10 s
* 11 s
* 12 s

and the loop wakes up late at ``12.8 s``,
the system may process multiple due scheduled action events in quick succession until the schedule catches up.

Why fixed-rate was chosen
-------------------------

Fixed-rate behavior is better for:

* ticking clocks
* periodic polling
* animation phases
* anything that should track wall time rather than callback duration

It avoids silent drift.

Pseudo code for repeating re-arm:

.. code-block:: text

    execute callback

    nextExecutionTime = previousNextExecutionTime + interval
    generation += 1
    arm new scheduled action event

That ``previousNextExecutionTime`` detail is what makes the behavior fixed-rate.

Lifetime and Visibility
=======================

Surface lifetime and surface visibility are separate concerns.

Surface lifetime
----------------

Scheduled actions belong to a surface object.
If the surface is destroyed, future scheduled action events must do nothing.

This is handled by:

* storing ``SurfaceWeakPtr`` in the event payload
* not keeping strong ownership in the event system
* re-validating the surface on delivery

Visibility
----------

A surface may remain alive while being:

* covered by an opaque page
* detached from the active top page
* logically inactive from the user's point of view

The design intentionally lets scheduled actions keep running in that state.

Reason:

Scheduling is tied to object lifetime, not page visibility.

This allows hidden surfaces to maintain internal state without reactivation code.

However, hidden surfaces must **not** force pointless redraws.
That is handled by the display layer, not the scheduler.

Rendering and Dirty-Driven Wakeups
==================================

The display is dirty-driven and only wakes for rendering when it has a reason.

What counts as a render reason
------------------------------

``Display`` wakes for render work when one of these is true:

* a resize is pending
* a visible page has outdated layout
* a visible page has outdated paint

The check is visibility-aware:

* pages are inspected from top to bottom
* once an opaque page is reached, deeper pages are ignored

This means:

* hidden pages can keep scheduled actions
* hidden pages can mutate state
* hidden pages do not force redraws until they become visible again

Render throttling
-----------------

Rendering is still throttled by ``Display::cMinimumRenderInterval``.
So the system is:

* prompt when the UI becomes dirty
* but not unbounded in redraw frequency

Typical scheduled callback behavior
-----------------------------------

Scheduled callbacks do **not** automatically invalidate surfaces.

That choice is deliberate.
The callback must explicitly call:

* ``surface->flags().setPaintOutdated()``
* ``surface->flags().setLayoutOutdated()``

when it changes visible state.

Why the framework does not auto-invalidate every callback:

* some callbacks may update non-visual state only
* some callbacks may decide conditionally whether anything changed
* explicit invalidation matches the rest of the UI design

Threading Model
===============

There are two separate thread-safety stories.

Scheduler management
--------------------

``Scheduler`` management methods are thread-safe.

This includes:

* adding actions
* rescheduling actions
* removing actions
* clearing all actions

These operations are protected by the scheduler mutex.

Callback execution
------------------

Callbacks are **not** executed concurrently by the scheduler.
They are executed on the application UI thread via the normal event loop.

So the safe mental model is:

* many threads may request changes to the schedule
* exactly one UI thread executes callbacks

This gives background threads a safe way to marshal delayed work back into the UI layer.

Time Source and Testability
===========================

The implementation uses ``impl::EventClockAccess`` instead of directly calling ``EventClock::now()`` everywhere.

This exists purely to make the system testable.

Why the seam matters
--------------------

Without a clock seam, scheduler tests would need:

* real sleeping
* wide timeouts
* timing-sensitive assertions

That would make the test suite slower and more fragile.

With ``EventClockAccess``:

* unit tests can use deterministic virtual time
* scheduled action delivery can be tested without wall-clock waiting
* repeat catch-up behavior can be checked precisely

What the scheduler tests cover
------------------------------

``UiSchedulerTest`` currently verifies:

* single-shot execution on the UI thread
* suppression of stale scheduled action events after rescheduling
* fixed-rate repeat catch-up
* ``removeAll()`` cancellation
* surface destruction cancellation
* cross-thread scheduling with UI-thread callback delivery
* prompt redraw for visible scheduled-action changes
* lack of redraw for hidden-page scheduled actions

These tests are the best executable specification of the design.

Common Extension Points
=======================

If you need to change behavior, these are the usual entry points.

Change public scheduling behavior
---------------------------------

Start in:

* :file:`src/erbsland/cterm/ui/event/Scheduler.hpp`
* :file:`src/erbsland/cterm/ui/event/Scheduler.cpp`

Examples:

* add pause or resume behavior
* introduce different repeat policies
* extend action metadata

Change event transport behavior
-------------------------------

Start in:

* :file:`src/erbsland/cterm/ui/event/EventScheduler.hpp`
* :file:`src/erbsland/cterm/ui/event/EventScheduler.cpp`
* :file:`src/erbsland/cterm/ui/Application.cpp`

Examples:

* change event wake-up strategy
* add more time-aware event types
* change event-loop scheduling policy

Change render wake-up behavior
------------------------------

Start in:

* :file:`src/erbsland/cterm/ui/Display.hpp`
* :file:`src/erbsland/cterm/ui/Display.cpp`

Examples:

* alter visible-page dirty detection
* change minimum render interval semantics
* introduce additional wake-up conditions

Debugging Checklist
===================

When a scheduled action does not run:

* Was the application already active when the action was armed?
* Was the action removed or rescheduled, causing a generation mismatch?
* Was the surface destroyed before delivery?
* Is the callback waiting on a wake-up that never arrives because nothing is polling the application loop?

When a scheduled action runs but nothing repaints:

* Did the callback call ``surface->flags().setPaintOutdated()`` or ``surface->flags().setLayoutOutdated()``?
* Is the surface currently hidden behind an opaque page?
* Is the display still inside the minimum render interval?

When an action appears to run more than once:

* Is it repeating rather than single-shot?
* Did the callback reschedule itself explicitly?
* Is the observed burst actually fixed-rate catch-up after a delayed loop iteration?

Reading Order for Contributors
==============================

If you want the fastest path to understanding the implementation, read in this order:

#. :file:`src/erbsland/cterm/ui/Surface.hpp`
   to see the public entry point.
#. :file:`src/erbsland/cterm/ui/event/Scheduler.hpp`
   to understand the stored action model.
#. :file:`src/erbsland/cterm/ui/event/Scheduler.cpp`
   to see scheduling, validation, and repeat re-arming.
#. :file:`src/erbsland/cterm/ui/event/impl/ScheduledActionEvent.hpp`
   to see what is actually transported through the event system.
#. :file:`src/erbsland/cterm/ui/Application.cpp`
   to see the event-loop integration and callback delivery.
#. :file:`src/erbsland/cterm/ui/Display.cpp`
   to understand redraw wake-ups and visibility-aware dirtiness.
#. :file:`test/unittest/src/ui/surface/UiSchedulerTest.cpp`
   to confirm the intended behavior in executable form.

That path mirrors the real data flow from user API to callback execution.

Demo Anchor
===========

The simplest live example is the clock in the UI hello-world demo:

* :file:`demo/ui-hello-world/src/UiHelloWorldApp.cpp`

It uses a repeating scheduled action to update the center header text once per second.
That demo is intentionally small and is a good first stop when you want to see the public API in use
without the complexity of the lower-level implementation.
