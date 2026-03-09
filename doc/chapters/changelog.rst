..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: Changelog
    single: Changes

*********
Changelog
*********

Version 1.2.0 - 2026-03-10
==========================

Release 1.2.0 brings richer rendering primitives, better Unicode handling, smarter terminal updates, and broader examples and test coverage.

Highlights
----------

*   Added ``BitmapDrawOptions`` and new ``Buffer::drawBitmap()`` overloads for configurable bitmap rendering with scale modes, animated color sequences, stripe-based color modes, custom block characters, and optional ``Char16Style`` integration.
*   Added ``Tile9Style`` plus matching ``Buffer`` support for repeating 3x3 and extended 16-tile patterns when filling rectangles, drawing frames, and drawing filled frames.
*   Added ``Rectangle::gridCells()`` to split layouts into evenly spaced grid cells.
*   Expanded ``Char`` and ``String`` with UTF-32 support, multi-codepoint handling, improved combining-character support, and more flexible color overlay and base-color operations.
*   Improved color composition with explicit ``Inherited`` handling across ``ColorPart``, ``Color``, text rendering, bitmap rendering, and buffer-based drawing.
*   Extended ``Terminal`` screen updates with clearer refresh strategies, optional line buffering, optional back-buffer diff updates, improved crop-mark handling, and better minimum-size rendering behavior.

Added
-----

*   New ``display-all-colors`` demo for exploring the color model and layout helpers.
*   New ``bitmap-showcase`` demo for bitmap rendering options, styles, and animated color modes.
*   New ``update-screen-modes`` demo for comparing clear, overwrite, and back-buffer update behavior.
*   New unit tests for ``Tile9Style``, terminal update behavior, bitmap rendering paths, and ``Rectangle::gridCells()``.
*   Added ``display-all-colors``, ``bitmap-showcase``, and ``update-screen-modes`` demo applications.
*   Added ``BitmapDrawOptions`` and configurable bitmap rendering in ``Buffer``.
*   Added ``Tile9Style`` and tile-based fill/frame rendering support.
*   Added ``Rectangle::gridCells()``.
*   Added UTF-32 and multi-codepoint support to ``Char`` and ``String``.

Improved
--------

*   Updated ``frame-weaver``, ``text-gallery``, ``retro-plasma``, and ``terminal-chronicle`` to use the newer rendering and color APIs.
*   Refined terminal output APIs and documentation around refresh modes, direct writes, buffer rendering, crop marks, and smart overwrites.
*   Improved bitmap, text, geometry, color, and output reference documentation.
*   Improved color overlay behavior and inherited color handling throughout the library.
*   Improved ``Terminal::updateScreen()`` with refresh-mode, back-buffer, crop-mark, and minimum-size handling updates.

Implementation
--------------

*   Improved memory efficiency.
*   Expanded unit tests and API/reference documentation across rendering, geometry, text, and terminal output.


Version 1.0.0 — 2026-03-08
==========================

First public open-source release.
