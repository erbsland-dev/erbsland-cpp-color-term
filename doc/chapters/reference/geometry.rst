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

