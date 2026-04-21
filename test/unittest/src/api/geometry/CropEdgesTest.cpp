// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/cterm/CropEdges.hpp>
#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(CropEdges)
class CropEdgesTest final : public el::UnitTest {
public:
    void testDefaultConstructionHasNoEdgesSet() {
        const CropEdges cropEdges;

        REQUIRE_EQUAL(cropEdges.flags().count(), 0U);
        REQUIRE_FALSE(cropEdges.isSet(Direction::None));
        REQUIRE_FALSE(cropEdges.isSet(Direction::North));
        REQUIRE_FALSE(cropEdges.isSet(Direction::NorthEast));
        REQUIRE_FALSE(cropEdges.isSet(Direction::East));
        REQUIRE_FALSE(cropEdges.isSet(Direction::SouthEast));
        REQUIRE_FALSE(cropEdges.isSet(Direction::South));
        REQUIRE_FALSE(cropEdges.isSet(Direction::SouthWest));
        REQUIRE_FALSE(cropEdges.isSet(Direction::West));
        REQUIRE_FALSE(cropEdges.isSet(Direction::NorthWest));
    }

    void testSetClearAndResetManageFlags() {
        CropEdges cropEdges;

        cropEdges.set(Direction::North);
        cropEdges.set(Direction::SouthWest);
        cropEdges.set(Direction::None);

        REQUIRE(cropEdges.isSet(Direction::North));
        REQUIRE(cropEdges.isSet(Direction::SouthWest));
        REQUIRE_EQUAL(cropEdges.flags().count(), 2U);

        cropEdges.clear(Direction::North);
        cropEdges.clear(Direction::None);

        REQUIRE_FALSE(cropEdges.isSet(Direction::North));
        REQUIRE(cropEdges.isSet(Direction::SouthWest));

        cropEdges.reset();

        REQUIRE_EQUAL(cropEdges.flags().count(), 0U);
    }

    void testComparisonOperatorsReflectFlagState() {
        CropEdges left;
        CropEdges right;

        left.set(Direction::East);
        right.set(Direction::East);

        REQUIRE(left == right);

        right.set(Direction::SouthEast);

        REQUIRE(left != right);
    }

    void testFromViewSetsEdgesAndCornersForClippedContent() {
        const auto cropEdges = CropEdges::fromView(Rectangle(2, 3, 4, 3), Rectangle(0, 0, 8, 8));

        REQUIRE(cropEdges.isSet(Direction::North));
        REQUIRE(cropEdges.isSet(Direction::NorthEast));
        REQUIRE(cropEdges.isSet(Direction::East));
        REQUIRE(cropEdges.isSet(Direction::SouthEast));
        REQUIRE(cropEdges.isSet(Direction::South));
        REQUIRE(cropEdges.isSet(Direction::SouthWest));
        REQUIRE(cropEdges.isSet(Direction::West));
        REQUIRE(cropEdges.isSet(Direction::NorthWest));

        REQUIRE_EQUAL(CropEdges::fromView(Rectangle(2, 3, 4, 3), Rectangle(2, 3, 4, 3)).flags().count(), 0U);
    }

    void testEdgeForViewReturnsExactDirectionsForCroppedFramePositions() {
        const auto viewRect = Rectangle(10, 20, 4, 3);
        const auto cropEdges = CropEdges::fromView(viewRect, Rectangle(7, 18, 10, 6));

        REQUIRE_EQUAL(cropEdges.edgeForView(Position(10, 20), viewRect), Direction::NorthWest);
        REQUIRE_EQUAL(cropEdges.edgeForView(Position(11, 20), viewRect), Direction::North);
        REQUIRE_EQUAL(cropEdges.edgeForView(Position(13, 20), viewRect), Direction::NorthEast);
        REQUIRE_EQUAL(cropEdges.edgeForView(Position(13, 21), viewRect), Direction::East);
        REQUIRE_EQUAL(cropEdges.edgeForView(Position(13, 22), viewRect), Direction::SouthEast);
        REQUIRE_EQUAL(cropEdges.edgeForView(Position(12, 22), viewRect), Direction::South);
        REQUIRE_EQUAL(cropEdges.edgeForView(Position(10, 22), viewRect), Direction::SouthWest);
        REQUIRE_EQUAL(cropEdges.edgeForView(Position(10, 21), viewRect), Direction::West);
        REQUIRE_EQUAL(cropEdges.edgeForView(Position(11, 21), viewRect), Direction::None);
    }

    void testEdgeForViewFallsBackToTheMatchingSingleEdgeAtCorners() {
        CropEdges cropEdges;
        const auto viewRect = Rectangle(10, 20, 4, 3);

        cropEdges.set(Direction::East);

        REQUIRE_EQUAL(cropEdges.edgeForView(Position(13, 20), viewRect), Direction::East);
        REQUIRE_EQUAL(cropEdges.edgeForView(Position(13, 22), viewRect), Direction::East);

        cropEdges.reset();
        cropEdges.set(Direction::South);

        REQUIRE_EQUAL(cropEdges.edgeForView(Position(13, 22), viewRect), Direction::South);
        REQUIRE_EQUAL(cropEdges.edgeForView(Position(10, 22), viewRect), Direction::South);
    }
};
