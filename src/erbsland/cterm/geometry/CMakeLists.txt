# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term PRIVATE
        Alignment.hpp
        Anchor.hpp
        AxisMapper.hpp
        Coordinate.hpp
        Direction.cpp
        Direction.hpp
        Margins.hpp
        Orientation.hpp
        Position.cpp
        Position.hpp
        Rectangle.cpp
        Rectangle.hpp
        Size.cpp
        Size.hpp
)
