# Erbsland Color Terminal Library for C++

[![CI](https://github.com/erbsland-dev/erbsland-cpp-color-term/actions/workflows/ci.yml/badge.svg)](https://github.com/erbsland-dev/erbsland-cpp-color-term/actions/workflows/ci.yml) [![Docs](https://github.com/erbsland-dev/erbsland-cpp-color-term/actions/workflows/build-docs.yml/badge.svg)](https://github.com/erbsland-dev/erbsland-cpp-color-term/actions/workflows/build-docs.yml)

- Unicode-safe terminal rendering and layout
- Rich terminal output, HTML rendering, and structured UI building blocks
- Clean cross-platform C++20 API for Linux, macOS, and Windows

The **Erbsland Color Terminal Library** is a lightweight **C++20 static library** for building rich terminal
applications.

It provides reliable building blocks for **colorful terminal output**, **Unicode-aware text handling**, **interactive
input**, **buffer-based rendering**, **wrapped paragraph layout**, **rich-text and HTML rendering**, and a **beta UI
framework** for structured terminal applications. These components help you build command-line tools, dashboards,
document-like terminal output, and full-screen terminal interfaces with far less low-level boilerplate.

The library is designed to be **lightweight**, **expressive**, and **portable**, with native support for **Linux**, *
*macOS**, and **Windows** terminals.

![Screenshot](doc/images/retro-plasma.jpg)

# Features

## Terminal Foundations

The foundational layer provides portable building blocks for terminal interaction and text handling.

- Cross-platform terminal backend for **Linux, macOS, and Windows** — write once and compile everywhere
- Direct control over **colors, cursor positioning, and screen management**
- Automatic terminal detection and initialization
- Convenient colored text output using **ANSI escape sequences**
- Optional **text-only mode** for non-interactive applications
- **Unicode-aware terminal strings** with accurate display-width calculation
- Automatic handling of **wide and combined Unicode code points**
- Configurable handling of **invalid UTF-8**, including strict rejection and deterministic replacement
- **Interactive key input** and traditional **line-based input**
- Geometry utilities for positions, sizes, rectangles, anchors, alignment, margins, and bitmaps
- Automatic **terminal state restoration on exit**, including unexpected exits caused by signals

## Rendering, Buffers, and Layout

The rendering layer builds on the terminal primitives to simplify complex screen output and reusable text layout.

- Efficient **cell buffer classes** for preparing terminal output
- Automatic **back-buffering and delta screen updates**
- Automatic switching to the **alternate screen** and restoration on exit
- **CursorWriter** and **CursorBuffer** for shared streaming output to terminals and scrollback-style buffers
- **RemappedBuffer** for efficient row- and column-oriented editing workloads
- **View system** to render sections of buffers on screen
- Reusable **paragraph layout** with wrapping, indentation, tab stops, spacing, wrap markers, and ellipsis handling
- Text rendering helpers for alignment, animation, and reusable **TextOptions**
- Frame and rectangle drawing utilities
- Character-combination framework for building block-based terminal graphics
- **Tile-9 fill** and **tile-16 frame** drawing helpers
- **Custom font support**
- **Bitmap font rendering** for large terminal text
- Color sequences and **animated color effects**
- Bitmap utilities for generating masks and rendering bitmaps to the screen

## Rich Text and HTML

The text layer can render structured terminal documents instead of only plain strings.

- **HtmlRenderer** for converting a focused HTML subset into terminal-friendly output
- **Style**, **StyleRule**, **StyleSelector**, and **TextNode** for document-style rendering and customization
- Rich handling for headings, lists, links, blockquotes, code blocks, and inline formatting
- Rendering either into **String** values or directly to any **CursorWriter**

## Beta UI Framework

The optional UI layer helps you build structured event-driven terminal applications.

- **Application**, **Page**, **Surface**, and **Layout** abstractions for full-screen terminal apps
- Built-in **Stack**, **Panel**, and **TextBox** surface types
- **KeyBindings**, focus routing, layout invalidation, and paint invalidation
- **Scheduler**, **EventScheduler**, and **EventThread** support for timed actions and background work

# Documentation

Full documentation, tutorials, and API references are available at:

→ https://color-term.erbsland.dev

The documentation includes:

- Step-by-step tutorial
- Integration and usage guides
- Complete API reference
- Demo gallery
- Implementation notes for contributors
- Changelog and release history
- Build and development instructions

# Installation

The recommended way to use the library is to add it to your repository as a **Git submodule** and build it together
with your **CMake project**.

From your project root, add the library as a submodule:

```console
git submodule add https://github.com/erbsland-dev/erbsland-cpp-color-term.git
```

Then include it in your top-level `CMakeLists.txt` using `add_subdirectory(erbsland-cpp-color-term)`.

For the full recommended project layout and integration steps, see:

→ https://color-term.erbsland.dev/chapters/usage/integrate-as-submodule.html

# Quick Start

```cmake
cmake_minimum_required(VERSION 3.28)
project(ExampleProject)

add_subdirectory(erbsland-cpp-color-term)

add_executable(example src/main.cpp)
target_link_libraries(example PRIVATE erbsland-color-term)
```

Example program:

```cpp
#include <erbsland/cterm/Terminal.hpp>
#include <erbsland/cterm/TerminalSession.hpp>

using namespace erbsland::cterm;

auto main() -> int {
    auto terminal = Terminal{{80, 25}};
    auto session = TerminalSession{terminal};

    terminal.printLine(fg::BrightGreen, "Hello terminal.");

    return 0;
}
```

# Building the Library

Configure and build the project with CMake:

```console
cmake -S . -B cmake-build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug
```

# Running the Unit Tests

The project includes standalone unit tests.

```console
ctest --test-dir cmake-build-debug --output-on-failure
```

# Requirements

- **C++20 compatible compiler**
- **CMake 3.28+**
- **Python 3.11+** only for development and project tooling such as helper scripts and test metadata generation

# License

Copyright (c) 2026 Tobias Erbsland  
https://erbsland.dev

Licensed under the **Apache License, Version 2.0**.
