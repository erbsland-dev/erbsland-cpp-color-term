..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

**************
UI HTML Viewer
**************

``ui-html-viewer`` shows how to build a structured document viewer on top of the UI framework. It reuses the HTML
rendering pipeline but delegates layout, header/footer lines, and scroll behavior to reusable UI surfaces.

Use This Demo When You Need...
==============================

* A support example for the UI framework version of a scrollable document viewer.
* A reference for combining ``HeaderLine`` and ``FooterLine`` with a reusable scrollable center surface.
* An example of parsing command line options in ``ui::Application`` and applying them to an existing surface tree.
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
* ``HeaderLine`` and ``FooterLine`` used without demo-specific subclasses.
* ``setupUi()`` builds the UI shell before ``processCommandLineArguments()`` loads the selected document and style.
* Shared ``Action`` objects for scrolling, style changes, quit handling, and automatic footer keyboard help.
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
