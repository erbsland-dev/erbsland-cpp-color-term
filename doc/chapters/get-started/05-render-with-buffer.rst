..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

Render the Screen from a Buffer
===============================

Now we replace the direct output example with a small buffer-based layout.
This is the point where the library starts to feel more like a terminal UI
toolkit than a simple print helper.

Instead of writing text directly to the terminal, the application now
prepares the entire screen in memory and renders it in a single update.

Replace :file:`signal-board/src/main.cpp`
=========================================

Replace the contents of the file with the following version:

.. code-block:: cpp
    :caption: <project>/signal-board/src/main.cpp

    #include <erbsland/cterm/all.hpp>

    using namespace erbsland::cterm;

    auto main() -> int {
        auto terminal = Terminal{Size{80, 25}};
        terminal.initializeScreen();

        auto buffer = Buffer{terminal.size() - Size{1, 1}};
        buffer.fill(Char{" ", Color{fg::Default, bg::Black}});

        const auto rect = Rectangle{0, 0, buffer.size().width(), 6};
        buffer.drawFilledFrame(
            rect,
            FrameStyle::LightWithRoundedCorners,
            Char{" ", Color{fg::Default, bg::Blue}});
        buffer.drawText(
            "Signal Board",
            Rectangle{2, 1, rect.width() - 4, 1},
            Alignment::Center,
            Color{fg::BrightWhite, bg::Blue});
        buffer.drawText(
            "The whole screen is prepared in memory and rendered in one pass.",
            Rectangle{2, 3, rect.width() - 4, 2},
            Alignment::CenterLeft,
            Color{fg::BrightWhite, bg::Blue});

        auto settings = UpdateSettings{};
        settings.setMinimumSize(Size{40, 8});
        terminal.updateScreen(buffer, settings);
        terminal.flush();
        terminal.restoreScreen();
        return 0;
    }

What Changed
============

This version introduces several important concepts:

* ``Buffer`` stores a complete screen-sized grid of colored terminal cells.
* ``fill()`` clears the buffer and defines the background color.
* ``drawFilledFrame()`` draws both the frame and its interior in a single call.
* ``drawText()`` places text inside rectangles without manually managing the cursor.
* ``UpdateSettings`` allows you to define constraints such as the minimum supported terminal size.

Because the entire scene is prepared in memory, the application can update the
terminal in a single operation. This approach scales much better once the layout
contains multiple panels, text blocks, or animated elements.

.. button-ref:: 06-layout-text-and-fonts
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Add Layout, Unicode Text, and Fonts →

