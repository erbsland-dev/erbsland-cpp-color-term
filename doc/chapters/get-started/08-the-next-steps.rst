..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

**************
The Next Steps
**************

This tutorial intentionally keeps the example compact, but the final
application already demonstrates the most important building blocks of
the library: terminal output, back-buffer rendering, geometry-based
layout, Unicode-aware text handling, bitmap fonts, animations, and
key-based input.

From here, you can easily expand the example into a more complete
terminal application.

Good Directions for Your Own Project
====================================

If you want to continue experimenting, here are a few natural ways to
extend the tutorial application:

* Split the rendering logic into separate functions or classes.
* Add additional panels using different ``FrameStyle`` values.
* Use ``ColorSequence`` for progress indicators or animated status highlights.
* Render large section headers with a custom ``Font``.
* Use ``InputDefinition`` to implement configurable key bindings.
* Reserve a dedicated footer or status bar and update it independently.

These small improvements quickly turn the example into a reusable
foundation for your own terminal dashboards or tools.

What to Read Next
=================

The :doc:`reference documentation</chapters/reference/index>` provides
complete API details for all types introduced in this tutorial.

If you prefer learning from complete applications instead of a guided
walk-through, continue with the :doc:`demo gallery</chapters/demos/index>`.
The demos provide focused examples for direct terminal output, animated
buffers, frame drawing, Unicode text layout, and bitmap fonts.

.. button-ref:: ../demos/index
    :ref-type: doc
    :color: info
    :align: center
    :outline:
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Explore the Demo Gallery →

.. button-ref:: ../reference/index
    :ref-type: doc
    :color: info
    :align: center
    :outline:
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-mb-4

    Open the API Reference →

