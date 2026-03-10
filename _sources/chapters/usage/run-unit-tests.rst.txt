..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

.. index::
    single: unittest
    single: build
    single: usage

*************************
How to Run the Unit Tests
*************************

Unit tests are an essential part of maintaining a reliable terminal library. This project uses the
`Erbsland Unit Test <https://unittest.erbsland.dev>`_ framework.

Follow the steps below to compile and run the tests locally.

Compile the Unit Tests
======================

To build the unit tests in debug mode using CMake and Ninja:

.. code-block:: console

    $ cmake -S . -B cmake-build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
    $ cmake --build cmake-build-debug --target erbsland-color-term-unittest

Alternatively, you can create your build directory outside the repository. In that case, adjust the paths accordingly in the following commands:

.. code-block:: console

    $ mkdir ~/build
    $ cd ~/build
    $ cmake -S /path/to/src/erbsland-cpp-color-term -G Ninja -DCMAKE_BUILD_TYPE=Debug
    $ cmake --build . --config Debug

The option ``-G Ninja`` selects the Ninja build system, which we recommend for its speed and efficiency. You can omit this option to fall back to the default generator available on your system.

Run All Unit Tests
==================

After compiling, you can execute all unit tests with the following commands:

.. code-block:: console

    $ cd cmake-build-debug
    $ ctest .

This tells CTest to run all tests defined in the current build directory. Be sure you're inside the right directory—otherwise, it won’t find the tests.

Run Individual Tests
====================

During development, it's often more efficient to run only a specific test suite. You can do this directly via the test binary:

.. code-block:: console

    $ ./cmake-build-debug/test/unittest/erbsland-color-term-unittest name:ColorSequence

In this example, only the ``ColorSequence`` suite is executed.

To explore all available test suites, targets, and tags, run:

.. code-block:: console

    $ ./cmake-build-debug/test/unittest/erbsland-color-term-unittest --list

Filter Syntax
=============

You can filter which tests to run using the format ``[+|-]<type>:<name>``. This makes it easy to include or exclude specific tests:

* ``name:TestX`` — runs only the specified test or suite
* ``+tag:Slow`` — explicitly includes tests tagged ``Slow``, even if they’re skipped by default
* ``-name:DateTest`` — runs all tests except ``DateTest``

Available Types
---------------

* ``name``: Name of a test class or individual test method
* ``tag``: A custom tag you’ve assigned to tests
* ``target``: A logical target or component under test

Options for Automation
======================

These options are especially useful in CI environments or automated scripts:

* ``-c``, ``--no-color`` — disables colorized terminal output
* ``-e`` — stops test execution after the first error
* ``-s``, ``--no-summary`` — skips the end-of-run summary output

More Information
================

You’ll find comprehensive documentation in the local path: :file:`test/erbsland-unittest/doc`
Or check the online docs at: `Erbsland Unit Test Documentation <https://unittest.erbsland.dev>`_

For a quick overview of all supported options, simply run:

.. code-block:: console

    ./cmake-build-debug/test/unittest/erbsland-color-term-unittest --help
