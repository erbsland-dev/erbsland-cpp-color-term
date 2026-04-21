# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

add_subdirectory(impl)

target_sources(erbsland-color-term PRIVATE
        Buttons.cpp
        Buttons.hpp
        Centered.cpp
        Centered.hpp
        Frame.cpp
        Frame.hpp
        Pages.cpp
        Pages.hpp
        ScrollArea.cpp
        ScrollArea.hpp
        Sections.cpp
        Sections.hpp
        SectionOptions.hpp
        SingleContentLayout.cpp
        SingleContentLayout.hpp
        Stack.cpp
        Stack.hpp
        Viewport.cpp
        Viewport.hpp
)
