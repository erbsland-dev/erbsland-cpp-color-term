..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

****
Text
****

The text classes provide utilities for constructing, formatting, and
rendering colored text in terminal buffers. They support mixed styles,
Unicode-aware layout, and animated text effects.

At the lowest level, :cpp:any:`Char <erbsland::cterm::Char>` represents a single colored terminal cell.
Higher-level classes such as :cpp:any:`String <erbsland::cterm::String>` and :cpp:any:`Text <erbsland::cterm::Text>` build on this
foundation to describe larger text fragments and layout-aware text
blocks.

Usage
=====

Working with Char
-----------------

:cpp:any:`Char <erbsland::cterm::Char>` represents one terminal character cell together with its
foreground and background color. Start with :cpp:any:`Char <erbsland::cterm::Char>` when you need
precise control over individual Unicode characters.

Constructing Characters
^^^^^^^^^^^^^^^^^^^^^^^

Use a Unicode code point when you create a regular character. This is the
recommended form because it is explicit and avoids text decoding.

.. code-block:: cpp

    const auto checkMark = Char{U'✓'};

For combined characters, construct :cpp:any:`Char <erbsland::cterm::Char>` from UTF-8 or UTF-32 text.
UTF-32 is faster if you already have Unicode code points available.

.. code-block:: cpp

    const auto combinedFromUtf8 = Char{"e\xCC\x81"};
    const auto combinedFromUtf32 = Char{U"e\u0301"};

You can add colors directly in several forms:

.. code-block:: cpp

    const auto whiteOnBlue = Char{U'a', fg::White, bg::Blue};
    const auto whiteOnly = Char{U'a', fg::White};
    const auto blueOnly = Char{U'a', bg::Blue};
    const auto colorObject = Char{U'b', Color{fg::White, bg::Blue}};

Testing and Recoloring Characters
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For single-code-point characters, compare against a Unicode code point to
test only the character and ignore the color:

.. code-block:: cpp

    if (character == U'?') {
        // ...
    }

Use ``isOneOf()`` when several alternatives are valid:

.. code-block:: cpp

    if (character.isOneOf(U'Y', U'y', U'J', U'j')) {
        // ...
    }

To adjust colors on an existing :cpp:any:`Char <erbsland::cterm::Char>`, choose the method that matches
your intent:

.. code-block:: cpp

    const auto base = Char{U'X', fg::Green, bg::Blue};

    const auto overlay = base.withColorOverlay(Color{fg::BrightWhite, bg::Inherited});
    const auto replaced = base.withColorReplaced(Color{fg::White, bg::Black});
    const auto basedOnTheme = base.withBaseColor(Color{fg::Inherited, bg::BrightBlack});

With ``withColorOverlay()``, any ``Inherited`` component keeps the
existing color, while ``Default`` resets that component to the terminal
default color.

Building Strings
----------------

:cpp:any:`String <erbsland::cterm::String>` stores a sequence of :cpp:any:`Char <erbsland::cterm::Char>` values. It is the right type for
status bars, prompts, labels, and any other text where different
characters may have different colors.

Building Colored Text Fragments
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Prefer ``append(...)`` to build mixed-style strings. It is easier to read
than manually iterating over temporary strings, and it appends all parts
in one place.

.. code-block:: cpp

    auto footer = String{};
    footer.append(
        bg::BrightBlack,
        fg::BrightYellow,
        "[Q]",
        fg::BrightWhite,
        " quit");

``append(...)`` accepts colors, :cpp:any:`Char <erbsland::cterm::Char>` values, strings, and other
:cpp:any:`String <erbsland::cterm::String>` objects. Colors stay active for the following text within the
same call.

You can also construct :cpp:any:`String <erbsland::cterm::String>` directly from UTF-8 or UTF-32 text:

.. code-block:: cpp

    const auto utf8Text = String{"Gruezi"};
    const auto utf32Text = String{U"Gru\u0308ezi"};

When :cpp:any:`String <erbsland::cterm::String>` is built from text, control codes are filtered out
automatically except for tab and newline.

Measuring and Counting Text
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Use ``size()`` to count stored characters and ``displayWidth()`` to count
terminal cells. The two differ for full-width and combining characters.

.. code-block:: cpp

    const auto text = String{"A界e\xCC\x81"};

    const auto characterCount = text.size();         // 3 terminal characters
    const auto terminalWidth = text.displayWidth();  // 4 cells

Use ``count()`` when you need the number of occurrences of a specific
character:

.. code-block:: cpp

    const auto warningCount = String{"!?!!"}.count(U'!');

Splitting Text
^^^^^^^^^^^^^^

``splitWords()`` separates text at spaces, tabs, carriage returns, and
newlines. ``splitLines()`` keeps empty lines and splits only at newline
characters.

.. code-block:: cpp

    const auto words = String{"alpha  beta\ngamma"}.splitWords();
    const auto lines = String{"first\n\nthird"}.splitLines();

Describing a Text Block
-----------------------

:cpp:any:`TextOptions <erbsland::cterm::TextOptions>` stores the reusable rendering configuration for a text element:
colors, fonts, animation, and paragraph layout rules. :cpp:any:`Text <erbsland::cterm::Text>` combines the content
with that configuration plus the target rectangle.

.. code-block:: cpp

    auto titleText = String{"COLOR TERM"};
    auto title = Text{titleText, Rectangle{0, 0, 60, 6}, Alignment::Center};
    title.setFont(Font::defaultAscii());
    title.setColorSequence(ColorSequence{
        Color{fg::BrightBlue, bg::Black},
        Color{fg::BrightCyan, bg::Black},
        Color{fg::BrightMagenta, bg::Black},
        Color{fg::BrightYellow, bg::Black},
    });
    title.setAnimation(TextAnimation::ColorDiagonal);

    buffer.drawText(title, animationCycle);

Because :cpp:any:`Text <erbsland::cterm::Text>` integrates with the geometry and color systems, it
supports wrapped paragraphs, Unicode-aware layout, animated titles,
and large bitmap fonts.

Formatting Paragraphs
---------------------

Explicit newlines in :cpp:any:`Text <erbsland::cterm::Text>` create separate paragraphs. Use
:cpp:any:`ParagraphOptions <erbsland::cterm::ParagraphOptions>` and
:cpp:any:`ParagraphSpacing <erbsland::cterm::ParagraphSpacing>` to control spacing, indentation, wrap marks, word
breaking, tab stops, background fill, and narrow-layout fallbacks.

.. code-block:: cpp

    auto help = Text{
        String{"Navigation\nUse arrow keys to move.\n\nActions\nPress Enter to select."},
        Rectangle{2, 2, 36, 12},
        Alignment::TopLeft};
    help.setParagraphSpacing(ParagraphSpacing::DoubleLine);

    buffer.drawText(help);

:cpp:any:`ParagraphSpacing::SingleLine <erbsland::cterm::ParagraphSpacing::SingleLine>` keeps the rendered paragraphs tight,
while ``DoubleLine`` makes sectioned help text and descriptive panels
easier to scan.

The full paragraph-formatting reference, including all option groups and related enums, is available on
:doc:`Paragraph Options <paragraph-options>`.

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

.. doxygentypedef:: erbsland::cterm::StringLines

.. doxygenclass:: erbsland::cterm::TextOptions
    :members:

.. doxygenclass:: erbsland::cterm::Text
    :members:

.. doxygenenum:: erbsland::cterm::TextAnimation
