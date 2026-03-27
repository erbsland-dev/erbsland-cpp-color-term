..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: requirements

************
Requirements
************

Code-Level Requirements
=======================

* A C++ compiler compatible with C++20 or later.
* A standard C++ library supporting C++20 features.

Build Requirements
==================

* **CMake**: Version 3.28 or newer.

Regular library use is purely CMake-based. If you consume the library as a submodule or build it together with your
application, no Python installation is required.

Development and Test Tooling
============================

* **Python**: Version 3.11 or newer for helper scripts, documentation tooling, and test metadata generation used while
  developing the library itself.
