..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: build
    single: install
    single: usage

*************************************
Install the Library as Static Library
*************************************

You can build and install Erbsland Color Terminal as a standalone static library.
This approach is useful if you:

* Want to reuse the terminal library across multiple independent projects.
* Prefer a system-wide installation.
* Integrate it into an existing build or packaging workflow.

For most use cases, source-based integration with :doc:`integrate-as-submodule` is still the simplest approach.

Build and Install
=================

Follow these steps to clone, build, and install the library:

.. code-block:: console

    $ git clone https://github.com/erbsland-dev/erbsland-cpp-color-term.git
    $ mkdir build
    $ cd build
    $ cmake ../erbsland-cpp-color-term -DCMAKE_BUILD_TYPE=Release
    -- The CXX compiler identification is (...)
    (...)
    -- Build files have been written to: (...)/build

    $ cmake --build .
    [  1%] Building CXX object (...)/Color.cpp.o
    [  3%] Building CXX object (...)/Terminal.cpp.o
    (...)
    [100%] Linking CXX static library liberbsland-color-term.a
    [100%] Built target erbsland-color-term

    $ cmake --install .
    -- Install configuration: "Release"
    -- Installing: (...)/lib/liberbsland-color-term.a
    -- Installing: (...)/include
    (...)

After installation, you will have:

* The static library ``liberbsland-color-term.a``.
* All public header files in the install include directory.
* An exported CMake package configuration for ``find_package``.

Installation Prefix
-------------------

By default, CMake installs to a platform-specific location:

* ``/usr/local`` on most Unix-like systems
* A platform-dependent directory on Windows

If you want to control the install location, set the prefix explicitly:

.. code-block:: console

    $ cmake --install . --prefix /your/custom/path

Alternatively, you can define ``CMAKE_INSTALL_PREFIX`` during configuration:

.. code-block:: console

    $ cmake ../erbsland-cpp-color-term \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/your/custom/path

.. card:: Faster Builds Using Ninja

    For faster and more reliable incremental builds, we recommend using *Ninja* as your build tool on all platforms.

    .. code-block:: console

        $ git clone https://github.com/erbsland-dev/erbsland-cpp-color-term.git
        $ mkdir build
        $ cd build
        $ cmake ../erbsland-cpp-color-term -G Ninja -DCMAKE_BUILD_TYPE=Release
        $ cmake --build . -j 8

Linking from a Consumer Project
===============================

The installation exports a CMake package that provides the color terminal target.

In your consuming project:

.. code-block:: cmake

    find_package(erbsland-color-term REQUIRED)

    add_executable(example src/main.cpp)
    target_compile_features(example PRIVATE cxx_std_20)
    target_link_libraries(example PRIVATE
        ErbslandDEV::erbsland-color-term
    )

This ensures:

* Proper include directories
* A modern imported CMake target
* A static library that can be packaged with your application
