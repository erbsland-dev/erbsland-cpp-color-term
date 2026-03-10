..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

********
Geometry
********

The geometry classes provide the building blocks for positioning and
layout inside a terminal buffer. They describe sizes, positions,
rectangles, and directions, and allow you to derive new regions from
existing ones.

Using explicit geometry types keeps layout code readable and makes it
easy to construct structured terminal interfaces.

Usage
=====

Building a Layout from Rectangles
---------------------------------

The geometry types are designed to make screen layout explicit and easy
to understand. Instead of calculating coordinates manually, you derive
smaller regions from a larger canvas.

.. code-block:: cpp

    const auto canvas = Rectangle{0, 0, 80, 24};
    const auto header = canvas.subRectangle(Anchor::TopCenter, Size{0, 3}, Margins{1, 2, 0, 2});
    const auto content = canvas.insetBy(Margins{4, 2, 2, 2});

    buffer.drawText("Header", header, Alignment::Center, Color{fg::BrightWhite, bg::Blue});
    buffer.drawText("Main content", content, Alignment::TopLeft, Color{fg::BrightWhite, bg::Black});

In this example, the screen is divided into a header area and a content
region derived from the main canvas.

Working with Positions and Directions
-------------------------------------

``Position``, ``Size``, and ``Direction`` are useful when implementing
custom cursor movement, grid logic, or simple simulations.

.. code-block:: cpp

    auto cursor = Position{10, 5};
    cursor += Direction::fromString("east").toDelta();

    if (Size{80, 24}.contains(cursor)) {
        buffer.set(cursor, Char{"@", Color{fg::BrightYellow, bg::Black}});
    }

Here a cursor moves one step to the east. The resulting position is then
checked against the available screen size before rendering a character.

Splitting a Rectangle into Grid Cells
-------------------------------------

``Rectangle::gridCells()`` divides a larger canvas into evenly spaced
sub-rectangles. This is useful for dashboards, menu grids, and
multi-panel layouts where each cell should keep a predictable size.

.. code-block:: cpp

    const auto canvas = Rectangle{0, 0, 80, 24};
    const auto cells = canvas.gridCells(2, 3, 2, 1);

    for (const auto &cell : cells) {
        buffer.drawFrame(cell, FrameStyle::Light, Color{fg::BrightCyan, bg::Black});
    }

    buffer.drawText("Overview", cells[0].insetBy(Margins{1}), Alignment::TopLeft);
    buffer.drawText("Status", cells[1].insetBy(Margins{1}), Alignment::TopLeft);
    buffer.drawText("Log", cells[2].insetBy(Margins{1}), Alignment::TopLeft);

If the requested number of rows, columns, and spacing no longer fits
into the rectangle, ``gridCells()`` throws ``std::invalid_argument``.

.. figure:: /images/text-gallery3.jpg
    :width: 100%

    The ``text-gallery`` demo uses rectangles and alignment to place
    multiple panels on one screen.

Interface
=========

.. doxygenenum:: erbsland::cterm::Alignment

.. doxygenenum:: erbsland::cterm::Anchor

.. doxygenclass:: erbsland::cterm::Direction
    :members:

.. doxygenclass:: erbsland::cterm::Margins
    :members:

.. doxygenclass:: erbsland::cterm::Position
    :members:

.. doxygenclass:: erbsland::cterm::Rectangle
    :members:

.. doxygenclass:: erbsland::cterm::Size
    :members:
