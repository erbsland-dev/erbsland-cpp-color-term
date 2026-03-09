..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

******
Output
******

The output classes provide the core rendering workflow for terminal
applications. You can either write text directly to the terminal or
prepare a full-screen buffer and render it in a single update.

Direct output is useful for simple tools and status messages, while
buffer-based rendering is the preferred approach for dashboards,
animations, and structured terminal interfaces.

Usage
=====

Preparing and Rendering a Screen
--------------------------------

``Buffer`` and ``Terminal`` form the core output workflow for
full-screen terminal applications.

.. code-block:: cpp

    auto terminal = Terminal{Size{90, 28}};
    terminal.initializeScreen();
    terminal.setRefreshMode(Terminal::RefreshMode::Overwrite);

    auto buffer = Buffer{terminal.size() - Size{1, 1}};
    buffer.fill(Char{" ", Color{fg::Default, bg::Black}});

    const auto panel = Rectangle{2, 2, buffer.size().width() - 4, buffer.size().height() - 4};
    buffer.drawFilledFrame(
        panel,
        FrameStyle::LightWithRoundedCorners,
        Char{" ", Color{fg::Inherited, bg::Blue}});
    buffer.drawText(
        "Frame and buffer rendering",
        panel.insetBy(Margins{2}),
        Alignment::Center,
        Color{fg::BrightWhite, bg::Blue});

    terminal.updateScreen(buffer);
    terminal.flush();

In this workflow, the entire screen is prepared in memory and then
rendered to the terminal in one operation. This approach makes complex
layouts and animations easier to manage.

Within buffers, ``Inherited`` means “keep the color that is already below”.
If you need an explicit reset to the terminal default color, use ``Default``.

Combining Frames Automatically
------------------------------

When multiple frames intersect, ``CharCombinationStyle`` resolves the
overlapping characters so the resulting frame lines match correctly.

.. code-block:: cpp

    auto frameStyle = Char16Style::lightFrame();
    auto combination = CharCombinationStyle::commonBoxFrame();

    buffer.drawFrame(Rectangle{2, 2, 18, 8}, frameStyle, combination, Color{fg::BrightCyan, bg::Black});
    buffer.drawFrame(Rectangle{8, 5, 20, 8}, frameStyle, combination, Color{fg::BrightYellow, bg::Black});

This mechanism ensures that intersecting frames produce consistent box
drawing characters instead of overlapping glyphs.

Repeating 9-Tile Styles
-----------------------

``Tile9Style`` fills or frames a rectangle with a repeating 3x3 tile
layout. This is useful when the border needs different corner, edge,
and center tiles, but should still stretch cleanly to any size.

.. code-block:: cpp

    const auto panel = Rectangle{2, 2, 30, 10};
    const auto style = Tile9Style::create("╔═╗║ ║╚═╝");

    buffer.fill(panel, style, Color{fg::BrightBlack, bg::Black});
    buffer.drawFrame(panel, style, Color{fg::BrightCyan, bg::Black});

The 9-tile layout covers the normal 3x3 cases. If you also need
special tiles for rectangles that collapse to a single row, a single
column, or a single cell, construct the style with 16 characters
instead.

Direct Writes, Line Buffering, and Smart Overwrites
---------------------------------------------------

``Terminal::print()`` and ``Terminal::write(std::string_view)`` use line
buffering by default. Partial lines stay buffered until a newline is
written or ``flush()`` is called. This keeps incremental log output tidy.

For full-screen applications, ``updateScreen()`` can also keep a back
buffer and only rewrite cells that changed since the previous frame.

.. code-block:: cpp

    terminal.setRefreshMode(Terminal::RefreshMode::Overwrite);
    terminal.setBackBufferEnabled(true);

    while (running) {
        terminal.updateScreen(buffer);
        terminal.flush();
    }

The smart overwrite path is used when ``RefreshMode::Overwrite`` is
active and the terminal back buffer is enabled. If you need every text
write to appear immediately, disable line buffering with
``setLineBufferEnabled(false)``.

``write(const Buffer &)`` is the low-level escape hatch when you want to
dump a buffer exactly as-is without clipping, crop marks, or diff-based
screen updates.

.. figure:: /images/frame-weaver3.jpg
    :width: 100%

    The ``frame-weaver`` demo focuses on frame styles, combinations, and
    buffer-based rendering.

Interface
=========

.. doxygenclass:: erbsland::cterm::Buffer
    :members:

.. doxygenclass:: erbsland::cterm::Char16Style
    :members:

.. doxygentypedef:: erbsland::cterm::Char16StylePtr

.. doxygenclass:: erbsland::cterm::CharCombinationStyle
    :members:

.. doxygentypedef:: erbsland::cterm::CharCombinationStylePtr

.. doxygenclass:: erbsland::cterm::SimpleCharCombinationStyle
    :members:

.. doxygenclass:: erbsland::cterm::MatrixCombinationStyle
    :members:

.. doxygenclass:: erbsland::cterm::Tile9Style
    :members:

.. doxygentypedef:: erbsland::cterm::Tile9StylePtr

.. doxygenenum:: erbsland::cterm::FrameStyle

.. doxygenclass:: erbsland::cterm::UpdateSettings
    :members:

.. doxygenclass:: erbsland::cterm::Terminal
    :members:

.. doxygentypedef:: erbsland::cterm::TerminalPtr
