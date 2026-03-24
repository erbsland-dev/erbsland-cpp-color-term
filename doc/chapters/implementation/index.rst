..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    !single: implementation notes

********************
Implementation Notes
********************

.. toctree::
    :maxdepth: 3
    :hidden:

    windows-backend
    posix-backend
    paragraph-layout-and-painting
    unicode-width-system

This chapter is written for developers working on the library itself.
It provides technical background information about internal implementation details,
code structure, and platform-specific behavior that are intentionally not part of the public API documentation.

Use these notes when you want to:

* extend or adapt an existing backend
* debug platform-specific integration issues
* understand where a specific low-level responsibility is implemented
* explore the design decisions behind core components

.. important::

    The pages in this chapter describe internal behavior and design decisions intended for contributors and maintainers.
    If you are using the library in an application, start with the usage guides and API reference instead.

.. grid:: 1
    :margin: 4 4 0 0
    :gutter: 1

    .. grid-item-card:: :fas:`window-maximize;sd-text-success` Windows Backend
        :link: windows-backend
        :link-type: doc

        Follow how the Windows console is initialized, how key input is processed,
        how the screen size is detected, and how termination state is restored.

    .. grid-item-card:: :fas:`terminal;sd-text-success` POSIX Backend
        :link: posix-backend
        :link-type: doc

        Explore the POSIX terminal setup, raw key input handling, TTY probing,
        and the pipe-based signal forwarding mechanism.

    .. grid-item-card:: :fas:`align-left;sd-text-success` Paragraph Layout and Painting
        :link: paragraph-layout-and-painting
        :link-type: doc

        Trace how paragraphs are tokenized, wrapped into physical lines,
        and finally painted into terminal buffers.

    .. grid-item-card:: :fas:`text-width;sd-text-success` Unicode Width System
        :link: unicode-width-system
        :link-type: doc

        Understand how the Unicode width table is generated, queried,
        and consistently applied during terminal layout.