..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*****************
Command Line Help
*****************

``command-line-help`` demonstrates how a regular command-line tool can keep a help screen readable across different
terminal widths. It uses ``Terminal::printParagraph()`` together with tab stops, wrap markers, and fallback behavior
to format realistic command help.

Use This Demo When You Need...
==============================

* A support example for width-aware command help or option lists.
* A place to study paragraph wrapping, indent handling, tab stops, and ellipsis behavior together.
* A deterministic demo you can rerun with different widths and paragraph options while debugging layout issues.

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/command-line-help
    $ ./cmake-build-debug/demo-apps/command-line-help --terminal-width=80

The second form is useful when you want deterministic wrapping for comparison or support work.

Captured Output (80x25)
=======================

.. include:: _captures/command-line-help.rstinc

Features Demonstrated
=====================

* ``Terminal::printParagraph()`` for structured, width-aware console text.
* Mixed-style ``String`` values for realistic command signatures and option names.
* Tab-stop based alignment of option descriptions.
* Configurable paragraph wrapping, break markers, ellipsis handling, and fallback behavior.
* A deliberate plain-output fallback for very narrow terminals.

Related Demos
=============

* :doc:`Terminal Chronicle <terminal-chronicle>` for lower-level print-style terminal output.
* :doc:`Text Gallery <text-gallery>` for other text layout and alignment examples inside a full-screen UI.
* :doc:`HTML Viewer <html-viewer>` for a larger text-rendering pipeline that ends in a scrollable document view.

Relevant Source Files
=====================

If you want to explore the implementation, start with :file:`demo/command-line-help/src/CommandLineHelpDemo.cpp`.

This file contains the argument parsing, help document construction, and the two output paths for wide and narrow
terminal widths.
