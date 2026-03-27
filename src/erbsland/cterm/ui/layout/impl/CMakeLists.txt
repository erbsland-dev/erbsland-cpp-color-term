# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term PRIVATE
        DimensionConstraints.cpp
        DimensionConstraints.hpp
        LayoutConstraints.cpp
        LayoutConstraints.hpp
        StackLayoutItem.cpp
        StackLayoutItem.hpp
        StackLayoutItems.cpp
        StackLayoutItems.hpp
)
