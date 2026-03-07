..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    !single: Getting started

***************
Getting Started
***************

.. toctree::
    :maxdepth: 1
    :hidden:

    01-project-structure
    02-cmake-configuration
    03-write-minimal-main
    04-compile-and-run
    05-render-with-buffer
    06-layout-text-and-fonts
    07-input-and-animation
    08-the-next-steps

.. rst-class:: sd-fs-4

    Build a Small Terminal Dashboard Step by Step

In this tutorial you will build a compact but realistic terminal application from scratch.
Starting with an empty CMake project, you will gradually implement a colorful terminal
dashboard while learning the most important features of the library.

Along the way you will explore:

* direct terminal output using ``Terminal::printLine()``
* buffer-based rendering with ``Buffer`` and ``Terminal::updateScreen()``
* layout using rectangles, anchors, alignment, and framed panels
* Unicode-aware text rendering and wrapped text blocks
* animated bitmap-font headlines using ``Font`` and ``ColorSequence``
* keyboard interaction and resize-aware redraws

The tutorial uses a small standalone project and only requires a C++20 toolchain,
CMake, and Git.

.. button-ref:: 01-project-structure
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Start with the Project Structure →

.. rst-class:: sd-fs-4

    Prefer to Explore First?

If you would like to see what the library can do before building the tutorial
project, take a look at the :doc:`../demos/index` section. Each demo page
includes screenshots and highlights the features used in the example.

.. button-ref:: ../demos/index
    :ref-type: doc
    :color: info
    :outline:
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Explore the Demo Gallery →

