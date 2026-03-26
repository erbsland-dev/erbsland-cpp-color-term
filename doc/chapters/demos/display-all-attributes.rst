..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

**********************
Display All Attributes
**********************

``display-all-attributes`` gives you a compact visual overview of the
ANSI character attributes supported by the library. It prints one row
for each standard text attribute and a few common combinations so you
can quickly compare what your terminal actually renders.

Unlike backend capability flags, the rendered rows reflect the behavior
of your real terminal emulator and theme. If a sample looks unchanged,
the attribute is probably ignored or deliberately suppressed by the
terminal.

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/display-all-attributes

What It Shows
=============

The demo is meant as a fast support checklist:

* ``Bold``, ``Dim``, ``Italic``, ``Underline``, ``Blink``, ``Reverse``,
  ``Hidden``, and ``Strikethrough`` on separate rows.
* The backend-reported support status for each attribute.
* The primary ANSI SGR enable and disable codes for each row.
* A small combination section to verify that resets and reapplication
  work correctly when multiple attributes are active together.

This is especially useful when you want to confirm that your terminal
supports italic, blink, or hidden text before relying on them in an
application.

Relevant Source Files
=====================

If you want to explore the implementation, start with:

:file:`demo/display-all-attributes/src/DisplayAllAttributesApp.cpp`

This file contains the row generation and shows how to use
``CharAttributes`` directly with ``Terminal::printLine()``.
