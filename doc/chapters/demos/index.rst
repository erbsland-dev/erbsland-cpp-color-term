..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*****
Demos
*****

This repository includes several small demo applications.

Use the gallery in two ways: browse by feature if you are solving a concrete problem, or open the individual pages as
reference implementations when you want to study real code.

.. toctree::
    :hidden:
    :maxdepth: 1

    minimum-effort
    terminal-chronicle
    command-line-help
    display-all-attributes
    display-all-colors
    frame-color-animations
    retro-plasma
    frame-weaver
    grid-layout
    key-input-demo
    html-viewer
    log-viewer
    text-gallery
    ui-choice
    ui-hello-world
    ui-html-viewer
    ui-sections
    ui-world-view
    bitmap-showcase
    update-screen-modes


Find a Demo by Feature
======================

.. list-table::
    :header-rows: 1
    :widths: 40 60

    * - Feature Family
      - Matching Demos
    * - Getting Started / Smallest Examples
      - :doc:`Minimum Effort <minimum-effort>`, :doc:`Terminal Chronicle <terminal-chronicle>`,
        :doc:`UI Hello World <ui-hello-world>`
    * - Direct Terminal Output / Command Help / Paragraphs
      - :doc:`Terminal Chronicle <terminal-chronicle>`, :doc:`Command Line Help <command-line-help>`,
        :doc:`Display All Attributes <display-all-attributes>`, :doc:`Display All Colors <display-all-colors>`
    * - Text Layout / Unicode / Fonts
      - :doc:`Command Line Help <command-line-help>`, :doc:`Text Gallery <text-gallery>`
    * - Colors / Attributes / Animated Color Sequences
      - :doc:`Display All Colors <display-all-colors>`, :doc:`Display All Attributes <display-all-attributes>`,
        :doc:`Frame Color Animations <frame-color-animations>`, :doc:`Retro Plasma <retro-plasma>`
    * - Frames / Intersections / Custom Frame Tiles
      - :doc:`Minimum Effort <minimum-effort>`, :doc:`Frame Color Animations <frame-color-animations>`,
        :doc:`Frame Weaver <frame-weaver>`, :doc:`Grid Layout <grid-layout>`
    * - Buffers / Cursor Buffers / Buffer Views / Scrolling
      - :doc:`HTML Viewer <html-viewer>`, :doc:`Log Viewer <log-viewer>`, :doc:`Key Input Demo <key-input-demo>`,
        :doc:`UI HTML Viewer <ui-html-viewer>`
    * - Bitmap Rendering
      - :doc:`Bitmap Showcase <bitmap-showcase>`, :doc:`UI World View <ui-world-view>`
    * - HTML Rendering
      - :doc:`HTML Viewer <html-viewer>`, :doc:`UI HTML Viewer <ui-html-viewer>`
    * - UI Framework Surfaces
      - :doc:`UI Hello World <ui-hello-world>`, :doc:`UI Choice <ui-choice>`,
        :doc:`UI Sections <ui-sections>`, :doc:`UI HTML Viewer <ui-html-viewer>`,
        :doc:`UI World View <ui-world-view>`
    * - Input Handling / Refresh Behavior / Screen Update Modes
      - :doc:`Key Input Demo <key-input-demo>`, :doc:`Retro Plasma <retro-plasma>`,
        :doc:`Update Screen Modes <update-screen-modes>`

Building the Demo Applications
==============================

When this repository is used as the top-level project, the demo applications are built automatically.

If you integrate the project into a larger build or disabled demos previously, you can enable them explicitly during
configuration:

.. code-block:: console

    $ cmake -B cmake-build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DERBSLAND_COLOR_TERM_ENABLE_DEMOS=ON .
    $ cmake --build cmake-build-debug

After a successful build, you will find the demo executables in :file:`cmake-build-debug/demo-apps/`.

Browse All Demo Pages
=====================

*   :doc:`minimum-effort`
*   :doc:`terminal-chronicle`
*   :doc:`command-line-help`
*   :doc:`display-all-attributes`
*   :doc:`display-all-colors`
*   :doc:`frame-color-animations`
*   :doc:`retro-plasma`
*   :doc:`frame-weaver`
*   :doc:`grid-layout`
*   :doc:`key-input-demo`
*   :doc:`html-viewer`
*   :doc:`log-viewer`
*   :doc:`text-gallery`
*   :doc:`ui-choice`
*   :doc:`ui-hello-world`
*   :doc:`ui-html-viewer`
*   :doc:`ui-sections`
*   :doc:`ui-world-view`
*   :doc:`bitmap-showcase`
*   :doc:`update-screen-modes`
