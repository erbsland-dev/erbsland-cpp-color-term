..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

****
Font
****

The font classes allow you to render large bitmap-based text directly
in the terminal. Fonts are primarily used for titles, banners, and
other decorative elements in terminal interfaces.

A ``Font`` defines how characters are rendered as bitmap glyphs,
which are then drawn onto the terminal buffer.

Usage
=====

Rendering a Bitmap-Font Title
-----------------------------

``Font`` integrates directly with ``Text``. Assign a font to a
``Text`` object and render it like any other text block.

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

The built-in ``defaultAscii()`` font renders large ASCII characters
using a bitmap representation. Because fonts integrate with ``Text``,
all existing features—such as alignment, color sequences, and text
animation—work automatically.

.. figure:: /images/text-gallery1.jpg
    :width: 100%

    The ``text-gallery`` demo uses the built-in ASCII font for its
    animated headline.

Interface
=========

.. doxygenclass:: erbsland::cterm::Font
    :members:

.. doxygentypedef:: erbsland::cterm::FontPtr

.. doxygenclass:: erbsland::cterm::FontGlyph
    :members:

