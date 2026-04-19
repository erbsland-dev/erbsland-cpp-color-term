..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

**************
String Sharing
**************

The terminal-text layer uses one shared storage model for
:cpp:any:`String <erbsland::cterm::String>` and
:cpp:any:`StringView <erbsland::cterm::StringView>`.

Overview
========

``String`` references one shared ``impl::StringData`` object and an :cpp:any:`IndexRange<erbsland::cterm::IndexRange>`
describing the visible sub-range inside that storage.

This gives us:

* cheap copies of ``String``
* cheap slices via ``substr()``
* a read-only ``StringView`` that points at the same storage

Detach Rules
============

All read-only operations use the shared storage directly.

Every mutating ``String`` API detaches first when one of these conditions is true:

* the backing storage is shared with another string or view
* the current string represents only a sub-range of the backing storage

Detaching materializes only the currently visible range into a fresh ``impl::StringData`` instance.
After that, the mutating operation works on a unique contiguous buffer again.

Thread Safety
=============

Implicit sharing is thread-safe:

* concurrent reads of shared copies are safe
* mutating one copied instance is safe because it detaches first
* mutating the same ``String`` object from multiple threads is **not thread safe**.

Design Notes
============

``StringView`` intentionally stays read-only. It shares the exact same storage representation as ``String``
but only exposes read-only methods and return ``StringView`` instances.
