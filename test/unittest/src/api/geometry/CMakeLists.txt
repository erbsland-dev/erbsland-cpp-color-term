# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term-unittest PRIVATE
        AlignmentTest.cpp
        AnchorTest.cpp
        AxisMapperTest.cpp
        CropEdgesTest.cpp
        DirectionTest.cpp
        GridLayoutTest.cpp
        IndexRangeTest.cpp
        MarginsTest.cpp
        OrientationTest.cpp
        PositionTest.cpp
        RectangleTest.cpp
        RectangleTestSupport.hpp
        SizeTest.cpp
)
