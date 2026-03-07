..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*****
Demos
*****

This repository includes four small demo applications. Each demo highlights a specific strength of the library and
shows how you can use it in practice. They are useful both as quick showcases and as reference implementations when
you start integrating the library into your own projects.

If you prefer learning by exploring real code, the demos are a great place to begin.

.. rubric:: Building the Demo Applications

When this repository is used as the top-level project, the demo applications are built automatically.

If you integrate the project into a larger build or disabled demos previously, you can enable them explicitly during
configuration:

.. code-block:: console

    $ cmake -B cmake-build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DERBSLAND_COLOR_TERM_ENABLE_DEMOS=ON .
    $ cmake --build cmake-build-debug

After a successful build, you will find the demo executables in:

:file:`cmake-build-debug/demo-apps/`

Each executable corresponds to one of the demos listed below.

.. toctree::
    :maxdepth: 1

    terminal-chronicle
    retro-plasma
    frame-weaver
    text-gallery
