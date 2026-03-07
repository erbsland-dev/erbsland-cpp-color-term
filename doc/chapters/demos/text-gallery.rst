..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

************
Text Gallery
************

``text-gallery`` demonstrates the text rendering capabilities of the library. The demo combines framed panels,
wrapped paragraphs, mixed-width Unicode text, animated bitmap-font titles, and small colored footer prompts
in one interactive terminal application.

It provides a compact overview of how structured text layouts, decorative titles, and styled UI elements can
be composed inside a terminal interface.

.. figure:: /images/text-gallery3.jpg
    :width: 100%

.. figure:: /images/text-gallery2.jpg
    :width: 100%

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/text-gallery

The application renders several panels and decorative elements while demonstrating how different text layout
features interact on the screen.

Features Shown
==============

This demo highlights several aspects of advanced text rendering:

* Text layout using ``Text`` together with ``Alignment`` options.
* Unicode-aware wrapping and centering of mixed-width characters.
* Bitmap-font rendering using ``Font::defaultAscii()``.
* Animated title coloring with ``ColorSequence`` and ``TextAnimation``.
* Colored footer prompts assembled from ``String`` and ``Char`` elements.

Relevant Source Files
=====================

If you want to explore the implementation, start with:

:file:`demo/text-gallery/src/TextGalleryApp.cpp`

This file contains the complete layout logic for the panels, titles, and
footer prompts shown in the demo.
