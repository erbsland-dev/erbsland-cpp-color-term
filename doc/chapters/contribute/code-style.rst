..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

**********************************
ErbslandDEV C++ Code Style Summary
**********************************

This guide defines the basic C++ style conventions used across ErbslandDEV codebases. It is focused on clarity, maintainability, and practical modern C++ usage.

Naming Conventions
==================

- **Classes**: PascalCase (e.g., ``Controller``, ``TestClassBase``)
- **Methods and Functions**: camelCase (e.g., ``addTestClass``, ``parseCommandLine``)
- **Member Variables**: ``_underscorePrefix`` (e.g., ``_console``, ``_testClasses``)
- **Global Constants**: ``cExample`` if not in dedicated namespace.
- **Namespaces**: all lowercase with nested structure (e.g., ``erbsland::unittest``)
- **Preprocessor Macros**: all uppercase ``EXAMPLE``

File Organization
=================

- **Header Guards**: Use ``#pragma once``
- **Include Order** (separate blocks with empty lines):
  1. Corresponding header (if in ``.cpp``) — *followed by two empty lines*
  2. Local includes: ``#include "Example.hpp"``
  3. Local subdirectory includes: ``#include "sub/sub/Example.hpp"``
  4. Local relative includes: ``#include "../../Example.hpp"``
  5. Project libraries: ``#include <erbsland/core/String.hpp>``
  6. Standard library headers: ``#include <vector>``
  7. *Two* empty lines after the include block
- **File Extensions**: Use ``.hpp`` for headers, ``.cpp`` for implementations
- **File Scope**: Aim for one class per file, with matching name
- **Maximum File Length**: Try to keep files under 500 lines

Formatting
==========

- **Indentation**: 4 spaces, no tabs
- **Line Length**: Target a maximum of 120 characters per line
- **Braces**: Opening brace stays on the same line
- **Spacing and Separation**:
  - Use empty lines to separate logical blocks
  - One empty line between function definitions if they span >3 lines or have documentation
  - Two empty lines around ``struct``, ``class``, or ``enum`` definitions
  - Two empty lines around function implementations in ``.cpp`` files

Comments
========

- **Public API Docs**: Use ``///`` and Doxygen ``@``-syntax
- **Inline Comments**: Use ``//`` for short clarifying notes
- **Block Comments**: Avoid ``/* ... */`` unless necessary
- **Special Doxygen Tags**:
  - ``@tested``: Indicates this is covered by a unit test (name or path to the tests)
  - ``@notest``: Explains why a unit test is not applicable
  - ``@needtest``: Flags untested functionality
  - ``@wip``: This part is work in progress—talk to the author before modifying

Constants & Literals
====================

- Prefer ``constexpr`` or ``const`` where applicable
- Never use macros for constants
- Move mechanic requires mutable instances!

Modern C++ Usage
================

- Use C++20 syntax
- Always use **trailing return types** for all non-void functions: ``auto create() -> std::string;``
- Use ``auto`` when the type is obvious or improves readability
- Use structured binding ``const auto &[a, b] =``
- Use ``[[nodiscard]]`` and ``noexcept`` where applicable
- Use ``override`` for overwritten/implemented functions
- Use ``final`` for final classes
- Use **designated initialization** if it improves the readability of the code
- Use ``std::unique_ptr`` / ``std::shared_ptr`` instead of raw pointers
- Prefer range-based ``for`` loops
- Use ``std::format`` and ``std::chrono`` for formatted output and timing
- Prefer ``std::ranges`` and ``std::views`` for expressive algorithms

Quality and Safety First
========================

- Avoid manual memory management and raw pointers
- Trust the compiler to optimize; prioritize clarity over micro-optimization
- Perform bounds and range checks
- Write unit tests for all modules and key functions
- Ensure good test coverage and fail clearly when things go wrong
- A crash is better than silent failure or undefined behavior
