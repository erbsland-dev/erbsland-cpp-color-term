..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: implementation notes; posix backend

*************
POSIX Backend
*************

This page documents the internal POSIX backend used on Linux, macOS, and similar systems.
It explains how the backend configures the terminal, reads key input, detects terminal size, and restores the screen when the process is terminated by a signal.

Relevant Source Files
=====================

You will find the implementation primarily in:

* :file:`src/erbsland/cterm/impl/PosixBackend.hpp`
* :file:`src/erbsland/cterm/impl/PosixBackend.cpp`
* :file:`src/erbsland/cterm/impl/PosixSignalDispatcher.hpp`
* :file:`src/erbsland/cterm/impl/PosixSignalDispatcher.cpp`

Initialization and Restore
==========================

The POSIX backend is implemented by ``erbsland::cterm::impl::PosixBackend``.
As on Windows, the library expects exactly one active backend instance and stores it in a global pointer protected by a mutex.

The constructor stores the global instance and, unless
:cpp:any:`TerminalFlag::NoSignalHandling <erbsland::cterm::TerminalFlag::NoSignalHandling>` is set,
creates a ``PosixSignalDispatcher`` that forwards termination signals out of the raw signal-handler context.

``initializePlatform()`` is intentionally lightweight.
It does not switch the terminal into raw mode immediately.
Instead, it resets the backend state used for screen-size detection:

#. ``_firstScreenSizeDetection`` is set so that the next size probe performs the extended startup check.
#. ``_hasNoTerminalAttached`` is cleared.
#. The cached last size is cleared.
#. The repeated-failure counter is reset.
#. The backend is marked as initialized.

The actual terminal-mode change for interactive key input happens later, when the input mode switches to ``Input::Mode::Key``.

``restorePlatform()`` performs cleanup in this order:

#. It closes any cached TTY file descriptor used for screen-size detection.
#. If the backend has not concluded that no terminal is attached, it emits ``ESC[?1049l`` when the alternate screen is active, then emits ``ESC[0m`` and ``ESC[?25h``.
#. It writes a trailing newline and flushes ``std::cout``.
#. If a raw key-input session is active, it restores the saved ``termios`` state with ``tcsetattr(STDIN_FILENO, TCSANOW, &_originalState)``.

This separation is important:

* Terminal escape-sequence cleanup is skipped once the backend knows that no terminal is attached.
* ``termios`` restoration is still attempted when a key-input session is active.
* The cached ``/dev/tty`` descriptor is always closed so it cannot leak across the lifetime of the process.

Reading Keys from the Console
=============================

The POSIX backend supports two input paths.

In ``Input::Mode::ReadLine``, ``readKey()`` delegates to ``readLine()``, and ``readLine()`` uses ``std::getline(std::cin, input)``.

In ``Input::Mode::Key``, the backend switches standard input into a non-canonical, no-echo mode.
That setup happens in ``initializeKeyInputSession()``:

#. It calls ``tcgetattr(STDIN_FILENO, &_originalState)`` and stores the original terminal state.
#. It copies that state and clears ``ICANON`` and ``ECHO`` in ``c_lflag``.
#. It sets ``VMIN = 0`` and ``VTIME = 0`` so reads do not block while waiting for a complete cooked line.
#. It applies the modified state immediately with ``tcsetattr(STDIN_FILENO, TCSANOW, &rawState)``.
#. Only after that succeeds does it mark the key-input session as active.

``restoreKeyInputSession()`` restores the original ``termios`` state and clears the session flag.

Once key mode is active, ``readKey()`` works as follows:

#. It prepares a ``select()`` call for ``STDIN_FILENO``.
#. If the requested timeout is greater than zero, it converts that timeout into ``timeval`` and passes it to ``select()``.
#. If the timeout is zero, it passes ``nullptr`` as the timeout pointer, which means to wait indefinitely.
#. When data becomes available, it reads up to 32 bytes from standard input with ``read()``.
#. If the first byte is ``ESC``, it performs additional 1 ms ``select()`` polls and appends more bytes as long as more data arrives immediately.
#. It passes the complete byte sequence to :cpp:any:`Key::fromConsoleInput() <erbsland::cterm::Key::fromConsoleInput>`.

This short polling phase after ``ESC`` is important because cursor keys and function keys usually arrive as multi-byte ANSI escape sequences.
Without it, the parser would often see only the leading ``ESC`` and could not reliably distinguish a plain Escape key from a longer control sequence.

Detecting Terminal Size and Interactive Terminal Availability
=============================================================

The POSIX backend exposes this state through ``isInteractive()``.
Internally, it derives the answer from screen-size probing instead of maintaining a separate detection path.

The first call to ``detectScreenSize()`` is more aggressive than later calls:

#. It records the time of the attempt.
#. It retries up to ten times.
#. It sleeps for 100 ms between retries.
#. If any attempt succeeds, it caches the size and returns it immediately.
#. If an attempt proves that no terminal is attached, it sets ``_hasNoTerminalAttached`` and stops trying.

Later calls are rate-limited to one real probe every 100 ms.
If a call arrives sooner, the backend simply returns the last cached size.

The actual probe is implemented in ``getScreenSize()``:

#. If the backend already has a file descriptor that worked before, it tries that descriptor first.
#. If the cached descriptor no longer works, it closes it and falls back to full detection.
#. It then tries ``STDOUT_FILENO``, ``STDERR_FILENO``, and ``STDIN_FILENO``.
#. If none of those succeed, it tries to open ``/dev/tty`` and probe that descriptor.
#. When ``/dev/tty`` succeeds, the backend keeps that descriptor open and caches it for later probes.

Each candidate descriptor is checked by ``getScreenSizeForFd()``:

#. Invalid descriptors, and descriptors for which ``isatty(fd) == 0``, are classified as ``NoTerminalAttached``.
#. Otherwise, the backend calls ``ioctl(fd, TIOCGWINSZ, &ws)``.
#. If ``ioctl()`` fails, or the returned row or column count is not positive, the result is ``NoTerminalSize``.
#. Only positive ``ws_col`` and ``ws_row`` values are accepted as success.

This produces three logical outcomes:

* ``Success`` means that a TTY exists and returned a usable size.
* ``NoTerminalSize`` means that a TTY may exist, but the backend could not retrieve a usable window size from it.
* ``NoTerminalAttached`` means that none of the checked descriptors behaved like a TTY at all.

If all checked candidates behave like ``NoTerminalAttached``, the backend remembers that no terminal is present, ``isInteractive()`` starts returning ``false``, and later terminal-specific cleanup is skipped.
If later incremental probes fail repeatedly without proving success, the backend increments ``_noSizeFailureCount`` and, after more than 100 such failures, also gives up and marks the environment as having no terminal attached.

Signal-Based Termination and Screen Restore
===========================================

The POSIX backend handles ``SIGINT``, ``SIGTERM``, ``SIGHUP``, and ``SIGQUIT`` through ``PosixSignalDispatcher``.
The key design rule is that the actual terminal restore must never run inside the raw POSIX signal handler.

Why the Signal Handler Must Stay Tiny
-------------------------------------

POSIX signal handlers run in an asynchronous context.
At that point, the interrupted thread may already hold locks, may be in the middle of a C++ library call, or may be using memory-allocation internals.
For that reason, most normal C and C++ operations are unsafe inside a signal handler.

In particular, the backend restore path is not signal-handler-safe because it uses:

* mutexes in ``restoreGlobalPlatform()``
* iostream operations such as ``std::cout``
* ``tcsetattr()``
* normal C++ object state
* code paths that are not documented as async-signal-safe

Running terminal cleanup directly inside ``onSignal()`` would therefore risk deadlocks, reentrancy bugs, or only partially restored terminal state.

The signal handler is intentionally reduced to the smallest practical operation:

* read the pre-published write end of a pipe from a ``volatile sig_atomic_t``
* call ``write()`` to forward the received signal number

That is the main reason the dispatcher exists at all.

How the Pipe-Based Handoff Works
--------------------------------

``PosixSignalDispatcher`` decouples the asynchronous signal context from normal backend code by using a unidirectional pipe and a dedicated watcher thread.

The full mechanism works like this:

#. ``openSignalPipe()`` creates a pipe.
#. The dispatcher switches the write end to non-blocking mode with ``fcntl(..., F_SETFL, ... | O_NONBLOCK)``.
#. It publishes that write descriptor through the static ``_signalWriteFd`` so the signal handler can access it without touching object state.
#. ``startWatcher()`` launches a thread that blocks in ``read()`` on the read end of the pipe.
#. ``registerHandlers()`` installs ``PosixSignalDispatcher::onSignal()`` for ``SIGINT``, ``SIGTERM``, ``SIGHUP``, and ``SIGQUIT``, while preserving previously installed handlers in ``_previousSignalActions``.
#. When a signal arrives, ``onSignal()`` writes the signal number as an ``int`` into the pipe and returns immediately.
#. The watcher thread wakes up, reads exactly one forwarded integer, and invokes the configured callback on a normal thread.
#. That callback is ``PosixBackend::handleProcessSignal()``.

Two subtle details matter here:

* The forwarded payload is only one ``int``, which is comfortably smaller than ``PIPE_BUF``.
  On POSIX systems, that means the pipe write is atomic, so the watcher never has to reconstruct messages from partial interleaving with other writes.
* Because the write end is non-blocking, a full pipe does not deadlock the signal handler.
  The write can fail and the signal may be dropped.
  The implementation intentionally accepts that trade-off because the alternative would be unsafe blocking behavior inside the signal handler.

The watcher thread itself remains simple:

* ``read() == 0`` means the pipe was closed, so the watcher exits.
* ``read() < 0`` with ``errno == EINTR`` causes a retry.
* Any short read is ignored.
* The first valid forwarded signal triggers the callback, and then the watcher exits.

This "first signal wins" behavior matches the intended process-lifetime model:
once a termination signal is received, the backend restores the terminal and the process is expected to end immediately afterward.

What Happens After the Signal Reaches the Watcher Thread
--------------------------------------------------------

``PosixBackend::handleProcessSignal()`` runs in normal thread context, not in the signal handler.
That makes it safe to execute the normal restore logic.

The method performs these steps:

#. It calls ``restoreGlobalPlatform()`` to restore the screen, cursor, colors, and any active raw-input session.
#. It installs the default disposition for the received signal with ``sigaction(signalNumber, SIG_DFL, ...)``.
#. It sends the same signal to the current process with ``kill(getpid(), signalNumber)``.
#. If process termination does not happen immediately for some reason, it falls back to ``std::_Exit(128 + signalNumber)``.

Re-raising the original signal is important because it preserves the normal process semantics that shells, parent processes, and debuggers expect.
The fallback ``_Exit`` call exists only to ensure that the process still terminates even if signal re-delivery unexpectedly does not finish the job.

Normal Shutdown of the Dispatcher
---------------------------------

When the backend is destroyed during a normal, non-signal shutdown, the dispatcher tears itself down carefully:

#. It restores the previously installed handlers with ``unregisterHandlers()``.
#. It writes a dedicated shutdown token ``-1`` into the pipe.
#. The watcher thread wakes up, sees that token, and exits without calling the signal callback.
#. The destructor joins the watcher thread.
#. It clears ``_signalWriteFd`` and closes both ends of the pipe.

This shutdown path uses the same wake-up mechanism as real signals.
That keeps the watcher logic compact and avoids the need for a separate cancellation mechanism.
