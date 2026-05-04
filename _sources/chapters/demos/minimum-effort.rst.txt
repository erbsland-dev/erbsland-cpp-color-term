..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

**************
Minimum Effort
**************

``minimum-effort`` is the smallest full-screen demo in the repository. It shows how quickly you can go from a fresh
terminal session to a framed layout with colored text, direct buffer drawing, and a clear title.

Use This Demo When You Need...
==============================

* A tiny starting point for a new project that still feels like a complete terminal application.
* The shortest possible example of ``Terminal`` setup, one persistent ``Buffer``, and one ``write()`` call.
* A support reference for simple ``drawFrame()`` plus ``drawText()`` composition without any application framework.

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/minimum-effort

The program renders one framed screen and exits immediately. The current title says ``15 Lines of Code``, so the page
uses that wording consistently.

Captured Output (80x25)
=======================

.. include:: _captures/minimum-effort.rstinc

Features Demonstrated
=====================

* ``Terminal`` initialization, one-screen rendering, and cleanup.
* One retained ``Buffer`` with ``fill()``, ``drawFrame()``, and ``drawText()``.
* A centered title rectangle derived from the full buffer geometry.
* A self-documenting screen that embeds the demo source directly in the rendered output.

Related Demos
=============

* :doc:`Terminal Chronicle <terminal-chronicle>` for direct print-style output without a retained buffer.
* :doc:`Frame Color Animations <frame-color-animations>` for richer frame rendering based on the same drawing ideas.
* :doc:`UI Hello World <ui-hello-world>` for the smallest comparable example built on the UI framework.

Relevant Source Files
=====================

If you want to explore the implementation, start with :file:`demo/minimum-effort/src/main.cpp`.

The full demo lives in that one file.
