..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

****************************
How to Write a New Unit Test
****************************

This project uses the *Erbsland Unit Test* system as its testing framework. This short guide shows how to add a new unit test.

File Structure
==============

- Unit tests go in :file:`test/unittest/src`
- Test data belongs in :file:`test/unittest/data`
- The framework and its documentation live in :file:`test/unittest/erbsland-unittest`

Basic Test Structure & Naming
=============================

The framework automatically registers test classes and methods—*if* you follow these naming rules:

- Class names must end in ``...Test`` (e.g., ``IntegerTest``)
- Filenames must match the class name and go in :file:`test/unittest/src` (e.g., :file:`IntegerTest.cpp`)
- You must add the file to :file:`test/unittest/src/CMakeLists.txt`
- Test methods must be named ``test...()``, return ``void``, and take no parameters

Minimal Example
===============

For a class ``Example``, create :file:`ExampleTest.cpp` like this:

.. code-block:: cpp

    // Copyright (c) 2025 Tobias Erbsland - https://erbsland.dev
    // SPDX-License-Identifier: Apache-2.0

    #include <erbsland/unittest/all.hpp>

    TESTED_TARGETS(Example)
    class ExampleTest : public el::UnitTest {
    public:
        Example example;

        void testConstruction() {
            example = Example{};
            REQUIRE(example.empty());
        }
    };

You can optionally use ``TESTED_TARGETS(...)`` or ``TAGS(...)`` to add metadata for filtering and test selection.

Test Macros
===========

Here’s what you can use to verify results:

* ``REQUIRE(expr)``
* ``REQUIRE_FALSE(expr)``
* ``REQUIRE_THROWS(expr)``
* ``REQUIRE_THROWS_AS(exception_class, expr)``
* ``REQUIRE_NOTHROW(expr)``
* ``REQUIRE_EQUAL(actual, expected)``
* ``REQUIRE_NOT_EQUAL(a, b)``
* ``REQUIRE_LESS(a, b)``
* ``REQUIRE_LESS_EQUAL(a, b)``
* ``REQUIRE_GREATER(a, b)``
* ``REQUIRE_GREATER_EQUAL(a, b)``

If you're testing custom types, make sure they can be formatted using ``std::format``:

.. code-block:: cpp

    template <>
    struct std::formatter<Example> : std::formatter<std::string> {
        auto format(const Example &value, format_context& ctx) const {
            return std::formatter<std::string>::format(value.toString(), ctx);
        }
    };

Shared Test Functions
=====================

To test multiple cases with the same logic, define helper methods. Use ``WITH_CONTEXT(...)`` to include calling context in error messages.

.. code-block:: cpp

    class ExampleTest : public el::UnitTest {
    public:
        void verifyOutput(Example ex, std::string expectedOutput) {
            std::string output;
            REQUIRE_NOTHROW(output = ex.toString());
            REQUIRE_EQUAL(output, expectedOutput);
        }

        void testConstruction() {
            WITH_CONTEXT(verifyOutput(Example{23}, "good"));
            WITH_CONTEXT(verifyOutput(Example{-2}, "bad"));
            WITH_CONTEXT(verifyOutput(Example{20302}, "max"));
        }
    };

Share Functionality Across Multiple Tests
=========================================

To reuse functionality across several test classes, create a shared base class. Just make sure the class name **does not** end in ``...Test``, or it will be mistakenly registered as a test.

Example in ``TestHelper.cpp``:

.. code-block:: cpp

    class TestHelper : public el::UnitTest {
        // Shared utility functions for tests
    };

Then inherit from it in your actual test class, using the ``UNITTEST_SUBCLASS(...)`` macro to ensure correct test registration:

.. code-block:: cpp

    class ExampleTest : public UNITTEST_SUBCLASS(TestHelper) {
        // Tests go here
    };

Add Extra Info on Test Failures
===============================

To make test failures easier to diagnose, you can either override ``additionalErrorMessages()`` or use ``runWithContext()`` for context-aware error reporting.

Option 1: ``additionalErrorMessages()``
---------------------------------------

This method lets you return more details when a test fails—like the current state of the object under test:

.. code-block:: cpp

    class ExampleTest : public el::UnitTest {
    public:
        Example example;

        auto additionalErrorMessages() -> std::string override {
            try {
                return std::format("example.state = {}", example.state);
            } catch (...) {
                return "Exception in additionalErrorMessages()";
            }
        }

        void testConstruction() {
            example = Example{};
            REQUIRE(example.empty());
        }
    };

This message is shown *only if* a test fails.

Option 2: ``runWithContext(...)``
---------------------------------

Use this when testing multiple cases in a loop, to show which input failed:

.. code-block:: cpp

    class ExampleTest : public el::UnitTest {
    public:
        void testConstruction() {
            struct TestData {
                int value;
                std::string expectedOutput;
            };

            const auto testData = std::vector<TestData>{
                {-1, "bad"},
                {0, "good"},
                {1, "good"},
                // ...
            };

            for (const auto &data : testData) {
                runWithContext(SOURCE_LOCATION(), [&]() {
                    auto ex = Example(data.value);
                    std::string output;
                    REQUIRE_NOTHROW(output = ex.toString());
                    REQUIRE_EQUAL(output, data.expectedOutput);
                }, [&]() -> std::string {
                    return std::format("Failed for value = {}", data.value);
                });
            }
        }
    };
