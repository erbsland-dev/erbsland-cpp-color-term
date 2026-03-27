..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*********************************************
Erbsland Color Terminal Library Documentation
*********************************************

The **Erbsland Color Terminal** library is a small and focused C++20 static library designed for building rich terminal applications.

It provides reliable building blocks for colorful terminal output, Unicode-aware text handling, key-based input,
buffer-based rendering, wrapped paragraph layout, rich-text and HTML rendering, and a beta UI framework for
structured terminal applications. These layers let you build command-line tools, dashboards, document-like terminal
output, and full-screen terminal interfaces without reinventing low-level terminal plumbing.

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

        Explore the public modules for terminal I/O, buffers, paragraph layout, rich text, and the beta UI framework.

    .. grid-item-card:: :fas:`microchip;sd-text-success` Implementation Notes
        :link: chapters/implementation/index
        :link-type: doc

        Read contributor-focused notes about backends, scheduling, paragraph layout, remapped buffers, stack layout,
        and rich-text rendering internals.

    .. grid-item-card:: :fas:`images;sd-text-success` Demo Gallery
        :link: chapters/demos/index
        :link-type: doc

        Browse the included demo applications with screenshots and notes about the showcased features.

    .. grid-item-card:: :fas:`layer-group;sd-text-success` Requirements
        :link: chapters/requirements
        :link-type: doc

        Review the compiler and CMake requirements for using the library, plus the extra tooling requirements for
        development work.

    .. grid-item-card:: :fas:`timeline;sd-text-success` Change Log
        :link: chapters/changelog
        :link-type: doc

        Track new features, improvements, and breaking changes between releases.

Features
========

Terminal Foundations
--------------------

The foundational layer provides portable building blocks for terminal interaction and text handling.

*   Cross-platform terminal backend for **Linux, macOS, and Windows** — write once and compile everywhere
*   Direct control over **colors, cursor positioning, and screen management**
*   Automatic terminal detection and initialization
*   Convenient colored text output using **ANSI escape sequences**
*   Optional **text-only mode** for non-interactive applications
*   **Unicode-aware terminal strings** with accurate display-width calculation
*   Automatic handling of **wide and combined Unicode code points**
*   Configurable handling of **invalid UTF-8**, including strict rejection and deterministic replacement
*   **Interactive key input** and traditional **line-based input**
*   Geometry utilities for positions, sizes, rectangles, anchors, alignment, margins, and bitmaps
*   Automatic **terminal state restoration on exit**, including unexpected exits caused by signals

Rendering, Buffers, and Layout
------------------------------

The rendering layer builds on the terminal primitives to simplify complex screen output and reusable text layout.

*   Efficient **cell buffer classes** for preparing terminal output
*   Automatic **back-buffering and delta screen updates**
*   Automatic switching to the **alternate screen** and restoration on exit
*   :cpp:any:`CursorWriter <erbsland::cterm::CursorWriter>` and
    :cpp:any:`CursorBuffer <erbsland::cterm::CursorBuffer>` for shared streaming output to terminals and
    scrollback-style buffers
*   :cpp:any:`RemappedBuffer <erbsland::cterm::RemappedBuffer>` for efficient row- and column-oriented editing
    workloads
*   **View system** to render sections of buffers on screen
*   Reusable paragraph layout with wrapping, indentation, tab stops, spacing, wrap markers, and ellipsis handling
*   Text rendering helpers for alignment, animation, and reusable **TextOptions**
*   Frame and rectangle drawing utilities
*   Character-combination framework for building block-based terminal graphics
*   **Tile-9 fill** and **tile-16 frame** drawing helpers
*   **Custom font support**
*   **Bitmap font rendering** for large terminal text
*   Color sequences and **animated color effects**
*   Bitmap utilities for generating masks and rendering bitmaps to the screen

Rich Text and HTML
------------------

The text layer can render structured terminal documents instead of only plain strings.

*   :cpp:any:`HtmlRenderer <erbsland::cterm::text::HtmlRenderer>` converts a focused HTML subset into
    terminal-friendly output
*   :cpp:any:`Style <erbsland::cterm::text::Style>`,
    :cpp:any:`StyleRule <erbsland::cterm::text::StyleRule>`,
    :cpp:any:`StyleSelector <erbsland::cterm::text::StyleSelector>`, and
    :cpp:any:`TextNode <erbsland::cterm::text::TextNode>` support document-style rendering and customization
*   Headings, lists, links, blockquotes, code blocks, and inline formatting can be rendered either into
    :cpp:any:`String <erbsland::cterm::String>` values or directly to any
    :cpp:any:`CursorWriter <erbsland::cterm::CursorWriter>`

Beta UI Framework
-----------------

The optional UI layer helps you build structured event-driven terminal applications.

*   :cpp:any:`ui::Application <erbsland::cterm::ui::Application>`,
    :cpp:any:`ui::Page <erbsland::cterm::ui::Page>`,
    :cpp:any:`ui::Surface <erbsland::cterm::ui::Surface>`, and
    :cpp:any:`ui::Layout <erbsland::cterm::ui::Layout>` provide the core abstractions for full-screen terminal apps
*   Built-in :cpp:any:`ui::Stack <erbsland::cterm::ui::layout::Stack>`,
    :cpp:any:`ui::Panel <erbsland::cterm::ui::surface::Panel>`, and
    :cpp:any:`ui::TextBox <erbsland::cterm::ui::surface::TextBox>` cover common surface and layout needs
*   :cpp:any:`ui::KeyBindings <erbsland::cterm::ui::KeyBindings>`, schedulers, and event threads support focus-aware
    input handling, timed actions, and background work

.. important::

    The :cpp:any:`erbsland::cterm::ui` framework is currently in beta and parts of the API may change in future
    releases.

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
    chapters/implementation/index
    chapters/contribute/index
    chapters/requirements
    chapters/changelog
    chapters/license

Indices and Tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
