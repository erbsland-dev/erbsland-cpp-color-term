..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

******
Buffer
******

The buffer classes represent rendered terminal content in memory before
it is written to the screen. :cpp:any:`ReadableBuffer <erbsland::cterm::ReadableBuffer>` provides the inspection
API, :cpp:any:`WritableBuffer <erbsland::cterm::WritableBuffer>` adds mutation and drawing operations, and
:cpp:any:`Buffer <erbsland::cterm::Buffer>` is the concrete 2D storage type used in most applications.
:cpp:any:`RemappedBuffer <erbsland::cterm::RemappedBuffer>` adds efficient row- and column-based reordering for
editors, scrollback views, and other applications that frequently insert, delete, rotate, or move whole lines.

Use these types when you want to build a frame off-screen, compare two
frames, copy content between buffers, or derive masks from the rendered
characters.

Usage
=====

Reading and Writing Through Buffer Interfaces
---------------------------------------------

:cpp:any:`ReadableBuffer <erbsland::cterm::ReadableBuffer>` and :cpp:any:`WritableBuffer <erbsland::cterm::WritableBuffer>` let helper functions operate on
terminal content without depending on one specific implementation.

.. code-block:: cpp

    auto renderStatusPanel(WritableBuffer &target, Rectangle panel) -> void {
        target.fill(panel, Char{" ", Color{fg::Inherited, bg::Blue}});
        target.drawText(
            "Status",
            panel.insetBy(Margins{1}),
            Alignment::TopLeft,
            Color{fg::BrightWhite, bg::Blue});
    }

    auto screen = Buffer{Size{80, 24}};
    renderStatusPanel(screen, Rectangle{2, 2, 24, 8});

Use :cpp:any:`ReadableBuffer <erbsland::cterm::ReadableBuffer>` when a function only needs to inspect content,
count differences, or create a bitmap mask. Use :cpp:any:`WritableBuffer <erbsland::cterm::WritableBuffer>` when
the function should be able to modify the target buffer without caring
whether it is a standalone :cpp:any:`Buffer <erbsland::cterm::Buffer>` or another writable implementation.

Cloning, Copying, and Resizing
------------------------------

:cpp:any:`Buffer <erbsland::cterm::Buffer>` supports the common frame-management tasks needed by
interactive terminal applications: creating a new frame, cloning the
current state, and resizing a buffer when the terminal size changes.

.. code-block:: cpp

    auto current = Buffer{Size{80, 24}};
    current.fill(Char{" ", Color{fg::Inherited, bg::Black}});

    const auto previous = current.clone();
    current.resize(Size{100, 30}, true, Char::space());
    current.setFrom(*previous, Char{" ", Color{fg::Inherited, bg::Black}});

``clone()`` returns a writable copy through the abstract interface, which
makes it easy to store previous frames for diffing or rollback logic.
When resizing a concrete :cpp:any:`Buffer <erbsland::cterm::Buffer>`, use the reorder overload if you want
to preserve existing content while expanding or cropping the canvas.

Working with Remapped Buffers
-----------------------------

:cpp:any:`RemappedBuffer <erbsland::cterm::RemappedBuffer>` is built for workloads where the visible content stays
logically grid-based, but the application constantly reshuffles rows or columns. Instead of rewriting every cell for
line-oriented edits, the buffer keeps remap tables and only refills the rows or columns that become newly empty.

.. code-block:: cpp

    auto history = RemappedBuffer{Size{80, 2'000}, Orientation::Vertical};
    history.fill(Char::space());

    history.eraseRows(0, Char::space(), 1);      // Scroll everything up by one row.
    history.set(Position{0, 1'999}, String{"new log line"});

    history.resize(Size{100, 2'000}, true, Char::space());

Use the plain :cpp:any:`RemappedBuffer::resize() <erbsland::cterm::RemappedBuffer::resize>` overload when you want
the fastest possible resize and you plan to redraw the content anyway. Use the reorder overload when the visible order
must stay intact while expanding or cropping the canvas.

.. important::

    For an efficient render loop, keep a persistent instance of :cpp:any:`Buffer <erbsland::cterm::Buffer>` and simply resize it when the terminal size changes. Reusing the same buffer avoids unnecessary memory allocations and keeps rendering fast.

    A typical render loop might look like this:

    .. code-block:: cpp

        struct MyApp {
            void renderLoop() {
                for (;;) {
                    _terminal.testScreenSize();
                    _buffer.resize(_terminal.size());

                    // Render the current frame into the buffer.
                    _terminal.updateScreen(_buffer);

                    // Handle key presses or other input.
                }
            }

            Terminal _terminal;
            Buffer _buffer;
        };

Building Buffers from Text Lines
--------------------------------

For status panes, generated reports, and static panels, :cpp:any:`Buffer <erbsland::cterm::Buffer>` can be
created directly from line-oriented text.

.. code-block:: cpp

    const auto help = Buffer::fromLinesInString(String{
        "Q  Quit\n"
        "R  Refresh\n"
        "H  Toggle help"});

    auto screen = Buffer{Size{40, 12}};
    screen.setFrom(help, Char::space());

This is often the fastest way to turn preformatted terminal text into a
buffer that can later be positioned inside a larger layout.

Comparing Frames and Deriving Masks
-----------------------------------

:cpp:any:`ReadableBuffer <erbsland::cterm::ReadableBuffer>` also provides analysis helpers that are useful for
tests, animation pipelines, and bitmap-based effects.

.. code-block:: cpp

    const auto changedCells = previous->countDifferencesTo(current);
    const auto frameMask = current.toMask({U'|', U'-', U'+', U'┌', U'┐', U'└', U'┘'});

    if (changedCells > 0 && frameMask.size().contains(Position{0, 0})) {
        // React to the changed frame content.
    }

``toMask()`` is especially handy when a rendered buffer should drive a
later bitmap transformation or when a test wants to reason about the
visible structure instead of raw cell colors.

Interface
=========

.. doxygenclass:: erbsland::cterm::ReadableBuffer
    :members:

.. doxygenclass:: erbsland::cterm::WritableBuffer
    :members:

.. doxygenclass:: erbsland::cterm::Buffer
    :members:

.. doxygenclass:: erbsland::cterm::RemappedBuffer
    :members:
