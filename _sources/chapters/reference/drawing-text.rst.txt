..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

**************
Text Rendering
**************

The text rendering classes describe how terminal text is drawn into a
writable buffer. They turn :cpp:any:`String <erbsland::cterm::String>`
content into aligned labels, wrapped paragraphs, reusable text presets,
and animated headlines.

Bitmap fonts plug into the same pipeline through
:cpp:any:`TextOptions <erbsland::cterm::TextOptions>` and
:cpp:any:`Text <erbsland::cterm::Text>`. For font creation and font
presets, see :doc:`font`.

This page focuses on placing text inside rectangles, rendering wrapped
text blocks, and reusing
:cpp:any:`TextOptions <erbsland::cterm::TextOptions>` across multiple
:cpp:any:`Text <erbsland::cterm::Text>` instances. For the underlying
:cpp:any:`Char <erbsland::cterm::Char>` and
:cpp:any:`String <erbsland::cterm::String>` value types, see
:doc:`text`.

.. dropdown:: Details about the example output on this page

    The examples below were rendered with the dedicated documentation
    helper :file:`doc/tools/drawing-text-reference.cpp` at a fixed width
    of 72 terminal columns. This makes it easy to regenerate the visual
    output together with the code snippets.

Usage
=====

Drawing Direct Strings and Aligned Labels
-----------------------------------------

:cpp:any:`WritableBuffer <erbsland::cterm::WritableBuffer>` provides two
main entry points for text that does not need paragraph formatting:

* ``drawText(Position, String)`` writes text at an exact start position.
* ``drawText(String, Rectangle, Alignment, Color)`` aligns text inside a
  rectangle.

.. code-block:: cpp

    auto footer = String{};
    footer.append(
        bg::BrightBlack,
        fg::BrightYellow,
        "[Q]",
        fg::BrightWhite,
        " quit  ",
        fg::BrightCyan,
        "[R]",
        fg::BrightWhite,
        " refresh");

    buffer.drawText(Position{4, 4}, footer);

    buffer.drawFilledFrame(
        Rectangle{42, 2, 24, 5},
        FrameStyle::LightWithRoundedCorners,
        Char{" ", Color{fg::Inherited, bg::Blue}},
        Color{fg::BrightCyan, bg::Inherited});
    buffer.drawText("Overview", Rectangle{42, 2, 24, 5}, Alignment::Center, Color{fg::BrightWhite, bg::Inherited});

Use the position-based overload for status lines, overlays, and other
exact placements. Use the rectangle overload for titles, centered
labels, and other layout-driven text.

.. erbsland-ansi::
    :escape-char: ␛
    :theme: ela-term

    ␛[97;40m    drawText(Position, String)         drawText(String, Rectangle, ...) ␛[39;49m
    ␛[97;40m                                                                        ␛[39;49m
    ␛[97;40m  ␛[90m012345678901234567890123456789␛[97m          ␛[96m╭──────────────────────╮␛[97m      ␛[39;49m
    ␛[97;40m                                          ␛[96m│␛[97;44m                      ␛[96;40m│␛[97m      ␛[39;49m
    ␛[97;40m    ␛[93;100m[Q]␛[97m quit  ␛[96m[R]␛[97m refresh␛[40m                 ␛[96m│␛[97;44m       Overview       ␛[96;40m│␛[97m      ␛[39;49m
    ␛[97;40m                                          ␛[96m│␛[97;44m                      ␛[96;40m│␛[97m      ␛[39;49m
    ␛[97;40m                                          ␛[96m╰──────────────────────╯␛[97m      ␛[39;49m
    ␛[97;40m                                                                        ␛[39;49m
    ␛[97;40m   ␛[90mexact position, mixed colors␛[97m        ␛[90msame API, aligned in a rectangle␛[97m ␛[39;49m
    ␛[97;40m                                                                        ␛[39;49m

Text Alignment Inside a Rectangle
---------------------------------

:cpp:any:`Text <erbsland::cterm::Text>` uses the same
:cpp:any:`Alignment <erbsland::cterm::Alignment>` model as the geometry
and bitmap helpers. This means you can place a text block inside its
target rectangle without manual offset calculations.

.. code-block:: cpp

    auto title = Text{String{"Short note"}, Rectangle{26, 2, 20, 4}, Alignment::Center};
    title.setColor(Color{fg::BrightYellow, bg::Inherited});
    buffer.drawText(title);

The same content can be anchored to the top-left, center, or
bottom-right simply by changing the alignment value.

.. erbsland-ansi::
    :escape-char: ␛
    :theme: ela-term

    ␛[97;40m   Text uses the same alignment model as other geometry-aware drawing   ␛[39;49m
    ␛[97;40m                                                                        ␛[39;49m
    ␛[97;40m  ┌──────────────────┐    ┌──────────────────┐    ┌──────────────────┐  ␛[39;49m
    ␛[97;40m  │␛[93;100mShort note␛[97m        ␛[40m│    │␛[100m    ␛[93mShort note␛[97m    ␛[40m│    │␛[100m                  ␛[40m│  ␛[39;49m
    ␛[97;40m  │␛[100m                  ␛[40m│    │␛[100m                  ␛[40m│    │␛[100m        ␛[93mShort note␛[97;40m│  ␛[39;49m
    ␛[97;40m  └──────────────────┘    └──────────────────┘    └──────────────────┘  ␛[39;49m
    ␛[97;40m        ␛[90mTopLeft␛[97m                  ␛[90mCenter␛[97m               ␛[90mBottomRight␛[97m       ␛[39;49m
    ␛[97;40m                                                                        ␛[39;49m
    ␛[97;40m                                                                        ␛[39;49m

Rendering Wrapped Paragraphs
----------------------------

:cpp:any:`Text <erbsland::cterm::Text>` becomes especially useful when
text needs wrapping, paragraph spacing, or paragraph-aware indentation.

.. code-block:: cpp

    auto help = Text{
        String{"Navigation: Use arrow keys to move.\n\nActions: Press Enter to open."},
        Rectangle{10, 3, 52, 7},
        Alignment::TopLeft};
    help.setParagraphSpacing(ParagraphSpacing::DoubleLine);
    help.setWrappedLineIndent(2);
    help.setColor(Color{fg::BrightWhite, bg::Inherited});

    buffer.drawText(help);

This keeps the content, target rectangle, and paragraph behavior in one
object. For the full paragraph-formatting reference, continue with
:doc:`paragraph-options`.

.. erbsland-ansi::
    :escape-char: ␛
    :theme: ela-term

    ␛[97;40m                                                                        ␛[39;49m
    ␛[97;40m      ╔════════════════════════␛[44mHelp Panel␛[40m════════════════════════╗      ␛[39;49m
    ␛[97;40m      ║␛[44m                                                          ␛[40m║      ␛[39;49m
    ␛[97;40m      ║␛[44m   Navigation: Use arrow keys to move.                    ␛[40m║      ␛[39;49m
    ␛[97;40m      ║␛[44m                                                          ␛[40m║      ␛[39;49m
    ␛[97;40m      ║␛[44m                                                          ␛[40m║      ␛[39;49m
    ␛[97;40m      ║␛[44m                                                          ␛[40m║      ␛[39;49m
    ␛[97;40m      ║␛[44m   Actions: Press Enter to open.                          ␛[40m║      ␛[39;49m
    ␛[97;40m      ║␛[44m                                                          ␛[40m║      ␛[39;49m
    ␛[97;40m      ║␛[44m                                                          ␛[40m║      ␛[39;49m
    ␛[97;40m      ║␛[44m                                                          ␛[40m║      ␛[39;49m
    ␛[97;40m      ╚══════════════════════════════════════════════════════════╝      ␛[39;49m
    ␛[97;40m     ␛[90mText automatically wraps and keeps explicit paragraph breaks.␛[97m      ␛[39;49m

Reusing TextOptions and Animation
---------------------------------

:cpp:any:`TextOptions <erbsland::cterm::TextOptions>` bundles color
sequences, fonts, animation, and paragraph layout settings into one
reusable preset.

.. code-block:: cpp

    auto options = TextOptions{Alignment::Center};
    options.setColorSequence(
        ColorSequence{
            Color{fg::BrightYellow, bg::Inherited},
            Color{fg::BrightRed, bg::Inherited},
            Color{fg::BrightMagenta, bg::Inherited},
            Color{fg::BrightCyan, bg::Inherited},
        });
    options.setAnimation(TextAnimation::ColorDiagonal);

    auto left = Text{String{"ALERT PANEL"}, Rectangle{4, 3, 28, 2}, Alignment::Center};
    left.setTextOptions(options);
    auto right = Text{String{"ALERT PANEL"}, Rectangle{40, 3, 28, 2}, Alignment::Center};
    right.setTextOptions(options);

    buffer.drawText(left, 1);
    buffer.drawText(right, 3);

This works well when several labels or headings should share one visual
style. ``TextAnimation::ColorDiagonal`` shifts the configured color
sequence diagonally across the rendered text based on the supplied
animation cycle.

.. erbsland-ansi::
    :escape-char: ␛
    :theme: ela-term

    ␛[97;40m                      Reuse one TextOptions preset                      ␛[39;49m
    ␛[97;40m  ╭──────────────────────────────╮    ╭──────────────────────────────╮  ␛[39;49m
    ␛[97;40m  │␛[100m                              ␛[40m│    │␛[100m                              ␛[40m│  ␛[39;49m
    ␛[97;40m  │␛[100m         ␛[93mA␛[91mL␛[95mE␛[96mR␛[93mT␛[91m ␛[95mP␛[96mA␛[93mN␛[91mE␛[95mL␛[97m          ␛[40m│    │␛[100m         ␛[95mA␛[96mL␛[93mE␛[91mR␛[95mT␛[96m ␛[93mP␛[91mA␛[95mN␛[96mE␛[93mL␛[97m          ␛[40m│  ␛[39;49m
    ␛[97;40m  │␛[100m                              ␛[40m│    │␛[100m                              ␛[40m│  ␛[39;49m
    ␛[97;40m  │␛[100m                              ␛[40m│    │␛[100m                              ␛[40m│  ␛[39;49m
    ␛[97;40m  │␛[100m                              ␛[40m│    │␛[100m                              ␛[40m│  ␛[39;49m
    ␛[97;40m  ╰──────────────────────────────╯    ╰──────────────────────────────╯  ␛[39;49m
    ␛[97;40m             ␛[90mcycle = 1␛[97m                           ␛[90mcycle = 3␛[97m              ␛[39;49m
    ␛[97;40m                                                                        ␛[39;49m

Interface
=========

.. doxygenclass:: erbsland::cterm::TextOptions
    :members:

.. doxygenclass:: erbsland::cterm::Text
    :members:

.. doxygenenum:: erbsland::cterm::TextAnimation
