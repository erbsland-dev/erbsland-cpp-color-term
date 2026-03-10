..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*****
Color
*****

The color classes provide access to the standard 16 terminal colors and
their combinations. They are used to style terminal output, buffer cells,
and text elements throughout the library.

Usage
=====

Applying Colors Directly
------------------------

Use ``Foreground``, ``Background``, and ``Color`` when you want to apply
explicit styling to terminal output or buffer cells.

.. code-block:: cpp

    auto accent = Color{fg::BrightCyan, bg::Black};
    auto warning = accent.overlayWith(Color{fg::BrightYellow, bg::Red});

    terminal.printLine(accent, "Normal status");
    terminal.printLine(warning, "Escalated status");

In this example, ``overlayWith()`` combines two colors, allowing you to
derive variants from an existing style while preserving the other color
components.

Use ``Default`` when you want to reset a foreground or background to the
terminal default color. Use ``Inherited`` when a later layer should keep
the color from the layer below, for example while composing buffer cells.

Building Animated Palettes
--------------------------

``ColorSequence`` represents an ordered list of colors. It is useful for
gradients, rotating palettes, or animated visual effects.

.. code-block:: cpp

    auto titleColors = ColorSequence{
        Color{fg::BrightBlue, bg::Black},
        Color{fg::BrightCyan, bg::Black},
        Color{fg::BrightMagenta, bg::Black},
        Color{fg::BrightYellow, bg::Black},
    };

    auto currentColor = titleColors.colorNormalized(0.35);
    terminal.printLine(currentColor, "Animated headline");

The ``colorNormalized()`` function selects a color from the sequence
based on a normalized position between ``0.0`` and ``1.0``. This makes
it easy to drive color changes with animation cycles or progress values.

.. figure:: /images/retro-plasma.jpg
    :width: 100%

    The ``retro-plasma`` demo uses ``ColorSequence`` values to switch
    between multiple animated palettes.

Interface
=========

.. doxygenclass:: erbsland::cterm::Color
    :members:

.. doxygentypedef:: erbsland::cterm::ColorList

.. doxygenclass:: erbsland::cterm::ColorBase
    :members:

.. doxygenclass:: erbsland::cterm::ColorPart
    :members:

.. doxygenenum:: erbsland::cterm::ColorRole

.. doxygentypedef:: erbsland::cterm::Background

.. doxygentypedef:: erbsland::cterm::bg

.. doxygentypedef:: erbsland::cterm::Foreground

.. doxygentypedef:: erbsland::cterm::fg

.. doxygenclass:: erbsland::cterm::ColorSequence
    :members:
