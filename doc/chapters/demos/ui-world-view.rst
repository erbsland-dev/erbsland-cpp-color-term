..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*************
UI World View
*************

``ui-world-view`` is a compact zoomable world-map viewer built on the beta UI framework. It combines bitmap rendering,
custom viewport logic, reusable horizontal and vertical scroll bars, and generated map data in one demo.

Use This Demo When You Need...
==============================

* A realistic example of a custom paint surface inside ``ui::Application``.
* A reference for combining ``Bitmap`` rendering with ``HeaderLine``, ``FooterLine``, actions, and scroll bars.
* An example of building the UI shell first, then loading command line selected data into that shell.
* A small end-to-end example of shipping generated demo data without any runtime rendering dependencies.

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/ui-world-view

Use the arrow keys to pan, ``+`` or ``]`` to zoom in, ``-`` or ``[`` to zoom out, ``L`` to toggle labels, ``C`` to
toggle the latitude/longitude grid, and ``Q`` or ``Esc`` to quit.

Captured Output (80x25)
=======================

.. include:: _captures/ui-world-view.rstinc

Features Demonstrated
=====================

* ``HorizontalScrollBar`` and ``VerticalScrollBar`` driven from a custom viewport model.
* Demo-local composite layout with a viewport, both scroll bars, and a corner filler surface.
* One multi-key ``Action`` for compact cursor-key panning and footer help.
* ``processCommandLineArguments()`` used to load ``world-map.txt`` and attach the map panel before the UI event system
  starts.
* ``CharCombinationStyle::commonBoxFrame()`` to combine latitude and longitude grid crossings on top of the map.
* A generated ``world-map.txt`` asset loaded at runtime without external dependencies.
* Country label overlays that stay available at lower zooms when collision filtering leaves enough room.
* Pinned coordinate labels and a toggleable grid overlay rendered above the map terrain.

Related Demos
=============

* :doc:`Bitmap Showcase <bitmap-showcase>` for lower-level bitmap drawing examples.
* :doc:`UI Hello World <ui-hello-world>` for the smallest UI framework example.
* :doc:`UI HTML Viewer <ui-html-viewer>` for another custom center panel with footer help and keyboard-driven updates.

Relevant Source Files
=====================

If you want to explore the implementation, start with :file:`demo/ui-world-view/src/UiWorldViewApp.cpp`,
:file:`demo/ui-world-view/src/WorldMapData.cpp`, and :file:`demo/ui-world-view/src/WorldMapView.cpp`.
