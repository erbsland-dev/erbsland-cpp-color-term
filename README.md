# Erbsland Color Terminal Library for C++
[![CI](https://github.com/erbsland-dev/erbsland-cpp-color-term/actions/workflows/ci.yml/badge.svg)](https://github.com/erbsland-dev/erbsland-cpp-color-term/actions/workflows/ci.yml) [![Docs](https://github.com/erbsland-dev/erbsland-cpp-color-term/actions/workflows/build-docs.yml/badge.svg)](https://github.com/erbsland-dev/erbsland-cpp-color-term/actions/workflows/build-docs.yml)

✔ Unicode-safe terminal layout  
✔ Cross-platform terminal abstraction  
✔ Clean C++20 API for colorful terminal output

The **Erbsland Color Terminal Library** is a lightweight **C++20 static library** for building rich terminal applications.

It provides reliable building blocks for **colorful terminal output**, **Unicode-aware text handling**, **interactive input**, and **2D geometry utilities**. These components help you build terminal user interfaces, command-line tools, and even terminal games with far less low-level boilerplate.

The library is designed to be **lightweight**, **expressive**, and **portable**, with native support for **Linux**, **macOS**, and **Windows** terminals.

![Screenshot](doc/images/retro-plasma.jpg)

# Features

## Low-Level Terminal Abstraction

The low-level layer provides portable building blocks for terminal interaction.

- Cross-platform terminal backend for **Linux, macOS, and Windows** — write once and compile everywhere
- Direct control over **colors, cursor positioning, and screen management**
- Automatic terminal detection and initialization
- Convenient colored text output using **ANSI escape sequences**
- Optional **text-only mode** for non-interactive applications
- **Unicode-aware terminal strings** with accurate display-width calculation
- Automatic handling of **wide and combined Unicode code points**
- Detection and rejection of **invalid UTF-8 sequences**
- **Interactive key input** and traditional **line-based input**
- Geometry utilities for positions, sizes, rectangles, anchors, alignment, margins, and bitmaps
- Efficient **line buffering** for handling large amounts of terminal output
- Automatic **terminal state restoration on exit**, including unexpected exits caused by signals

## High-Level Terminal Utilities

The high-level layer builds on the low-level primitives to simplify the implementation of complex terminal interfaces.

- Efficient **cell buffer classes** for preparing terminal output
- Automatic **back-buffering and delta screen updates**
- Automatic switching to the **alternate screen** and restoration on exit
- **View system** to render sections of buffers on screen
- Frame and rectangle drawing utilities
- Character-combination framework for building block-based terminal graphics
- **Tile-9 fill** and **tile-16 frame** drawing helpers
- **Custom font support**
- Text wrapping and alignment utilities
- **Bitmap font rendering** for large terminal text
- Color sequences and **animated color effects**
- Bitmap utilities for generating masks and rendering bitmaps to the screen

# Documentation

Full documentation, tutorials, and API references are available at:

→ https://color-term.erbsland.dev

The documentation includes:

- Step-by-step tutorial
- Integration and usage guides
- Complete API reference
- Build and development instructions

# Installation

The recommended way to use the library is by including it directly in your **CMake project**.

Clone the repository and add it as a subdirectory:

```console
git clone https://github.com/erbsland-dev/erbsland-cpp-color-term.git
```

Then include it in your `CMakeLists.txt`.

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

using namespace erbsland::cterm;

auto main() -> int {
    auto terminal = Terminal{{80, 25}};
    terminal.initializeScreen();

    terminal.printLine(fg::BrightGreen, "Hello terminal.");

    terminal.restoreScreen();
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
- **Python 3** (used by the build tooling)

# License

Copyright (c) 2026 Tobias Erbsland  
https://erbsland.dev

Licensed under the **Apache License, Version 2.0**.
