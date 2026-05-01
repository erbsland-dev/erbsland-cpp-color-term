..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

**************
UI Hello World
**************

``ui-hello-world`` is the smallest UI framework demo in the repository. It shows how little user code is needed to
build a complete page with a live header, centered content, key handling, and a footer once the framework manages
layout and painting.

Use This Demo When You Need...
==============================

* The smallest possible starting point for the ``ui`` framework.
* A support reference for ``Page``, ``Stack``, ``TextBox``, ``HeaderLine``, and ``FooterLine`` working together.
* A compact example of subclassing ``ui::Application`` and overriding ``setupUi()``.
* An example of live status updates without writing a custom application render loop.

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/ui-hello-world

Press ``Q`` or ``Esc`` to quit.

Captured Output (80x25)
=======================

.. include:: _captures/ui-hello-world.rstinc

Features Demonstrated
=====================

* ``HeaderLine`` and ``FooterLine`` used directly for the top and bottom rows.
* A centered, growing ``TextBox`` as the main content surface.
* ``ui::Application::setupUi()`` used as the single place where the page and surface tree are assembled.
* Shared ``Action`` objects for quit handling and automatic footer keyboard help.
* ``DynamicText`` callbacks for live clock text in the middle header section.
* A surface-local update interval that refreshes the clock once per second.

Related Demos
=============

* :doc:`UI HTML Viewer <ui-html-viewer>` for a more complete UI application with scrolling document content.
* :doc:`Minimum Effort <minimum-effort>` for the smallest non-UI full-screen equivalent.
* :doc:`Text Gallery <text-gallery>` for a retained-buffer example focused on centered text and layout polish.

Relevant Source Files
=====================

If you want to explore the implementation, start with :file:`demo/ui-hello-world/src/UiHelloWorldApp.cpp`.
