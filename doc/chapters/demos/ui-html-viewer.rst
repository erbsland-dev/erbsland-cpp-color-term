..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

**************
UI HTML Viewer
**************

``ui-html-viewer`` shows how to build a structured document viewer on top of the UI framework. It reuses the HTML
rendering pipeline but delegates layout, status lines, and scroll behavior to reusable UI surfaces.

Use This Demo When You Need...
==============================

* A support example for the UI framework version of a scrollable document viewer.
* A reference for combining ``StatusLine`` with a reusable scrollable center surface.
* A practical example of reusing ``HtmlRenderer`` output inside a higher-level UI application.

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/ui-html-viewer
    $ ./cmake-build-debug/demo-apps/ui-html-viewer --style=simple

Use the arrow keys or ``j`` and ``k`` to scroll, ``PgUp`` and ``PgDn`` to page, ``Home`` and ``End`` to jump, ``S``
to change style presets, and ``Q`` or ``Esc`` to quit.

Captured Output (80x25)
=======================

.. include:: _captures/ui-html-viewer.rstinc

Features Demonstrated
=====================

* ``ScrollingBufferView`` as the document viewport.
* ``StatusLine`` used for both header and footer without demo-specific subclasses.
* ``KeyBindings`` overloads for special keys, characters, and combined input sequences.
* ``HtmlRenderer`` and the rich-text style system reused unchanged from the non-UI viewer.
* UI-driven paint invalidation for live status updates.

Related Demos
=============

* :doc:`HTML Viewer <html-viewer>` for the lower-level buffer-first version of the same idea.
* :doc:`UI Hello World <ui-hello-world>` for the smallest UI framework example.
* :doc:`Log Viewer <log-viewer>` for another scrollable content surface built without the UI framework.

Relevant Source Files
=====================

If you want to explore the implementation, start with :file:`demo/ui-html-viewer/src/UiHtmlViewerApp.cpp` and
:file:`demo/ui-html-viewer/src/HtmlDocumentPanel.cpp`.
