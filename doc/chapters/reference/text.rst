..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

****
Text
****

The text classes provide utilities for constructing, formatting, and
rendering colored text in terminal buffers. They support mixed styles,
Unicode-aware layout, and animated text effects.

At the lowest level, ``Char`` represents a single colored terminal cell.
Higher-level classes such as ``String`` and ``Text`` build on this
foundation to describe larger text fragments and layout-aware text
blocks.

Usage
=====

Building Colored Text Fragments
-------------------------------

``String`` stores a sequence of colored terminal characters. This makes
it useful for prompts, status bars, and other mixed-style text where
individual characters may use different colors.

.. code-block:: cpp

    auto footer = String{};
    for (const auto &character : String{"[Q]"}) {
        footer.append(character.withColorOverlay(Color{fg::BrightYellow, bg::BrightBlack}));
    }
    for (const auto &character : String{" quit"}) {
        footer.append(character.withColorOverlay(Color{fg::BrightWhite, bg::BrightBlack}));
    }

Each ``Char`` in the sequence can carry its own color, allowing flexible
construction of styled text fragments.

When you recolor existing ``Char`` values with ``withColorOverlay()``, inherited
components keep the original character color while default components reset
that part to the terminal default.

When ``String`` is built from UTF-8 or UTF-32 text, control codes are filtered
out automatically except for tab and newline.

Describing a Text Block
-----------------------

``Text`` describes a complete text element for buffer rendering. It
combines the content with layout information such as the target
rectangle, alignment, font, and animation settings.

.. code-block:: cpp

    auto title = Text{String{"COLOR TERM"}, Rectangle{0, 0, 60, 6}, Alignment::Center};
    title.setFont(Font::defaultAscii());
    title.setColorSequence(ColorSequence{
        Color{fg::BrightBlue, bg::Black},
        Color{fg::BrightCyan, bg::Black},
        Color{fg::BrightMagenta, bg::Black},
        Color{fg::BrightYellow, bg::Black},
    });
    title.setAnimation(TextAnimation::ColorDiagonal);

    buffer.drawText(title, animationCycle);

Because ``Text`` integrates with the geometry and color systems, it
supports wrapped paragraphs, Unicode-aware layout, animated titles,
and large bitmap fonts.

.. figure:: /images/text-gallery2.jpg
    :width: 100%

    The ``text-gallery`` demo shows wrapped text, mixed-width Unicode
    text, and animated titles built with these classes.

Interface
=========

.. doxygenclass:: erbsland::cterm::Char
    :members:

.. doxygenclass:: erbsland::cterm::String
    :members:

.. doxygentypedef:: erbsland::cterm::BlockStringLines

.. doxygenclass:: erbsland::cterm::Text
    :members:

.. doxygenenum:: erbsland::cterm::TextAnimation
