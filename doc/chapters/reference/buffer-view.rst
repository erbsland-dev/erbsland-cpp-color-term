..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

************
Buffer Views
************

Buffer views expose a rectangular window onto a larger readable buffer.
They are useful whenever the logical content is bigger than the visible
terminal area, for example in scrollable panels, minimaps, editors, and
diagnostic tools.

:cpp:any:`BufferViewBase <erbsland::cterm::BufferViewBase>` provides the shared view behavior, :cpp:any:`BufferView <erbsland::cterm::BufferView>`
owns a shared pointer to the underlying content, and
:cpp:any:`BufferConstRefView <erbsland::cterm::BufferConstRefView>` is the lightweight stack-friendly variant for
temporary rendering.

Usage
=====

Viewing a Portion of a Larger Buffer
------------------------------------

Use a buffer view when only part of a large logical canvas should be
shown on screen.

.. code-block:: cpp

    auto world = Buffer{Size{120, 40}};
    world.fill(Char{" ", Color{fg::Inherited, bg::Black}});
    world.drawText("Visible window", Rectangle{10, 6, 20, 3}, Alignment::Center);

    auto view = BufferConstRefView{world, Rectangle{8, 4, 40, 12}};

    auto settings = UpdateSettings{};
    settings.setShowCropMarks(true);
    terminal.updateScreen(view, settings);

The view translates local coordinates back into the underlying content.
That makes it ideal for rendering only the visible part of a larger
document or world buffer.

Scrolling by Moving the View Rectangle
--------------------------------------

:cpp:any:`BufferViewBase <erbsland::cterm::BufferViewBase>` stores the currently visible rectangle and can be
reused while the user scrolls or pans around a larger buffer.

.. code-block:: cpp

    auto sharedWorld = std::make_shared<Buffer>(world);
    auto view = BufferView{sharedWorld, Rectangle{0, 0, 40, 12}};
    view.setViewRect(Rectangle{16, 10, 40, 12});
    terminal.updateScreen(view);

Moving the view rectangle changes which part of the content is visible
without requiring the underlying buffer to be copied or redrawn.

Choosing Between Shared and Referenced Views
--------------------------------------------

:cpp:any:`BufferView <erbsland::cterm::BufferView>` is the right choice when the view needs to outlive the
current scope or be stored in a larger object. :cpp:any:`BufferConstRefView <erbsland::cterm::BufferConstRefView>` is
the better fit for short-lived wrappers around an existing buffer.

.. code-block:: cpp

    auto sharedBuffer = std::make_shared<Buffer>(Size{80, 24});
    auto cachedView = BufferView{sharedBuffer, Rectangle{4, 4, 30, 10}};

    auto preview = BufferConstRefView{*sharedBuffer, Rectangle{0, 0, 20, 6}};
    terminal.updateScreen(preview);

Both variants implement :cpp:any:`ReadableBuffer <erbsland::cterm::ReadableBuffer>`, so the rest of the rendering
pipeline can treat them like any other source of terminal cells.

Showing Cropped Edges Explicitly
--------------------------------

:cpp:any:`CropEdges <erbsland::cterm::CropEdges>` describes which sides of a view are clipped by the
available content. :cpp:any:`BufferViewBase <erbsland::cterm::BufferViewBase>` can use that information to render
custom per-edge crop characters inside the view itself.

.. code-block:: cpp

    auto sharedBuffer = std::make_shared<Buffer>(world);
    auto view = BufferView{sharedBuffer, Rectangle{8, 4, 40, 12}};
    view.setShowCropCharacters(true);
    view.setCropCharacter(Direction::East, Char{U'▶', fg::BrightYellow});
    view.setCropCharacter(Direction::South, Char{U'▼', fg::BrightYellow});

    const auto cropEdges = CropEdges::fromView(view.viewRect(), sharedBuffer->rect());
    if (cropEdges.isSet(Direction::East)) {
        terminal.printLine("There is more content to the right.");
    }

This is useful for scrollable views where the user should immediately see
that additional content exists beyond the visible window.

Interface
=========

.. doxygenclass:: erbsland::cterm::BufferViewBase
    :members:

.. doxygenclass:: erbsland::cterm::BufferView
    :members:

.. doxygenclass:: erbsland::cterm::BufferConstRefView
    :members:

.. doxygenclass:: erbsland::cterm::CropEdges
    :members:
