..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

***********************
The CMake Configuration
***********************

Write the Top-Level CMake File
==============================

Start by creating the project-wide :file:`CMakeLists.txt` in the root directory.
This file defines the overall project and registers the subdirectories that
contain the library and the application.

#. Create and open the file:

   .. code-block:: console

        $ nano CMakeLists.txt

#. Add the following content:

   .. code-block:: cmake
      :caption: <project>/CMakeLists.txt

        cmake_minimum_required(VERSION 3.28)
        project(SignalBoardProject LANGUAGES CXX)

        add_subdirectory(erbsland-cpp-color-term)
        add_subdirectory(signal-board)

The library directory is added first so its targets are available when the
application is configured.

Write the Application CMake File
================================

Next, create the CMake configuration for the tutorial application itself.

#. Create and open the file:

   .. code-block:: console

        $ nano signal-board/CMakeLists.txt

#. Add the following content:

   .. code-block:: cmake
      :caption: <project>/signal-board/CMakeLists.txt

        cmake_minimum_required(VERSION 3.28)
        project(SignalBoard LANGUAGES CXX)

        add_executable(signal-board
            src/main.cpp
        )

        target_compile_features(signal-board PRIVATE cxx_std_20)
        target_link_libraries(signal-board PRIVATE erbsland-color-term)

This configuration creates the executable and links it against the
``erbsland-color-term`` library that was registered in the top-level build.

Important Details
=================

A few important lines in this configuration are worth highlighting:

* ``add_subdirectory(erbsland-cpp-color-term)`` registers the library and
  makes its targets available to the rest of the build.
* ``target_compile_features(... cxx_std_20)`` enables the required C++20
  language features for the application.
* ``target_link_libraries(... erbsland-color-term)`` links the executable
  with the terminal rendering library.

The Current Project State
=========================

Your directory should now look like this:

.. code-block:: none
    :emphasize-lines: 5, 6

    signal-board-project
        ├── erbsland-cpp-color-term
        ├── signal-board
        │   ├── src
        │   └── CMakeLists.txt         # [new] Application build file
        └── CMakeLists.txt             # [new] Top-level build file

.. button-ref:: 03-write-minimal-main
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Write a Minimal Main Function →

