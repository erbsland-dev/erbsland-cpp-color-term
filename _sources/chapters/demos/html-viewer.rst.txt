..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

***********
HTML Viewer
***********

``html-viewer`` demonstrates the buffer-oriented path from HTML input to terminal output. It parses HTML, renders the
document into a growing ``CursorBuffer``, and in interactive mode exposes the visible area through a manually managed
``BufferView``.

Use This Demo When You Need...
==============================

* A support example for rendering HTML into terminal-native styled text.
* A practical reference for ``CursorBuffer`` plus ``BufferView`` driven scrolling.
* A bridge between document rendering and ordinary buffer-based full-screen output.

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/html-viewer
    $ ./cmake-build-debug/demo-apps/html-viewer --style=simple

The interactive viewer uses the arrow keys, ``PgUp`` and ``PgDn``, ``Home`` and ``End`` for navigation, ``S`` to
change style presets, and ``Q`` to quit. The capture below shows the deterministic ``--print`` mode with the bundled
tea document at 80 columns.

Captured Output (80x25)
=======================

.. include:: _captures/html-viewer.rstinc

Features Demonstrated
=====================

* ``HtmlRenderer`` rendering directly into a ``CursorBuffer``.
* ``BufferView`` used as an explicit viewport over the rendered document.
* Header and footer painting with ordinary buffer drawing calls in interactive mode.
* Manual scroll offset management and style switching.
* A print mode that turns the same HTML pipeline into plain terminal history output.

Related Demos
=============

* :doc:`UI HTML Viewer <ui-html-viewer>` for the same rendering goal built on the UI framework.
* :doc:`Command Line Help <command-line-help>` for another example of dense text layout in a terminal setting.
* :doc:`Log Viewer <log-viewer>` for another ``CursorBuffer`` plus viewport based application.

Relevant Source Files
=====================

If you want to explore the implementation, start with :file:`demo/html-viewer/src/HtmlViewerApp.hpp` and
:file:`demo/html-viewer/src/HtmlViewerApp.cpp`.
