..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: integration
    single: submodule
    single: build
    single: usage

**********************************
Integrate the Library as Submodule
**********************************

The recommended way to consume Erbsland Color Terminal is to add it to your repository and build it together with
your application. This keeps the dependency pinned to a known revision and makes local changes easy to test.

The library is designed to be linked as a **static library**.

Project Structure
=================

A typical project layout using the parser as a submodule might look like this:

.. code-block:: none

    <project root>                       # Your project root directory
        ├── erbsland-cpp-color-term      # The Erbsland Color Terminal sources
        │   └── (...)
        ├── <project>                    # Your application source directory
        │   ├── src                      # Application source files
        │   │   └── (...)
        │   └── CMakeLists.txt           # CMake build file for your app
        ├── (...)
        └── CMakeLists.txt               # Top-level CMake configuration file

The CMake Configuration
=======================

Below is a minimal example of how to configure your project using CMake:

.. code-block:: cmake
    :caption: <project>/CMakeLists.txt

    cmake_minimum_required(VERSION 3.25)
    project(ExampleProject)

    add_subdirectory(erbsland-cpp-color-term)
    add_subdirectory(example)

.. code-block:: cmake
    :caption: <project>/example/CMakeLists.txt

    cmake_minimum_required(VERSION 3.25)
    project(Example)

    add_executable(example
        src/main.cpp
    )

    target_compile_features(example PRIVATE cxx_std_20)
    target_link_libraries(example PRIVATE erbsland-color-term)

How to Compile
==============

To build your project with this setup, run the following commands from your project root:

.. code-block:: console

    $ mkdir cmake-build
    $ cmake . -B cmake-build
    ...
    -- Build files have been written to: ~/example/cmake-build

    $ cmake --build cmake-build
    [  1%] Building CXX object Color.cpp.o
    [  3%] Building CXX object Terminal.cpp.o
    ...
    [100%] Linking CXX executable example
    [100%] Built target example

.. card:: Faster Builds Using Ninja

    For faster and more reliable incremental builds, we recommend using *Ninja* as your build tool on all platforms.

    .. code-block:: console

        $ mkdir cmake-build
        $ cmake . -G Ninja -B cmake-build
        $ cmake -B cmake-build -j 8
