..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*****************
Strings and Chars
*****************

The string classes describe terminal text before it is rendered. They
let you represent a single display character with color and character
attributes, build styled text fragments, measure Unicode-aware width,
and split or wrap text without interacting with a buffer.

This page focuses on working with :cpp:any:`Char <erbsland::cterm::Char>`,
:cpp:any:`CharStyle <erbsland::cterm::CharStyle>`, and
:cpp:any:`String <erbsland::cterm::String>` as data types. For rendering
text blocks into rectangles with :cpp:any:`Text <erbsland::cterm::Text>`,
see :doc:`drawing-text`.

Usage
=====

Working with Char
-----------------

:cpp:any:`Char <erbsland::cterm::Char>` represents a single terminal
display cell together with its foreground color, background color, and
optional character attributes such as bold or underline.

Constructing Characters
^^^^^^^^^^^^^^^^^^^^^^^

Use a Unicode code point when you construct a regular character. This is
the most explicit form when you already know the exact character.

.. code-block:: cpp

    const auto checkMark = Char{U'✓'};
    const auto whiteOnBlue = Char{U'a', fg::White, bg::Blue};

For combined characters, construct :cpp:any:`Char <erbsland::cterm::Char>`
from UTF-8 or UTF-32 text:

.. code-block:: cpp

    const auto combinedFromUtf8 = Char{"e\xCC\x81"};
    const auto combinedFromUtf32 = Char{U"e\u0301"};

To append a combining mark programmatically, use ``withCombining()``:

.. code-block:: cpp

    const auto acute = Char{U"e"}.withCombining(char32_t{0x0301});

``displayWidth()`` follows terminal cell width, so wide and combining
characters behave correctly during layout:

.. code-block:: cpp

    const auto wide = Char{U'界'};
    const auto combined = Char{"e\xCC\x81"};

    const auto wideCells = wide.displayWidth();          // 2
    const auto combinedCells = combined.displayWidth();  // 1

Testing and Comparing Characters
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To compare only the character value and ignore color, compare against a
Unicode code point:

.. code-block:: cpp

    if (character == U'?') {
        // ...
    }

Use ``isOneOf()`` for small sets of accepted characters:

.. code-block:: cpp

    if (character.isOneOf(U'Y', U'y', U'J', U'j')) {
        // ...
    }

For screen-level comparisons, ``renderedEquals()`` is often more useful
than ``operator==`` because it treats inherited colors like terminal
defaults:

.. code-block:: cpp

    const auto inherited = Char{U'X', fg::Inherited, bg::Inherited};
    const auto defaults = Char{U'X', fg::Default, bg::Default};

    const auto sameOnScreen = inherited.renderedEquals(defaults);

Recoloring Characters
^^^^^^^^^^^^^^^^^^^^^

To adjust the colors of an existing :cpp:any:`Char <erbsland::cterm::Char>`,
choose the method that matches your intent:

.. code-block:: cpp

    const auto base = Char{U'X', fg::Green, bg::Blue};

    const auto overlay = base.withColorOverlay(Color{fg::BrightWhite, bg::Inherited});
    const auto replaced = base.withColorReplaced(Color{fg::White, bg::Black});
    const auto basedOnTheme = base.withBaseColor(Color{fg::Inherited, bg::BrightBlack});

With ``withColorOverlay()``, any ``Inherited`` component keeps the
existing color, while ``Default`` resets that component to the terminal
default.

Working with Character Attributes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Use :cpp:any:`CharAttributes <erbsland::cterm::CharAttributes>` when you
want a character or string fragment to explicitly enable, disable, or
inherit ANSI text attributes.

.. code-block:: cpp

    auto emphasis = CharAttributes{};
    emphasis.setBold(true);
    emphasis.setUnderline(true);

    const auto heading = Char{U'H', fg::BrightWhite, emphasis};
    const auto plain = heading.withAttributes(CharAttributes::reset());

Unspecified attributes inherit from the surrounding writer or string
context, while specified bits overwrite that base state.

Working with Character Styles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Use :cpp:any:`CharStyle <erbsland::cterm::CharStyle>` when you want to
bundle color and character attributes into one reusable value.

.. code-block:: cpp

    auto emphasis = CharAttributes{};
    emphasis.setBold(true);

    const auto headingStyle = CharStyle{Color{fg::BrightWhite, bg::Blue}, emphasis};
    auto heading = Char{U'H', headingStyle};

    heading.setStyle(heading.style().withOverlay(CharStyle{Color{fg::Inherited, bg::Black}}));

``withOverlay()`` keeps inherited color components and unspecified
attributes from the existing style, while ``withBase()`` resolves a
style against an underlying base theme.

Building Strings
----------------

:cpp:any:`String <erbsland::cterm::String>` stores a sequence of
:cpp:any:`Char <erbsland::cterm::Char>` values. It is the right type for
status bars, prompts, labels, and any text where characters may use
different colors.

Building Colored Text Fragments
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Prefer ``append(...)`` when building mixed-style strings. It keeps
colors, attributes, and text in a single readable sequence.

.. code-block:: cpp

    auto footer = String{};
    footer.append(
        bg::BrightBlack,
        fg::BrightYellow,
        "[Q]",
        fg::BrightWhite,
        " quit");

    auto highlighted = CharAttributes{};
    highlighted.setBold(true);
    highlighted.setUnderline(true);

    footer.append(
        "  ",
        highlighted,
        "Save",
        CharAttributes::reset(),
        fg::BrightBlack,
        " shortcut");

``append(...)`` accepts colors, :cpp:any:`CharStyle <erbsland::cterm::CharStyle>`,
:cpp:any:`Char <erbsland::cterm::Char>` values,
:cpp:any:`CharAttributes <erbsland::cterm::CharAttributes>`, plain text,
and other :cpp:any:`String <erbsland::cterm::String>` instances. Colors
and attributes remain active for subsequent elements within the same
call.

You can also construct a :cpp:any:`String <erbsland::cterm::String>`
directly from UTF-8 or UTF-32 text:

.. code-block:: cpp

    const auto utf8Text = String{"Gruezi"};
    const auto utf32Text = String{U"Gru\u0308ezi"};

Control codes are filtered out automatically, except for tab and
newline, which are preserved for layout and splitting.

Handling Invalid UTF-8 Input
----------------------------

:cpp:any:`EncodingErrors <erbsland::cterm::EncodingErrors>` controls how
UTF-8-based constructors react when the input is malformed.

.. code-block:: cpp

    const auto strict = String{"Gr\xC3", EncodingErrors::Throw};     // throws
    const auto lossy = String{"Gr\xC3", EncodingErrors::Replace};    // inserts U+FFFD
    const auto compact = String{"Gr\xC3", EncodingErrors::Ignore};   // drops the broken bytes when supported

Use :cpp:any:`EncodingErrors::Throw <erbsland::cterm::EncodingErrors::Throw>`
at the boundary where invalid data should fail fast. Use
:cpp:any:`EncodingErrors::Replace <erbsland::cterm::EncodingErrors::Replace>`
when the application should stay readable even with damaged input.

Searching, Slicing, and Measuring
---------------------------------

Use ``size()`` to count stored characters and ``displayWidth()`` to
measure terminal cell width. These differ for full-width and combining
characters.

.. code-block:: cpp

    const auto text = String{"A界e\xCC\x81"};

    const auto characterCount = text.size();         // 3 terminal characters
    const auto terminalWidth = text.displayWidth();  // 4 cells

For content-aware processing, the following helpers are commonly useful:

.. code-block:: cpp

    const auto warningCount = String{"!?!!"}.count(U'!');
    const auto nextQuestion = String{"abc?def"}.indexOf(U'?');
    const auto middle = String{"AB界D"}.substr(1, 2);

``count(Char)`` and ``indexOf(Char)`` compare both character and color.
The ``char32_t`` overloads ignore color and match only the character.

Splitting, Wrapping, and Lines
------------------------------

``splitWords()`` separates text at spaces, tabs, carriage returns, and
newlines. ``splitLines()`` keeps empty lines and splits only at newline
characters.

.. code-block:: cpp

    const auto words = String{"alpha  beta\ngamma"}.splitWords();
    const auto lines = String{"first\n\nthird"}.splitLines();

When you want string-level wrapping without creating a
:cpp:any:`Text <erbsland::cterm::Text>` object, use ``wrapIntoLines()``:

.. code-block:: cpp

    const auto wrapped = String{"alpha beta\ngamma"}.wrapIntoLines(6);
    const auto wrappedWithSpacing = String{"alpha beta\ngamma"}.wrapIntoLines(
        6,
        ParagraphSpacing::DoubleLine);

    const auto lineCount = String{"AA\nBB"}.terminalLines(2);

This is useful for preprocessing text, estimating layout, or building
buffers line by line.

:cpp:any:`StringLines <erbsland::cterm::StringLines>` is the line-based
companion type returned by ``splitLines()`` and ``wrapIntoLines()``.
You can also rebuild a string from line data:

.. code-block:: cpp

    const auto text = String::fromLines({"one", "two"}, Color{fg::BrightWhite, bg::Blue});

If you want to render these lines directly into a buffer, see
:cpp:any:`Buffer::fromLines() <erbsland::cterm::Buffer::fromLines()>`
and :doc:`drawing-text`.

Interface
=========

.. doxygenclass:: erbsland::cterm::Char
    :members:

.. doxygenclass:: erbsland::cterm::CharAttributes
    :members:

.. doxygenclass:: erbsland::cterm::CharStyle
    :members:

.. doxygenenum:: erbsland::cterm::EncodingErrors

.. doxygenclass:: erbsland::cterm::String
    :members:

.. doxygentypedef:: erbsland::cterm::StringLines
