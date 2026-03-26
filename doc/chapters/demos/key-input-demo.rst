..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

**************
Key Input Demo
**************

``key-input-demo`` visualizes detected key presses inside a horizontally
scrolling ``RemappedBuffer``. Every key press becomes a colored stamp in the
field, so the demo doubles as both an input test and a compact illustration
of retained buffer content that moves independently from new writes.

It is a helpful example when you want to combine immediate key handling with
buffer-based animation.

Run the Demo
============

Start the demo from the build directory:

.. code-block:: console

    $ ./cmake-build-debug/demo-apps/key-input-demo

Press any key to stamp it into the scrolling field. Press ``Esc`` to quit.

ANSI Output Example
===================

The following capture shows the scrolling field after one ``A`` key press:

.. erbsland-ansi::
    :escape-char: ␛
    :theme: ela-term

    ␛[97;100m  Key Input Demo                                 ␛[96mhorizontal RemappedBuffer 250x30

    ␛[39;40m     ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙
    ␛[39m     ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙
    ␛[39m     ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙
    ␛[39m     ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙
    ␛[39m     ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙
    ␛[39m     ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙
    ␛[39m     ␛[90m∙␛[39m            ␛[97;41m[A]␛[39;40m    ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙
    ␛[39m     ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙
    ␛[39m     ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙
    ␛[39m     ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙
    ␛[39m     ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙␛[39m                   ␛[90m∙

    ␛[97;100m ␛[93m[esc]␛[97m quit ␛[96mPress any other key to stamp a colored key block into the scrolling␛[0m

Features Shown
==============

This demo brings several interactive building blocks together:

* ``Input::Mode::Key`` with immediate key handling.
* ``Key::toDisplayText()`` for user-facing labels.
* Horizontal ``RemappedBuffer`` scrolling.
* Mixed animation and event-driven drawing in one retained buffer.

Relevant Source Files
=====================

If you want to explore the implementation, start with:

:file:`demo/key-input-demo/src/KeyInputDemoApp.cpp`

This file contains the scrolling buffer setup, key stamping logic, and the
footer/help text generation.
