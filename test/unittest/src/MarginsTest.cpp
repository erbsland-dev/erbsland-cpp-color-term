// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include <erbsland/unittest/UnitTest.hpp>

#include "TestHelper.hpp"


class MarginsTest : public el::UnitTest {
public:
    Margins margins;

    void testUniformConstructor() {
        margins = Margins(5);
        REQUIRE_EQUAL(margins.top(), 5);
        REQUIRE_EQUAL(margins.right(), 5);
        REQUIRE_EQUAL(margins.bottom(), 5);
        REQUIRE_EQUAL(margins.left(), 5);
    }

    void testHorizontalVerticalConstructor() {
        margins = Margins(2, 3);
        REQUIRE_EQUAL(margins.top(), 3);
        REQUIRE_EQUAL(margins.bottom(), 3);
        REQUIRE_EQUAL(margins.left(), 2);
        REQUIRE_EQUAL(margins.right(), 2);
    }

    void testIndividualConstructorAndGetters() {
        margins = Margins(1, 2, 3, 4);
        REQUIRE_EQUAL(margins.top(), 1);
        REQUIRE_EQUAL(margins.right(), 2);
        REQUIRE_EQUAL(margins.bottom(), 3);
        REQUIRE_EQUAL(margins.left(), 4);
    }

    void testEqualityAndInequality() {
        margins = Margins(1, 2, 3, 4);
        REQUIRE_EQUAL(margins, Margins(1, 2, 3, 4));
        REQUIRE_NOT_EQUAL(margins, Margins(1, 2, 3, 5));
    }

    void testUnaryNegation() {
        margins = Margins(1, 2, 3, 4);
        Margins neg = -margins;
        REQUIRE_EQUAL(neg.top(), -1);
        REQUIRE_EQUAL(neg.right(), -2);
        REQUIRE_EQUAL(neg.bottom(), -3);
        REQUIRE_EQUAL(neg.left(), -4);
    }
};
