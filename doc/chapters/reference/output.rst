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
        Char{" ", Color{fg::Default, bg::Blue}});
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

.. doxygenenum:: erbsland::cterm::FrameStyle

.. doxygenclass:: erbsland::cterm::UpdateSettings
    :members:

.. doxygenclass:: erbsland::cterm::Terminal
    :members:

.. doxygentypedef:: erbsland::cterm::TerminalPtr

