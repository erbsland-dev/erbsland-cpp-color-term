..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

*********************
The Project Structure
*********************

Organizing the Example Project
==============================

In this tutorial, we will build a small application called ``signal-board``.
To keep the project self-contained, the library is included as a Git submodule.
This makes the tutorial easy to reproduce while still allowing you to update
the library independently when needed.

By the end of this step, your directory structure will look like this:

.. code-block:: none

    signal-board-project               # Root directory of the tutorial project
        ├── erbsland-cpp-color-term    # The library as a Git submodule
        │   └── (...)
        ├── signal-board               # The tutorial application
        │   ├── src                    # Application source files
        │   │   └── main.cpp
        │   └── CMakeLists.txt
        └── CMakeLists.txt             # Top-level build configuration

Create the Project Skeleton
===========================

#. Create the project directory and add the library as a submodule:

   .. code-block:: console

        $ mkdir signal-board-project
        $ cd signal-board-project
        $ git init
        Initialized empty Git repository in ~/signal-board-project/.git/
        $ git submodule add https://github.com/erbsland-dev/erbsland-cpp-color-term.git erbsland-cpp-color-term
        Cloning into '~/signal-board-project/erbsland-cpp-color-term'...
        ...

#. Create the directory for the tutorial application:

   .. code-block:: console

        $ mkdir signal-board
        $ mkdir signal-board/src

At this point, your project structure should look like this:

.. code-block:: none
    :emphasize-lines: 2-4

    signal-board-project
        ├── erbsland-cpp-color-term    # [new] Library submodule
        └── signal-board               # [new] Application directory
            └── src                    # [new] C++ source directory

.. note::

    If you clone this project later, remember to initialize the submodules:

    .. code-block:: console

        git submodule update --init --recursive

.. button-ref:: 02-cmake-configuration
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Write the CMake Configuration →

.. card:: About the Command-Line Examples

    This tutorial uses small command-line examples so the workflow is easy to
    follow and reproduce. You can use the same project structure with any IDE
    or editor, as long as the resulting files contain the same content.