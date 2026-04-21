// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(Margins)
class MarginsTest final : public el::UnitTest {
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

    void testSideAccess() {
        using Side = Margins::Side;

        margins = Margins(1, 2, 3, 4);

        REQUIRE_EQUAL(margins.at(Side::Top), 1);
        REQUIRE_EQUAL(margins.at(Side::Right), 2);
        REQUIRE_EQUAL(margins.at(Side::Bottom), 3);
        REQUIRE_EQUAL(margins.at(Side::Left), 4);
        REQUIRE_EQUAL(margins[Side::Top], 1);
        REQUIRE_EQUAL(margins[Side::Right], 2);
        REQUIRE_EQUAL(margins[Side::Bottom], 3);
        REQUIRE_EQUAL(margins[Side::Left], 4);
    }

    void testSideSetters() {
        using Side = Margins::Side;

        margins = Margins{};
        margins.setTop(1);
        margins.setRight(2);
        margins.setBottom(3);
        margins.setLeft(4);

        REQUIRE_EQUAL(margins, Margins(1, 2, 3, 4));

        margins.set(Side::Top, 5);
        margins.set(Side::Right, 6);
        margins.set(Side::Bottom, 7);
        margins.set(Side::Left, 8);

        REQUIRE_EQUAL(margins, Margins(5, 6, 7, 8));
    }

    void testEqualityAndInequality() {
        margins = Margins(1, 2, 3, 4);
        REQUIRE_EQUAL(margins, Margins(1, 2, 3, 4));
        REQUIRE_NOT_EQUAL(margins, Margins(1, 2, 3, 5));
    }

    void testUnaryNegation() {
        margins = Margins(1, 2, 3, 4);
        const auto neg = -margins;
        REQUIRE_EQUAL(neg.top(), -1);
        REQUIRE_EQUAL(neg.right(), -2);
        REQUIRE_EQUAL(neg.bottom(), -3);
        REQUIRE_EQUAL(neg.left(), -4);
    }

    void testExtentIgnoresNegativeSides() {
        margins = Margins(-1, 2, 3, -4);

        REQUIRE_EQUAL(margins.horizontalExtent(), 2);
        REQUIRE_EQUAL(margins.verticalExtent(), 3);
        REQUIRE_EQUAL(margins.extent(), Size(2, 3));
        REQUIRE_EQUAL(margins.extent(Orientation::Horizontal), 2);
        REQUIRE_EQUAL(margins.extent(Orientation::Vertical), 3);
    }

    void testSpacingUsesLargestPositiveSidePerAxis() {
        margins = Margins(5, -2, -3, 4);

        REQUIRE_EQUAL(margins.horizontalSpacing(), 4);
        REQUIRE_EQUAL(margins.verticalSpacing(), 5);
        REQUIRE_EQUAL(margins.spacing(), Size(4, 5));
        REQUIRE_EQUAL(margins.spacing(Orientation::Horizontal), 4);
        REQUIRE_EQUAL(margins.spacing(Orientation::Vertical), 5);
    }

    void testComponentMax() {
        using Side = Margins::Side;

        margins = Margins(1, 6, -3, 4);
        const auto other = Margins(5, 2, -7, 8);

        REQUIRE_EQUAL(margins.componentMax(other), Margins(5, 6, -3, 8));
        REQUIRE_EQUAL(margins.componentMax(other, Side::Top), Margins(5, 6, -3, 4));
        REQUIRE_EQUAL(margins.componentMax(other, Side::Right), Margins(1, 6, -3, 4));
        REQUIRE_EQUAL(margins.componentMax(other, Side::Bottom), Margins(1, 6, -3, 4));
        REQUIRE_EQUAL(margins.componentMax(other, Side::Left), Margins(1, 6, -3, 8));
    }

    void testComponentMin() {
        using Side = Margins::Side;

        margins = Margins(1, 6, -3, 4);
        const auto other = Margins(5, 2, -7, 8);

        REQUIRE_EQUAL(margins.componentMin(other), Margins(1, 2, -7, 4));
        REQUIRE_EQUAL(margins.componentMin(other, Side::Top), Margins(1, 6, -3, 4));
        REQUIRE_EQUAL(margins.componentMin(other, Side::Right), Margins(1, 2, -3, 4));
        REQUIRE_EQUAL(margins.componentMin(other, Side::Bottom), Margins(1, 6, -7, 4));
        REQUIRE_EQUAL(margins.componentMin(other, Side::Left), Margins(1, 6, -3, 4));
    }
};
