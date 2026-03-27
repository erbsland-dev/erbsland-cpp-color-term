# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

add_subdirectory(event)
add_subdirectory(layout)
add_subdirectory(surface)

target_sources(erbsland-color-term PRIVATE
        Application.cpp
        Application.hpp
        Display.cpp
        Display.hpp
        DimensionPolicy.hpp
        Geometry.hpp
        KeyBindings.cpp
        KeyBindings.hpp
        Layout.cpp
        Layout.hpp
        Page.cpp
        Page.hpp
        PaintContext.hpp
        SizePolicy.hpp
        Surface.cpp
        Surface.hpp
        ui_namespace.hpp
)
