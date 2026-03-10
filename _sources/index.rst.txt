..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*********************************************
Erbsland Color Terminal Library Documentation
*********************************************

The **Erbsland Color Terminal** library is a small and focused C++20 static library designed for building rich terminal applications.

It provides reliable building blocks for colorful terminal output, Unicode-aware text handling, key-based input, and geometry helpers that simplify the development of terminal user interfaces, tools, and games.

.. figure:: /images/retro-plasma.jpg
    :width: 100%

.. button-ref:: chapters/get-started/index
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-2 sd-font-weight-bold sd-p-3

    Get Started with the Tutorial →

Topics
======

.. grid:: 3
    :margin: 4 4 0 0
    :gutter: 1

    .. grid-item-card:: :fas:`user;sd-text-success` Usage Guides
        :link: chapters/usage/index
        :link-type: doc

        Learn how to install the library, integrate it into your application, and run its unit tests.

    .. grid-item-card:: :fas:`user-graduate;sd-text-success` Step-by-Step Tutorial
        :link: chapters/get-started/index
        :link-type: doc

        Start with a compact introduction to the library and build your first terminal program step by step.

    .. grid-item-card:: :fas:`code;sd-text-success` API Reference
        :link: chapters/reference/index
        :link-type: doc

        Explore the public modules, classes, and functions that make up the color terminal API.

    .. grid-item-card:: :fas:`images;sd-text-success` Demo Gallery
        :link: chapters/demos/index
        :link-type: doc

        Browse the included demo applications with screenshots and notes about the showcased features.

    .. grid-item-card:: :fas:`layer-group;sd-text-success` Requirements
        :link: chapters/requirements
        :link-type: doc

        Review the compiler, CMake, and Python requirements used by the library and its tooling.

    .. grid-item-card:: :fas:`timeline;sd-text-success` Change Log
        :link: chapters/changelog
        :link-type: doc

        Track new features, improvements, and breaking changes between releases.

Features
========

The library provides a set of focused features that make terminal development easier and more reliable:

* ANSI foreground and background colors with typed color parts and color sequences
* Correct handling of zero-width and full-width Unicode characters
* Terminal output, screen handling, and platform-specific key input
* Unicode-aware terminal strings and per-cell character buffers
* Terminal size detection with resize callbacks
* Geometry helpers for positions, sizes, rectangles, anchors, alignments, margins, and bitmaps
* Standalone unit tests for the extracted terminal and geometry modules
* Bitmap font support
* Frame and rectangle drawing utilities
* Text wrapping and alignment helpers
* Cross-platform support for Linux, macOS, and Windows

ANSI Foreground/Background Colors
---------------------------------

.. figure:: /images/terminal-chronicle.jpg
    :width: 100%

The library provides easy-to-use low-level functionality for colorful, Unicode-aware terminal output.

Convenient print helpers allow you to combine text and colors while keeping your code readable and expressive.

A dedicated ``String`` class makes it possible to work with colored text safely. Combined Unicode characters are grouped correctly, and each visible character can have its own foreground and background color.

Handling of Zero-Width and Full-Width Characters
------------------------------------------------

.. figure:: /images/text-gallery2.jpg
    :width: 100%

Mixing East Asian full-width characters or combined Unicode characters often breaks terminal layouts.

The **Erbsland Color Terminal** library correctly calculates the display width of commonly used Unicode character sequences and automatically handles them during text output.

This allows you to mix different scripts and Unicode symbols without breaking the layout of your terminal interface.

Frame and Rectangle Drawing
---------------------------

.. figure:: /images/frame-weaver3.jpg
    :width: 100%

The library includes utilities for drawing frames, rectangles, and lines in various styles.

When lines intersect, the library automatically selects the best matching line characters to produce visually consistent borders.

Text Wrapping and Alignment
---------------------------

.. figure:: /images/text-gallery3.jpg
    :width: 100%

Text wrapping and alignment are integrated directly into the library.

You can easily format text blocks within a defined area, apply alignment rules, and keep layouts stable even when working with Unicode text.

Bitmap Fonts
------------

.. figure:: /images/text-gallery1.jpg
    :width: 100%

The library can render large bitmap fonts directly in the terminal.

You can use the built-in character set or provide your own custom bitmap font to create banners, headings, or decorative output.

Contents at a Glance
====================

.. toctree::
    :maxdepth: 3

    chapters/get-started/index
    chapters/usage/index
    chapters/reference/index
    chapters/demos/index
    chapters/contribute/index
    chapters/requirements
    chapters/changelog
    chapters/license

Indices and Tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
