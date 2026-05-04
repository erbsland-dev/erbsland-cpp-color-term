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
    action-scheduling
    invocation-and-event-threads
    ui-paint-system
    ui-scrollable-areas
    paragraph-layout-and-painting
    remapped-buffer
    stack-layout
    string-sharing
    text-node-renderer
    unicode-width-system

This chapter is written for developers working on the library itself.
It provides technical background information about internal implementation details,
code structure, and platform-specific behavior that are intentionally not part of the public API documentation.

.. important::

    The pages in this chapter describe internal behavior and design decisions intended for contributors and maintainers.
    If you are using the library in an application, start with the usage guides and API reference instead.

    We do our best to update implementation notes when the described implementation changes, nevertheless, please
    carefully validate against the actual implementation first, before doing any changes.

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

    .. grid-item-card:: :fas:`clock;sd-text-success` Action Scheduling
        :link: action-scheduling
        :link-type: doc

        Follow how surface-local scheduled actions are stored, turned into events,
        delivered on the UI thread, and connected to dirty-driven rendering.

    .. grid-item-card:: :fas:`share-alt;sd-text-success` Invocation and Event Threads
        :link: invocation-and-event-threads
        :link-type: doc

        Understand how generic invocation events move work onto the UI thread
        or a managed worker thread, and how cooperative shutdown is coordinated.

    .. grid-item-card:: :fas:`paint-roller;sd-text-success` UI Paint System
        :link: ui-paint-system
        :link-type: doc

        Follow how dirty surfaces become clipped paint calls, how opacity chooses the repaint base,
        and how overlapping siblings are composed in order.

    .. grid-item-card:: :fas:`arrows-up-down-left-right;sd-text-success` UI Scrollable Areas
        :link: ui-scrollable-areas
        :link-type: doc

        Understand the scroll metric helpers, managed child containers,
        visibility filtering, viewports, scroll areas, and custom-painted
        scroll surfaces.

    .. grid-item-card:: :fas:`align-left;sd-text-success` Paragraph Layout and Painting
        :link: paragraph-layout-and-painting
        :link-type: doc

        Trace how paragraphs are tokenized, wrapped into physical lines,
        and finally painted into terminal buffers.

    .. grid-item-card:: :fas:`list-tree;sd-text-success` TextNodeRenderer
        :link: text-node-renderer
        :link-type: doc

        Follow how rich-text nodes are planned into linear render blocks,
        how list prefixes are attached, and how plain-string and terminal output diverge.

    .. grid-item-card:: :fas:`shuffle;sd-text-success` Remapped Buffer
        :link: remapped-buffer
        :link-type: doc

        Understand how remap tables, storage layout, and resize paths work
        together to keep row- and column-oriented edits efficient.

    .. grid-item-card:: :fas:`clone;sd-text-success` String Sharing
        :link: string-sharing
        :link-type: doc

        Follow how `String`, `StringView`, shared backing storage, and
        copy-on-write detaches fit together.

    .. grid-item-card:: :fas:`table-columns;sd-text-success` Stack Layout
        :link: stack-layout
        :link-type: doc

        Follow how stack children receive their initial sizes, how free space
        is distributed, and how overflow is reduced in ordered shrink passes.

    .. grid-item-card:: :fas:`text-width;sd-text-success` Unicode Width System
        :link: unicode-width-system
        :link-type: doc

        Understand how the Unicode width table is generated, queried,
        and consistently applied during terminal layout.
