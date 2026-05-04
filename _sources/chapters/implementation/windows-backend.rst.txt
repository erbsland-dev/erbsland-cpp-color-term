..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: implementation notes; windows backend

***************
Windows Backend
***************

This page documents the internal Windows backend used by
:cpp:any:`Terminal <erbsland::cterm::Terminal>`.
It is intended for contributors who need to understand where Windows-specific terminal behavior lives and how shutdown cleanup is coordinated.

For the public backend API contract that this implementation satisfies,
see :doc:`../reference/backend`.

Relevant Source Files
=====================

You will find the implementation primarily in:

* :file:`src/erbsland/cterm/impl/WindowsBackend.hpp`
* :file:`src/erbsland/cterm/impl/WindowsBackend.cpp`
* :file:`src/erbsland/cterm/impl/WindowsSignalDispatcher.hpp`
* :file:`src/erbsland/cterm/impl/WindowsSignalDispatcher.cpp`

Initialization and Restore
==========================

The Windows backend is implemented by ``erbsland::cterm::impl::WindowsBackend``.
As on the POSIX side, the library expects exactly one active backend instance at a time.
The constructor stores that global instance and, unless
:cpp:any:`TerminalFlag::NoSignalHandling <erbsland::cterm::TerminalFlag::NoSignalHandling>` is set,
creates a ``WindowsSignalDispatcher`` that watches for console termination events.

``initializePlatform()`` performs platform setup in the following order:

#. It switches the console input and output code pages to UTF-8 with ``SetConsoleCP()`` and ``SetConsoleOutputCP()``.
#. It disables iostream synchronization with the C stdio layer and detaches ``std::cin`` from the implicit ``std::cout`` flush, so console I/O is driven only by the C++ streams used by the backend.
#. It enables ``ENABLE_VIRTUAL_TERMINAL_PROCESSING`` on the standard output handle so the Windows console interprets ANSI escape sequences.
#. It queries the current cursor visibility with ``GetConsoleCursorInfo()``, stores that state, and hides the cursor with ``SetConsoleCursorInfo()``.
#. It marks the backend as initialized.

The backend does not switch into a special key-input mode during initialization.
Input remains in ``Input::Mode::ReadLine`` until the caller explicitly switches to key mode.

``restorePlatform()`` performs the corresponding cleanup:

#. If initialization never completed, it returns immediately.
#. If the backend saved a cursor state, it restores that state through the Win32 cursor API.
#. If the backend believes the alternate screen buffer is active, it emits ``ESC[?1049l`` to leave it.
#. It emits ``ESC[0m`` to reset colors and ``ESC[?25h`` to force the cursor visible.
#. It writes a trailing newline and flushes ``std::cout``.

Two details are worth keeping in mind when you extend this code:

* ``supportsCursorVisibilityCodes()`` returns ``false``, so normal cursor visibility changes use the Win32 API rather than ANSI escape sequences.
* The restore path still emits ``ESC[?25h`` as an additional safety step, even though the primary cursor restoration is done through the Win32 API.

Reading Keys from the Console
=============================

The Windows backend supports two input paths.

In ``Input::Mode::ReadLine``, both ``readKey()`` and ``waitForKey()`` delegate to ``readLine()``, and ``readLine()``
uses ``std::getline(std::cin, input)``.

In ``Input::Mode::Key``, ``readKey()`` works directly with the Windows console input queue:

#. It waits on ``STD_INPUT_HANDLE`` with ``WaitForSingleObject()``.
#. A timeout less than or equal to ``0`` performs a non-blocking poll.
#. Any positive timeout is passed to ``WaitForSingleObject()`` as milliseconds.
#. After the wait succeeds, the backend repeatedly calls ``GetNumberOfConsoleInputEvents()`` and ``ReadConsoleInputW()`` until the queued events are drained.
#. It ignores all non-key events.
#. It ignores key-release events and only reacts to records with ``bKeyDown != 0``.
#. It maps well-known virtual key codes such as arrows, page navigation keys, insert, delete, enter, escape, tab, backspace, and ``F1`` through ``F12`` to the corresponding :cpp:any:`Key <erbsland::cterm::Key>` values.
#. For other keys, it decodes ``uChar.UnicodeChar`` as UTF-16, including surrogate pairs, and collects the resulting
   Unicode text input.

``waitForKey()`` follows the same path but uses an infinite wait on ``WaitForSingleObject()``.

The return value is the last recognized key seen while draining the queue.
As a result, one call can consume multiple queued key events but returns only one library key.

Character input is decoded from the UTF-16 payload that ``ReadConsoleInputW()`` provides.
Special keys are handled through virtual key codes, while textual input comes from the Unicode character field of the
console record.

Detecting Terminal Size and Interactive Console Availability
============================================================

The Windows backend exposes ``isInteractive()`` directly.
Internally, interactivity is inferred from whether the backend can successfully query the active console screen buffer.

``detectScreenSize()`` uses the following logic:

#. It obtains ``STD_OUTPUT_HANDLE`` with ``GetStdHandle()``.
#. If the handle is null or invalid, it returns ``std::nullopt``.
#. It calls ``GetConsoleScreenBufferInfo()`` on that handle.
#. If that call fails, it returns ``std::nullopt``.
#. It computes the visible terminal dimensions from ``info.srWindow`` rather than from the full scrollback buffer size.
#. If the computed width or height is not positive, it returns ``std::nullopt``.
#. Otherwise, it returns :cpp:any:`Size <erbsland::cterm::Size>` with the visible window width and height.

For contributors, this means:

* ``isInteractive()`` and ``detectScreenSize()`` rely on the same console-buffer probe.
* A successful ``GetConsoleScreenBufferInfo()`` call is effectively treated as proof that an interactive console is present.
* Redirected output or non-console hosts usually appear as "no detectable screen size".
* The reported size is the visible console window, not the total underlying buffer.

Handling Termination Events and Restoring the Screen
====================================================

The Windows backend uses ``WindowsSignalDispatcher`` to move termination handling out of the raw console-control callback and into a normal worker thread.
This mirrors the POSIX design goal, even though the Windows callback model differs from POSIX signals.

The dispatcher is set up like this:

#. The constructor stores a global dispatcher pointer in an atomic variable.
#. It starts a watcher thread that waits on a condition variable.
#. It registers a console-control handler with ``SetConsoleCtrlHandler()``.

The registered callback ``onConsoleControl()`` recognizes these events:

* ``CTRL_C_EVENT`` mapped to exit code ``130``
* ``CTRL_BREAK_EVENT`` mapped to exit code ``131``
* ``CTRL_CLOSE_EVENT``, ``CTRL_LOGOFF_EVENT``, and ``CTRL_SHUTDOWN_EVENT`` mapped to exit code ``1``

The callback does not restore the terminal directly.
Instead, it calls ``pushSignal()``, which stores the first pending exit code under a mutex and notifies the watcher thread.
Additional events are ignored once one exit code is already pending.

The watcher thread wakes up, extracts the exit code, releases the mutex, and calls the backend callback in a normal thread context.
That backend callback is ``WindowsBackend::handleProcessSignal()``:

#. It calls ``restoreGlobalPlatform()`` so the cursor, alternate screen, and color state are cleaned up.
#. It terminates the process immediately with ``std::_Exit(exitCode)``.

When the backend or dispatcher is destroyed during normal shutdown, the dispatcher first unregisters the console-control handler, then stops and joins the watcher thread, and finally clears the global dispatcher pointer.

For maintainers, the key point is that screen restoration never happens inside the raw Windows console-control callback itself.
The callback only forwards the event.
The actual cleanup runs on the dedicated watcher thread, where mutexes, iostreams, and the usual backend code are safe to use.
